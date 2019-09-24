/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:   GPL-2.0+
 */
#include <cache_cp15.h>
#include <system.h>
#include <cache_v7.h>

#include <types.h>

static void cp_delay (void)
{
    volatile int i;

    /* copro seems to need some delay between reading and writing */
    for (i = 0; i < 100; i++)
        nop();
    asm volatile("" : : : "memory");//内存屏障
}

/* cache_bit must be either CR_I or CR_C */
static void cache_disable(u32 cache_bit)
{
    u32 reg;
    //读取控制寄存器C1
    reg = get_cr();
    cp_delay();

    if (cache_bit == CR_C) {
        /* if cache isn;t enabled no need to disable */
        //如果cache不在使能状态，则直接返回
        if ((reg & CR_C) != CR_C)
            return;
        /* if disabling data cache, disable mmu too */
        cache_bit |= CR_M;
    }
    reg = get_cr();
    cp_delay();
    //如果cache使能了则需要清空所有cache
    if (cache_bit == (CR_C | CR_M))
        flush_dcache_all();
    //禁止dcache和mmmu
    set_cr(reg & ~cache_bit);
}

void icache_disable(void)
{
    cache_disable(CR_I);
}

int icache_status(void)
{
    return (get_cr() & CR_I) != 0;
}

void dcache_disable(void)
{
    cache_disable(CR_C);
}

int dcache_status(void)
{
    return (get_cr() & CR_C) != 0;
}

