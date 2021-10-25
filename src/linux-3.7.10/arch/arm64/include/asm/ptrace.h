/*
 * Based on arch/arm/include/asm/ptrace.h
 *
 * Copyright (C) 1996-2003 Russell King
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_PTRACE_H
#define __ASM_PTRACE_H

#include <uapi/asm/ptrace.h>

/* AArch32-specific ptrace requests */
#define COMPAT_PTRACE_GETREGS		12
#define COMPAT_PTRACE_SETREGS		13
#define COMPAT_PTRACE_GET_THREAD_AREA	22
#define COMPAT_PTRACE_SET_SYSCALL	23
#define COMPAT_PTRACE_GETVFPREGS	27
#define COMPAT_PTRACE_SETVFPREGS	28
#define COMPAT_PTRACE_GETHBPREGS	29
#define COMPAT_PTRACE_SETHBPREGS	30
#define COMPAT_PSR_MODE_USR	0x00000010
#define COMPAT_PSR_T_BIT	0x00000020
#define COMPAT_PSR_IT_MASK	0x0600fc00	/* If-Then execution state mask */
/*
 * These are 'magic' values for PTRACE_PEEKUSR that return info about where a
 * process is located in memory.
 */
#define COMPAT_PT_TEXT_ADDR		0x10000
#define COMPAT_PT_DATA_ADDR		0x10004
#define COMPAT_PT_TEXT_END_ADDR		0x10008
#ifndef __ASSEMBLY__

/* sizeof(struct user) for AArch32 */
#define COMPAT_USER_SZ	296
/* AArch32 uses x13 as the stack pointer... */
#define compat_sp	regs[13]
/* ... and x14 as the link register. */
#define compat_lr	regs[14]

/*
 * This struct defines the way the registers are stored on the stack during an
 * exception. Note that sizeof(struct pt_regs) has to be a multiple of 16 (for
 * stack alignment). struct user_pt_regs must form a prefix of struct pt_regs.
 */
struct pt_regs {
	union {
		struct user_pt_regs user_regs;
		struct {
			u64 regs[31];
			u64 sp;
			u64 pc;
			u64 pstate;
		};
	};
	u64 orig_x0;
	u64 syscallno;
};

#define arch_has_single_step()	(1)

#ifdef CONFIG_COMPAT
#define compat_thumb_mode(regs) \
	(((regs)->pstate & COMPAT_PSR_T_BIT))
#else
#define compat_thumb_mode(regs) (0)
#endif

#define user_mode(regs)	\
	(((regs)->pstate & PSR_MODE_MASK) == PSR_MODE_EL0t)

#define compat_user_mode(regs)	\
	(((regs)->pstate & (PSR_MODE32_BIT | PSR_MODE_MASK)) == \
	 (PSR_MODE32_BIT | PSR_MODE_EL0t))

#define processor_mode(regs) \
	((regs)->pstate & PSR_MODE_MASK)

#define interrupts_enabled(regs) \
	(!((regs)->pstate & PSR_I_BIT))

#define fast_interrupts_enabled(regs) \
	(!((regs)->pstate & PSR_F_BIT))

#define user_stack_pointer(regs) \
	((regs)->sp)

/*
 * Are the current registers suitable for user mode? (used to maintain
 * security in signal handlers)
 */
static inline int valid_user_regs(struct user_pt_regs *regs)
{
	if (user_mode(regs) && (regs->pstate & PSR_I_BIT) == 0) {
		regs->pstate &= ~(PSR_F_BIT | PSR_A_BIT);

		/* The T bit is reserved for AArch64 */
		if (!(regs->pstate & PSR_MODE32_BIT))
			regs->pstate &= ~COMPAT_PSR_T_BIT;

		return 1;
	}

	/*
	 * Force PSR to something logical...
	 */
	regs->pstate &= PSR_f | PSR_s | (PSR_x & ~PSR_A_BIT) | \
			COMPAT_PSR_T_BIT | PSR_MODE32_BIT;

	if (!(regs->pstate & PSR_MODE32_BIT)) {
		regs->pstate &= ~COMPAT_PSR_T_BIT;
		regs->pstate |= PSR_MODE_EL0t;
	}

	return 0;
}

#define instruction_pointer(regs)	(regs)->pc

#ifdef CONFIG_SMP
extern unsigned long profile_pc(struct pt_regs *regs);
#else
#define profile_pc(regs) instruction_pointer(regs)
#endif

extern int aarch32_break_trap(struct pt_regs *regs);

#endif /* __ASSEMBLY__ */
#endif
