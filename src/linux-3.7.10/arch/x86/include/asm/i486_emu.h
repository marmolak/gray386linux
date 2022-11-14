#ifndef _ASM_X86_I486_EMU_H
#define _ASM_X86_I486_EMU_H

#ifndef __ASSEMBLY__

#include <linux/ptrace.h>

extern int parse_emu_instruction(struct pt_regs *regs);

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_I486_EMU_H */
