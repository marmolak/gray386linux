/*
    comedi/drivers/ke_counter.c
    Comedi driver for Kolter-Electronic PCI Counter 1 Card

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
Driver: ke_counter
Description: Driver for Kolter Electronic Counter Card
Devices: [Kolter Electronic] PCI Counter Card (ke_counter)
Author: Michael Hillmann
Updated: Mon, 14 Apr 2008 15:42:42 +0100
Status: tested

Configuration Options: not applicable, uses PCI auto config

This driver is a simple driver to read the counter values from
Kolter Electronic PCI Counter Card.
*/

#include "../comedidev.h"

#define CNT_DRIVER_NAME         "ke_counter"
#define PCI_VENDOR_ID_KOLTER    0x1001
#define CNT_CARD_DEVICE_ID      0x0014

/*-- board specification structure ------------------------------------------*/

struct cnt_board_struct {

	const char *name;
	int device_id;
	int cnt_channel_nbr;
	int cnt_bits;
};

static const struct cnt_board_struct cnt_boards[] = {
	{
	 .name = CNT_DRIVER_NAME,
	 .device_id = CNT_CARD_DEVICE_ID,
	 .cnt_channel_nbr = 3,
	 .cnt_bits = 24}
};

/*-- counter write ----------------------------------------------------------*/

/* This should be used only for resetting the counters; maybe it is better
   to make a special command 'reset'. */
static int cnt_winsn(struct comedi_device *dev,
		     struct comedi_subdevice *s, struct comedi_insn *insn,
		     unsigned int *data)
{
	int chan = CR_CHAN(insn->chanspec);

	outb((unsigned char)((data[0] >> 24) & 0xff),
	     dev->iobase + chan * 0x20 + 0x10);
	outb((unsigned char)((data[0] >> 16) & 0xff),
	     dev->iobase + chan * 0x20 + 0x0c);
	outb((unsigned char)((data[0] >> 8) & 0xff),
	     dev->iobase + chan * 0x20 + 0x08);
	outb((unsigned char)((data[0] >> 0) & 0xff),
	     dev->iobase + chan * 0x20 + 0x04);

	/* return the number of samples written */
	return 1;
}

/*-- counter read -----------------------------------------------------------*/

static int cnt_rinsn(struct comedi_device *dev,
		     struct comedi_subdevice *s, struct comedi_insn *insn,
		     unsigned int *data)
{
	unsigned char a0, a1, a2, a3, a4;
	int chan = CR_CHAN(insn->chanspec);
	int result;

	a0 = inb(dev->iobase + chan * 0x20);
	a1 = inb(dev->iobase + chan * 0x20 + 0x04);
	a2 = inb(dev->iobase + chan * 0x20 + 0x08);
	a3 = inb(dev->iobase + chan * 0x20 + 0x0c);
	a4 = inb(dev->iobase + chan * 0x20 + 0x10);

	result = (a1 + (a2 * 256) + (a3 * 65536));
	if (a4 > 0)
		result = result - s->maxdata;

	*data = (unsigned int)result;

	/* return the number of samples read */
	return 1;
}

static const void *cnt_find_boardinfo(struct comedi_device *dev,
				      struct pci_dev *pcidev)
{
	const struct cnt_board_struct *board;
	int i;

	for (i = 0; i < ARRAY_SIZE(cnt_boards); i++) {
		board = &cnt_boards[i];
		if (board->device_id == pcidev->device)
			return board;
	}
	return NULL;
}

static int cnt_attach_pci(struct comedi_device *dev,
			  struct pci_dev *pcidev)
{
	const struct cnt_board_struct *board;
	struct comedi_subdevice *s;
	int ret;

	comedi_set_hw_dev(dev, &pcidev->dev);

	board = cnt_find_boardinfo(dev, pcidev);
	if (!board)
		return -ENODEV;
	dev->board_ptr = board;
	dev->board_name = board->name;

	ret = comedi_pci_enable(pcidev, dev->board_name);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 0);

	ret = comedi_alloc_subdevices(dev, 1);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	dev->read_subdev = s;

	s->type = COMEDI_SUBD_COUNTER;
	s->subdev_flags = SDF_READABLE /* | SDF_COMMON */ ;
	s->n_chan = board->cnt_channel_nbr;
	s->maxdata = (1 << board->cnt_bits) - 1;
	s->insn_read = cnt_rinsn;
	s->insn_write = cnt_winsn;

	/*  select 20MHz clock */
	outb(3, dev->iobase + 248);

	/*  reset all counters */
	outb(0, dev->iobase);
	outb(0, dev->iobase + 0x20);
	outb(0, dev->iobase + 0x40);

	dev_info(dev->class_dev, "%s: %s attached\n",
		dev->driver->driver_name, dev->board_name);

	return 0;
}

static void cnt_detach(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);

	if (pcidev) {
		if (dev->iobase)
			comedi_pci_disable(pcidev);
	}
}

static struct comedi_driver ke_counter_driver = {
	.driver_name	= "ke_counter",
	.module		= THIS_MODULE,
	.attach_pci	= cnt_attach_pci,
	.detach		= cnt_detach,
};

static int __devinit ke_counter_pci_probe(struct pci_dev *dev,
					  const struct pci_device_id *ent)
{
	return comedi_pci_auto_config(dev, &ke_counter_driver);
}

static void __devexit ke_counter_pci_remove(struct pci_dev *dev)
{
	comedi_pci_auto_unconfig(dev);
}

static DEFINE_PCI_DEVICE_TABLE(ke_counter_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_KOLTER, CNT_CARD_DEVICE_ID) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, ke_counter_pci_table);

static struct pci_driver ke_counter_pci_driver = {
	.name		= "ke_counter",
	.id_table	= ke_counter_pci_table,
	.probe		= ke_counter_pci_probe,
	.remove		= __devexit_p(ke_counter_pci_remove),
};
module_comedi_pci_driver(ke_counter_driver, ke_counter_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
