#ifndef __NVBIOS_GPIO_H__
#define __NVBIOS_GPIO_H__

struct nouveau_bios;

enum dcb_gpio_func_name {
	DCB_GPIO_PANEL_POWER = 0x01,
	DCB_GPIO_TVDAC0 = 0x0c,
	DCB_GPIO_TVDAC1 = 0x2d,
	DCB_GPIO_PWM_FAN = 0x09,
	DCB_GPIO_FAN_SENSE = 0x3d,
	DCB_GPIO_UNUSED = 0xff
};

struct dcb_gpio_func {
	u8 func;
	u8 line;
	u8 log[2];

	/* so far, "param" seems to only have an influence on PWM-related
	 * GPIOs such as FAN_CONTROL and PANEL_BACKLIGHT_LEVEL.
	 * if param equals 1, hardware PWM is available
	 * if param equals 0, the host should toggle the GPIO itself
	 */
	u8 param;
};

u16 dcb_gpio_table(struct nouveau_bios *);
u16 dcb_gpio_entry(struct nouveau_bios *, int idx, int ent, u8 *ver);
int dcb_gpio_parse(struct nouveau_bios *, int idx, u8 func, u8 line,
		   struct dcb_gpio_func *);

#endif
