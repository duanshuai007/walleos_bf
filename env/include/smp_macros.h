#ifndef _ENV_INCLUDE_SMP_MACROS_H_ 
#define _ENV_INCLUDE_SMP_MACROS_H_

#define GIC_DIST_BASE           0xF0001000
#define GIC_CPU_BASE            0xF0000100
#define PRIVATE_TIMER_BASE      0xF0000600
#define GLOBAL_TIMER_BASE       0xF0000200
/*
   *    0x0000 - 0x00fc         SCU register
   *    0x0100 - 0x01ff         Interrupt controller interfaces
   *    0x0200 - 0x02ff         Global timer
   *    0x0600 - 0x06ff         Private timers and watchdogs
   *    0x1000 - 0x1fff         Interrupt Distributor
   */

#define GIC_CPU_CTRL            0x00    //rw
#define GIC_CPU_PRIMASK         0x04    //rw
#define GIC_CPU_BINPOINT        0x08    //rw
#define GIC_CPU_INTACK          0x0c    //ro
#define GIC_CPU_EOI             0x10    //wo
#define GIC_CPU_RUNNINGPRI      0x14    //ro
#define GIC_CPU_HIGHPRI         0x18    //ro
#define GIC_CPU_ABPR            0x1c    //rw  Aliased Binary Point Register
#define GIC_CPU_AIAR            0x20    //ro  Aliased Interrupt Acknowledge Register
#define GIC_CPU_AEOIR           0x24    //wo  Aliased End of Interrupt Register
#define GIC_CPU_AHPPIR          0x28    //ro  Aliased Highest Priority Pending Interrupt Register

#define GIC_DIST_CTRL           0x000   //rw    GICD_CTLR
#define GIC_DIST_TYPER          0x004   //ro    GICD_TYPER
#define GIC_DIST_IIDR           0x008   //ro    
#define GIC_DIST_GROUP          0x080   //rw    GICD_IGROUPR
#define GIC_DIST_ENABLE_SET     0x100   //rw    GICD_ISENABLER
#define GIC_DIST_ENABLE_CLEAR   0x180   //rw    GICD_ICENABLER
#define GIC_DIST_PENDING_SET    0x200   //rw    GICD_ISPENDR
#define GIC_DIST_PENDING_CLEAR  0x280   //rw    GICD_ICPENDR
#define GIC_DIST_ACTIVE_BIT     0x300   //rw    GICD_ISACTIVER
#define GIC_DIST_ACTIVE_CLEAR   0x380   //rw    GICD_ICACTIVER
#define GIC_DIST_PRI            0x400   //rw    GICD_IPRIORITYR
#define GIC_DIST_TARGET         0x800   //??    GICD_ITARGETSR
#define GIC_DIST_CONFIG         0xc00   //rw    GICD_ICFGR
#define GIC_DIST_NSACR          0xe00   //rw    GICD_NSACRn
#define GIC_DIST_SOFTINT        0xf00   //wo    GICD_SGIR
#define GIC_DIST_CLEAR_PEND     0xf10   //rw    GICD_CPENDSGIR
#define GIC_DIST_SET_PEND       0xf20   //rw    GICD_SPENDSGIR

#define NXP_IRQ_PRIORITY_HIGHEST        0
#define NXP_IRQ_PRIORITY_LOWEST         15

#define NXP_IRQ_PRIORITY_TIMER          0
#define NXP_IRQ_PRIORITY_SYS_TICK       NXP_IRQ_PRIORITY_LOWEST
#define NXP_IRQ_PRIORITY_DMA            1
#define NXP_IRQ_PRIORITY_EINT           1
#define NXP_IRQ_PRIORITY_DISPLAY_UINT   2   // Display, Camera
#define NXP_IRQ_PRIORITY_STORAGE        2   // SDHC, NAND
#define NXP_IRQ_PRIORITY_AUDIO          4
#define NXP_IRQ_PRIORITY_COPROCESSOR    5   // Video codec, 3D Accelerator, Scaler
#define NXP_IRQ_PRIORITY_NOMAL_0        8
#define NXP_IRQ_PRIORITY_NOMAL_1        10
#define NXP_IRQ_PRIORITY_NOMAL_2        12
#define NXP_IRQ_PRIORITY_NOMAL_3        14


//#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")

#define smp_mb()    dmb()
#define smp_rmb()   dmb()
#define smp_wmb()   dmb()


#define __stringify_1(x)        #x
#define __stringify(x)          __stringify_1(x)

#define read_cpuid(reg)                         \
    ({                              \
     unsigned int __val;                 \
     asm("mrc    p15, 0, %0, c0, c0, " __stringify(reg)  \
         : "=r" (__val)                  \
         :                           \
         : "cc");                        \
     __val;                          \
     })
#define read_cpuid_ext(ext_reg)                     \
    ({                              \
     unsigned int __val;                 \
     asm("mrc    p15, 0, %0, c0, " ext_reg       \
         : "=r" (__val)                  \
         :                           \
         : "cc");                        \
     __val;                          \
     })


#define CPUID_ID    0
#define CPUID_CACHETYPE 1
#define CPUID_TCM   2
#define CPUID_TLBTYPE   3
#define CPUID_MPIDR 5

#define NR_CPUS     4

#define __chk_io_ptr(x)         (void)0
#define cpu_to_le32             __cpu_to_le32
#define __raw_writel(v,a)       (*(volatile unsigned int *)(a) = (v))
#define writel_relaxed(v,c)     ((void)__raw_writel((u32) (v),c))
#define PHY_BASEADDR_INTC0      (0xC0002000)
#define PHY_BASEADDR_INTC1      (0xC0003000)
#define VIC0_INT_BASE           (void *)(PHY_BASEADDR_INTC0)
#define VIC1_INT_BASE           (void *)(PHY_BASEADDR_INTC1)
#define VIC_VECT_CNTL0          0x200
#define VIC_PL192_VECT_ADDR     0xF00
#define VIC_VECT_ADDR0          0x100

#define __PB_IO_MAP_MPPR_VIRT   0xF0000000
#define GIC_CPUI_BASE           (void __iomem *)(__PB_IO_MAP_MPPR_VIRT + 0x00000100)

#define CPU_STATUS_INACTIVITY   0
#define CPU_STATUS_POSSIBLE     1
#define CPU_STATUS_INLINE       2
#define CPU_STATUS_ACTIVITY     3

#define SCR_ARM_SECOND_BOOT     0xC0010C1C  // RTC scratch
#define PHY_BASEADDR_ALIVE      0xC0010800

#define SCR_ALIVE_BASE          PHY_BASEADDR_ALIVE
#define SCR_SIGNAGURE_RESET     (SCR_ALIVE_BASE + 0x068)
#define SCR_SIGNAGURE_SET       (SCR_ALIVE_BASE + 0x06C)
#define SCR_SIGNAGURE_READ      (SCR_ALIVE_BASE + 0x070)
#define SCR_WAKE_FN_RESET       (SCR_ALIVE_BASE + 0x0AC)        // ALIVESCRATCHRST1
#define SCR_WAKE_FN_SET         (SCR_ALIVE_BASE + 0x0B0)
#define SCR_WAKE_FN_READ        (SCR_ALIVE_BASE + 0x0B4)
#define SCR_CRC_RET_RESET       (SCR_ALIVE_BASE + 0x0B8)        // ALIVESCRATCHRST2
#define SCR_CRC_RET_SET         (SCR_ALIVE_BASE + 0x0BC)
#define SCR_CRC_RET_READ        (SCR_ALIVE_BASE + 0x0C0)
#define SCR_CRC_PHY_RESET       (SCR_ALIVE_BASE + 0x0C4)        // ALIVESCRATCHRST3
#define SCR_CRC_PHY_SET         (SCR_ALIVE_BASE + 0x0C8)
#define SCR_CRC_PHY_READ        (SCR_ALIVE_BASE + 0x0CC)
#define SCR_CRC_LEN_RESET       (SCR_ALIVE_BASE + 0x0D0)        // ALIVESCRATCHRST4
#define SCR_CRC_LEN_SET         (SCR_ALIVE_BASE + 0x0D4)
#define SCR_CRC_LEN_READ        (SCR_ALIVE_BASE + 0x0D8)

#define SCR_RESET_SIG_RESET     (SCR_ALIVE_BASE + 0x0DC)        // ALIVESCRATCHRST5
#define SCR_RESET_SIG_SET       (SCR_ALIVE_BASE + 0x0E0)
#define SCR_RESET_SIG_READ      (SCR_ALIVE_BASE + 0x0E4)

#define SCR_USER_SIG6_RESET     (SCR_ALIVE_BASE + 0x0E8)        // ALIVESCRATCHRST6
#define SCR_USER_SIG6_SET       (SCR_ALIVE_BASE + 0x0EC)
#define SCR_USER_SIG6_READ      (SCR_ALIVE_BASE + 0x0F0)

#define SCR_USER_SIG7_RESET     (SCR_ALIVE_BASE + 0x0F4)        // ALIVESCRATCHRST7
#define SCR_USER_SIG7_SET       (SCR_ALIVE_BASE + 0x0F8)
#define SCR_USER_SIG7_READ      (SCR_ALIVE_BASE + 0x0FC)

#define SCR_USER_SIG8_RESET     (SCR_ALIVE_BASE + 0x100)    // ALIVESCRATCHRST8
#define SCR_USER_SIG8_SET       (SCR_ALIVE_BASE + 0x104)
#define SCR_USER_SIG8_READ      (SCR_ALIVE_BASE + 0x108)

#define SCU_CTRL                0x00
#define SCU_CONFIG              0x04
#define SCU_CPU_STATUS          0x08
#define SCU_INVALIDATE          0x0c
#define SCU_FPGA_REVISION       0x10
#define MPPR_REG				0xF0000000

#define TWD_TIMER_LOAD          0x00
#define TWD_TIMER_COUNTER       0x04
#define TWD_TIMER_CONTROL       0x08
#define TWD_TIMER_INTSTAT       0x0C

#define TWD_WDOG_LOAD           0x20
#define TWD_WDOG_COUNTER        0x24
#define TWD_WDOG_CONTROL        0x28
#define TWD_WDOG_INTSTAT        0x2C
#define TWD_WDOG_RESETSTAT      0x30
#define TWD_WDOG_DISABLE        0x34

#define GLOBAL_TIMER_COUNT_L	( GLOBAL_TIMER_BASE + 0x00 )
#define GLOBAL_TIMER_COUNT_H	( GLOBAL_TIMER_BASE + 0x04 )
#define GLOBAL_TIMER_CTRL		( GLOBAL_TIMER_BASE + 0x08 )
#define GLOBAL_TIMER_INT_STATUS	( GLOBAL_TIMER_BASE + 0x0C )
#define GLOBAL_TIMER_COMP_L		( GLOBAL_TIMER_BASE + 0x10 )
#define GLOBAL_TIMER_COMP_H		( GLOBAL_TIMER_BASE + 0x14 )
#define GLOBAL_TIMER_AUTO_INC	( GLOBAL_TIMER_BASE + 0x18 )

#define TWD_TIMER_CONTROL_ENABLE    (1 << 0)
#define TWD_TIMER_CONTROL_ONESHOT   (0 << 1)
#define TWD_TIMER_CONTROL_PERIODIC  (1 << 1)
#define TWD_TIMER_CONTROL_IT_ENABLE (1 << 2)

#define DEFAULT_ALL_DISABLE			0x0000ffff
#define DEFAULT_ALL_DISABLE_MASK	0xffff0000
//GIC中断号表，27->globaltimer中断,29->privatetimer中断,30->watchdog中断,31->VIC中断
#define GLOBAL_TIMER_IRQ_OFS		27
#define PRIVATE_TIMER_IRQ_OFS		29
#define WDT_IRQ_OFS					30
#define GIC_VIC_IRQ_OFS				31

#define raw_smp_processor_id()      0
#define smp_processor_id()          raw_smp_processor_id()

#endif
