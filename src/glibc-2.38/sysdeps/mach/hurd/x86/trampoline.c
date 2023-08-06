/* Set thread_state for sighandler, and sigcontext to recover.  x86 version.
   Copyright (C) 1994-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <hurd/signal.h>
#include <hurd/userlink.h>
#include <thread_state.h>
#include <mach/exception.h>
#include <mach/machine/eflags.h>
#include <assert.h>
#include <errno.h>
#include "hurdfault.h"
#include <intr-msg.h>
#include <sys/ucontext.h>


/* Fill in a siginfo_t structure for SA_SIGINFO-enabled handlers.  */
static void fill_siginfo (siginfo_t *si, int signo,
			  const struct hurd_signal_detail *detail,
			  const struct machine_thread_all_state *state)
{
  si->si_signo = signo;
  si->si_errno = detail->error;
  si->si_code = detail->code;

  /* XXX We would need a protocol change for sig_post to include
   * this information.  */
  si->si_pid = -1;
  si->si_uid = -1;

  /* Address of the faulting instruction or memory access.  */
  if (detail->exc == EXC_BAD_ACCESS)
    si->si_addr = (void *) detail->exc_subcode;
  else
    {
#ifdef __x86_64__
      si->si_addr = (void *) state->basic.rip;
#else
      si->si_addr = (void *) state->basic.eip;
#endif
    }

  /* XXX On SIGCHLD, this should be the exit status of the child
   * process.  We would need a protocol change for the proc server
   * to send this information along with the signal.  */
  si->si_status = 0;

  si->si_band = 0;              /* SIGPOLL is not supported yet.  */
  si->si_value.sival_int = 0;   /* sigqueue() is not supported yet.  */
}

/* Fill in a ucontext_t structure SA_SIGINFO-enabled handlers.  */
static void fill_ucontext (ucontext_t *uc, const struct sigcontext *sc)
{
  uc->uc_flags = 0;
  uc->uc_link = NULL;
  uc->uc_sigmask = sc->sc_mask;
#ifdef __x86_64__
  uc->uc_stack.ss_sp = (__ptr_t) sc->sc_ursp;
#else
  uc->uc_stack.ss_sp = (__ptr_t) sc->sc_uesp;
#endif
  uc->uc_stack.ss_size = 0;
  uc->uc_stack.ss_flags = 0;

  /* Registers.  */
#ifdef __x86_64__
  memcpy (&uc->uc_mcontext.gregs[REG_GSFS], &sc->sc_gs,
          (REG_ERR - REG_GSFS) * sizeof (long));
#else
  memcpy (&uc->uc_mcontext.gregs[REG_GS], &sc->sc_gs,
          (REG_TRAPNO - REG_GS) * sizeof (int));
  memcpy (&uc->uc_mcontext.gregs[REG_EIP], &sc->sc_eip,
	  (NGREG - REG_EIP) * sizeof (int));
#endif
  uc->uc_mcontext.gregs[REG_TRAPNO] = 0;
  uc->uc_mcontext.gregs[REG_ERR] = 0;

  /* XXX FPU state.  */
  memset (&uc->uc_mcontext.fpregs, 0, sizeof (fpregset_t));
}

struct sigcontext *
_hurd_setup_sighandler (struct hurd_sigstate *ss, const struct sigaction *action,
			__sighandler_t handler,
			int signo, struct hurd_signal_detail *detail,
			int rpc_wait,
			struct machine_thread_all_state *state)
{
  void trampoline (void);
  void rpc_wait_trampoline (void);
  void firewall (void);
  void *sigsp;
  struct sigcontext *scp;
  struct
    {
      union
        {
          int signo;
          /* Make sure signo takes up a pointer-sized slot on the stack.
             (This should already be the case considering the siginfop
             pointer below, but better be explicit.)  */
          void *_pointer_sized;
        };
      union
	{
	  /* Extra arguments for traditional signal handlers */
	  struct
	    {
	      long int sigcode;
	      struct sigcontext *scp;       /* Points to ctx, below.  */
	    } legacy;

	  /* Extra arguments for SA_SIGINFO handlers */
	  struct
	    {
	      siginfo_t *siginfop;          /* Points to siginfo, below.  */
	      ucontext_t *uctxp;            /* Points to uctx, below.  */
	    } posix;
	};

#ifdef __x86_64__
      void *_pad;
#endif
      void *sigreturn_addr;
      void *sigreturn_returns_here;
      struct sigcontext *return_scp; /* Same; arg to sigreturn.  */

      /* NB: sigreturn assumes link is next to ctx.  */
      struct sigcontext ctx;
      struct hurd_userlink link;
      ucontext_t ucontext;
      siginfo_t siginfo;
    } *stackframe;

#ifdef __x86_64__
  _Static_assert (offsetof (typeof (*stackframe), sigreturn_addr) % 16 == 0,
                  "sigreturn_addr must be 16-byte aligned");
#endif

  if (ss->context)
    {
      /* We have a previous sigcontext that sigreturn was about
	 to restore when another signal arrived.  We will just base
	 our setup on that.  */
      if (! _hurdsig_catch_memory_fault (ss->context))
	{
	  memcpy (&state->basic, &ss->context->sc_i386_thread_state,
		  sizeof (state->basic));
	  memcpy (&state->fpu, &ss->context->sc_i386_float_state,
		  sizeof (state->fpu));
	  state->set |= (1 << i386_REGS_SEGS_STATE) | (1 << i386_FLOAT_STATE);
	}
    }

  if (! machine_get_basic_state (ss->thread, state))
    return NULL;

  /* Save the original SP in the gratuitous `esp' slot.
     We may need to reset the SP (the `uesp' slot) to avoid clobbering an
     interrupted RPC frame.  */
#ifdef __x86_64__
  state->basic.rsp = state->basic.ursp;
#else
  state->basic.esp = state->basic.uesp;
#endif

  if ((action->sa_flags & SA_ONSTACK)
      && !(ss->sigaltstack.ss_flags & (SS_DISABLE|SS_ONSTACK)))
    {
      sigsp = ss->sigaltstack.ss_sp + ss->sigaltstack.ss_size;
      ss->sigaltstack.ss_flags |= SS_ONSTACK;
    }
  else
    {
#ifdef __x86_64__
      /* Per the ABI, we're not supposed to clobber 128 bytes below
         SP -- the red zone.  */
      sigsp = (char *) state->basic.ursp - 128;
#else
      sigsp = (char *) state->basic.uesp;
#endif
    }

  /* Push the arguments to call `trampoline' on the stack.  */
  sigsp -= sizeof (*stackframe);
#ifdef __x86_64__
  /* Align SP at 16 bytes.  Coupled with the fact that sigreturn_addr is
     16-byte aligned within the stackframe struct, this ensures that it ends
     up on a 16-byte aligned address, as required by the ABI.  */
  sigsp = (void *) ((uintptr_t) sigsp & ~15UL);
#endif
  stackframe = sigsp;

  if (_hurdsig_catch_memory_fault (stackframe))
    {
      /* We got a fault trying to write the stack frame.
	 We cannot set up the signal handler.
	 Returning NULL tells our caller, who will nuke us with a SIGILL.  */
      return NULL;
    }
  else
    {
      int ok;

      extern void _hurdsig_longjmp_from_handler (void *, jmp_buf, int);

      /* Add a link to the thread's active-resources list.  We mark this as
	 the only user of the "resource", so the cleanup function will be
	 called by any longjmp which is unwinding past the signal frame.
	 The cleanup function (in sigunwind.c) will make sure that all the
	 appropriate cleanups done by sigreturn are taken care of.  */
      stackframe->link.cleanup = &_hurdsig_longjmp_from_handler;
      stackframe->link.cleanup_data = &stackframe->ctx;
      stackframe->link.resource.next = NULL;
      stackframe->link.resource.prevp = NULL;
      stackframe->link.thread.next = ss->active_resources;
      stackframe->link.thread.prevp = &ss->active_resources;
      if (stackframe->link.thread.next)
	stackframe->link.thread.next->thread.prevp
	  = &stackframe->link.thread.next;
      ss->active_resources = &stackframe->link;

      /* Set up the sigcontext from the current state of the thread.  */

      scp = &stackframe->ctx;
      scp->sc_onstack = ss->sigaltstack.ss_flags & SS_ONSTACK ? 1 : 0;

      /* struct sigcontext is laid out so that starting at sc_gs mimics a
	 struct i386_thread_state.  */
      _Static_assert (offsetof (struct sigcontext, sc_i386_thread_state)
		      % __alignof__ (struct i386_thread_state) == 0,
		      "sc_i386_thread_state layout mismatch");
      memcpy (&scp->sc_i386_thread_state,
	      &state->basic, sizeof (state->basic));

      /* struct sigcontext is laid out so that starting at sc_fpkind mimics
	 a struct i386_float_state.  */
      _Static_assert (offsetof (struct sigcontext, sc_i386_float_state)
		      % __alignof__ (struct i386_float_state) == 0,
		      "sc_i386_float_state layout mismatch");
      ok = machine_get_state (ss->thread, state, i386_FLOAT_STATE,
			      &state->fpu, &scp->sc_i386_float_state,
			      sizeof (state->fpu));

      /* Set up the arguments for the signal handler.  */
      stackframe->signo = signo;
      if (action->sa_flags & SA_SIGINFO)
	{
	  stackframe->posix.siginfop = &stackframe->siginfo;
	  stackframe->posix.uctxp = &stackframe->ucontext;
	  fill_siginfo (&stackframe->siginfo, signo, detail, state);
	  fill_ucontext (&stackframe->ucontext, scp);
	}
      else
	{
	  if (detail->exc)
	    {
	      int nsigno;
	      _hurd_exception2signal_legacy (detail, &nsigno);
	      assert (nsigno == signo);
	    }
	  else
	    detail->code = 0;

	  stackframe->legacy.sigcode = detail->code;
	  stackframe->legacy.scp = &stackframe->ctx;
	}

      /* Set up the bottom of the stack.  */
      stackframe->sigreturn_addr = &__sigreturn;
      stackframe->sigreturn_returns_here = firewall; /* Crash on return.  */
      stackframe->return_scp = &stackframe->ctx;

      _hurdsig_end_catch_fault ();

      if (! ok)
	return NULL;
    }

  /* Modify the thread state to call the trampoline code on the new stack.  */
#ifdef __x86_64__
  if (rpc_wait)
    {
      /* The signalee thread was blocked in a mach_msg_trap system call,
         still waiting for a reply.  We will have it run the special
         trampoline code which retries the message receive before running
         the signal handler.

         To do this we change the OPTION argument (in rsi) to enable only
         message reception, since the request message has already been
         sent.  */

      assert (state->basic.rsi & MACH_RCV_MSG);
      /* Disable the message-send, since it has already completed.  The
         calls we retry need only wait to receive the reply message.  */
      state->basic.rsi &= ~MACH_SEND_MSG;

      /* Limit the time to receive the reply message, in case the server
         claimed that `interrupt_operation' succeeded but in fact the RPC
         is hung.  */
      state->basic.rsi |= MACH_RCV_TIMEOUT;
      state->basic.r9 = _hurd_interrupted_rpc_timeout;

      state->basic.rip = (uintptr_t) rpc_wait_trampoline;
      /* The reply-receiving trampoline code runs initially on the original
         user stack.  We pass it the signal stack pointer in %rbx.  */
      state->basic.rbx = (uintptr_t) sigsp;
      /* After doing the message receive, the trampoline code will need to
         update the %rax value to be restored by sigreturn.  To simplify
         the assembly code, we pass the address of its slot in SCP to the
         trampoline code in %r12.  */
      state->basic.r12 = (uintptr_t) &scp->sc_rax;
    }
  else
    {
      state->basic.rip = (uintptr_t) trampoline;
      state->basic.ursp = (uintptr_t) sigsp;
    }
  /* We pass the handler function to the trampoline code in %r13.  */
  state->basic.r13 = (uintptr_t) handler;

  /* The x86 ABI says the DF bit is clear on entry to any function.  */
  state->basic.rfl &= ~EFL_DF;

#else

  if (rpc_wait)
    {
      /* The signalee thread was blocked in a mach_msg_trap system call,
	 still waiting for a reply.  We will have it run the special
	 trampoline code which retries the message receive before running
	 the signal handler.

	 To do this we change the OPTION argument on its stack to enable only
	 message reception, since the request message has already been
	 sent.  */

      struct mach_msg_trap_args *args = (void *) state->basic.esp;

      if (_hurdsig_catch_memory_fault (args))
	{
	  /* Faulted accessing ARGS.  Bomb.  */
	  return NULL;
	}

      assert (args->option & MACH_RCV_MSG);
      /* Disable the message-send, since it has already completed.  The
	 calls we retry need only wait to receive the reply message.  */
      args->option &= ~MACH_SEND_MSG;

      /* Limit the time to receive the reply message, in case the server
	 claimed that `interrupt_operation' succeeded but in fact the RPC
	 is hung.  */
      args->option |= MACH_RCV_TIMEOUT;
      args->timeout = _hurd_interrupted_rpc_timeout;

      _hurdsig_end_catch_fault ();

      state->basic.eip = (uintptr_t) rpc_wait_trampoline;
      /* The reply-receiving trampoline code runs initially on the original
	 user stack.  We pass it the signal stack pointer in %ebx.  */
      state->basic.ebx = (uintptr_t) sigsp;
      /* After doing the message receive, the trampoline code will need to
	 update the %eax value to be restored by sigreturn.  To simplify
	 the assembly code, we pass the address of its slot in SCP to the
	 trampoline code in %ecx.  */
      state->basic.ecx = (uintptr_t) &scp->sc_eax;
    }
  else
    {
      state->basic.eip = (uintptr_t) trampoline;
      state->basic.uesp = (uintptr_t) sigsp;
    }
  /* We pass the handler function to the trampoline code in %edx.  */
  state->basic.edx = (uintptr_t) handler;

  /* The x86 ABI says the DF bit is clear on entry to any function.  */
  state->basic.efl &= ~EFL_DF;
#endif

  return scp;
}

/* The trampoline code follows.  This used to be located inside
   _hurd_setup_sighandler, but was optimized away by gcc 2.95.

   If you modify this, update
   - in gcc: libgcc/config/i386/gnu-unwind.h x86_gnu_fallback_frame_state,
   - in gdb: gdb/i386-gnu-tdep.c gnu_sigtramp_code.  */

#ifdef __x86_64__
asm ("rpc_wait_trampoline:\n"
  /* This is the entry point when we have an RPC reply message to receive
     before running the handler.  The MACH_MSG_SEND bit has already been
     cleared in the OPTION argument in our %rsi.  The interrupted user
     stack pointer has not been changed, so the system call can find its
     arguments; the signal stack pointer is in %rbx.  For our convenience,
     %r12 points to the sc_rax member of the sigcontext.  */
     "movq $-25, %rax\n"	/* mach_msg_trap */
     "syscall\n"
     /* When the sigcontext was saved, %rax was MACH_RCV_INTERRUPTED.  But
        now the message receive has completed and the original caller of
        the RPC (i.e. the code running when the signal arrived) needs to
        see the final return value of the message receive in %rax.  So
        store the new %rax value into the sc_rax member of the sigcontext
        (whose address is in %r12 to make this code simpler).  */
     "movq %rax, (%r12)\n"
     /* Switch to the signal stack.  */
     "movq %rbx, %rsp\n"

     "trampoline:\n"
     /* Entry point for running the handler normally.  The arguments to the
        handler function are on the top of the stack, same as in the i386
        version:

        0(%rsp)  SIGNO
        8(%rsp)  SIGCODE
        16(%rsp) SCP

        Pop them off to the registers, to pass as arguments to the handler.
     */
     "popq %rdi\n"
     "popq %rsi\n"
     "popq %rdx\n"
     /* Pop the _pad member to make the stack 16-byte aligned, as per the
        ABI.  */
     "addq $8, %rsp\n"
     "call *%r13\n"		/* Call the handler function.  */
     /* The word at the top of stack is &__sigreturn; following are a dummy
        word to fill the slot for the address for __sigreturn to return to,
        and a copy of SCP for __sigreturn's argument.  Load the SCP as for a
        call, and "return" to calling __sigreturn (SCP); this call never
        returns.  */
     "movq 16(%rsp), %rdi\n"
     "ret");
#else
asm ("rpc_wait_trampoline:\n");
  /* This is the entry point when we have an RPC reply message to receive
     before running the handler.  The MACH_MSG_SEND bit has already been
     cleared in the OPTION argument on our stack.  The interrupted user
     stack pointer has not been changed, so the system call can find its
     arguments; the signal stack pointer is in %ebx.  For our convenience,
     %ecx points to the sc_eax member of the sigcontext.  */
asm (/* Retry the interrupted mach_msg system call.  */
     "movl $-25, %eax\n"	/* mach_msg_trap */
     "lcall $7, $0\n"
     /* When the sigcontext was saved, %eax was MACH_RCV_INTERRUPTED.  But
	now the message receive has completed and the original caller of
	the RPC (i.e. the code running when the signal arrived) needs to
	see the final return value of the message receive in %eax.  So
	store the new %eax value into the sc_eax member of the sigcontext
	(whose address is in %ecx to make this code simpler).  */
     "movl %eax, (%ecx)\n"
     /* Switch to the signal stack.  */
     "movl %ebx, %esp\n");

 asm ("trampoline:\n");
  /* Entry point for running the handler normally.  The arguments to the
     handler function are already on the top of the stack:

       0(%esp)	SIGNO
       4(%esp)	SIGCODE
       8(%esp)	SCP
     */
asm ("call *%edx\n"		/* Call the handler function.  */
     "addl $12, %esp\n"		/* Pop its args.  */
     /* The word at the top of stack is &__sigreturn; following are a dummy
	word to fill the slot for the address for __sigreturn to return to,
	and a copy of SCP for __sigreturn's argument.  "Return" to calling
	__sigreturn (SCP); this call never returns.  */
     "ret");
#endif

asm ("firewall:\n"
     "hlt");
