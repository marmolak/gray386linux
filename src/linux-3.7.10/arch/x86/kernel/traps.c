/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 *
 *  Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 */

/*
 * Handle hardware traps and faults.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/interrupt.h>
#include <linux/kallsyms.h>
#include <linux/spinlock.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/kdebug.h>
#include <linux/kgdb.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/kexec.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/bug.h>
#include <linux/nmi.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/io.h>

#ifdef CONFIG_EISA
#include <linux/ioport.h>
#include <linux/eisa.h>
#endif

#if defined(CONFIG_EDAC)
#include <linux/edac.h>
#endif

#include <asm/kmemcheck.h>
#include <asm/stacktrace.h>
#include <asm/processor.h>
#include <asm/debugreg.h>
#include <linux/atomic.h>
#include <asm/ftrace.h>
#include <asm/traps.h>
#include <asm/desc.h>
#include <asm/i387.h>
#include <asm/fpu-internal.h>
#include <asm/mce.h>
#include <asm/rcu.h>

#include <asm/mach_traps.h>

#ifdef CONFIG_X86_64
#include <asm/x86_init.h>
#include <asm/pgalloc.h>
#include <asm/proto.h>
#else
#include <asm/processor-flags.h>
#include <asm/setup.h>

asmlinkage int system_call(void);

/* Do we ignore FPU interrupts ? */
char ignore_fpu_irq;

/*
 * The IDT has to be page-aligned to simplify the Pentium
 * F0 0F bug workaround.
 */
gate_desc idt_table[NR_VECTORS] __page_aligned_data = { { { { 0, 0 } } }, };
#endif

DECLARE_BITMAP(used_vectors, NR_VECTORS);
EXPORT_SYMBOL_GPL(used_vectors);

static inline void conditional_sti(struct pt_regs *regs)
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_enable();
}

static inline void preempt_conditional_sti(struct pt_regs *regs)
{
	inc_preempt_count();
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_enable();
}

static inline void conditional_cli(struct pt_regs *regs)
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_disable();
}

static inline void preempt_conditional_cli(struct pt_regs *regs)
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_disable();
	dec_preempt_count();
}

static int __kprobes
do_trap_no_signal(struct task_struct *tsk, int trapnr, char *str,
		  struct pt_regs *regs,	long error_code)
{
#ifdef CONFIG_X86_32
	if (regs->flags & X86_VM_MASK) {
		/*
		 * Traps 0, 1, 3, 4, and 5 should be forwarded to vm86.
		 * On nmi (interrupt 2), do_trap should not be called.
		 */
		if (trapnr < X86_TRAP_UD) {
			if (!handle_vm86_trap((struct kernel_vm86_regs *) regs,
						error_code, trapnr))
				return 0;
		}
		return -1;
	}
#endif
	if (!user_mode(regs)) {
		if (!fixup_exception(regs)) {
			tsk->thread.error_code = error_code;
			tsk->thread.trap_nr = trapnr;
			die(str, regs, error_code);
		}
		return 0;
	}

	return -1;
}

static void __kprobes
do_trap(int trapnr, int signr, char *str, struct pt_regs *regs,
	long error_code, siginfo_t *info)
{
	struct task_struct *tsk = current;


	if (!do_trap_no_signal(tsk, trapnr, str, regs, error_code))
		return;
	/*
	 * We want error_code and trap_nr set for userspace faults and
	 * kernelspace faults which result in die(), but not
	 * kernelspace faults which are fixed up.  die() gives the
	 * process no chance to handle the signal and notice the
	 * kernel fault information, so that won't result in polluting
	 * the information about previously queued, but not yet
	 * delivered, faults.  See also do_general_protection below.
	 */
	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = trapnr;

#ifdef CONFIG_X86_64
	if (show_unhandled_signals && unhandled_signal(tsk, signr) &&
	    printk_ratelimit()) {
		pr_info("%s[%d] trap %s ip:%lx sp:%lx error:%lx",
			tsk->comm, tsk->pid, str,
			regs->ip, regs->sp, error_code);
		print_vma_addr(" in ", regs->ip);
		pr_cont("\n");
	}
#endif

	if (info)
		force_sig_info(signr, info, tsk);
	else
		force_sig(signr, tsk);
}

#define DO_ERROR(trapnr, signr, str, name)				\
dotraplinkage void do_##name(struct pt_regs *regs, long error_code)	\
{									\
	exception_enter(regs);						\
	if (notify_die(DIE_TRAP, str, regs, error_code,			\
			trapnr, signr) == NOTIFY_STOP) {		\
		exception_exit(regs);					\
		return;							\
	}								\
	conditional_sti(regs);						\
	do_trap(trapnr, signr, str, regs, error_code, NULL);		\
	exception_exit(regs);						\
}

#define DO_ERROR_INFO(trapnr, signr, str, name, sicode, siaddr)		\
dotraplinkage void do_##name(struct pt_regs *regs, long error_code)	\
{									\
	siginfo_t info;							\
	info.si_signo = signr;						\
	info.si_errno = 0;						\
	info.si_code = sicode;						\
	info.si_addr = (void __user *)siaddr;				\
	exception_enter(regs);						\
	if (notify_die(DIE_TRAP, str, regs, error_code,			\
			trapnr, signr) == NOTIFY_STOP) {		\
		exception_exit(regs);					\
		return;							\
	}								\
	conditional_sti(regs);						\
	do_trap(trapnr, signr, str, regs, error_code, &info);		\
	exception_exit(regs);						\
}

DO_ERROR_INFO(X86_TRAP_DE, SIGFPE, "divide error", divide_error, FPE_INTDIV,
		regs->ip)
DO_ERROR(X86_TRAP_OF, SIGSEGV, "overflow", overflow)
DO_ERROR(X86_TRAP_BR, SIGSEGV, "bounds", bounds)

/* f0 0f b1 b3 64 1d 00 00 lock cmpxchg DWORD PTR [reg_dest+0x00001d64],reg_src */
#define emu_cmpxchg_32bits_dst_addr(byte, reg_dest, reg_src)			\
if (insts == (byte)) {								\
	insts >>= 8;								\
	++cmpxchg_inst_size;							\
	if (cmpxchg_inst_size == 4) {						\
		if (get_user(insts, (u32 __user *)(regs->ip + cmpxchg_inst_size)) \
		    == -EFAULT)	{						\
			printk("Unable to get instructions to handle. "		\
			"Falling thru to default action.");			\
			local_irq_restore(flags);				\
			goto do_default_trap;					\
		}								\
		cmpxchg_inst_size += sizeof(u32); 				\
	} else {								\
		if (get_user(tmp_insts, (u32 __user *)(regs->ip + cmpxchg_inst_size)) \
				== -EFAULT) {					\
			printk("Unable to get instructions to handle. "		\
			"Falling thru to default action.");			\
			local_irq_restore(flags);				\
			goto do_default_trap;					\
		}								\
		insts = (tmp_insts << 8) | (insts & 0xff);			\
		cmpxchg_inst_size += sizeof(u32) - 1; 				\
	}									\
										\
	cmpxchg_dest = (u32 __user *)(regs->reg_dest + insts);			\
	if (!access_ok(VERIFY_WRITE, cmpxchg_dest, sizeof(*cmpxchg_dest))) {	\
		printk("Unable to read user memory. Doing default trap.\n"	\
			"Address:%lx reg_dest:%lx reg_src:%lx\n",		\
			(u32) cmpxchg_dest,					\
			regs->reg_dest,						\
			regs->reg_src); 					\
		local_irq_restore(flags);					\
		goto do_default_trap;						\
	}									\
										\
	if (regs->ax == *cmpxchg_dest) {					\
		*cmpxchg_dest = regs->reg_src;					\
		regs->flags |= X86_EFLAGS_ZF;					\
	} else {								\
		regs->ax = *cmpxchg_dest;					\
		regs->flags &= ~X86_EFLAGS_ZF;					\
	}									\
										\
	goto finish_cmpxchg;							\
}										\

/* f0 0f b1 0b (regs) lock cmpxchg DWORD PTR [reg_dest],reg_src */
#define emu_cmpxchg_src_dst_regs(byte, reg_dest, reg_src) 			\
if (insts == (byte)) {								\
	/* No add involved so skip byte */ 					\
	++cmpxchg_inst_size;							\
										\
	cmpxchg_dest = (u32 __user *)regs->reg_dest;				\
	if (!access_ok(VERIFY_WRITE, cmpxchg_dest, sizeof(*cmpxchg_dest))) {	\
		printk("Unable to read user memory. Doing default trap.\n"	\
			"Address:%lx reg_dest:%lx reg_src:%lx\n",		\
			(u32) cmpxchg_dest,					\
			regs->reg_dest,						\
			regs->reg_src); 					\
		local_irq_restore(flags);					\
		goto do_default_trap;						\
	}									\
										\
	if (regs->ax == *cmpxchg_dest) {					\
		*cmpxchg_dest = regs->reg_src;					\
		regs->flags |= X86_EFLAGS_ZF;					\
	} else {								\
		regs->ax = *cmpxchg_dest;					\
		regs->flags &= ~X86_EFLAGS_ZF;					\
	}									\
	goto finish_cmpxchg;							\
}										\

/* f0 0f b1 4a (byte) 04 (+addr) lock cmpxchg DWORD PTR [reg_dest+0x4],reg_src */
#define emu_cmpxchg_8bits_dst_addr(byte, reg_dest, reg_src) 			\
if (insts == (byte)) {								\
	insts >>= 8;								\
	++cmpxchg_inst_size;							\
	if (cmpxchg_inst_size == 4) {						\
		if (get_user(insts, (u32 __user *)(regs->ip + cmpxchg_inst_size)) \
		    == -EFAULT) {						\
			printk("Unable to get instructions to handle. "		\
			"Falling thru to default action.");			\
			goto do_default_trap;					\
		}								\
	}									\
	++cmpxchg_inst_size;							\
										\
	/* One byte address. */							\
	insts &= (u32)0xff;							\
										\
	cmpxchg_dest = (u32 __user *)(regs->reg_dest + insts);			\
	if (!access_ok(VERIFY_WRITE, cmpxchg_dest, sizeof(*cmpxchg_dest))) {	\
		printk("Unable to read user memory. Doing default trap.\n"	\
			"Address:%lx reg_dest:%lx reg_src:%lx\n",		\
			(u32) cmpxchg_dest,					\
			regs->reg_dest,						\
			regs->reg_src); 					\
		local_irq_restore(flags);					\
		goto do_default_trap;						\
	}									\
										\
	if (regs->ax == *cmpxchg_dest) {					\
		*cmpxchg_dest = regs->reg_src;					\
		regs->flags |= X86_EFLAGS_ZF;					\
	} else {								\
		regs->ax = *cmpxchg_dest;					\
		regs->flags &= ~X86_EFLAGS_ZF;					\
	}									\
	goto finish_cmpxchg;							\
}										\

#define emu_cmpxchg_32bits_dest_addr_only(byte, reg_src) 			\
if (insts == (byte)) {								\
	insts >>= 8;								\
	++cmpxchg_inst_size;							\
	if (cmpxchg_inst_size == 4) {						\
		if (get_user(insts, (u32 __user *)(regs->ip + cmpxchg_inst_size)) \
		    == -EFAULT) { 						\
			printk("Unable to get instructions to handle. "		\
			"Falling thru to default action.");			\
			local_irq_restore(flags);				\
			goto do_default_trap;					\
		}								\
		cmpxchg_inst_size += sizeof(u32); 				\
	} else {								\
		if (get_user(tmp_insts, (u32 __user *)(regs->ip + cmpxchg_inst_size)) \
		    == -EFAULT) {						\
			printk("Unable to get instructions to handle. "		\
			"Falling thru to default action.");			\
			local_irq_restore(flags);				\
			goto do_default_trap;					\
		}								\
		/* should be less than 4 bytes but ... */			\
		insts = (tmp_insts << 8) | (insts & 0xff);			\
		cmpxchg_inst_size += sizeof(u32) - 1; 				\
	}									\
										\
	cmpxchg_dest = (u32 __user *)insts;					\
	if (!access_ok(VERIFY_WRITE, cmpxchg_dest, sizeof(*cmpxchg_dest))) {	\
		printk("Unable to read user memory. Doing default trap.\n"	\
			"Address:%lx reg_src:%lx\n",				\
			(u32) cmpxchg_dest,					\
			regs->reg_src); 					\
		local_irq_restore(flags);					\
		goto do_default_trap;						\
	}									\
										\
	if (regs->ax == *cmpxchg_dest) {					\
		*cmpxchg_dest = regs->reg_src;					\
		regs->flags |= X86_EFLAGS_ZF;					\
	} else {								\
		regs->ax = *cmpxchg_dest;					\
		regs->flags &= ~X86_EFLAGS_ZF;					\
	}									\
	goto finish_cmpxchg;							\
}										\

dotraplinkage void do_invalid_op(struct pt_regs *regs, long error_code)
{
	u32 cmpxchg_inst_size;
	u32 *cmpxchg_dest;
	u32 insts;
	u32 tmp_insts;
	unsigned long flags;

	siginfo_t info;
	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_ILLOPN;
	info.si_addr = (void __user *)regs->ip;

	if (get_user(insts, (u32 __user *)regs->ip) == -EFAULT)
	{
		printk("Unable to get instructions to handle. "
		       "Falling thru to default action.");
	} else if (
		/* check for cmpxchg or lock cmpxchg -
		 * only 32 bit is supported
		 */
		(((insts & 0xb10fu) == 0xb10fu)
		|| ((insts & 0xb10ff0u) == 0xb10ff0u))
	){
		/* Decode cmpxchg instruction. */
		cmpxchg_inst_size = 2;

		/* Skip prefix if present. */
		if ((insts & 0xffu) == 0xf0u) {
			++cmpxchg_inst_size;
			insts >>= 8;
		}
		/* Get rid of 2 byte opcode. */
		insts >>= 16;

		local_irq_save(flags);

		emu_cmpxchg_src_dst_regs(0x00u, ax, ax)
		emu_cmpxchg_src_dst_regs(0x01u, cx, ax)
		emu_cmpxchg_src_dst_regs(0x02u, dx, ax)
		emu_cmpxchg_src_dst_regs(0x03u, bx, ax)
		// SIB not supported now
		emu_cmpxchg_32bits_dest_addr_only(0x05, ax)
		emu_cmpxchg_src_dst_regs(0x06u, si, ax)
		emu_cmpxchg_src_dst_regs(0x07u, di, ax)

		emu_cmpxchg_src_dst_regs(0x08u, ax, cx)
		emu_cmpxchg_src_dst_regs(0x09u, cx, cx)
		emu_cmpxchg_src_dst_regs(0x0au, dx, cx)
		emu_cmpxchg_src_dst_regs(0x0bu, bx, cx)
		// SIB not supported now
		emu_cmpxchg_32bits_dest_addr_only(0x0d, cx)
		emu_cmpxchg_src_dst_regs(0x0eu, si, cx)
		emu_cmpxchg_src_dst_regs(0x0fu, di, cx)

		emu_cmpxchg_src_dst_regs(0x11u, cx, dx)
		emu_cmpxchg_src_dst_regs(0x13u, bx, dx)
		emu_cmpxchg_src_dst_regs(0x16u, si, dx)
		emu_cmpxchg_src_dst_regs(0x1eu, si, bx)
		emu_cmpxchg_src_dst_regs(0x2bu, bx, bp)
		emu_cmpxchg_src_dst_regs(0x2eu, si, bp)
		emu_cmpxchg_src_dst_regs(0x33u, bx, si)

		emu_cmpxchg_8bits_dst_addr(0x40u, ax, ax)
		emu_cmpxchg_8bits_dst_addr(0x41u, cx, ax)
		emu_cmpxchg_8bits_dst_addr(0x42u, dx, ax)
		emu_cmpxchg_8bits_dst_addr(0x43u, bx, ax)
		// SIB not supported now
		emu_cmpxchg_8bits_dst_addr(0x45u, bp, ax)
		emu_cmpxchg_8bits_dst_addr(0x46u, si, ax)
		emu_cmpxchg_8bits_dst_addr(0x47u, di, ax)

		emu_cmpxchg_8bits_dst_addr(0x48u, ax, cx)
		emu_cmpxchg_8bits_dst_addr(0x49u, cx, cx)
		emu_cmpxchg_8bits_dst_addr(0x4au, dx, cx)
		emu_cmpxchg_8bits_dst_addr(0x4bu, bx, cx)
		// SIB not supported now
		emu_cmpxchg_8bits_dst_addr(0x4du, bp, cx)
		emu_cmpxchg_8bits_dst_addr(0x4eu, si, cx)
		emu_cmpxchg_8bits_dst_addr(0x4fu, di, cx)

		emu_cmpxchg_8bits_dst_addr(0x50u, ax, dx)
		emu_cmpxchg_8bits_dst_addr(0x51u, cx, dx)
		emu_cmpxchg_8bits_dst_addr(0x52u, dx, dx)
		emu_cmpxchg_8bits_dst_addr(0x53u, bx, dx)
		// SIB not supported now
		emu_cmpxchg_8bits_dst_addr(0x55u, bp, dx)
		emu_cmpxchg_8bits_dst_addr(0x56u, si, dx)
		emu_cmpxchg_8bits_dst_addr(0x57u, di, dx)

		emu_cmpxchg_8bits_dst_addr(0x58u, ax, bx)
		emu_cmpxchg_8bits_dst_addr(0x59u, cx, bx)
		emu_cmpxchg_8bits_dst_addr(0x5au, dx, bx)
		emu_cmpxchg_8bits_dst_addr(0x5bu, bx, bx)
		// SIB not supported now
		emu_cmpxchg_8bits_dst_addr(0x5du, bp, bx)
		emu_cmpxchg_8bits_dst_addr(0x5eu, si, bx)
		emu_cmpxchg_8bits_dst_addr(0x5fu, di, bx)

		emu_cmpxchg_8bits_dst_addr(0x6au, dx, bp)
		emu_cmpxchg_8bits_dst_addr(0x73u, bx, si)
		emu_cmpxchg_8bits_dst_addr(0x77u, di, si)

		emu_cmpxchg_32bits_dst_addr(0x8bu, bx, cx)
		emu_cmpxchg_32bits_dst_addr(0x93u, bx, dx)
		emu_cmpxchg_32bits_dst_addr(0xabu, bx, bp)
		emu_cmpxchg_32bits_dst_addr(0xb3u, bx, si)
		emu_cmpxchg_32bits_dst_addr(0xbbu, bx, di)

		emu_cmpxchg_32bits_dest_addr_only(0x15, dx)

finish_cmpxchg:
		/* Finally skip n bytes belongs to instruction. */
		instruction_pointer_set(regs, regs->ip + cmpxchg_inst_size);
		local_irq_restore(flags);
		return;

	} else if (
		/* check for endbr32 */
		insts == 0xfb1e0ff3u
	){
		instruction_pointer_set(regs, regs->ip + sizeof(insts));
		return;
	}

do_default_trap:
	exception_enter(regs);
	if (notify_die(DIE_TRAP, "invalid opcode", regs, error_code,
			X86_TRAP_UD, SIGILL) == NOTIFY_STOP) {
		exception_exit(regs);
		return;
	}
	conditional_sti(regs);
	do_trap(X86_TRAP_UD, SIGILL, "invalid opcode", regs, error_code, &info);
	exception_exit(regs);
}
DO_ERROR(X86_TRAP_OLD_MF, SIGFPE, "coprocessor segment overrun",
		coprocessor_segment_overrun)
DO_ERROR(X86_TRAP_TS, SIGSEGV, "invalid TSS", invalid_TSS)
DO_ERROR(X86_TRAP_NP, SIGBUS, "segment not present", segment_not_present)
#ifdef CONFIG_X86_32
DO_ERROR(X86_TRAP_SS, SIGBUS, "stack segment", stack_segment)
#endif
DO_ERROR_INFO(X86_TRAP_AC, SIGBUS, "alignment check", alignment_check,
		BUS_ADRALN, 0)

#ifdef CONFIG_X86_64
/* Runs on IST stack */
dotraplinkage void do_stack_segment(struct pt_regs *regs, long error_code)
{
	exception_enter(regs);
	if (notify_die(DIE_TRAP, "stack segment", regs, error_code,
		       X86_TRAP_SS, SIGBUS) != NOTIFY_STOP) {
		preempt_conditional_sti(regs);
		do_trap(X86_TRAP_SS, SIGBUS, "stack segment", regs, error_code, NULL);
		preempt_conditional_cli(regs);
	}
	exception_exit(regs);
}

dotraplinkage void do_double_fault(struct pt_regs *regs, long error_code)
{
	static const char str[] = "double fault";
	struct task_struct *tsk = current;

	exception_enter(regs);
	/* Return not checked because double check cannot be ignored */
	notify_die(DIE_TRAP, str, regs, error_code, X86_TRAP_DF, SIGSEGV);

	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = X86_TRAP_DF;

	/*
	 * This is always a kernel trap and never fixable (and thus must
	 * never return).
	 */
	for (;;)
		die(str, regs, error_code);
}
#endif

dotraplinkage void __kprobes
do_general_protection(struct pt_regs *regs, long error_code)
{
	struct task_struct *tsk;

	exception_enter(regs);
	conditional_sti(regs);

#ifdef CONFIG_X86_32
	if (regs->flags & X86_VM_MASK) {
		local_irq_enable();
		handle_vm86_fault((struct kernel_vm86_regs *) regs, error_code);
		goto exit;
	}
#endif

	tsk = current;
	if (!user_mode(regs)) {
		if (fixup_exception(regs))
			goto exit;

		tsk->thread.error_code = error_code;
		tsk->thread.trap_nr = X86_TRAP_GP;
		if (notify_die(DIE_GPF, "general protection fault", regs, error_code,
			       X86_TRAP_GP, SIGSEGV) != NOTIFY_STOP)
			die("general protection fault", regs, error_code);
		goto exit;
	}

	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = X86_TRAP_GP;

	if (show_unhandled_signals && unhandled_signal(tsk, SIGSEGV) &&
			printk_ratelimit()) {
		pr_info("%s[%d] general protection ip:%lx sp:%lx error:%lx",
			tsk->comm, task_pid_nr(tsk),
			regs->ip, regs->sp, error_code);
		print_vma_addr(" in ", regs->ip);
		pr_cont("\n");
	}

	force_sig(SIGSEGV, tsk);
exit:
	exception_exit(regs);
}

/* May run on IST stack. */
dotraplinkage void __kprobes notrace do_int3(struct pt_regs *regs, long error_code)
{
#ifdef CONFIG_DYNAMIC_FTRACE
	/*
	 * ftrace must be first, everything else may cause a recursive crash.
	 * See note by declaration of modifying_ftrace_code in ftrace.c
	 */
	if (unlikely(atomic_read(&modifying_ftrace_code)) &&
	    ftrace_int3_handler(regs))
		return;
#endif
	exception_enter(regs);
#ifdef CONFIG_KGDB_LOW_LEVEL_TRAP
	if (kgdb_ll_trap(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
				SIGTRAP) == NOTIFY_STOP)
		goto exit;
#endif /* CONFIG_KGDB_LOW_LEVEL_TRAP */

	if (notify_die(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
			SIGTRAP) == NOTIFY_STOP)
		goto exit;

	/*
	 * Let others (NMI) know that the debug stack is in use
	 * as we may switch to the interrupt stack.
	 */
	debug_stack_usage_inc();
	preempt_conditional_sti(regs);
	do_trap(X86_TRAP_BP, SIGTRAP, "int3", regs, error_code, NULL);
	preempt_conditional_cli(regs);
	debug_stack_usage_dec();
exit:
	exception_exit(regs);
}

#ifdef CONFIG_X86_64
/*
 * Help handler running on IST stack to switch back to user stack
 * for scheduling or signal handling. The actual stack switch is done in
 * entry.S
 */
asmlinkage __kprobes struct pt_regs *sync_regs(struct pt_regs *eregs)
{
	struct pt_regs *regs = eregs;
	/* Did already sync */
	if (eregs == (struct pt_regs *)eregs->sp)
		;
	/* Exception from user space */
	else if (user_mode(eregs))
		regs = task_pt_regs(current);
	/*
	 * Exception from kernel and interrupts are enabled. Move to
	 * kernel process stack.
	 */
	else if (eregs->flags & X86_EFLAGS_IF)
		regs = (struct pt_regs *)(eregs->sp -= sizeof(struct pt_regs));
	if (eregs != regs)
		*regs = *eregs;
	return regs;
}
#endif

/*
 * Our handling of the processor debug registers is non-trivial.
 * We do not clear them on entry and exit from the kernel. Therefore
 * it is possible to get a watchpoint trap here from inside the kernel.
 * However, the code in ./ptrace.c has ensured that the user can
 * only set watchpoints on userspace addresses. Therefore the in-kernel
 * watchpoint trap can only occur in code which is reading/writing
 * from user space. Such code must not hold kernel locks (since it
 * can equally take a page fault), therefore it is safe to call
 * force_sig_info even though that claims and releases locks.
 *
 * Code in ./signal.c ensures that the debug control register
 * is restored before we deliver any signal, and therefore that
 * user code runs with the correct debug control register even though
 * we clear it here.
 *
 * Being careful here means that we don't have to be as careful in a
 * lot of more complicated places (task switching can be a bit lazy
 * about restoring all the debug state, and ptrace doesn't have to
 * find every occurrence of the TF bit that could be saved away even
 * by user code)
 *
 * May run on IST stack.
 */
dotraplinkage void __kprobes do_debug(struct pt_regs *regs, long error_code)
{
	struct task_struct *tsk = current;
	int user_icebp = 0;
	unsigned long dr6;
	int si_code;

	exception_enter(regs);

	get_debugreg(dr6, 6);

	/* Filter out all the reserved bits which are preset to 1 */
	dr6 &= ~DR6_RESERVED;

	/*
	 * If dr6 has no reason to give us about the origin of this trap,
	 * then it's very likely the result of an icebp/int01 trap.
	 * User wants a sigtrap for that.
	 */
	if (!dr6 && user_mode(regs))
		user_icebp = 1;

	/* Catch kmemcheck conditions first of all! */
	if ((dr6 & DR_STEP) && kmemcheck_trap(regs))
		goto exit;

	/* DR6 may or may not be cleared by the CPU */
	set_debugreg(0, 6);

	/*
	 * The processor cleared BTF, so don't mark that we need it set.
	 */
	clear_tsk_thread_flag(tsk, TIF_BLOCKSTEP);

	/* Store the virtualized DR6 value */
	tsk->thread.debugreg6 = dr6;

	if (notify_die(DIE_DEBUG, "debug", regs, PTR_ERR(&dr6), error_code,
							SIGTRAP) == NOTIFY_STOP)
		goto exit;

	/*
	 * Let others (NMI) know that the debug stack is in use
	 * as we may switch to the interrupt stack.
	 */
	debug_stack_usage_inc();

	/* It's safe to allow irq's after DR6 has been saved */
	preempt_conditional_sti(regs);

	if (regs->flags & X86_VM_MASK) {
		handle_vm86_trap((struct kernel_vm86_regs *) regs, error_code,
					X86_TRAP_DB);
		preempt_conditional_cli(regs);
		debug_stack_usage_dec();
		goto exit;
	}

	/*
	 * Single-stepping through system calls: ignore any exceptions in
	 * kernel space, but re-enable TF when returning to user mode.
	 *
	 * We already checked v86 mode above, so we can check for kernel mode
	 * by just checking the CPL of CS.
	 */
	if ((dr6 & DR_STEP) && !user_mode(regs)) {
		tsk->thread.debugreg6 &= ~DR_STEP;
		set_tsk_thread_flag(tsk, TIF_SINGLESTEP);
		regs->flags &= ~X86_EFLAGS_TF;
	}
	si_code = get_si_code(tsk->thread.debugreg6);
	if (tsk->thread.debugreg6 & (DR_STEP | DR_TRAP_BITS) || user_icebp)
		send_sigtrap(tsk, regs, error_code, si_code);
	preempt_conditional_cli(regs);
	debug_stack_usage_dec();

exit:
	exception_exit(regs);
}

/*
 * Note that we play around with the 'TS' bit in an attempt to get
 * the correct behaviour even in the presence of the asynchronous
 * IRQ13 behaviour
 */
void math_error(struct pt_regs *regs, int error_code, int trapnr)
{
	struct task_struct *task = current;
	siginfo_t info;
	unsigned short err;
	char *str = (trapnr == X86_TRAP_MF) ? "fpu exception" :
						"simd exception";

	if (notify_die(DIE_TRAP, str, regs, error_code, trapnr, SIGFPE) == NOTIFY_STOP)
		return;
	conditional_sti(regs);

	if (!user_mode_vm(regs))
	{
		if (!fixup_exception(regs)) {
			task->thread.error_code = error_code;
			task->thread.trap_nr = trapnr;
			die(str, regs, error_code);
		}
		return;
	}

	/*
	 * Save the info for the exception handler and clear the error.
	 */
	save_init_fpu(task);
	task->thread.trap_nr = trapnr;
	task->thread.error_code = error_code;
	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_addr = (void __user *)regs->ip;
	if (trapnr == X86_TRAP_MF) {
		unsigned short cwd, swd;
		/*
		 * (~cwd & swd) will mask out exceptions that are not set to unmasked
		 * status.  0x3f is the exception bits in these regs, 0x200 is the
		 * C1 reg you need in case of a stack fault, 0x040 is the stack
		 * fault bit.  We should only be taking one exception at a time,
		 * so if this combination doesn't produce any single exception,
		 * then we have a bad program that isn't synchronizing its FPU usage
		 * and it will suffer the consequences since we won't be able to
		 * fully reproduce the context of the exception
		 */
		cwd = get_fpu_cwd(task);
		swd = get_fpu_swd(task);

		err = swd & ~cwd;
	} else {
		/*
		 * The SIMD FPU exceptions are handled a little differently, as there
		 * is only a single status/control register.  Thus, to determine which
		 * unmasked exception was caught we must mask the exception mask bits
		 * at 0x1f80, and then use these to mask the exception bits at 0x3f.
		 */
		unsigned short mxcsr = get_fpu_mxcsr(task);
		err = ~(mxcsr >> 7) & mxcsr;
	}

	if (err & 0x001) {	/* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		info.si_code = FPE_FLTINV;
	} else if (err & 0x004) { /* Divide by Zero */
		info.si_code = FPE_FLTDIV;
	} else if (err & 0x008) { /* Overflow */
		info.si_code = FPE_FLTOVF;
	} else if (err & 0x012) { /* Denormal, Underflow */
		info.si_code = FPE_FLTUND;
	} else if (err & 0x020) { /* Precision */
		info.si_code = FPE_FLTRES;
	} else {
		/*
		 * If we're using IRQ 13, or supposedly even some trap
		 * X86_TRAP_MF implementations, it's possible
		 * we get a spurious trap, which is not an error.
		 */
		return;
	}
	force_sig_info(SIGFPE, &info, task);
}

dotraplinkage void do_coprocessor_error(struct pt_regs *regs, long error_code)
{
#ifdef CONFIG_X86_32
	ignore_fpu_irq = 1;
#endif
	exception_enter(regs);
	math_error(regs, error_code, X86_TRAP_MF);
	exception_exit(regs);
}

dotraplinkage void
do_simd_coprocessor_error(struct pt_regs *regs, long error_code)
{
	exception_enter(regs);
	math_error(regs, error_code, X86_TRAP_XF);
	exception_exit(regs);
}

dotraplinkage void
do_spurious_interrupt_bug(struct pt_regs *regs, long error_code)
{
	conditional_sti(regs);
#if 0
	/* No need to warn about this any longer. */
	pr_info("Ignoring P6 Local APIC Spurious Interrupt Bug...\n");
#endif
}

asmlinkage void __attribute__((weak)) smp_thermal_interrupt(void)
{
}

asmlinkage void __attribute__((weak)) smp_threshold_interrupt(void)
{
}

/*
 * 'math_state_restore()' saves the current math information in the
 * old math state array, and gets the new ones from the current task
 *
 * Careful.. There are problems with IBM-designed IRQ13 behaviour.
 * Don't touch unless you *really* know how it works.
 *
 * Must be called with kernel preemption disabled (eg with local
 * local interrupts as in the case of do_device_not_available).
 */
void math_state_restore(void)
{
	struct task_struct *tsk = current;

	if (!tsk_used_math(tsk)) {
		local_irq_enable();
		/*
		 * does a slab alloc which can sleep
		 */
		if (init_fpu(tsk)) {
			/*
			 * ran out of memory!
			 */
			do_group_exit(SIGKILL);
			return;
		}
		local_irq_disable();
	}

	__thread_fpu_begin(tsk);

	/*
	 * Paranoid restore. send a SIGSEGV if we fail to restore the state.
	 */
	if (unlikely(restore_fpu_checking(tsk))) {
		drop_init_fpu(tsk);
		force_sig(SIGSEGV, tsk);
		return;
	}

	tsk->fpu_counter++;
}
EXPORT_SYMBOL_GPL(math_state_restore);

dotraplinkage void __kprobes
do_device_not_available(struct pt_regs *regs, long error_code)
{
	exception_enter(regs);
	BUG_ON(use_eager_fpu());

#ifdef CONFIG_MATH_EMULATION
	if (read_cr0() & X86_CR0_EM) {
		struct math_emu_info info = { };

		conditional_sti(regs);

		info.regs = regs;
		math_emulate(&info);
		exception_exit(regs);
		return;
	}
#endif
	math_state_restore(); /* interrupts still off */
#ifdef CONFIG_X86_32
	conditional_sti(regs);
#endif
	exception_exit(regs);
}

#ifdef CONFIG_X86_32
dotraplinkage void do_iret_error(struct pt_regs *regs, long error_code)
{
	siginfo_t info;

	exception_enter(regs);
	local_irq_enable();

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_BADSTK;
	info.si_addr = NULL;
	if (notify_die(DIE_TRAP, "iret exception", regs, error_code,
			X86_TRAP_IRET, SIGILL) != NOTIFY_STOP) {
		do_trap(X86_TRAP_IRET, SIGILL, "iret exception", regs, error_code,
			&info);
	}
	exception_exit(regs);
}
#endif

/* Set of traps needed for early debugging. */
void __init early_trap_init(void)
{
	set_intr_gate_ist(X86_TRAP_DB, &debug, DEBUG_STACK);
	/* int3 can be called from all */
	set_system_intr_gate_ist(X86_TRAP_BP, &int3, DEBUG_STACK);
	set_intr_gate(X86_TRAP_PF, &page_fault);
	load_idt(&idt_descr);
}

void __init trap_init(void)
{
	int i;

#ifdef CONFIG_EISA
	void __iomem *p = early_ioremap(0x0FFFD9, 4);

	if (readl(p) == 'E' + ('I'<<8) + ('S'<<16) + ('A'<<24))
		EISA_bus = 1;
	early_iounmap(p, 4);
#endif

	set_intr_gate(X86_TRAP_DE, &divide_error);
	set_intr_gate_ist(X86_TRAP_NMI, &nmi, NMI_STACK);
	/* int4 can be called from all */
	set_system_intr_gate(X86_TRAP_OF, &overflow);
	set_intr_gate(X86_TRAP_BR, &bounds);
	set_intr_gate(X86_TRAP_UD, &invalid_op);
	set_intr_gate(X86_TRAP_NM, &device_not_available);
#ifdef CONFIG_X86_32
	set_task_gate(X86_TRAP_DF, GDT_ENTRY_DOUBLEFAULT_TSS);
#else
	set_intr_gate_ist(X86_TRAP_DF, &double_fault, DOUBLEFAULT_STACK);
#endif
	set_intr_gate(X86_TRAP_OLD_MF, &coprocessor_segment_overrun);
	set_intr_gate(X86_TRAP_TS, &invalid_TSS);
	set_intr_gate(X86_TRAP_NP, &segment_not_present);
	set_intr_gate_ist(X86_TRAP_SS, &stack_segment, STACKFAULT_STACK);
	set_intr_gate(X86_TRAP_GP, &general_protection);
	set_intr_gate(X86_TRAP_SPURIOUS, &spurious_interrupt_bug);
	set_intr_gate(X86_TRAP_MF, &coprocessor_error);
	set_intr_gate(X86_TRAP_AC, &alignment_check);
#ifdef CONFIG_X86_MCE
	set_intr_gate_ist(X86_TRAP_MC, &machine_check, MCE_STACK);
#endif
	set_intr_gate(X86_TRAP_XF, &simd_coprocessor_error);

	/* Reserve all the builtin and the syscall vector: */
	for (i = 0; i < FIRST_EXTERNAL_VECTOR; i++)
		set_bit(i, used_vectors);

#ifdef CONFIG_IA32_EMULATION
	set_system_intr_gate(IA32_SYSCALL_VECTOR, ia32_syscall);
	set_bit(IA32_SYSCALL_VECTOR, used_vectors);
#endif

#ifdef CONFIG_X86_32
	set_system_trap_gate(SYSCALL_VECTOR, &system_call);
	set_bit(SYSCALL_VECTOR, used_vectors);
#endif

	/*
	 * Should be a barrier for any external CPU state:
	 */
	cpu_init();

	x86_init.irqs.trap_init();

#ifdef CONFIG_X86_64
	memcpy(&nmi_idt_table, &idt_table, IDT_ENTRIES * 16);
	set_nmi_gate(X86_TRAP_DB, &debug);
	set_nmi_gate(X86_TRAP_BP, &int3);
#endif
}
