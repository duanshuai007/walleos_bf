/**
 *2017-10-09 11:26
 */
#include <mmu/s5p4418_mmu.h>
#include <smp_macros.h>
#include <stdio.h>
#include <cache_v7.h>
#include <cache_cp15.h>
#include <types.h>
#include <string.h>
#include <mmu/mmu_asm.h>
#include <s5p4418_serial_stdio.h>
#include <config.h>
#include <io.h>

//栈底
extern unsigned long _stack_start;

u32 PAGE_TABLE_START				= 0;
static const u32 PAGE_TABLE_SIZE    = 0x0000c000;

static inline u32 GET_START(const u32 entry)
{
    return (((entry - MMU_TABLE_SIZE) & 0xffff0000) - 0x10000);
}

enum {
    FLD_FAULT   = 0,
    FLD_COARSE  = 1,
    FLD_SECTION = 2,
    FLD_FINE    = 3
};

enum {
    AP_FAULT    = 0,
    AP_CLIENT   = 1,
    AP_RESERVED = 2,
    AP_MANAGER  = 3
};

enum {
    SECTION_BUFFERABLE  = 2,
    SECTION_CACHEABLE   = 3,
    SECTION_SBO         = 4,
    SECTION_DOMAIN      = 5,
    SECTION_AP          = 10,
    SECTION_SHARED      = 16,
};

//调试信息，用来打印mmu页表的起始地址
unsigned long mmu_table_start;
/*------------------------------------------------------------------------------
 * Virtual Map Address Table
 *  mmu地址空间映射表，按照一一映射的方式将2G内存全映射出来
 *  
 */

static const u32 ptable[] = {
// Non-Cache,   Cache,          Physical  ,     Size(MB),
#ifdef DMA_SIZE
	0x00000000,		0x40000000,		0x40000000,		  (2048 - DMA_SIZE),
	MAX_MEMORY - (DMA_SIZE * SIZE_1MB),     //non-Cahce
					0x00000000,				//Cache
									MAX_MEMORY - (DMA_SIZE * SIZE_1MB),
													  (DMA_SIZE),
#else
	0x00000000,		0x40000000,		0x40000000,		  1024,		/*DDR*/
	0x00000000,		0x80000000,		0x80000000,		  1024,
#endif
	0xC0000000,              0,     0xC0000000,         3,      /* NOMAL IO, Reserved */
    0xF0000000,              0,     0xF0000000,         1,      /* Cortex-A9 MPcore Private Memory Region (SCU) */
    0xFFF00000,              0,     0xFFF00000,         1,      /* SRAM : VECTOR */
    0xCF000000,              0,     0xCF000000,         1,      /* PL310 L2 Cache */
    0x2C000000,              0,     0x2C000000,         1,      /* NAND  */
    0x04000000,              0,     0x04000000,         1,      /* CS 1 */
    0x00000000,              0,     0x00000000,         1,      /* CS 0 对地址0x00000000未映射*/
             0,              0,              0,         0       /* PAGE_END */
};

static void make_page_table(u32 *ptable)
{
    int index;
    unsigned int virt, phys, num_of_MB;
    unsigned int mode, i, addr, data;
    unsigned int *table;

#define     ACCESS_ALIGN    4
#define     MODE_SECTION    2
#define     MODE_OFS        0
#define     BUFFER_OFS      2
#define     CACHE_OFS       3
#define     XN_OFS          4
#define     DOMAIN_OFS      5
#define     P_OFS           9
#define     AP_OFS          10
#define     TEX_OFS         12
#define     APX_OFS         15
#define     S_OFS           16
#define     NG_OFS          17
#define     NS_OFS          19
#define     SB_ADDR_OFS     20

    table = (u32*)ptable;

    DEBUG( MMU_DEBUG, "PAGE_TABLE_START = %08x\r\n", PAGE_TABLE_START);
    /* Clear page table zone (16K). */
    memset((char*)PAGE_TABLE_START, 0x00, MMU_TABLE_SIZE);

    /*
     * ROM Base: Non-Cacheable, NO-Bufferable
     */
    index = 0;
    //mode = (0<<SECTION_CACHEABLE) | (0<<SECTION_BUFFERABLE) | (FLD_SECTION<<0); // No Cachable & No Bufferable
#ifdef SMP_CACHE_COHERENCY
    mode = (MODE_SECTION << MODE_OFS) | (1 << BUFFER_OFS) | (1 << XN_OFS) | (3<< AP_OFS);
    //mode = 0xC16;       //0b1100 0001 0110 对应段描述符=0b10，B=1,C=0,XN=0,Domain=0,P=0,AP=0b11, XN=0包含可执行代码，=1不包含可执行代码，P->ECC校验
#else
    mode = mode | AP_CLIENT<<SECTION_AP;    /* set kernel R/W permission */
#endif

    virt = 0, phys = 0;
    num_of_MB = 1;
    //第一个页表项addr = PAGE_TABLE_START + 0 = 0x42C80000
    //phys = 0x00000000 --- 0x00100000
    addr = PAGE_TABLE_START + ((virt>>20)*ACCESS_ALIGN);
    for (i=0; i < num_of_MB; i++) {
        data = phys | mode;
        *(volatile unsigned int *)(addr)= data;
        phys += SIZE_1MB;  //物理地址对应1mb
        addr += ACCESS_ALIGN;   //每个页表占4字节
    }

    /*
     * Cacheable, Bufferable, R/W
     */
    index = 0;
    //mode = (1<<SECTION_CACHEABLE) | (1<<SECTION_BUFFERABLE) | (FLD_SECTION<<0); // Cachable & Bufferable
#ifdef SMP_CACHE_COHERENCY
    //mode = 0x15C06;     //0b 0001 0101 1100 0000 0110 对应描述符 映射类型0b10:分段式，B=1，C=0，XN=0，Domain=0，P=0，AP=0b11，TEX=0b101(支持写时分配cache)，APX=0，S=1，nG=0
    //mode = 0x15C06 | (1 << SECTION_CACHEABLE);  /* Cacheable */

    mode = (MODE_SECTION << MODE_OFS)|(1<<BUFFER_OFS)|(1<<CACHE_OFS)|(3<<AP_OFS)|(5<<TEX_OFS)|(1<<S_OFS);
#else
    mode = mode | AP_CLIENT<<SECTION_AP;                    // set kernel R/W permission
#endif

    while (1) {
        //temp = *(table+index++) & 0xfff<<20;
        index++;
        virt = *(table+index++) & 0xfff<<20;
        phys = *(table+index++) & 0xfff<<20;
        num_of_MB = *(table+index++);

        if (num_of_MB == 0)
            break;

        if (0 != virt) {
        //addr = PAGE_TABLE_START + 0x800*4 = 0x42C80000+0x2000 = 0x42C82000
        //addr = 0x42C820000 --- 0x42C83000
        //第2k-第3k个页表项
        //phys = 0x80000000 - 0x80100000 - 0x80200000 --- 0xC0000000
        //virt = 0x80000000 --- 0xC0000000
            addr = PAGE_TABLE_START + ((virt>>20)*ACCESS_ALIGN);
            for (i=0; i < num_of_MB; i++) {
                data = phys | mode;
                *(volatile unsigned int *)(addr)= data;
                phys += SIZE_1MB;  //物理地址+1MB
                addr += ACCESS_ALIGN;   //页表地址对应4字节
            }
        }
    }

    /*
     * No Cacheable, No Bufferable, R/W
     */
    index = 0;
    //mode = (0<<SECTION_CACHEABLE) | (0<<SECTION_BUFFERABLE) | (FLD_SECTION<<0); // No Cachable & No Bufferable
#ifdef SMP_CACHE_COHERENCY
    //mode = 0xC16;       //0b1100 0001 0110 分段式，B=1，C=0，XN=1(不包含可执行代码)，Domain=0，P=0，AP=0b11
    mode = (MODE_SECTION << MODE_OFS) | (0 << BUFFER_OFS) | (0 << CACHE_OFS) | (0 << XN_OFS) | (3<< AP_OFS);
#else
    mode = mode | AP_CLIENT<<SECTION_AP;                    // set kernel R/W permission
#endif

    while (1) {
        virt = *(table+index++) & 0xfff<<20;
        //temp = *(table+index++) & 0xfff<<20;
        index++;
        phys = *(table+index++) & 0xfff<<20;
        num_of_MB = *(table+index++);

        if (num_of_MB==0)
            break;

        if (0 != virt) {
            addr = PAGE_TABLE_START + ((virt>>20)*ACCESS_ALIGN);
            for (i=0; i < num_of_MB; i++) {
                data = phys | mode;
                *(volatile unsigned int *)(addr)= data;
                phys += SIZE_1MB;
                addr += ACCESS_ALIGN;
            }
        }
    }
    return;
}

#if defined (SMP_SCU_ENABE)

#if 0
#define read_cpuid()    \
    ({  \
     unsigned int __val;                 \
     asm("mrc p15, 0, %0, c0, c0, 0" : "=r" (__val) : : "cc"); \
     __val;                          \
     })
#endif

void scu_enable(void *scu_base)
{
    u32 scu_ctrl;

    const u32 CUPID_MASK    = 0xff0ffff0;
    const u32 CUPID         = 0x410fc090;
    const u32 CTRL_OFS      = 0x30;

#ifdef CONFIG_ARM_ERRATA_764369
    /* Cortex-A9 only */
    if ((read_cpuid(0) & CUPID_MASK) == CUPID) {
        scu_ctrl = readl(scu_base + CTRL_OFS);
        if (!(scu_ctrl & 1))
            //__raw_writel(scu_ctrl | 0x1, scu_base + 0x30);
            write32(scu_base + CTRL_OFS, scu_ctrl | 0x1);
    }
#endif

    scu_ctrl = readl(scu_base + SCU_CTRL);
    /* already enabled? */
    if (scu_ctrl & 1)
        return;

    scu_ctrl |= 1;
    //writel(scu_ctrl, scu_base + SCU_CTRL);
    writel(scu_base + SCU_CTRL, scu_ctrl);
    /*
     * Ensure that the data accessed by CPU0 before the SCU was
     * initialised is visible to the other CPUs.
     */
    flush_dcache_all();
}
#endif

void mmu_on(void)
{
    DEBUG( MMU_DEBUG, "PAGE_TABLE_START = %08x\r\n", PAGE_TABLE_START);
    PAGE_TABLE_START = GET_START(_stack_start);
    mmu_table_start = PAGE_TABLE_START;
    /*禁止dcache并且清空所有的dcache*/
    dcache_disable();
    /*使dcache和tlb无效*/
    arm_init_before_mmu();              /* Flush DCACHE */
    /* copy vector table */
    //  memcpy(vector_base, (void const *)CONFIG_SYS_TEXT_BASE, 64);
    mmu_page_table_flush(PAGE_TABLE_START, PAGE_TABLE_SIZE);
    make_page_table((u32*)ptable);      /*  Make MMU PAGE TABLE */

    DEBUG( MMU_DEBUG, "enable_mmu, PAGE_TABLE_START = %08x\r\n", PAGE_TABLE_START);
    //调用mmu_asm中的函数
    enable_mmu(PAGE_TABLE_START);
    DEBUG( MMU_DEBUG, "enable_mmu end\r\n");
#if defined (SMP_SCU_ENABE)
    scu_enable((void *)MPPR_REG);
#endif
}

void mmu_off(void)
{
    //禁止dcache并且清空所有的dcache
    dcache_disable();
    //使dcache和tlb无效
    arm_init_before_mmu();              /* Flush DCACHE */
    /* copy vector table */
    //  memcpy(vector_base, (void const *)CONFIG_SYS_TEXT_BASE, 64);
    mmu_page_table_flush(PAGE_TABLE_START, PAGE_TABLE_SIZE);

    disable_mmu();
}
