#ifndef _ENV_INCLUDE_SYSTEM_H_
#define _ENV_INCLUDE_SYSTEM_H_

enum cpu_arch{
    CPU_ARCH_UNKNOWN  =  0,
    CPU_ARCH_ARMv3    =  1,
    CPU_ARCH_ARMv4    =  2,
    CPU_ARCH_ARMv4T   =  3,
    CPU_ARCH_ARMv5    =  4,
    CPU_ARCH_ARMv5T   =  5,
    CPU_ARCH_ARMv5TE  =  6,
    CPU_ARCH_ARMv5TEJ =  7,
    CPU_ARCH_ARMv6    =  8,
    CPU_ARCH_ARMv7    =  9,
};
/*
 * CR1 bits (CP#15 CR1)
 */
enum cr{
    CR_M   = (1 << 0),    /* MMU enable               */
    CR_A   = (1 << 1),    /* Alignment abort enable       */
    CR_C   = (1 << 2),    /* Dcache enable            */
    CR_W   = (1 << 3),    /* Write buffer enable          */
    CR_P   = (1 << 4),    /* 32-bit exception handler     */
    CR_D   = (1 << 5),    /* 32-bit data address range        */
    CR_L   = (1 << 6),    /* Implementation defined       */
    CR_B   = (1 << 7),    /* Big endian               */
    CR_S   = (1 << 8),    /* System MMU protection        */
    CR_R   = (1 << 9),    /* ROM MMU protection           */
    CR_F   = (1 << 10),   /* Implementation defined       */
    CR_Z   = (1 << 11),   /* Implementation defined       */
    CR_I   = (1 << 12),   /* Icache enable            */
    CR_V   = (1 << 13),   /* Vectors relocated to 0xffff0000  */
    CR_RR  = (1 << 14),   /* Round Robin cache replacement    */
    CR_L4  = (1 << 15),   /* LDR pc can set T bit         */
    CR_DT  = (1 << 16),
    CR_IT  = (1 << 18),
    CR_ST  = (1 << 19),
    CR_FI  = (1 << 21),   /* Fast interrupt (lower latency mode)  */
    CR_U   = (1 << 22),   /* Unaligned access operation       */
    CR_XP  = (1 << 23),   /* Extended page tables         */
    CR_VE  = (1 << 24),   /* Vectored interrupts          */
    CR_EE  = (1 << 25),   /* Exception (Big) Endian       */
    CR_TRE = (1 << 28),   /* TEX remap enable         */
    CR_AFE = (1 << 29),   /* Access flag enable           */
    CR_TE  = (1 << 30),   /* Thumb exception enable       */
};

/*
 * This is used to ensure the compiler did actually allocate the register we
 * asked it for some inline assembly sequences.  Apparently we can't trust
 * the compiler from one version to another so a bit of paranoia won't hurt.
 * This string is meant to be concatenated with the inline asm string and
 * will cause compilation to stop on mismatch.
 * (for details, see gcc PR 15089)
 */
#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

#ifndef __ASSEMBLY__

#define isb() __asm__ __volatile__ ("" : : : "memory")

#define nop() __asm__ __volatile__("mov\tr0,r0\t@ nop\n\t");

#ifdef __ARM_ARCH_7A__
#define wfi() __asm__ __volatile__ ("wfi" : : : "memory")
#else
#define wfi()
#endif

static inline unsigned int get_cr(void)
{
    unsigned int val;
    asm("mrc p15, 0, %0, c1, c0, 0  @ get CR" : "=r" (val) : : "cc");
    return val;
}

static inline void set_cr(unsigned int val)
{
    asm volatile("mcr p15, 0, %0, c1, c0, 0 @ set CR"
            : : "r" (val) : "cc");
    isb();
}

static inline unsigned int get_dacr(void)
{
    unsigned int val;
    asm("mrc p15, 0, %0, c3, c0, 0  @ get DACR" : "=r" (val) : : "cc");
    return val;
}

static inline void set_dacr(unsigned int val)
{
    asm volatile("mcr p15, 0, %0, c3, c0, 0 @ set DACR"
            : : "r" (val) : "cc");
    isb();  // 内存屏障
}

/* options available for data cache on each page */
enum dcache_option {
    DCACHE_OFF = 0x12,
    DCACHE_WRITETHROUGH = 0x1a,
    DCACHE_WRITEBACK = 0x1e,
};

/* Size of an MMU section */
enum {
    MMU_SECTION_SHIFT   = 20,
    MMU_SECTION_SIZE    = 1 << MMU_SECTION_SHIFT,
};

/**
 * Change the cache settings for a region.
 *
 * \param start     start address of memory region to change
 * \param size     size of memory region to change
 * \param option  dcache option to select
 */
void mmu_set_region_dcache_behaviour(unsigned int start, int size,
        enum dcache_option option);

/**
 * Register an update to the page tables, and flush the TLB
 *
 * \param start     start address of update in page table
 * \param stop     stop address of update in page table
 */
//void mmu_page_table_flush(unsigned long start, unsigned long stop);

#endif /* __ASSEMBLY__ */

#define arch_align_stack(x) (x)

#endif

