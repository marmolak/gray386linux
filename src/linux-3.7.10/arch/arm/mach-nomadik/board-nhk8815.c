/*
 *  linux/arch/arm/mach-nomadik/board-8815nhk.c
 *
 *  Copyright (C) STMicroelectronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 *  NHK15 board specifc driver definition
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/amba/bus.h>
#include <linux/amba/mmci.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/onenand.h>
#include <linux/mtd/partitions.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/pinctrl/machine.h>
#include <asm/hardware/vic.h>
#include <asm/sizes.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/flash.h>
#include <asm/mach/time.h>

#include <plat/gpio-nomadik.h>
#include <plat/mtu.h>
#include <plat/pincfg.h>

#include <linux/platform_data/mtd-nomadik-nand.h>
#include <mach/fsmc.h>

#include "cpu-8815.h"

/* Initial value for SRC control register: all timers use MXTAL/8 source */
#define SRC_CR_INIT_MASK	0x00007fff
#define SRC_CR_INIT_VAL		0x2aaa8000

/* These addresses span 16MB, so use three individual pages */
static struct resource nhk8815_nand_resources[] = {
	{
		.name = "nand_addr",
		.start = NAND_IO_ADDR,
		.end = NAND_IO_ADDR + 0xfff,
		.flags = IORESOURCE_MEM,
	}, {
		.name = "nand_cmd",
		.start = NAND_IO_CMD,
		.end = NAND_IO_CMD + 0xfff,
		.flags = IORESOURCE_MEM,
	}, {
		.name = "nand_data",
		.start = NAND_IO_DATA,
		.end = NAND_IO_DATA + 0xfff,
		.flags = IORESOURCE_MEM,
	}
};

static int nhk8815_nand_init(void)
{
	/* FSMC setup for nand chip select (8-bit nand in 8815NHK) */
	writel(0x0000000E, FSMC_PCR(0));
	writel(0x000D0A00, FSMC_PMEM(0));
	writel(0x00100A00, FSMC_PATT(0));

	/* enable access to the chip select area */
	writel(readl(FSMC_PCR(0)) | 0x04, FSMC_PCR(0));

	return 0;
}

/*
 * These partitions are the same as those used in the 2.6.20 release
 * shipped by the vendor; the first two partitions are mandated
 * by the boot ROM, and the bootloader area is somehow oversized...
 */
static struct mtd_partition nhk8815_partitions[] = {
	{
		.name	= "X-Loader(NAND)",
		.offset = 0,
		.size	= SZ_256K,
	}, {
		.name	= "MemInit(NAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= SZ_256K,
	}, {
		.name	= "BootLoader(NAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= SZ_2M,
	}, {
		.name	= "Kernel zImage(NAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 3 * SZ_1M,
	}, {
		.name	= "Root Filesystem(NAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 22 * SZ_1M,
	}, {
		.name	= "User Filesystem(NAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct nomadik_nand_platform_data nhk8815_nand_data = {
	.parts		= nhk8815_partitions,
	.nparts		= ARRAY_SIZE(nhk8815_partitions),
	.options	= NAND_COPYBACK | NAND_CACHEPRG | NAND_NO_PADDING,
	.init		= nhk8815_nand_init,
};

static struct platform_device nhk8815_nand_device = {
	.name		= "nomadik_nand",
	.dev		= {
		.platform_data = &nhk8815_nand_data,
	},
	.resource	= nhk8815_nand_resources,
	.num_resources	= ARRAY_SIZE(nhk8815_nand_resources),
};

/* These are the partitions for the OneNand device, different from above */
static struct mtd_partition nhk8815_onenand_partitions[] = {
	{
		.name	= "X-Loader(OneNAND)",
		.offset = 0,
		.size	= SZ_256K,
	}, {
		.name	= "MemInit(OneNAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= SZ_256K,
	}, {
		.name	= "BootLoader(OneNAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= SZ_2M-SZ_256K,
	}, {
		.name	= "SysImage(OneNAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 4 * SZ_1M,
	}, {
		.name	= "Root Filesystem(OneNAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 22 * SZ_1M,
	}, {
		.name	= "User Filesystem(OneNAND)",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct onenand_platform_data nhk8815_onenand_data = {
	.parts		= nhk8815_onenand_partitions,
	.nr_parts	= ARRAY_SIZE(nhk8815_onenand_partitions),
};

static struct resource nhk8815_onenand_resource[] = {
	{
		.start		= 0x30000000,
		.end		= 0x30000000 + SZ_128K - 1,
		.flags		= IORESOURCE_MEM,
	},
};

static struct platform_device nhk8815_onenand_device = {
	.name		= "onenand-flash",
	.id		= -1,
	.dev		= {
		.platform_data	= &nhk8815_onenand_data,
	},
	.resource	= nhk8815_onenand_resource,
	.num_resources	= ARRAY_SIZE(nhk8815_onenand_resource),
};

static void __init nhk8815_onenand_init(void)
{
#ifdef CONFIG_MTD_ONENAND
       /* Set up SMCS0 for OneNand */
	writel(0x000030db, FSMC_BCR(0));
	writel(0x02100551, FSMC_BTR(0));
#endif
}

static struct mmci_platform_data mmcsd_plat_data = {
	.ocr_mask = MMC_VDD_29_30,
	.f_max = 48000000,
	.gpio_wp = -1,
	.gpio_cd = 111,
	.cd_invert = true,
	.capabilities = MMC_CAP_MMC_HIGHSPEED |
	MMC_CAP_SD_HIGHSPEED | MMC_CAP_4_BIT_DATA,
};

static int __init nhk8815_mmcsd_init(void)
{
	int ret;

	ret = gpio_request(112, "card detect bias");
	if (ret)
		return ret;
	gpio_direction_output(112, 0);
	amba_apb_device_add(NULL, "mmci", NOMADIK_SDI_BASE, SZ_4K, IRQ_SDMMC, 0, &mmcsd_plat_data, 0x10180180);
	return 0;
}
module_init(nhk8815_mmcsd_init);

static struct resource nhk8815_eth_resources[] = {
	{
		.name = "smc91x-regs",
		.start = 0x34000000 + 0x300,
		.end = 0x34000000 + SZ_64K - 1,
		.flags = IORESOURCE_MEM,
	}, {
		.start = NOMADIK_GPIO_TO_IRQ(115),
		.end = NOMADIK_GPIO_TO_IRQ(115),
		.flags = IORESOURCE_IRQ | IRQF_TRIGGER_RISING,
	}
};

static struct platform_device nhk8815_eth_device = {
	.name = "smc91x",
	.resource = nhk8815_eth_resources,
	.num_resources = ARRAY_SIZE(nhk8815_eth_resources),
};

static int __init nhk8815_eth_init(void)
{
	int gpio_nr = 115; /* hardwired in the board */
	int err;

	err = gpio_request(gpio_nr, "eth_irq");
	if (!err) err = nmk_gpio_set_mode(gpio_nr, NMK_GPIO_ALT_GPIO);
	if (!err) err = gpio_direction_input(gpio_nr);
	if (err)
		pr_err("Error %i in %s\n", err, __func__);
	return err;
}
device_initcall(nhk8815_eth_init);

static struct platform_device *nhk8815_platform_devices[] __initdata = {
	&nhk8815_nand_device,
	&nhk8815_onenand_device,
	&nhk8815_eth_device,
	/* will add more devices */
};

static void __init nomadik_timer_init(void)
{
	u32 src_cr;

	/* Configure timer sources in "system reset controller" ctrl reg */
	src_cr = readl(io_p2v(NOMADIK_SRC_BASE));
	src_cr &= SRC_CR_INIT_MASK;
	src_cr |= SRC_CR_INIT_VAL;
	writel(src_cr, io_p2v(NOMADIK_SRC_BASE));

	nmdk_timer_init(io_p2v(NOMADIK_MTU0_BASE));
}

static struct sys_timer nomadik_timer = {
	.init	= nomadik_timer_init,
};

static struct i2c_board_info __initdata nhk8815_i2c0_devices[] = {
	{
		I2C_BOARD_INFO("stw4811", 0x2d),
	},
};

static struct i2c_board_info __initdata nhk8815_i2c1_devices[] = {
	{
		I2C_BOARD_INFO("camera", 0x10),
	},
	{
		I2C_BOARD_INFO("stw5095", 0x1a),
	},
	{
		I2C_BOARD_INFO("lis3lv02dl", 0x1d),
	},
};

static struct i2c_board_info __initdata nhk8815_i2c2_devices[] = {
	{
		I2C_BOARD_INFO("stw4811-usb", 0x2d),
	},
};

static unsigned long out_low[] = { PIN_OUTPUT_LOW };
static unsigned long out_high[] = { PIN_OUTPUT_HIGH };
static unsigned long in_nopull[] = { PIN_INPUT_NOPULL };
static unsigned long in_pullup[] = { PIN_INPUT_PULLUP };

static struct pinctrl_map __initdata nhk8815_pinmap[] = {
	PIN_MAP_MUX_GROUP_DEFAULT("uart0", "pinctrl-stn8815", "u0_a_1", "u0"),
	PIN_MAP_MUX_GROUP_DEFAULT("uart1", "pinctrl-stn8815", "u1_a_1", "u1"),
	/* Hog in MMC/SD card mux */
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("pinctrl-stn8815", "mmcsd_a_1", "mmcsd"),
	/* MCCLK */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO8_B10", out_low),
	/* MCCMD */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO9_A10", in_pullup),
	/* MCCMDDIR */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO10_C11", out_high),
	/* MCDAT3-0 */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO11_B11", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO12_A11", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO13_C12", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO14_B12", in_pullup),
	/* MCDAT0DIR */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO15_A12", out_high),
	/* MCDAT31DIR */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO16_C13", out_high),
	/* MCMSFBCLK */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO24_C15", in_pullup),
	/* CD input GPIO */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO111_H21", in_nopull),
	/* CD bias drive */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO112_J21", out_low),
};

static void __init nhk8815_platform_init(void)
{
	pinctrl_register_mappings(nhk8815_pinmap, ARRAY_SIZE(nhk8815_pinmap));
	cpu8815_platform_init();
	nhk8815_onenand_init();
	platform_add_devices(nhk8815_platform_devices,
			     ARRAY_SIZE(nhk8815_platform_devices));

	amba_apb_device_add(NULL, "uart0", NOMADIK_UART0_BASE, SZ_4K, IRQ_UART0, 0, NULL, 0);
	amba_apb_device_add(NULL, "uart1", NOMADIK_UART1_BASE, SZ_4K, IRQ_UART1, 0, NULL, 0);

	i2c_register_board_info(0, nhk8815_i2c0_devices,
				ARRAY_SIZE(nhk8815_i2c0_devices));
	i2c_register_board_info(1, nhk8815_i2c1_devices,
				ARRAY_SIZE(nhk8815_i2c1_devices));
	i2c_register_board_info(2, nhk8815_i2c2_devices,
				ARRAY_SIZE(nhk8815_i2c2_devices));
}

MACHINE_START(NOMADIK, "NHK8815")
	/* Maintainer: ST MicroElectronics */
	.atag_offset	= 0x100,
	.map_io		= cpu8815_map_io,
	.init_irq	= cpu8815_init_irq,
	.handle_irq	= vic_handle_irq,
	.timer		= &nomadik_timer,
	.init_machine	= nhk8815_platform_init,
	.restart	= cpu8815_restart,
MACHINE_END
