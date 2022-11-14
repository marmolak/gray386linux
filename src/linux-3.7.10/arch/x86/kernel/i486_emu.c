#include <asm/i486_emu.h>
#include <linux/uaccess.h>
#include <linux/irqflags.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <asm/barrier.h>

/* modrm bits */
#define MODRM_REG(b)    ( ( ( b ) >> 3 ) & 7 )
#define MODRM_NNN(b)    ( ( ( b ) >> 3 ) & 7 )
#define MODRM_MOD(b)    ( ( ( b ) >> 6 ) & 3 )
#define MODRM_RM(b)     ( ( b ) & 7 )

struct parser_state {
    unsigned long *inst_src;
    unsigned long *inst_dest;
    struct pt_regs *regs;
    u32 orig_ip;
    u32 insts;
    u32 insts2;
    unsigned int regs_only;
    unsigned int lock_prefix;
};

static inline int parse_reg(struct pt_regs *regs, unsigned long reg, unsigned long **inst_src)
{
    switch (reg) {
        case 0: {
            *inst_src = &regs->ax;
            break;
        }
        case 1: {
            *inst_src = &regs->cx;
            break;
        }
        case 2: {
            *inst_src = &regs->dx;
            break;
        }
        case 3: {
            *inst_src = &regs->bx;
            break;
        }
        case 4: {
            *inst_src = &regs->sp;
            break;
        }
        case 5: {
            *inst_src = &regs->bp;
            break;
        }
        case 6: {
            *inst_src = &regs->si;
            break;
        }
        case 7: {
            *inst_src = &regs->di;
            break;
        }
        default:
            return -EOPNOTSUPP;
    }

    return 1;
}

static inline int parse_modrm_register(struct parser_state *pstate)
{
    int ret;
    unsigned long mod;
    unsigned long rm;
    unsigned long reg;
    unsigned long inst_size;

    mod = MODRM_MOD(pstate->insts);
    rm = MODRM_RM(pstate->insts);
    reg = MODRM_REG(pstate->insts);

    /* src part */
    ret = parse_reg(pstate->regs, reg, &(pstate->inst_src));
    if (ret < 0) {
        return -EOPNOTSUPP;
    }

    if (mod == 0x3) {
        /* In case where src and dest are regs, then rm and reg are same from parser point of view. */
        ret = parse_reg(pstate->regs, rm, &(pstate->inst_dest));
        if (ret < 0) {
            return -EOPNOTSUPP;
        }

        pstate->regs_only = 1;
        pstate->regs->ip += 1;

        return 1;
    }
    pstate->regs_only = 0;

    /* RM part - add offsets in next part.  */
    switch (rm) {
    case 0x0: {
        pstate->inst_dest = &pstate->regs->ax;
        break;
    }
    case 0x1: {
        pstate->inst_dest = &pstate->regs->cx;
        break;
    }
    case 0x2: {
        pstate->inst_dest = &pstate->regs->dx;
        break;
    }
    case 0x3: {
        pstate->inst_dest = &pstate->regs->bx;
        break;
    }
    /* 0x4 is for SIP which is not supported for now. */
    case 0x5: {
        /* Not supported by all combinations. */
        pstate->inst_dest = &pstate->regs->bp;
        break;
    }
    case 0x6: {
        pstate->inst_dest = &pstate->regs->si;
        break;
    }
    case 0x7: {
        pstate->inst_dest = &pstate->regs->di;
        break;
    }
    default:
        return -EOPNOTSUPP;
    }

    pstate->insts >>= 8;
    pstate->regs->ip += 1;
    inst_size = pstate->regs->ip - pstate->orig_ip;

    switch (mod) {
    case 0x0: {
        if (rm != 0x5) {
            /* Need to get 32-bit address only in one case when mod = 0x0 and rm = 0x5. */
            return 1;
        }

        pstate->regs->ip += (4 - inst_size);

        /* Last byte after parsing instruction without lock prefix. */
        if (inst_size < 4) {
            pstate->regs->ip += 3;
            pstate->insts = (pstate->insts) | (pstate->insts2 << 8);
        } else {
            pstate->regs->ip += 4;
            pstate->insts = pstate->insts2;
        }

        /* Example:
            f0 0f b1 15 24 d2 0c 08 lock cmpxchg DWORD PTR ds:0x80cd224,edx
        */
        pstate->inst_dest = (unsigned long *)(pstate->insts);
        break;
    }
    case 0x1: {
        if (inst_size == 4) {
            pstate->insts = pstate->insts2;
        }

        pstate->regs->ip += 1;
        pstate->inst_dest = (unsigned long *)((unsigned long)(*(pstate->inst_dest)) + (pstate->insts & 0xff));
        break;
    }
    case 0x2: {
        pstate->regs->ip += (4 - inst_size);

        if (inst_size < 4) {
            pstate->regs->ip += 3;
            pstate->insts = (pstate->insts) | (pstate->insts2 << 8);
        } else {
            pstate->regs->ip += 4;
            pstate->insts = pstate->insts2;
        }

        pstate->inst_dest = (unsigned long *)((*(pstate->inst_dest)) + pstate->insts);
        break;
    }
    default:
        return -EOPNOTSUPP;
    }

    return 1;
}

static inline int parse_emu_cmpxchg(struct parser_state *pstate)
{
    int ret;

    ret = parse_modrm_register(pstate);
    if (ret < 0) {
        return -EOPNOTSUPP;
    }

    if (pstate->regs_only == 0 && !access_ok(VERIFY_WRITE, (unsigned long __user *)(pstate->inst_dest), sizeof(*(pstate->inst_dest)))) {
        printk("Check for write to user memory failed. Doing default trap.\n"
            "Address:%lx reg_dest:%lx reg_src:%lx\n",
            (unsigned long) pstate->inst_dest,
            (unsigned long) pstate->inst_dest,
            (unsigned long) pstate->inst_src);

        return -ENOMEM;
    }

    if (pstate->regs->ax == *(pstate->inst_dest)) {
		pstate->regs->flags |= X86_EFLAGS_ZF;
        *(pstate->inst_dest) = *(pstate->inst_src);
	} else {
		pstate->regs->flags &= ~X86_EFLAGS_ZF;
		pstate->regs->ax = *(pstate->inst_dest);
	}

    return 1;
}

static inline int parse_emu_xadd(struct parser_state *pstate)
{
    int ret;
    unsigned long dest;
    unsigned long result_dest;
    unsigned long flags;
    unsigned long src;

    ret = parse_modrm_register(pstate);
    if (ret < 0) {
        return -EOPNOTSUPP;
    }

    if (pstate->regs_only == 0 && !access_ok(VERIFY_WRITE, (unsigned long __user *)(pstate->inst_dest), sizeof(*(pstate->inst_dest)))) {
        printk("Check for write to user memory failed. Doing default trap.\n"
            "Address:%lx reg_dest:%lx reg_src:%lx\n",
            (unsigned long) pstate->inst_dest,
            (unsigned long) pstate->inst_dest,
            (unsigned long) pstate->inst_src);

        return -ENOMEM;
    }

    flags = pstate->regs->flags;

    if (pstate->regs_only == 0) {
        /* TODO: can sleep! */
        get_user(result_dest, (unsigned long __user *)pstate->inst_dest);
        dest = result_dest;
    } else {
        /* Register. */
        result_dest = dest = *(pstate->inst_dest);
    }

    src = *(pstate->inst_src);
    
    /* Flags: CF, PF, AF, SF, ZF, and OF */
    __asm__ volatile (
            "addl %2, %3\n\t"
            "pushfl\n\t"
            "popl %1\n\t"
        : "=r" (result_dest), "=m" (flags)
        : "r" (dest), "0" (src)
        : "cc"
    );

    /* Destroy first. */
    pstate->regs->flags &= ~(X86_EFLAGS_CF | X86_EFLAGS_PF | X86_EFLAGS_SF | X86_EFLAGS_ZF | X86_EFLAGS_OF | X86_EFLAGS_AF);
    /* Get only interesting flags. */
    flags &= (X86_EFLAGS_CF | X86_EFLAGS_PF | X86_EFLAGS_SF | X86_EFLAGS_ZF | X86_EFLAGS_OF | X86_EFLAGS_AF);
    /* Add flags if needed. */
    pstate->regs->flags |= flags;

    /* SRC is always register in case of xadd */
    *(pstate->inst_src) = *(pstate->inst_dest);
    if (pstate->regs_only == 0) {
        /* TODO: check if destination is register or not */
        put_user(result_dest, (unsigned long __user *)pstate->inst_dest);
    } else {
        *(pstate->inst_dest) = result_dest;
    }

    return 1;
}

static inline int parse_emu_bswap(struct parser_state *pstate)
{
    int ret;
    unsigned long reg;

    if (unlikely(pstate->lock_prefix == 1)) {
        return -EOPNOTSUPP;
    }

    reg = MODRM_RM(pstate->insts);

    ret = parse_reg(pstate->regs, reg, &(pstate->inst_dest));
    if (ret < 0) {
        return -EOPNOTSUPP;
    }
    pstate->regs->ip += 1;

    *(pstate->inst_dest) =
        ((*(pstate->inst_dest) & 0x000000FFUL) << 24) |
        ((*(pstate->inst_dest) & 0x0000FF00UL) <<  8) |
        ((*(pstate->inst_dest) & 0x00FF0000UL) >>  8) |
        ((*(pstate->inst_dest) & 0xFF000000UL) >> 24);

    return 1;
}

int parse_emu_instruction(struct pt_regs *regs)
{
    int ret;
    unsigned long flags;
    struct parser_state pstate;

    if (get_user(pstate.insts, (unsigned long __user *)(regs->ip)) == -EFAULT)
    {
		printk("Unable to get instructions to handle. Part one.");
		return -ENOMEM;
    }
    if (get_user(pstate.insts2, (unsigned long __user *)((regs->ip) + sizeof(pstate.insts))) == -EFAULT)
    {
		printk("Unable to get instructions to handle. Part two.");
		return -ENOMEM;
    }

	local_irq_save(flags);

    /* Quick and dirty hack to skip endbr32. */
    if (pstate.insts == 0xfb1e0ff3u)
    {
        instruction_pointer_set(regs, regs->ip + sizeof(pstate.insts));
        ret = 1;
        goto end;
    }

    pstate.regs = regs;
    pstate.orig_ip = regs->ip;
    /* Skip lock prefix if present. */
	if ((pstate.insts & 0xff) == 0xf0) {
		pstate.insts >>= 8;
        regs->ip += 1;
        pstate.lock_prefix = 1;
	} else {
        pstate.lock_prefix = 0;
    }

    /* For now, just check for instructions started with 0f. */
    if ((pstate.insts & 0xff) != 0x0f)
    {
        ret = -EOPNOTSUPP;
        goto end;
    }
    pstate.insts >>= 8;
    regs->ip += 1;

    /* Check for operation. */
    switch (pstate.insts & 0xff) {

    /* cmpxchg */
    case 0xb1: {
        regs->ip += 1;
        pstate.insts >>= 8;
        ret = parse_emu_cmpxchg(&pstate);
        break;
    }

    /* xadd */
    case 0xc1: {
        regs->ip += 1;
        pstate.insts >>= 8;
        ret = parse_emu_xadd(&pstate);
        break;
    }

    /* bswap - yes, there can be more elegant way. */
    case 0xc8:
    case 0xc9:
    case 0xca:
    case 0xcb:
    case 0xce:
    case 0xcf:
    {
        ret = parse_emu_bswap(&pstate);
        break;
    }

    default: {
        ret = -EOPNOTSUPP;
        goto end;
    }

    } /* end switch */

end:
    if (ret < 0) {
        /* Rollback in case of fail. */
        regs->ip = pstate.orig_ip;
    }
     local_irq_restore(flags);
    return ret;
}
