/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <config.h>
#include <types.h>
#include <io.h>
#include <interrupt.h>
#include <uspienv.h>
#include <stddef.h>
#include <alloc.h>
#include <mmu/s5p4418_mmu.h>
#include <graphic/graphic.h>
#include <scheduler.h>
#include "idletask.h"
#include "neverstoptask.h"
#include <uspios.h>
#include <timeslicetimerhandler.h>
#include <synchronize.h>
#include <s5p4418_clk.h>
#include <s5p4418_gpio.h>
#include <s5p4418_pwm.h>
#include <s5p4418_serial.h>
#include <s5p4418_serial_stdio.h>
#include <s5p4418_timer.h>
#include <i2c/i2c_util.h>
#include <lcd/lcd.h>
#include "gesturetask.h"
#include "dtouchtimertask.h"
#include <fs/file.h>
#include <alloc.h>
#include <s5p4418_tick_delay.h>
#include <smp.h>
#include <stdio.h>
#include <drivers/mmc.h>
#include <s5p4418_led.h>
#include <errno.h>
#include <string.h>
#include <drivers/board_r.h>
#include <lwip/ip_addr.h>
#include <lwip/ip4_addr.h>
#include <lwip/apps/net_main.h>
#include <drivers/ethernet/nx_gmac.h>
#include <smp_macros.h>
#include <global_timer.h>

#include <stdarg.h>

int init_mmc_for_env(struct mmc *mmc);

extern unsigned long _text_start;
extern unsigned long _text_end;
extern unsigned long _data_start;
extern unsigned long _data_end;
extern unsigned long _bss_start;
extern unsigned long _bss_end;
extern unsigned long _stack_fiq_end;
extern unsigned long _stack_irq_end;
extern unsigned long _stack_sys_end;
extern unsigned long _stack_abt_end;
extern unsigned long _stack_und_end;
extern unsigned long _stack_svc_end;

extern unsigned long _stack_irq_end1;
extern unsigned long _stack_sys_end1;

extern unsigned long _stack_irq_end2;
extern unsigned long _stack_sys_end2;

extern unsigned long _stack_irq_end3;
extern unsigned long _stack_sys_end3;

extern unsigned long heap_start;
extern unsigned long heap_size;
extern unsigned long _stack_start;
extern unsigned long mmu_table_start;

#ifdef SERIAL_FIFO
extern struct serial_msg s_msg;
#endif

u32 test = 0;

int hello_world (int argc, char * const argv[])
{
    UNUSED(argc);
    UNUSED(argv);
#ifdef LOCK
    initPrintLock();    
#endif

#ifdef SERIAL_FIFO
    init_msg_queue(&s_msg);
#endif

#ifdef SMP
    smp_cpu_base_init(); 
#endif
    s5p4418_clk_init();
    s5p4418_gpiochip_init();
    led_init();
    s5p4418_serial_initial();

    s5p4418_interrupt_init();
    s5p4418_timer_init();   //timer1 init
    bd_display_run();
    graphic_init(FALSE);
    if(!USPiEnvInitialize())
    {
        return 0;
    }
    
#ifdef SMP
    /*
	 *	配置smp cpu该段代码内禁止加入其他程序
	 */
    platform_smp_prepare_cpus();
    smp_setup_processor_id();
    gic_init_bases();
	/*
     *	配置smp cpu 结束
     */
#endif
    s5p4418_tick_delay_initial();
    i2c_init(); 

    mdelay(2000);
    printk_init();

    printk("sizeof(u64) = %d\r\n", sizeof(u64));
    printk("sizeof(s64) = %d\r\n", sizeof(s64));
    printk("sizeof(s64_t) = %d\r\n", sizeof(s64_t));
    printk("sizeof(ulong) = %d\r\n", sizeof(ulong));
    printk("sizeof(long) = %d\r\n", sizeof(long));
    printk("sizeof(u32) = %d\r\n", sizeof(u32));
    printk("sizeof(u8)  = %d\r\n", sizeof(u8));
    printk("sizeof(double) = %d\r\n", sizeof(double));
    printk("_bnd(u64, _AUPBND) = %d\r\n", _bnd(u64, _AUPBND));
    printk("sizeof(char *) = %d\r\n", sizeof(char *));



    printf("[text]\t:{ %08x --- %08x }\r\n", _text_start, _text_end);
    printf("[data]\t:{ %08x --- %08x }\r\n",_data_start, _data_end);
    printf("[bss]\t:{ %08x --- %08x }\r\n", _bss_start, _bss_end);
    printf("[mmu]\t:{ %08x --- %08x }\r\n", mmu_table_start, mmu_table_start + (16*1024));
    printf("[irq]\t:{ %08x --- %08x }\r\n",  _stack_fiq_end, _stack_irq_end );
    printf("[fiq]\t:{ %08x --- %08x }\r\n",  _stack_sys_end, _stack_fiq_end);
    printf("[sys]\t:{ %08x --- %08x }\r\n",  _stack_abt_end, _stack_sys_end);
    printf("[abt]\t:{ %08x --- %08x }\r\n",  _stack_und_end, _stack_abt_end);
    printf("[und]\t:{ %08x --- %08x }\r\n",  _stack_svc_end, _stack_und_end);
    printf("[svc]\t:{ %08x --- %08x }\r\n",  _stack_irq_end1, _stack_svc_end);
    printf("[irq1]\t:{ %08x --- %08x }\r\n",  _stack_sys_end1, _stack_irq_end1 );
    printf("[sys1]\t:{ %08x --- %08x }\r\n",  _stack_irq_end2, _stack_sys_end1);
    printf("[irq2]\t:{ %08x --- %08x }\r\n",  _stack_sys_end2, _stack_irq_end2 );
    printf("[sys2]\t:{ %08x --- %08x }\r\n",  _stack_irq_end3, _stack_sys_end2);
    printf("[irq3]\t:{ %08x --- %08x }\r\n",  _stack_sys_end3, _stack_irq_end3 );
    printf("[sys3]\t:{ %08x --- %08x }\r\n",  _stack_start, _stack_sys_end3);
    printf("[heap]\t:{ %08x --- %08x }\r\n", heap_start, heap_start + heap_size);

#if 1 
    board_init_r();
    
    struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
    if (init_mmc_for_env(mmc))
    {
        printf("init_mmc_for_env failed\r\n");
        return 1;   
    }
    printf("init_mmc_for_env succeed\r\n");

    if (!fmount())
    {
        printf ("main : Cannot mount sdcard\r\n");
        return 0;
    }
    printf("fmount success\r\n");
#endif

   //启用private timer替代定时器工作，关闭原定时器
    s5p4418_timer_Deinit();
    writel(PRIVATE_TIMER_BASE + TWD_TIMER_COUNTER, TWD_COUNT - 1);
    writel(PRIVATE_TIMER_BASE + TWD_TIMER_LOAD, TWD_COUNT - 1);
    writel(PRIVATE_TIMER_BASE + TWD_TIMER_CONTROL,
            TWD_TIMER_CONTROL_ENABLE | TWD_TIMER_CONTROL_PERIODIC | TWD_TIMER_CONTROL_IT_ENABLE | ((TWD_TIMER_PRESCALER - 1) << 8));

	global_timer.prescaler = GLOBAL_TIMER_PRESCALER;
	global_timer.auto_increment_enable = DISABLE;
	global_timer.irq_enable	= DISABLE;
	global_timer.comp_enable = DISABLE;
	global_timer.enable = ENABLE;
	global_timer.count.upper = GLOBAL_TIMER_COUNTH;
	global_timer.count.lower = GLOBAL_TIMER_COUNTL;
	global_timer.comp.upper = GLOBAL_TIMER_COMPH;
	global_timer.comp.lower = GLOBAL_TIMER_COMPL;
	global_timer.irq_counts = 0;
	global_timer.auto_increment_value = GLOBAL_TIMER_INC_VALUE;
	global_timer_init(&global_timer);

#ifdef SMP
    cpu_up(3);
    cpu_up(2);
    cpu_up(1);
#endif

    Scheduler();

    TScheduler * pScheduler = ScheduerGet();
    TTask * pTask = pScheduler->m_pCurrent;
 
    printf("new idle task\r\n");
    new_IdleTask();

    for (unsigned i = 1; i <= 4; i ++)
    {
        new_NeverStopTask(i, i);
    }

#if 1 
    printf("------------Test Print-------------\r\n");
    //printk("longlong temp = %d, %d, %llu\r\n", 12, 39032, 39519158473268);
    //printk("longlong temp = %llu, %d, %d\r\n", 39519158473268, 321, 689);
    //printk("longlong temp = %llu, %d, %llu\r\n", 39519158473268, 3213, 1928392082239);
    //printk("longlong temp = %llu, %llu, %d\r\n", 39519158473268, 321213213213, 1239);
    //printk("longlong temp = %llu, %llu, %d\r\n", 19870209123, 321213213213, 1239);
    printf("temp = %llu\r\n", (u64)19870209);
    printk("temp = %llu\r\n", (u64)19870209);
    printf("temp = %llu, %d\r\n", 1987019292922, 123);
    u64 val10 = 19283932289394;
    u32 val11 = 3219;
    u64 val12 = 1932918;
    printk("longlong val = %llu\r\n", val10);
    printk("longlong val = %llu\r\n", val12);
    printf("longlong val = %llu, %d, %llu\r\n", val10, val11, val12);
    printf("ll val = %llu, %llu\r\n", val10, val12);
    printk("longlong val = %llu, %d, %llu\r\n", val10, val11, val12);
    printk("ll val = %llu, %llu\r\n", val10, val12);
    printf("------------Test Print end-------------\r\n");
#endif

    setSchedulerTimer(StartKernelTimer(0.1 * HZ, TimeSliceTimerHandler, 0, 0)); 
    SchedulerBlockTask(&pTask);
    printf ("Hit any key to exit ... \r\n");
	while(1);

    return 0;
}

int init_mmc_for_env(struct mmc *mmc)
{
    if (!mmc) {
        printf("No MMC card found\n\n");
        return -1; 
    }   

    if (mmc_init(mmc)) {
        printf("MMC init failed\n\n");
        return -1; 
    }   

    return 0;
}

