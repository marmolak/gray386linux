/*
 * comedi/drivers/pcm3730.c
 * Driver for PCM3730 and clones
 * Blaine Lee
 * from pcl725 by David S.
 */
/*
Driver: pcm3730
Description: PCM3730
Author: Blaine Lee
Devices: [Advantech] PCM-3730 (pcm3730)
Status: unknown

Configuration options:
  [0] - I/O port base
*/

#include "../comedidev.h"

#include <linux/ioport.h>

#define PCM3730_SIZE 4		/*  consecutive io port addresses */

#define PCM3730_DOA 0		/*  offsets for each port */
#define PCM3730_DOB 2
#define PCM3730_DOC 3
#define PCM3730_DIA 0
#define PCM3730_DIB 2
#define PCM3730_DIC 3

static int pcm3730_do_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	if (data[0]) {
		s->state &= ~data[0];
		s->state |= (data[0] & data[1]);
		outb(s->state, dev->iobase + (unsigned long)(s->private));
	}
	data[1] = s->state;

	return insn->n;
}

static int pcm3730_di_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	data[1] = inb(dev->iobase + (unsigned long)(s->private));
	return insn->n;
}

static int pcm3730_attach(struct comedi_device *dev,
			  struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	unsigned long iobase;
	int ret;

	iobase = it->options[0];
	printk(KERN_INFO "comedi%d: pcm3730: 0x%04lx ", dev->minor, iobase);
	if (!request_region(iobase, PCM3730_SIZE, "pcm3730")) {
		printk("I/O port conflict\n");
		return -EIO;
	}
	dev->iobase = iobase;
	dev->board_name = "pcm3730";
	dev->iobase = dev->iobase;
	dev->irq = 0;

	ret = comedi_alloc_subdevices(dev, 6);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	s->type = COMEDI_SUBD_DO;
	s->subdev_flags = SDF_WRITABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_do_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DOA;

	s = &dev->subdevices[1];
	s->type = COMEDI_SUBD_DO;
	s->subdev_flags = SDF_WRITABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_do_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DOB;

	s = &dev->subdevices[2];
	s->type = COMEDI_SUBD_DO;
	s->subdev_flags = SDF_WRITABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_do_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DOC;

	s = &dev->subdevices[3];
	s->type = COMEDI_SUBD_DI;
	s->subdev_flags = SDF_READABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_di_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DIA;

	s = &dev->subdevices[4];
	s->type = COMEDI_SUBD_DI;
	s->subdev_flags = SDF_READABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_di_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DIB;

	s = &dev->subdevices[5];
	s->type = COMEDI_SUBD_DI;
	s->subdev_flags = SDF_READABLE;
	s->maxdata = 1;
	s->n_chan = 8;
	s->insn_bits = pcm3730_di_insn_bits;
	s->range_table = &range_digital;
	s->private = (void *)PCM3730_DIC;

	printk(KERN_INFO "\n");

	return 0;
}

static void pcm3730_detach(struct comedi_device *dev)
{
	if (dev->iobase)
		release_region(dev->iobase, PCM3730_SIZE);
}

static struct comedi_driver pcm3730_driver = {
	.driver_name	= "pcm3730",
	.module		= THIS_MODULE,
	.attach		= pcm3730_attach,
	.detach		= pcm3730_detach,
};
module_comedi_driver(pcm3730_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
