/*
    comedi/drivers/contec_pci_dio.c

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
Driver: contec_pci_dio
Description: Contec PIO1616L digital I/O board
Devices: [Contec] PIO1616L (contec_pci_dio)
Author: Stefano Rivoir <s.rivoir@gts.it>
Updated: Wed, 27 Jun 2007 13:00:06 +0100
Status: works

Configuration Options: not applicable, uses comedi PCI auto config
*/

#include "../comedidev.h"

#define PCI_DEVICE_ID_PIO1616L 0x8172

/*
 * Register map
 */
#define PIO1616L_DI_REG		0x00
#define PIO1616L_DO_REG		0x02

static int contec_do_insn_bits(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	unsigned int mask = data[0];
	unsigned int bits = data[1];

	if (mask) {
		s->state &= ~mask;
		s->state |= (bits & mask);

		outw(s->state, dev->iobase + PIO1616L_DO_REG);
	}

	data[1] = s->state;

	return insn->n;
}

static int contec_di_insn_bits(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	data[1] = inw(dev->iobase + PIO1616L_DI_REG);

	return insn->n;
}

static int contec_attach_pci(struct comedi_device *dev,
			     struct pci_dev *pcidev)
{
	struct comedi_subdevice *s;
	int ret;

	comedi_set_hw_dev(dev, &pcidev->dev);

	dev->board_name = dev->driver->driver_name;

	ret = comedi_pci_enable(pcidev, dev->board_name);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 0);

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_DI;
	s->subdev_flags	= SDF_READABLE;
	s->n_chan	= 16;
	s->maxdata	= 1;
	s->range_table	= &range_digital;
	s->insn_bits	= contec_di_insn_bits;

	s = &dev->subdevices[1];
	s->type		= COMEDI_SUBD_DO;
	s->subdev_flags	= SDF_WRITABLE;
	s->n_chan	= 16;
	s->maxdata	= 1;
	s->range_table	= &range_digital;
	s->insn_bits	= contec_do_insn_bits;

	dev_info(dev->class_dev, "%s attached\n", dev->board_name);

	return 0;
}

static void contec_detach(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);

	if (pcidev) {
		if (dev->iobase)
			comedi_pci_disable(pcidev);
	}
}

static struct comedi_driver contec_pci_dio_driver = {
	.driver_name	= "contec_pci_dio",
	.module		= THIS_MODULE,
	.attach_pci	= contec_attach_pci,
	.detach		= contec_detach,
};

static int __devinit contec_pci_dio_pci_probe(struct pci_dev *dev,
					      const struct pci_device_id *ent)
{
	return comedi_pci_auto_config(dev, &contec_pci_dio_driver);
}

static void __devexit contec_pci_dio_pci_remove(struct pci_dev *dev)
{
	comedi_pci_auto_unconfig(dev);
}

static DEFINE_PCI_DEVICE_TABLE(contec_pci_dio_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CONTEC, PCI_DEVICE_ID_PIO1616L) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, contec_pci_dio_pci_table);

static struct pci_driver contec_pci_dio_pci_driver = {
	.name		= "contec_pci_dio",
	.id_table	= contec_pci_dio_pci_table,
	.probe		= contec_pci_dio_pci_probe,
	.remove		= __devexit_p(contec_pci_dio_pci_remove),
};
module_comedi_pci_driver(contec_pci_dio_driver, contec_pci_dio_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
