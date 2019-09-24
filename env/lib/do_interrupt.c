#include <s5p4418_serial_stdio.h>
#include <interrupt.h>
#include <io.h>
#include <synchronize.h>
#include <smp.h>
#include <headsmp.h>
#include <s5p4418_clk.h>

extern unsigned int _stack_abt_end;
extern unsigned int _stack_und_end;

void reset_cpu(void)
{
    /* Software reset */
    //NX_CLKPWR_SetSoftwareResetEnable(CTRUE);
    //NX_CLKPWR_DoSoftwareReset();
	s5p4418_set_softwareReset_Enable(TRUE);
	s5p4418_do_softwareReset();
}

#define pc_pointer(v) \
        ((v) & ~PCMASK)

#define instruction_pointer(regs) \
        (pc_pointer((regs)->ARM_pc))

#define USR26_MODE  0x00
#define FIQ26_MODE  0x01
#define IRQ26_MODE  0x02
#define SVC26_MODE  0x03
#define USR_MODE    0x10
#define FIQ_MODE    0x11
#define IRQ_MODE    0x12
#define SVC_MODE    0x13
#define ABT_MODE    0x17
#define UND_MODE    0x1b
#define SYSTEM_MODE 0x1f
#define MODE_MASK   0x1f
#define T_BIT       0x20
#define F_BIT       0x40
#define I_BIT       0x80
#define CC_V_BIT    (1 << 28)
#define CC_C_BIT    (1 << 29)
#define CC_Z_BIT    (1 << 30)
#define CC_N_BIT    (1 << 31)
#define PCMASK      0

/* this struct defines the way the registers are stored on the
   stack during a system call. */

struct pt_regs {
    long uregs[18];
};

#define ARM_cpsr    uregs[16]
#define ARM_pc      uregs[15]
#define ARM_lr      uregs[14]
#define ARM_sp      uregs[13]
#define ARM_ip      uregs[12]
#define ARM_fp      uregs[11]
#define ARM_r10     uregs[10]
#define ARM_r9      uregs[9]
#define ARM_r8      uregs[8]
#define ARM_r7      uregs[7]
#define ARM_r6      uregs[6]
#define ARM_r5      uregs[5]
#define ARM_r4      uregs[4]
#define ARM_r3      uregs[3]
#define ARM_r2      uregs[2]
#define ARM_r1      uregs[1]
#define ARM_r0      uregs[0]
#define ARM_ORIG_r0 uregs[17]

#define user_mode(regs) \
    (((regs)->ARM_cpsr & 0xf) == 0)

#ifdef CONFIG_ARM_THUMB
#define thumb_mode(regs) \
    (((regs)->ARM_cpsr & T_BIT))
#else
#define thumb_mode(regs) (0)
#endif

#define processor_mode(regs) \
    ((regs)->ARM_cpsr & MODE_MASK)

#define interrupts_enabled(regs) \
    (!((regs)->ARM_cpsr & I_BIT))

#define fast_interrupts_enabled(regs) \
    (!((regs)->ARM_cpsr & F_BIT))

#define condition_codes(regs) \
    ((regs)->ARM_cpsr & (CC_V_BIT|CC_C_BIT|CC_Z_BIT|CC_N_BIT))

void bad_mode (void)
{
    reset_cpu();
    while(1);
}

void show_regs (struct pt_regs *regs)
{
    unsigned long flags;
    const char *processor_modes[] = {
    "USER_26",  "FIQ_26",   "IRQ_26",   "SVC_26",
    "UK4_26",   "UK5_26",   "UK6_26",   "UK7_26",
    "UK8_26",   "UK9_26",   "UK10_26",  "UK11_26",
    "UK12_26",  "UK13_26",  "UK14_26",  "UK15_26",
    "USER_32",  "FIQ_32",   "IRQ_32",   "SVC_32",
    "UK4_32",   "UK5_32",   "UK6_32",   "ABT_32",
    "UK8_32",   "UK9_32",   "UK10_32",  "UND_32",
    "UK12_32",  "UK13_32",  "UK14_32",  "SYS_32",
    };

    flags = condition_codes (regs);

    printf ("pc : [<%08lx>]    lr : [<%08lx>]\r\n"
        "sp : %08lx  ip : %08lx  fp : %08lx\r\n",
        instruction_pointer (regs),
        regs->ARM_lr, regs->ARM_sp, regs->ARM_ip, regs->ARM_fp);
    printf ("r10: %08lx  r9 : %08lx  r8 : %08lx\r\n",
        regs->ARM_r10, regs->ARM_r9, regs->ARM_r8);
    printf ("r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\r\n",
        regs->ARM_r7, regs->ARM_r6, regs->ARM_r5, regs->ARM_r4);
    printf ("r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\r\n",
        regs->ARM_r3, regs->ARM_r2, regs->ARM_r1, regs->ARM_r0);
    printf ("Flags: %c%c%c%c",
        flags & CC_N_BIT ? 'N' : 'n',
        flags & CC_Z_BIT ? 'Z' : 'z',
        flags & CC_C_BIT ? 'C' : 'c', flags & CC_V_BIT ? 'V' : 'v');
    printf ("  IRQs %s  FIQs %s  Mode %s%s\r\n",
        interrupts_enabled (regs) ? "on" : "off",
        fast_interrupts_enabled (regs) ? "on" : "off",
        processor_modes[processor_mode (regs)],
        thumb_mode (regs) ? " (T)" : "");
}

void do_data_abort(struct pt_regs *pt_regs)
{
    printf ("[%s]:data abort\r\n", __func__);
    u32 cpuid = get_cpuid();
    printf("cpu[%d]do_data_abort\r\n", cpuid);
    show_regs (pt_regs);
    bad_mode ();
}

void do_undefined_instruction(struct pt_regs *pt_regs)
{   
    printf ("do_undefined_instruction\r\n");
    u32 cpuid = get_cpuid();
    printf("cpu[%d]do_undefined_instruction\r\n", cpuid);
    show_regs (pt_regs);
    bad_mode ();    
}

void do_software_interrupt(struct pt_regs *pt_regs)
{   
    printf ("do_software_interrupt\r\n");
    u32 cpuid = get_cpuid();
    printf("cpu[%d]do_software_interrupt\r\n", cpuid);
    show_regs (pt_regs);
    bad_mode ();    
}

void do_prefetch_abort(struct pt_regs *pt_regs)
{   
    printf ("do_prefetch_abort\r\n");
    u32 cpuid = get_cpuid();
    printf("cpu[%d]do_prefetch_abort\r\n", cpuid);
    show_regs (pt_regs);
    bad_mode ();    
}

void do_not_used(struct pt_regs *pt_regs)
{   
    printf ("not used\r\n");
    u32 cpuid = get_cpuid();
    printf("cpu[%d]do_not_used\r\n", cpuid);
    show_regs (pt_regs);
    bad_mode ();    
}

void do_irq(void)
{
    s5p4418_InterruptHandler();
}

void do_fiq(void)
{
    printf("do_fiq\r\n");
}
