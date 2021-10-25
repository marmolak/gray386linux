/*
 * Copyright 2004-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __MACH_MXS_COMMON_H__
#define __MACH_MXS_COMMON_H__

extern const u32 *mxs_get_ocotp(void);
extern int mxs_reset_block(void __iomem *);
extern void mxs_timer_init(void);
extern void mxs_restart(char, const char *);
extern int mxs_saif_clkmux_select(unsigned int clkmux);

extern int mx23_clocks_init(void);
extern void mx23_map_io(void);

extern int mx28_clocks_init(void);
extern void mx28_map_io(void);

extern void icoll_init_irq(void);
extern void icoll_handle_irq(struct pt_regs *);

#endif /* __MACH_MXS_COMMON_H__ */
