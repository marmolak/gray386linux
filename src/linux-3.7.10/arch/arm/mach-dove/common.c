/*
 * arch/arm/mach-dove/common.c
 *
 * Core functions for Marvell Dove 88AP510 System On Chip
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/clk-provider.h>
#include <linux/ata_platform.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <asm/page.h>
#include <asm/setup.h>
#include <asm/timex.h>
#include <asm/hardware/cache-tauros2.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/mach/pci.h>
#include <mach/dove.h>
#include <mach/pm.h>
#include <mach/bridge-regs.h>
#include <asm/mach/arch.h>
#include <linux/irq.h>
#include <plat/time.h>
#include <linux/platform_data/usb-ehci-orion.h>
#include <plat/irq.h>
#include <plat/common.h>
#include <plat/addr-map.h>
#include "common.h"

/*****************************************************************************
 * I/O Address Mapping
 ****************************************************************************/
static struct map_desc dove_io_desc[] __initdata = {
	{
		.virtual	= (unsigned long) DOVE_SB_REGS_VIRT_BASE,
		.pfn		= __phys_to_pfn(DOVE_SB_REGS_PHYS_BASE),
		.length		= DOVE_SB_REGS_SIZE,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long) DOVE_NB_REGS_VIRT_BASE,
		.pfn		= __phys_to_pfn(DOVE_NB_REGS_PHYS_BASE),
		.length		= DOVE_NB_REGS_SIZE,
		.type		= MT_DEVICE,
	},
};

void __init dove_map_io(void)
{
	iotable_init(dove_io_desc, ARRAY_SIZE(dove_io_desc));
}

/*****************************************************************************
 * CLK tree
 ****************************************************************************/
static int dove_tclk;

static DEFINE_SPINLOCK(gating_lock);
static struct clk *tclk;

static struct clk __init *dove_register_gate(const char *name,
					     const char *parent, u8 bit_idx)
{
	return clk_register_gate(NULL, name, parent, 0,
				 (void __iomem *)CLOCK_GATING_CONTROL,
				 bit_idx, 0, &gating_lock);
}

static void __init dove_clk_init(void)
{
	struct clk *usb0, *usb1, *sata, *pex0, *pex1, *sdio0, *sdio1;
	struct clk *nand, *camera, *i2s0, *i2s1, *crypto, *ac97, *pdma;
	struct clk *xor0, *xor1, *ge, *gephy;

	tclk = clk_register_fixed_rate(NULL, "tclk", NULL, CLK_IS_ROOT,
				       dove_tclk);

	usb0 = dove_register_gate("usb0", "tclk", CLOCK_GATING_BIT_USB0);
	usb1 = dove_register_gate("usb1", "tclk", CLOCK_GATING_BIT_USB1);
	sata = dove_register_gate("sata", "tclk", CLOCK_GATING_BIT_SATA);
	pex0 = dove_register_gate("pex0", "tclk", CLOCK_GATING_BIT_PCIE0);
	pex1 = dove_register_gate("pex1", "tclk", CLOCK_GATING_BIT_PCIE1);
	sdio0 = dove_register_gate("sdio0", "tclk", CLOCK_GATING_BIT_SDIO0);
	sdio1 = dove_register_gate("sdio1", "tclk", CLOCK_GATING_BIT_SDIO1);
	nand = dove_register_gate("nand", "tclk", CLOCK_GATING_BIT_NAND);
	camera = dove_register_gate("camera", "tclk", CLOCK_GATING_BIT_CAMERA);
	i2s0 = dove_register_gate("i2s0", "tclk", CLOCK_GATING_BIT_I2S0);
	i2s1 = dove_register_gate("i2s1", "tclk", CLOCK_GATING_BIT_I2S1);
	crypto = dove_register_gate("crypto", "tclk", CLOCK_GATING_BIT_CRYPTO);
	ac97 = dove_register_gate("ac97", "tclk", CLOCK_GATING_BIT_AC97);
	pdma = dove_register_gate("pdma", "tclk", CLOCK_GATING_BIT_PDMA);
	xor0 = dove_register_gate("xor0", "tclk", CLOCK_GATING_BIT_XOR0);
	xor1 = dove_register_gate("xor1", "tclk", CLOCK_GATING_BIT_XOR1);
	gephy = dove_register_gate("gephy", "tclk", CLOCK_GATING_BIT_GIGA_PHY);
	ge = dove_register_gate("ge", "gephy", CLOCK_GATING_BIT_GBE);

	orion_clkdev_add(NULL, "orion_spi.0", tclk);
	orion_clkdev_add(NULL, "orion_spi.1", tclk);
	orion_clkdev_add(NULL, "orion_wdt", tclk);
	orion_clkdev_add(NULL, "mv64xxx_i2c.0", tclk);

	orion_clkdev_add(NULL, "orion-ehci.0", usb0);
	orion_clkdev_add(NULL, "orion-ehci.1", usb1);
	orion_clkdev_add(NULL, "mv643xx_eth_port.0", ge);
	orion_clkdev_add(NULL, "sata_mv.0", sata);
	orion_clkdev_add("0", "pcie", pex0);
	orion_clkdev_add("1", "pcie", pex1);
	orion_clkdev_add(NULL, "sdhci-dove.0", sdio0);
	orion_clkdev_add(NULL, "sdhci-dove.1", sdio1);
	orion_clkdev_add(NULL, "orion_nand", nand);
	orion_clkdev_add(NULL, "cafe1000-ccic.0", camera);
	orion_clkdev_add(NULL, "kirkwood-i2s.0", i2s0);
	orion_clkdev_add(NULL, "kirkwood-i2s.1", i2s1);
	orion_clkdev_add(NULL, "mv_crypto", crypto);
	orion_clkdev_add(NULL, "dove-ac97", ac97);
	orion_clkdev_add(NULL, "dove-pdma", pdma);
	orion_clkdev_add(NULL, "mv_xor_shared.0", xor0);
	orion_clkdev_add(NULL, "mv_xor_shared.1", xor1);
}

/*****************************************************************************
 * EHCI0
 ****************************************************************************/
void __init dove_ehci0_init(void)
{
	orion_ehci_init(DOVE_USB0_PHYS_BASE, IRQ_DOVE_USB0, EHCI_PHY_NA);
}

/*****************************************************************************
 * EHCI1
 ****************************************************************************/
void __init dove_ehci1_init(void)
{
	orion_ehci_1_init(DOVE_USB1_PHYS_BASE, IRQ_DOVE_USB1);
}

/*****************************************************************************
 * GE00
 ****************************************************************************/
void __init dove_ge00_init(struct mv643xx_eth_platform_data *eth_data)
{
	orion_ge00_init(eth_data, DOVE_GE00_PHYS_BASE,
			IRQ_DOVE_GE00_SUM, IRQ_DOVE_GE00_ERR,
			1600);
}

/*****************************************************************************
 * SoC RTC
 ****************************************************************************/
void __init dove_rtc_init(void)
{
	orion_rtc_init(DOVE_RTC_PHYS_BASE, IRQ_DOVE_RTC);
}

/*****************************************************************************
 * SATA
 ****************************************************************************/
void __init dove_sata_init(struct mv_sata_platform_data *sata_data)
{
	orion_sata_init(sata_data, DOVE_SATA_PHYS_BASE, IRQ_DOVE_SATA);

}

/*****************************************************************************
 * UART0
 ****************************************************************************/
void __init dove_uart0_init(void)
{
	orion_uart0_init(DOVE_UART0_VIRT_BASE, DOVE_UART0_PHYS_BASE,
			 IRQ_DOVE_UART_0, tclk);
}

/*****************************************************************************
 * UART1
 ****************************************************************************/
void __init dove_uart1_init(void)
{
	orion_uart1_init(DOVE_UART1_VIRT_BASE, DOVE_UART1_PHYS_BASE,
			 IRQ_DOVE_UART_1, tclk);
}

/*****************************************************************************
 * UART2
 ****************************************************************************/
void __init dove_uart2_init(void)
{
	orion_uart2_init(DOVE_UART2_VIRT_BASE, DOVE_UART2_PHYS_BASE,
			 IRQ_DOVE_UART_2, tclk);
}

/*****************************************************************************
 * UART3
 ****************************************************************************/
void __init dove_uart3_init(void)
{
	orion_uart3_init(DOVE_UART3_VIRT_BASE, DOVE_UART3_PHYS_BASE,
			 IRQ_DOVE_UART_3, tclk);
}

/*****************************************************************************
 * SPI
 ****************************************************************************/
void __init dove_spi0_init(void)
{
	orion_spi_init(DOVE_SPI0_PHYS_BASE);
}

void __init dove_spi1_init(void)
{
	orion_spi_1_init(DOVE_SPI1_PHYS_BASE);
}

/*****************************************************************************
 * I2C
 ****************************************************************************/
void __init dove_i2c_init(void)
{
	orion_i2c_init(DOVE_I2C_PHYS_BASE, IRQ_DOVE_I2C, 10);
}

/*****************************************************************************
 * Time handling
 ****************************************************************************/
void __init dove_init_early(void)
{
	orion_time_set_base(TIMER_VIRT_BASE);
}

static int __init dove_find_tclk(void)
{
	return 166666667;
}

static void __init dove_timer_init(void)
{
	dove_tclk = dove_find_tclk();
	orion_time_init(BRIDGE_VIRT_BASE, BRIDGE_INT_TIMER1_CLR,
			IRQ_DOVE_BRIDGE, dove_tclk);
}

struct sys_timer dove_timer = {
	.init = dove_timer_init,
};

/*****************************************************************************
 * Cryptographic Engines and Security Accelerator (CESA)
 ****************************************************************************/
void __init dove_crypto_init(void)
{
	orion_crypto_init(DOVE_CRYPT_PHYS_BASE, DOVE_CESA_PHYS_BASE,
			  DOVE_CESA_SIZE, IRQ_DOVE_CRYPTO);
}

/*****************************************************************************
 * XOR 0
 ****************************************************************************/
void __init dove_xor0_init(void)
{
	orion_xor0_init(DOVE_XOR0_PHYS_BASE, DOVE_XOR0_HIGH_PHYS_BASE,
			IRQ_DOVE_XOR_00, IRQ_DOVE_XOR_01);
}

/*****************************************************************************
 * XOR 1
 ****************************************************************************/
void __init dove_xor1_init(void)
{
	orion_xor1_init(DOVE_XOR1_PHYS_BASE, DOVE_XOR1_HIGH_PHYS_BASE,
			IRQ_DOVE_XOR_10, IRQ_DOVE_XOR_11);
}

/*****************************************************************************
 * SDIO
 ****************************************************************************/
static u64 sdio_dmamask = DMA_BIT_MASK(32);

static struct resource dove_sdio0_resources[] = {
	{
		.start	= DOVE_SDIO0_PHYS_BASE,
		.end	= DOVE_SDIO0_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= IRQ_DOVE_SDIO0,
		.end	= IRQ_DOVE_SDIO0,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device dove_sdio0 = {
	.name		= "sdhci-dove",
	.id		= 0,
	.dev		= {
		.dma_mask		= &sdio_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= dove_sdio0_resources,
	.num_resources	= ARRAY_SIZE(dove_sdio0_resources),
};

void __init dove_sdio0_init(void)
{
	platform_device_register(&dove_sdio0);
}

static struct resource dove_sdio1_resources[] = {
	{
		.start	= DOVE_SDIO1_PHYS_BASE,
		.end	= DOVE_SDIO1_PHYS_BASE + 0xff,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= IRQ_DOVE_SDIO1,
		.end	= IRQ_DOVE_SDIO1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device dove_sdio1 = {
	.name		= "sdhci-dove",
	.id		= 1,
	.dev		= {
		.dma_mask		= &sdio_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= dove_sdio1_resources,
	.num_resources	= ARRAY_SIZE(dove_sdio1_resources),
};

void __init dove_sdio1_init(void)
{
	platform_device_register(&dove_sdio1);
}

void __init dove_init(void)
{
	pr_info("Dove 88AP510 SoC, TCLK = %d MHz.\n",
		(dove_tclk + 499999) / 1000000);

#ifdef CONFIG_CACHE_TAUROS2
	tauros2_init(0);
#endif
	dove_setup_cpu_mbus();

	/* Setup root of clk tree */
	dove_clk_init();

	/* internal devices that every board has */
	dove_rtc_init();
	dove_xor0_init();
	dove_xor1_init();
}

void dove_restart(char mode, const char *cmd)
{
	/*
	 * Enable soft reset to assert RSTOUTn.
	 */
	writel(SOFT_RESET_OUT_EN, RSTOUTn_MASK);

	/*
	 * Assert soft reset.
	 */
	writel(SOFT_RESET, SYSTEM_SOFT_RESET);

	while (1)
		;
}

#if defined(CONFIG_MACH_DOVE_DT)
/*
 * Auxdata required until real OF clock provider
 */
struct of_dev_auxdata dove_auxdata_lookup[] __initdata = {
	OF_DEV_AUXDATA("marvell,orion-spi", 0xf1010600, "orion_spi.0", NULL),
	OF_DEV_AUXDATA("marvell,orion-spi", 0xf1014600, "orion_spi.1", NULL),
	OF_DEV_AUXDATA("marvell,orion-wdt", 0xf1020300, "orion_wdt", NULL),
	OF_DEV_AUXDATA("marvell,mv64xxx-i2c", 0xf1011000, "mv64xxx_i2c.0",
		       NULL),
	OF_DEV_AUXDATA("marvell,orion-sata", 0xf10a0000, "sata_mv.0", NULL),
	OF_DEV_AUXDATA("marvell,dove-sdhci", 0xf1092000, "sdhci-dove.0", NULL),
	OF_DEV_AUXDATA("marvell,dove-sdhci", 0xf1090000, "sdhci-dove.1", NULL),
	{},
};

static struct mv643xx_eth_platform_data dove_dt_ge00_data = {
	.phy_addr = MV643XX_ETH_PHY_ADDR_DEFAULT,
};

static void __init dove_dt_init(void)
{
	pr_info("Dove 88AP510 SoC, TCLK = %d MHz.\n",
		(dove_tclk + 499999) / 1000000);

#ifdef CONFIG_CACHE_TAUROS2
	tauros2_init(0);
#endif
	dove_setup_cpu_mbus();

	/* Setup root of clk tree */
	dove_clk_init();

	/* Internal devices not ported to DT yet */
	dove_rtc_init();
	dove_xor0_init();
	dove_xor1_init();

	dove_ge00_init(&dove_dt_ge00_data);
	dove_ehci0_init();
	dove_ehci1_init();
	dove_pcie_init(1, 1);

	of_platform_populate(NULL, of_default_bus_match_table,
			     dove_auxdata_lookup, NULL);
}

static const char * const dove_dt_board_compat[] = {
	"marvell,dove",
	NULL
};

DT_MACHINE_START(DOVE_DT, "Marvell Dove (Flattened Device Tree)")
	.map_io		= dove_map_io,
	.init_early	= dove_init_early,
	.init_irq	= orion_dt_init_irq,
	.timer		= &dove_timer,
	.init_machine	= dove_dt_init,
	.restart	= dove_restart,
	.dt_compat	= dove_dt_board_compat,
MACHINE_END
#endif
