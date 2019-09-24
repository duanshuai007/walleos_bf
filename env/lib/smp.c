#include <types.h>
#include <smp.h>
#include <smp_macros.h>
#include <s5p4418_led.h>
#include <s5p4418_tick_delay.h>
#include <s5p4418_timer.h>
#include <s5p4418_serial_stdio.h>
#include <s5p4418_mmu.h>
#include <scheduler.h>
#include <task.h>
#include <idletask.h>
#include <neverstoptask.h>
#include <gesturetask.h>
#include <dtouchtimertask.h>
#include <uspios.h>
#include <timeslicetimerhandler.h>
#include <cache_v7.h>
#include <mmu/mmu_asm.h>
#include <headsmp.h>
#include <io.h>
#include <lwip/apps/net_main.h>
#include <ethernet/nx_gmac.h>
#include <serial_shell.h>
#include <synchronize.h>
#include <config.h>
#include <timer.h>
#include <smp_macros.h>
#include <global_timer.h>

#ifdef SMP_MMU
extern u32 PAGE_TABLE_START;
#endif

extern void vfp_enable(void);

extern volatile u32_t jiffies;
extern void __secondary_startup(void);

volatile u32 shell_flag = 0;
volatile int pen_release = -1;
int cpu_logical_map[NR_CPUS];

struct sGICD_TYPE sGICD_TYPE;

void *scu_base = (void *)(0xF0000000);

void write_pen_release(int val)
{
    pen_release = val;
    smp_wmb();

	flush_cache((u32)&pen_release, sizeof(pen_release));
    
	//__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
    //outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

//设置CPU ID
void smp_setup_processor_id(void)
{
    u32 i;
    u32 cpu = get_cpuid();

    cpu_logical_map[0] = cpu;
    for (i = 1; i < NR_CPUS; ++i)
    {   
        cpu_logical_map[i] = ((i == cpu) ? 0 : i); 
    }   
}

void platform_smp_prepare_cpus(void)
{
    scu_enable(scu_base);
    writel(SCR_ARM_SECOND_BOOT, (u32_t)__secondary_startup);// RTC scratch
    writel(SCR_SIGNAGURE_RESET, (-1UL));    //ALIVESCRATCHRSTREG
}

void  gic_dist_init(void)
{
    u8 i;
    u32 cpumask;
    u8 gic_irqs;

    //const u32 ENABLEGRP1_OFF = 1;
    //const u32 ENABLEGRP0_OFF = 0;
    cpumask = (1<<0)|(1<<1)|(1<<2)|(1<<3);
    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16; 

    gic_irqs = (sGICD_TYPE.ucITLinesNumber + 1) * 32;
    //关闭PPI,打开SGI.
    //PPI需要在用到时调用mt_enable_ppi打开。SGI必须全部打开
    //SGI用作处理器间中断的基础
    writel(GIC_DIST_BASE + GIC_DIST_CTRL, 0);

    //设置中断在group0或是group1中，在group0中cpu0能中断，在group1中cpu0不能中断
    for(i = 1; i < sGICD_TYPE.ucITLinesNumber + 1; i++)
    {
        writel(GIC_DIST_BASE + GIC_DIST_GROUP + i*4, 0);
    }
    /*
     *  Set all global interrupts to be edge triggered, active low.
     */
    for( i = 32; i < gic_irqs; i += 16)
        writel(GIC_DIST_BASE + GIC_DIST_CONFIG + i * 4 / 16, 0);
    /*
     *  Set all global interrupts to all CPU.
     */
    for(i = 32; i < gic_irqs; i += 4)
        writel(GIC_DIST_BASE + GIC_DIST_TARGET + i * 4 / 4, cpumask);
	/*
     *  Set priority on all global interrupts.
     */
    for(i = 32; i < gic_irqs; i += 4)
        writel(GIC_DIST_BASE + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

    /*
     *  Disable all interrupts.  Leave the PPI and SGIs alone
     *  as these enables are banked registers.
     */
    for(i = 32; i < gic_irqs; i += 32)
        writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

    //在系统复位后IGROUP的寄存器全是0，这决定了所有的中断都属在GROUP0中
    //writel(((1 << ENABLEGRP0_OFF) | (1 << ENABLEGRP1_OFF)), GIC_DIST_BASE + GIC_DIST_CTRL);
    writel(GIC_DIST_BASE + GIC_DIST_CTRL, 1);
}


void gic_cpu_init(void)
{
    int i;
	u32 val, invt_val;
    u32 cpuid;
    //const u32 FIQBYPASS_OFF = 5;
    //const u32 IRQBYPASS_OFF = 6;
    //const u32 EOIMODENS_OFF = 9;
#define CPU_ID0  0
#define CPU_ID1  1
#define CPU_ID2  2
#define CPU_ID3  3
    //操作自己的CPU INTERFACE的ICCPMR寄存器，只有优先级高于0xF0的
    //中断才会送入本处理器,数值越小优先级越高，所以此处放弃了过滤
    cpuid = get_cpuid();
	val = DEFAULT_ALL_DISABLE;
	invt_val = DEFAULT_ALL_DISABLE_MASK;
    switch(cpuid)
    {
        case CPU_ID0:
			{
//cpu0 gic中断设置
#if(CPU0_GLOBAL_TIMER_IRQ == TRUE)
				val |= (1 << GLOBAL_TIMER_IRQ_OFS);	
				invt_val &= ~(1 << GLOBAL_TIMER_IRQ_OFS);
#endif	//CPU0_GLOBAL_TIMER_IRQ
#if(CPU0_PRIVATE_TIMER_IRQ == TRUE)
				val |= (1 << PRIVATE_TIMER_IRQ_OFS);
				invt_val &= ~(1 << PRIVATE_TIMER_IRQ_OFS);
#endif
#if(CPU0_GIC_VIC_IRQ == TRUE)
				val |= (1 << GIC_VIC_IRQ_OFS);
				invt_val &= ~(1 << GIC_VIC_IRQ_OFS);
#endif
			}break;
//cpu1 gic中断设置
#ifdef CPU1_INTERRUPT
        case CPU_ID1:
			{
#if(CPU1_GLOBAL_TIMER_IRQ == TRUE)
				val |= (1 << GLOBAL_TIMER_IRQ_OFS);
				invt_val &= ~(1 << GLOBAL_TIMER_IRQ_OFS);
#endif	//CPU1_GLOBAL_TIMER_IRQ
#if(CPU1_PRIVATE_TIMER_IRQ == TRUE)
				val |= (1 << PRIVATE_TIMER_IRQ_OFS);
				invt_val &= ~(1 << PRIVATE_TIMER_IRQ_OFS);
#endif
#if(CPU1_GIC_VIC_IRQ == TRUE)
				val |= (1 << GIC_VIC_IRQ_OFS);
				invt_val &= ~(1 << GIC_VIC_IRQ_OFS);
#endif
			}break;
#endif	//CPU1_INTERRUPT
//cpu2 gic中断设置
#ifdef CPU2_INTERRUPT
        case CPU_ID2:
			{
#if(CPU2_GLOBAL_TIMER_IRQ == TRUE)
				val |= (1 << GLOBAL_TIMER_IRQ_OFS);
				invt_val &= ~(1 << GLOBAL_TIMER_IRQ_OFS);
#endif	//CPU1_GLOBAL_TIMER_IRQ
#if(CPU2_PRIVATE_TIMER_IRQ == TRUE)
				val |= (1 << PRIVATE_TIMER_IRQ_OFS);
				invt_val &= ~(1 << PRIVATE_TIMER_IRQ_OFS);
#endif
#if(CPU2_GIC_VIC_IRQ == TRUE)
				val |= (1 << GIC_VIC_IRQ_OFS);
				invt_val &= ~(1 << GIC_VIC_IRQ_OFS);
#endif
			}break;
#endif	//CPU2_INTERRUPT
//cpu3 gic中断设置
#ifdef CPU3_INTERRUPT
        case CPU_ID3:
			{
#if(CPU3_GLOBAL_TIMER_IRQ == TRUE)
				val |= (1 << GLOBAL_TIMER_IRQ_OFS);
				invt_val &= ~(1 << GLOBAL_TIMER_IRQ_OFS);
#endif	//CPU1_GLOBAL_TIMER_IRQ
#if(CPU2_PRIVATE_TIMER_IRQ == TRUE)
				val |= (1 << PRIVATE_TIMER_IRQ_OFS);
				invt_val &= ~(1 << PRIVATE_TIMER_IRQ_OFS);
#endif
#if(CPU2_GIC_VIC_IRQ == TRUE)
				val |= (1 << GIC_VIC_IRQ_OFS);
				invt_val &= ~(1 << GIC_VIC_IRQ_OFS);
#endif
			}break;
#endif	//CPU3_INTERRUPT

        default:
            {
            }break;
    }

	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR,		invt_val);
	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_SET,			val);
	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + 4,	0xffffffff);
	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + 8,	0xffffffff);
	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + 12,	0xffffffff);
	writel(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + 16,	0xffffffff);

    for(i = 0; i < 32; i += 4)
        writel(GIC_DIST_BASE + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);
    //设置为0-3 = 0b0000 提供16个supported levels
    writel(GIC_CPU_BASE + GIC_CPU_PRIMASK, 0xf0);
    //设定CPU interface的control register。enable了group 0的中断，
    //disable了group 1的中断，group 0的interrupt source触发IRQ中断（而不是FIQ中断）。
    //writel((1 << FIQBYPASS_OFF)|(1 << IRQBYPASS_OFF )|1, GIC_CPU_BASE + GIC_CPU_CTRL);
    writel(GIC_CPU_BASE + GIC_CPU_CTRL, 1);
}

void smp_cpu_base_init(void)
{
    //ALIVEPWRGATEREG = 0xC0010800
    //写入1，表示Enable writing data to Alive Register
    writel( 0xC0010800, 1); 
    //SCR_ARM_SECOND_BOOT = 0xC0010C1C = RTCscratch
    //reset value = 0x0000_0000
    writel(SCR_ARM_SECOND_BOOT, 0xffffffff);

    /*  
     * NOTE> Control for ACP register access.
     */
    const unsigned int tie_reg = 0xC0011000;  //TIEOFFREG00
    unsigned int val;

    //TIEOFFREG28 = 0xC0011070
    val = readl(tie_reg + 0x70) & ~((3 << 30) | (3 << 10));
    writel((tie_reg + 0x70), val);
    //TIEOFFREG32 = 0xC0011080
    val = readl(tie_reg + 0x80) & ~(3 << 3); 
    writel((tie_reg + 0x80), val);
}

void gic_init_bases(void)
{
    int val = readl(GIC_DIST_BASE + GIC_DIST_TYPER);
 
    sGICD_TYPE.ucITLinesNumber  = val & 0x1f;
    sGICD_TYPE.ucCPUNumber      = ((val >> 5)&0x7) + 1;
    sGICD_TYPE.ucSecurityExtn   = (val & (1<<10)) ? 1 : 0;
    sGICD_TYPE.ucLSPI           = (val >> 11);

    gic_dist_init();
    gic_cpu_init();
}

//核间通信的方式
void gic_raise_softirq(unsigned int cpu, unsigned int irq)
{
    unsigned long map = 0;

    /* Convert our logical CPU mask into a physical one. */
    map |= 1 << cpu_logical_map[cpu];

    /*
     * Ensure that stores to Normal memory are visible to the
     * other CPUs before issuing the IPI.
     */
    dsb();

    /* this always happens on GIC0 */
    writel(GIC_DIST_BASE + GIC_DIST_SOFTINT, map << 16 | irq);
}


void gic_secondary_init(void)
{
    gic_cpu_init();
}

static void platform_secondary_init(void)
{
    /*  
     * if any interrupts are already enabled for the primary
     * core (e.g. timer irq), then they will not have been enabled
     * for us: do so
     */
    gic_secondary_init();

    /*  
     * let the primary processor know we're out of the
     * pen, then head off into the C entry point
     */
    write_pen_release(-1);

    /*  
     * Synchronise with the boot thread.
     */
    // spin_lock(&boot_lock);
    // spin_unlock(&boot_lock);
}

ip_addr_t ipaddr;
ip_addr_t netmask;
ip_addr_t gw; 

extern volatile u32_t pTimer;

void secondary_startup(unsigned int cpu)
{
    printk("enter secondary_startup, cpu[%d]\r\n", cpu);
    //printk("---sp = %08x\r\n", get_sp());
    //printk("cpu = %d\r\n", get_cpuid());
    platform_secondary_init();

#ifdef SMP_MMU
    enable_mmu(PAGE_TABLE_START);
#endif

    if(cpu == 1)
    {
        vfp_enable();
        //开启cpu0的中断使能
#ifdef CPU1_INTERRUPT
	  	setcpsr();
        dsb();
#endif
        //网卡初始化
        if(!initNetDriver())
        {   
            printk("net init failed\r\n");
        }   
        else
        {   
            printk ("net configing....\r\n");
            initNetModule();
            printk("dhcp_auto_config_ip...\r\n");
            dhcp_auto_config_ip(&ipaddr, &netmask, &gw);

            printk("ip:%d.%d.%d.%d\r\n", ip4_addr1_16(&ipaddr), ip4_addr2_16(&ipaddr), ip4_addr3_16(&ipaddr), ip4_addr4_16(&ipaddr));
            printk("mask:%d.%d.%d.%d\r\n", ip4_addr1_16(&netmask), ip4_addr2_16(&netmask), ip4_addr3_16(&netmask), ip4_addr4_16(&netmask));
            printk("gw:%d.%d.%d.%d\r\n", ip4_addr1_16(&gw), ip4_addr2_16(&gw), ip4_addr3_16(&gw), ip4_addr4_16(&gw));
        }

        shell_flag = 1;

        while(1)
        {
            pollingData();
        }
    }

    if(cpu == 2)
    {
        vfp_enable();
#ifdef CPU2_INTERRUPT
        setcpsr();
        dsb();
#endif
        while(1)
        {
            if(shell_flag == 1)
            {
                tester_serial_shell();
            }
        }
    }
    
    if(cpu == 3)
    {
        vfp_enable();
        led_init();
#ifdef CPU3_INTERRUPT
        setcpsr();
        dsb();
#endif
        Timer(cpu);
        writel(PRIVATE_TIMER_BASE + TWD_TIMER_COUNTER, TWD_COUNT - 1);
        writel(PRIVATE_TIMER_BASE + TWD_TIMER_LOAD, TWD_COUNT - 1);
        writel(PRIVATE_TIMER_BASE + TWD_TIMER_CONTROL,
                TWD_TIMER_CONTROL_ENABLE | TWD_TIMER_CONTROL_PERIODIC | TWD_TIMER_CONTROL_IT_ENABLE | ((TWD_TIMER_PRESCALER - 1) << 8));

        Scheduler();
        new_IdleTask();

        setSchedulerTimer(StartKernelTimer(0.1 * HZ, TimeSliceTimerHandler, 0, 0));

        new_GestureTask(1, 2); 
        new_DTouchTimerTask(2);

        unsigned nTicks = TimerGetTicks();
        boolean LED_SW = FALSE; 
        while (1)
        {
            if(!USPiTouchScreenUpdate())
            {
                if (TimerGetTicks() > nTicks + 1000)
                {
                    led_on(LED_SW);
                    LED_SW = !LED_SW;

                    nTicks = TimerGetTicks();
                }
            }
        }        
    }
}


int boot_second_core(void)
{
    writel(SCR_ARM_SECOND_BOOT, (u32_t)__secondary_startup);
    return 0;
}

static int boot_secondary(unsigned int cpu)
{
    unsigned long timeout;
    const u32 ENOSYS = 38;

    boot_second_core();//设置rtc scratch为__secondary_startup

    //写入pen值，用来区分不同cpu核心
    write_pen_release(cpu_logical_map[cpu]);

    /*
     * Send the secondary(第二的) CPU a soft interrupt, thereby(由此) causing
     * the boot monitor to read the system wide flags register,
     * and branch to the address found there.
     */
    mdelay(10);
    
    gic_raise_softirq(cpu, 1);//使能软中断

    timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
        smp_rmb();
        if (pen_release == -1)
            break;

        udelay(10);
    }

    /*
     * now the secondary core is starting up let it run its
     * calibrations, then wait for it to finish
     */

    //flush_cache_all();

    return pen_release != -1 ? -ENOSYS : 0;
}

int cpu_up(unsigned int cpu)
{
    int ret;
    ret = boot_secondary(cpu);
    if (ret == 0)
    {
        printk("--->cpu[%d] up success\r\n", cpu);
    }
    else
    {
        printk("--->cpu[%d] up failed: ret = %d\r\n", cpu, ret);
    }

    return 0;
}
