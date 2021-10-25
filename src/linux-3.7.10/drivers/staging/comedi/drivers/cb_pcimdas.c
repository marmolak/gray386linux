/*
    comedi/drivers/cb_pcimdas.c
    Comedi driver for Computer Boards PCIM-DAS1602/16

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 2000 David A. Schleef <ds@schleef.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/*
Driver: cb_pcimdas
Description: Measurement Computing PCI Migration series boards
Devices: [ComputerBoards] PCIM-DAS1602/16 (cb_pcimdas)
Author: Richard Bytheway
Updated: Wed, 13 Nov 2002 12:34:56 +0000
Status: experimental

Written to support the PCIM-DAS1602/16 on a 2.4 series kernel.

Configuration Options:
    [0] - PCI bus number
    [1] - PCI slot number

Developed from cb_pcidas and skel by Richard Bytheway (mocelet@sucs.org).
Only supports DIO, AO and simple AI in it's present form.
No interrupts, multi channel or FIFO AI, although the card looks like it could support this.
See http://www.mccdaq.com/PDFs/Manuals/pcim-das1602-16.pdf for more details.
*/

#include "../comedidev.h"

#include <linux/delay.h>
#include <linux/interrupt.h>

#include "plx9052.h"
#include "8255.h"

/* #define CBPCIMDAS_DEBUG */
#undef CBPCIMDAS_DEBUG

#define PCI_VENDOR_ID_COMPUTERBOARDS	0x1307

/* Registers for the PCIM-DAS1602/16 */

/* sizes of io regions (bytes) */
#define BADR3_SIZE 16

/* DAC Offsets */
#define ADC_TRIG 0
#define DAC0_OFFSET 2
#define DAC1_OFFSET 4

/* AI and Counter Constants */
#define MUX_LIMITS 0
#define MAIN_CONN_DIO 1
#define ADC_STAT 2
#define ADC_CONV_STAT 3
#define ADC_INT 4
#define ADC_PACER 5
#define BURST_MODE 6
#define PROG_GAIN 7
#define CLK8254_1_DATA 8
#define CLK8254_2_DATA 9
#define CLK8254_3_DATA 10
#define CLK8254_CONTROL 11
#define USER_COUNTER 12
#define RESID_COUNT_H 13
#define RESID_COUNT_L 14

/* Board description */
struct cb_pcimdas_board {
	const char *name;
	unsigned short device_id;
	int ai_se_chans;	/*  Inputs in single-ended mode */
	int ai_diff_chans;	/*  Inputs in differential mode */
	int ai_bits;		/*  analog input resolution */
	int ai_speed;		/*  fastest conversion period in ns */
	int ao_nchan;		/*  number of analog out channels */
	int ao_bits;		/*  analogue output resolution */
	int has_ao_fifo;	/*  analog output has fifo */
	int ao_scan_speed;	/*  analog output speed for 1602 series (for a scan, not conversion) */
	int fifo_size;		/*  number of samples fifo can hold */
	int dio_bits;		/*  number of dio bits */
	int has_dio;		/*  has DIO */
	const struct comedi_lrange *ranges;
};

static const struct cb_pcimdas_board cb_pcimdas_boards[] = {
	{
	 .name = "PCIM-DAS1602/16",
	 .device_id = 0x56,
	 .ai_se_chans = 16,
	 .ai_diff_chans = 8,
	 .ai_bits = 16,
	 .ai_speed = 10000,	/* ?? */
	 .ao_nchan = 2,
	 .ao_bits = 12,
	 .has_ao_fifo = 0,	/* ?? */
	 .ao_scan_speed = 10000,
	 /* ?? */
	 .fifo_size = 1024,
	 .dio_bits = 24,
	 .has_dio = 1,
/*	.ranges = &cb_pcimdas_ranges, */
	 },
};

/*
 * this structure is for data unique to this hardware driver.  If
 * several hardware drivers keep similar information in this structure,
 * feel free to suggest moving the variable to the struct comedi_device
 * struct.
 */
struct cb_pcimdas_private {
	/* base addresses */
	unsigned long BADR3;

	/* Used for AO readback */
	unsigned int ao_readback[2];
};

/*
 * "instructions" read/write data in "one-shot" or "software-triggered"
 * mode.
 */
static int cb_pcimdas_ai_rinsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	const struct cb_pcimdas_board *thisboard = comedi_board(dev);
	struct cb_pcimdas_private *devpriv = dev->private;
	int n, i;
	unsigned int d;
	unsigned int busy;
	int chan = CR_CHAN(insn->chanspec);
	unsigned short chanlims;
	int maxchans;

	/*  only support sw initiated reads from a single channel */

	/* check channel number */
	if ((inb(devpriv->BADR3 + 2) & 0x20) == 0)	/* differential mode */
		maxchans = thisboard->ai_diff_chans;
	else
		maxchans = thisboard->ai_se_chans;

	if (chan > (maxchans - 1))
		return -ETIMEDOUT;	/* *** Wrong error code. Fixme. */

	/* configure for sw initiated read */
	d = inb(devpriv->BADR3 + 5);
	if ((d & 0x03) > 0) {	/* only reset if needed. */
		d = d & 0xfd;
		outb(d, devpriv->BADR3 + 5);
	}
	outb(0x01, devpriv->BADR3 + 6);	/* set bursting off, conversions on */
	outb(0x00, devpriv->BADR3 + 7);	/* set range to 10V. UP/BP is controlled by a switch on the board */

	/*
	 * write channel limits to multiplexer, set Low (bits 0-3) and
	 * High (bits 4-7) channels to chan.
	 */
	chanlims = chan | (chan << 4);
	outb(chanlims, devpriv->BADR3 + 0);

	/* convert n samples */
	for (n = 0; n < insn->n; n++) {
		/* trigger conversion */
		outw(0, dev->iobase + 0);

#define TIMEOUT 1000		/* typically takes 5 loops on a lightly loaded Pentium 100MHz, */
		/* this is likely to be 100 loops on a 2GHz machine, so set 1000 as the limit. */

		/* wait for conversion to end */
		for (i = 0; i < TIMEOUT; i++) {
			busy = inb(devpriv->BADR3 + 2) & 0x80;
			if (!busy)
				break;
		}
		if (i == TIMEOUT) {
			printk("timeout\n");
			return -ETIMEDOUT;
		}
		/* read data */
		d = inw(dev->iobase + 0);

		/* mangle the data as necessary */
		/* d ^= 1<<(thisboard->ai_bits-1); // 16 bit data from ADC, so no mangle needed. */

		data[n] = d;
	}

	/* return the number of samples read/written */
	return n;
}

static int cb_pcimdas_ao_winsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcimdas_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	/* Writing a list of values to an AO channel is probably not
	 * very useful, but that's how the interface is defined. */
	for (i = 0; i < insn->n; i++) {
		switch (chan) {
		case 0:
			outw(data[i] & 0x0FFF, dev->iobase + DAC0_OFFSET);
			break;
		case 1:
			outw(data[i] & 0x0FFF, dev->iobase + DAC1_OFFSET);
			break;
		default:
			return -1;
		}
		devpriv->ao_readback[chan] = data[i];
	}

	/* return the number of samples read/written */
	return i;
}

/* AO subdevices should have a read insn as well as a write insn.
 * Usually this means copying a value stored in devpriv. */
static int cb_pcimdas_ao_rinsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcimdas_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	for (i = 0; i < insn->n; i++)
		data[i] = devpriv->ao_readback[chan];

	return i;
}

static const void *cb_pcimdas_find_boardinfo(struct comedi_device *dev,
					     struct pci_dev *pcidev)
{
	const struct cb_pcimdas_board *thisboard;
	int i;

	for (i = 0; i < ARRAY_SIZE(cb_pcimdas_boards); i++) {
		thisboard = &cb_pcimdas_boards[i];
		if (thisboard->device_id == pcidev->device)
			return thisboard;
	}
	return NULL;
}

static int cb_pcimdas_attach_pci(struct comedi_device *dev,
				 struct pci_dev *pcidev)
{
	const struct cb_pcimdas_board *thisboard;
	struct cb_pcimdas_private *devpriv;
	struct comedi_subdevice *s;
	unsigned long iobase_8255;
	int ret;

	comedi_set_hw_dev(dev, &pcidev->dev);

	thisboard = cb_pcimdas_find_boardinfo(dev, pcidev);
	if (!thisboard)
		return -ENODEV;
	dev->board_ptr = thisboard;
	dev->board_name = thisboard->name;

	ret = alloc_private(dev, sizeof(*devpriv));
	if (ret)
		return ret;
	devpriv = dev->private;

	/*  Warn about non-tested features */
	switch (thisboard->device_id) {
	case 0x56:
		break;
	default:
		dev_dbg(dev->class_dev, "THIS CARD IS UNSUPPORTED.\n");
		dev_dbg(dev->class_dev,
			"PLEASE REPORT USAGE TO <mocelet@sucs.org>\n");
	}

	ret = comedi_pci_enable(pcidev, dev->board_name);
	if (ret)
		return ret;

	dev->iobase = pci_resource_start(pcidev, 2);
	devpriv->BADR3 = pci_resource_start(pcidev, 3);
	iobase_8255 = pci_resource_start(pcidev, 4);

/* Dont support IRQ yet */
/*  get irq */
/* if(request_irq(pcidev->irq, cb_pcimdas_interrupt, IRQF_SHARED, "cb_pcimdas", dev )) */
/* { */
/* printk(" unable to allocate irq %u\n", pcidev->irq); */
/* return -EINVAL; */
/* } */
/* dev->irq = pcidev->irq; */

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/* dev->read_subdev=s; */
	/*  analog input subdevice */
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND;
	s->n_chan = thisboard->ai_se_chans;
	s->maxdata = (1 << thisboard->ai_bits) - 1;
	s->range_table = &range_unknown;
	s->len_chanlist = 1;	/*  This is the maximum chanlist length that */
	/*  the board can handle */
	s->insn_read = cb_pcimdas_ai_rinsn;

	s = &dev->subdevices[1];
	/*  analog output subdevice */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = thisboard->ao_nchan;
	s->maxdata = 1 << thisboard->ao_bits;
	/* ranges are hardware settable, but not software readable. */
	s->range_table = &range_unknown;
	s->insn_write = &cb_pcimdas_ao_winsn;
	s->insn_read = &cb_pcimdas_ao_rinsn;

	s = &dev->subdevices[2];
	/* digital i/o subdevice */
	if (thisboard->has_dio)
		subdev_8255_init(dev, s, NULL, iobase_8255);
	else
		s->type = COMEDI_SUBD_UNUSED;

	dev_info(dev->class_dev, "%s attached\n", dev->board_name);

	return 0;
}

static void cb_pcimdas_detach(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);

	if (dev->irq)
		free_irq(dev->irq, dev);
	if (pcidev) {
		if (dev->iobase)
			comedi_pci_disable(pcidev);
	}
}

static struct comedi_driver cb_pcimdas_driver = {
	.driver_name	= "cb_pcimdas",
	.module		= THIS_MODULE,
	.attach_pci	= cb_pcimdas_attach_pci,
	.detach		= cb_pcimdas_detach,
};

static int __devinit cb_pcimdas_pci_probe(struct pci_dev *dev,
					  const struct pci_device_id *ent)
{
	return comedi_pci_auto_config(dev, &cb_pcimdas_driver);
}

static void __devexit cb_pcimdas_pci_remove(struct pci_dev *dev)
{
	comedi_pci_auto_unconfig(dev);
}

static DEFINE_PCI_DEVICE_TABLE(cb_pcimdas_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_COMPUTERBOARDS, 0x0056) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcimdas_pci_table);

static struct pci_driver cb_pcimdas_pci_driver = {
	.name		= "cb_pcimdas",
	.id_table	= cb_pcimdas_pci_table,
	.probe		= cb_pcimdas_pci_probe,
	.remove		= __devexit_p(cb_pcimdas_pci_remove),
};
module_comedi_pci_driver(cb_pcimdas_driver, cb_pcimdas_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
