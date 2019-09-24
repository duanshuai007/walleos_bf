/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * SPDX-License-Identifier:  GPL-2.0+
 */
#include <cache_v7.h>
#include <armv7.h>
#include <types.h>

/*
 * Write the level and type you want to Cache Size Selection Register(CSSELR)
 * to get size details from Current Cache Size ID Register(CCSIDR)
 */
void set_csselr(u32 level, u32 type)
{   
    u32 csselr = level << 1 | type;
    /* Write to Cache Size Selection Register(CSSELR) */
    asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
}

u32 get_ccsidr(void)
{
    u32 ccsidr;

    /* Read current CP15 Cache Size ID Register */
    asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
    return ccsidr;
}

u32 get_clidr(void)
{
    u32 clidr;

    /* Read current CP15 Cache Level ID Register */
    asm volatile ("mrc p15,1,%0,c0,c0,1" : "=r" (clidr));
    return clidr;
}

void v7_inval_dcache_level_setway(u32 level, u32 num_sets,
        u32 num_ways, u32 way_shift,
        u32 log2_line_len)
{
    int way, set, setway;

    /*
     * For optimal assembly code:
     *  a. count down
     *  b. have bigger loop inside
     */
    for (way = num_ways - 1; way >= 0 ; way--) 
    {
        for (set = num_sets - 1; set >= 0; set--) 
        {
            setway = (level << 1) | (set << log2_line_len) |
                (way << way_shift);
            /* Invalidate data/unified cache line by set/way */
            asm volatile (" mcr p15, 0, %0, c7, c6, 2"
                    : : "r" (setway));
        }
    }
    /* DSB to make sure the operation is complete */
    CP15DSB;
}

void v7_clean_inval_dcache_level_setway(u32 level, u32 num_sets,
        u32 num_ways, u32 way_shift,
        u32 log2_line_len)
{
    int way, set, setway;

    /*
     * For optimal assembly code:
     *  a. count down
     *  b. have bigger loop inside
     */
    for (way = num_ways - 1; way >= 0 ; way--) {
        for (set = num_sets - 1; set >= 0; set--) {
            setway = (level << 1) | (set << log2_line_len) |
                (way << way_shift);
            /*
             * Clean & Invalidate data/unified
             * cache line by set/way
             */
            asm volatile (" mcr p15, 0, %0, c7, c14, 2"
                    : : "r" (setway));
        }
    }
    /* DSB to make sure the operation is complete */
    CP15DSB;
}

#define s32 signed int

s32 log_2_n_round_up(u32 n)
{
    s32 log2n = -1; 
    u32 temp = n;

    while (temp) {
        log2n++;
        temp >>= 1;
    }   

    if (n & (n - 1)) 
        return log2n + 1; /* not power of 2 - round up */
    else
        return log2n; /* power of 2 */
}

void v7_maint_dcache_level_setway(u32 level, u32 operation)
{
    u32 ccsidr;
    u32 num_sets, num_ways, log2_line_len, log2_num_ways;
    u32 way_shift;
    //设置cache size selection register
    set_csselr(level, ARMV7_CSSELR_IND_DATA_UNIFIED);
    //读取cache size id register
    ccsidr = get_ccsidr();
    //这里不太理解为什么+2
    log2_line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
            CCSIDR_LINE_SIZE_OFFSET) + 2;
    /* Converting from words to bytes */
    log2_line_len += 2;

    num_ways  = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK) >>
            CCSIDR_ASSOCIATIVITY_OFFSET) + 1;
    num_sets  = ((ccsidr & CCSIDR_NUM_SETS_MASK) >>
            CCSIDR_NUM_SETS_OFFSET) + 1;
    /*
     * According to ARMv7 ARM number of sets and number of ways need
     * not be a power of 2
     */
    //求num_ways的是2的多少次幂，比如输入4，输出2，向上对齐。，输入5，则返回3.
    log2_num_ways = log_2_n_round_up(num_ways);

    way_shift = (32 - log2_num_ways);
    if (operation == ARMV7_DCACHE_INVAL_ALL) {
        v7_inval_dcache_level_setway(level, num_sets, num_ways,
                way_shift, log2_line_len);
    } else if (operation == ARMV7_DCACHE_CLEAN_INVAL_ALL) {
        v7_clean_inval_dcache_level_setway(level, num_sets, num_ways,
                way_shift, log2_line_len);
    }
}

void v7_maint_dcache_all(u32 operation)
{
    u32 level, cache_type, level_start_bit = 0;
    //获取cache level ID register
    u32 clidr = get_clidr();

    for (level = 0; level < 7; level++) {
        cache_type = (clidr >> level_start_bit) & 0x7;
        if ((cache_type == ARMV7_CLIDR_CTYPE_DATA_ONLY) ||
                (cache_type == ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA) ||
                (cache_type == ARMV7_CLIDR_CTYPE_UNIFIED))
            v7_maint_dcache_level_setway(level, operation);
        level_start_bit += 3;
    }
}

void v7_dcache_clean_inval_range(u32 start,
        u32 stop, u32 line_len)
{
    u32 mva;

    /* Align start to cache line boundary */
    start &= ~(line_len - 1);
    for (mva = start; mva < stop; mva = mva + line_len) {
        /* DCCIMVAC - Clean & Invalidate data cache by MVA to PoC */
        asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (mva));
    }
}

void v7_dcache_inval_range(u32 start, u32 stop, u32 line_len)
{
    u32 mva;

    /*
     * If start address is not aligned to cache-line do not
     * invalidate the first cache-line
     */
    if (start & (line_len - 1)) {
      //  printf(0, "ERROR: %s - start address is not aligned - 0x%08x\n",
        //        __func__, start);
        /* move to next cache line */
        start = (start + line_len - 1) & ~(line_len - 1);
    }

    /*
     * If stop address is not aligned to cache-line do not
     * invalidate the last cache-line
     */
    if (stop & (line_len - 1)) {
       // printf(0, "ERROR: %s - stop address is not aligned - 0x%08x\n",
         //       __func__, stop);
        /* align to the beginning of this cache line */
        stop &= ~(line_len - 1);
    }

    for (mva = start; mva < stop; mva = mva + line_len) {
        /* DCIMVAC - Invalidate data cache by MVA to PoC */
        asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (mva));
    }
}

void v7_dcache_maint_range(u32 start, u32 stop, u32 range_op)
{
    u32 line_len, ccsidr;
    //读取cache size id register
    ccsidr = get_ccsidr();
    //获取line_len
    line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
            CCSIDR_LINE_SIZE_OFFSET) + 2;
    /* Converting from words to bytes */
    line_len += 2;
    /* converting from log2(linelen) to linelen */
    line_len = 1 << line_len;

    switch (range_op) {
        case ARMV7_DCACHE_CLEAN_INVAL_RANGE:
            v7_dcache_clean_inval_range(start, stop, line_len);
            break;
        case ARMV7_DCACHE_INVAL_RANGE:
            v7_dcache_inval_range(start, stop, line_len);
            break;
    }

    /* DSB to make sure the operation is complete */
    CP15DSB;
}

/* Invalidate TLB */
void v7_inval_tlb(void)
{
    /* Invalidate entire unified TLB */
    asm volatile ("mcr p15, 0, %0, c8, c7, 0" : : "r" (0));
    /* Invalidate entire data TLB */
    asm volatile ("mcr p15, 0, %0, c8, c6, 0" : : "r" (0));
    /* Invalidate entire instruction TLB */
    asm volatile ("mcr p15, 0, %0, c8, c5, 0" : : "r" (0));
    /* Full system DSB - make sure that the invalidation is complete */
    CP15DSB;
    /* Full system ISB - make sure the instruction stream sees it */
    CP15ISB;
}

void invalidate_dcache_all(void)
{
    //维护所有的dcache
    v7_maint_dcache_all(ARMV7_DCACHE_INVAL_ALL);
}

/*
 * Performs a clean & invalidation of the entire data cache
 * at all levels
 */
void flush_dcache_all(void)
{
    //v7架构下维修所有dcache
    v7_maint_dcache_all(ARMV7_DCACHE_CLEAN_INVAL_ALL);
}

/*
 * Invalidates range in all levels of D-cache/unified cache used:
 * Affects the range [start, stop - 1]
 */
void invalidate_dcache_range(u32 start, u32 stop)
{

    v7_dcache_maint_range(start, stop, ARMV7_DCACHE_INVAL_RANGE);
}

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified
 * cache used:
 * Affects the range [start, stop - 1]
 */
void flush_dcache_range(u32 start, u32 stop)
{
    v7_dcache_maint_range(start, stop, ARMV7_DCACHE_CLEAN_INVAL_RANGE);
}

void arm_init_before_mmu(void)
{
    //使所有的dcache无效
    invalidate_dcache_all();
    //使tlb无效
    v7_inval_tlb();
}

void mmu_page_table_flush(u32 start, u32 stop)
{
    //dcache的range管理
    flush_dcache_range(start, stop);
    //使tlb无效
    v7_inval_tlb();
}

/*
 * Flush range from all levels of d-cache/unified-cache used:
 * Affects the range [start, start + size - 1]
 */
void  flush_cache(u32 start, u32 size)
{
    flush_dcache_range(start, start + size);
}

/* Invalidate entire I-cache and branch predictor array */
void invalidate_icache_all(void)
{
    /*
     * Invalidate all instruction caches to PoU.
     * Also flushes branch target cache.
     */
    asm volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0));

    /* Invalidate entire branch predictor array */
    asm volatile ("mcr p15, 0, %0, c7, c5, 6" : : "r" (0));

    /* Full system DSB - make sure that the invalidation is complete */
    CP15DSB;

    /* ISB - make sure the instruction stream sees it */
    CP15ISB;
}

