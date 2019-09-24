#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

#include <drivers/s5p4418_drone2.h> 
/*****
    配置系统烧入地址，实际地址需要在arm.ld中进行配置
 *****/
#define CONFIG_SYS_TEXT_BASE    0x42d00000

#define MAX_MEMORY              0xC0000000
#define MEM_HEAP_START          0x41c00000
//配置dma内存大小---no cache
//如果不使用DMA,将下行注释到即可
//单位是MB
#define DMA_SIZE                256

#define DO_TICK_CPU             0	//cpu_id
#define SMP_CACHE_COHERENCY
#define SMP_SCU_ENABE
#define CONFIG_ARM_ERRATA_764369
#define SIZE_1MB                0x100000
#define MMU_TABLE_SIZE          16*1024

#define DEBUG_ON
#define MMU_DEBUG               FALSE
#define ALLOC_DEBUG             FALSE
#define GESTURE_ON              FALSE
#define TOUCH_ON                FALSE
#define TOUCH_INT_ON            FALSE
#define KEY_INT_ON              FALSE 
#define SCHEDUER_ON             FALSE
#define INT_ON                  FALSE
#define PWM_ON                  FALSE
#define GPIO_ON                 FALSE
#define CLK_ON                  FALSE
#define RSTGEN_ON               FALSE

#define PART_DEBUG
//  页内存申请使能
#define MEM_PAGE_ALLOC
//  使能内存申请过程中的临界保护
#define ALLOC_ENABLE_CRITICAL

#define CONFIG_PARTITIONS       1
//  设置多核心
#define SMP
#ifdef SMP
#define CPU_MAX_NUM 4
#else
#define CPU_MAX_NUM 1
#endif

#define LOCK
#define SMP_MMU                 
#define CPU1_INTERRUPT          //关闭CPU1的中断,打开会导致程序运行不正常
#define CPU2_INTERRUPT          
#define CPU3_INTERRUPT          
//private timer config
#define TWD_TIMER_PRESCALER     200
#define TWD_COUNT               HZ
//global timer config
#define GLOBAL_TIMER_PRESCALER  200
#define GLOBAL_TIMER_COUNTH     0
#define GLOBAL_TIMER_COUNTL     1000
#define GLOBAL_TIMER_COMPH      0
#define GLOBAL_TIMER_COMPL      1000
#define GLOBAL_TIMER_INC_VALUE  1000
//for every process global timer config
#define CPU0_GLOBAL_TIMER_IRQ   FALSE
#define CPU1_GLOBAL_TIMER_IRQ   FALSE
#define CPU2_GLOBAL_TIMER_IRQ   FALSE
#define CPU3_GLOBAL_TIMER_IRQ   FALSE
#define	CPU0_PRIVATE_TIMER_IRQ  TRUE
#define CPU0_GIC_VIC_IRQ        TRUE
#define CPU1_PRIVATE_TIMER_IRQ  TRUE
#define CPU1_GIC_VIC_IRQ        TRUE
#define CPU2_PRIVATE_TIMER_IRQ  TRUE
#define CPU2_GIC_VIC_IRQ        TRUE
#define CPU2_PRIVATE_TIMER_IRQ  TRUE
#define CPU2_GIC_VIC_IRQ        TRUE
#define TEST                    FALSE
//#define DELAY_SWITCH			
/**
  FONT MARCOS
 **/
#define CHARWIDTH               8
#define CHARHEIGHT              16
#define CHAREXTRAHEIGHT         3
/**
  定义打印输出宏
 **/
#define SERIAL_FIFO             ENABLE

#ifdef DEBUG_ON
#define DEBUG(enable, fmt, ...) \
	    do{\
		if(enable == TRUE){\
        printk(fmt, ##__VA_ARGS__);}}while(0)
#else
#define DEBUG(fmt, ...)
#endif

#endif
