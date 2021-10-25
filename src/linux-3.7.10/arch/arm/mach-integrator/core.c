/*
 *  linux/arch/arm/mach-integrator/core.c
 *
 *  Copyright (C) 2000-2003 Deep Blue Solutions Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/memblock.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/termios.h>
#include <linux/amba/bus.h>
#include <linux/amba/serial.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/platform.h>
#include <mach/cm.h>
#include <mach/irqs.h>

#include <asm/mach-types.h>
#include <asm/mach/time.h>
#include <asm/pgtable.h>

#include "common.h"

#ifdef CONFIG_ATAGS

#define INTEGRATOR_RTC_IRQ	{ IRQ_RTCINT }
#define INTEGRATOR_UART0_IRQ	{ IRQ_UARTINT0 }
#define INTEGRATOR_UART1_IRQ	{ IRQ_UARTINT1 }
#define KMI0_IRQ		{ IRQ_KMIINT0 }
#define KMI1_IRQ		{ IRQ_KMIINT1 }

static AMBA_APB_DEVICE(rtc, "rtc", 0,
	INTEGRATOR_RTC_BASE, INTEGRATOR_RTC_IRQ, NULL);

static AMBA_APB_DEVICE(uart0, "uart0", 0,
	INTEGRATOR_UART0_BASE, INTEGRATOR_UART0_IRQ, &integrator_uart_data);

static AMBA_APB_DEVICE(uart1, "uart1", 0,
	INTEGRATOR_UART1_BASE, INTEGRATOR_UART1_IRQ, &integrator_uart_data);

static AMBA_APB_DEVICE(kmi0, "kmi0", 0, KMI0_BASE, KMI0_IRQ, NULL);
static AMBA_APB_DEVICE(kmi1, "kmi1", 0, KMI1_BASE, KMI1_IRQ, NULL);

static struct amba_device *amba_devs[] __initdata = {
	&rtc_device,
	&uart0_device,
	&uart1_device,
	&kmi0_device,
	&kmi1_device,
};

int __init integrator_init(bool is_cp)
{
	int i;

	/*
	 * The Integrator/AP lacks necessary AMBA PrimeCell IDs, so we need to
	 * hard-code them. The Integator/CP and forward have proper cell IDs.
	 * Else we leave them undefined to the bus driver can autoprobe them.
	 */
	if (!is_cp) {
		rtc_device.periphid	= 0x00041030;
		uart0_device.periphid	= 0x00041010;
		uart1_device.periphid	= 0x00041010;
		kmi0_device.periphid	= 0x00041050;
		kmi1_device.periphid	= 0x00041050;
	}

	for (i = 0; i < ARRAY_SIZE(amba_devs); i++) {
		struct amba_device *d = amba_devs[i];
		amba_device_register(d, &iomem_resource);
	}

	return 0;
}

#endif

/*
 * On the Integrator platform, the port RTS and DTR are provided by
 * bits in the following SC_CTRLS register bits:
 *        RTS  DTR
 *  UART0  7    6
 *  UART1  5    4
 */
#define SC_CTRLC	__io_address(INTEGRATOR_SC_CTRLC)
#define SC_CTRLS	__io_address(INTEGRATOR_SC_CTRLS)

static void integrator_uart_set_mctrl(struct amba_device *dev, void __iomem *base, unsigned int mctrl)
{
	unsigned int ctrls = 0, ctrlc = 0, rts_mask, dtr_mask;
	u32 phybase = dev->res.start;

	if (phybase == INTEGRATOR_UART0_BASE) {
		/* UART0 */
		rts_mask = 1 << 4;
		dtr_mask = 1 << 5;
	} else {
		/* UART1 */
		rts_mask = 1 << 6;
		dtr_mask = 1 << 7;
	}

	if (mctrl & TIOCM_RTS)
		ctrlc |= rts_mask;
	else
		ctrls |= rts_mask;

	if (mctrl & TIOCM_DTR)
		ctrlc |= dtr_mask;
	else
		ctrls |= dtr_mask;

	__raw_writel(ctrls, SC_CTRLS);
	__raw_writel(ctrlc, SC_CTRLC);
}

struct amba_pl010_data integrator_uart_data = {
	.set_mctrl = integrator_uart_set_mctrl,
};

static DEFINE_RAW_SPINLOCK(cm_lock);

/**
 * cm_control - update the CM_CTRL register.
 * @mask: bits to change
 * @set: bits to set
 */
void cm_control(u32 mask, u32 set)
{
	unsigned long flags;
	u32 val;

	raw_spin_lock_irqsave(&cm_lock, flags);
	val = readl(CM_CTRL) & ~mask;
	writel(val | set, CM_CTRL);
	raw_spin_unlock_irqrestore(&cm_lock, flags);
}

EXPORT_SYMBOL(cm_control);

/*
 * We need to stop things allocating the low memory; ideally we need a
 * better implementation of GFP_DMA which does not assume that DMA-able
 * memory starts at zero.
 */
void __init integrator_reserve(void)
{
	memblock_reserve(PHYS_OFFSET, __pa(swapper_pg_dir) - PHYS_OFFSET);
}

/*
 * To reset, we hit the on-board reset register in the system FPGA
 */
void integrator_restart(char mode, const char *cmd)
{
	cm_control(CM_CTRL_RESET, CM_CTRL_RESET);
}
