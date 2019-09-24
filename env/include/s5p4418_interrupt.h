#ifndef _ENV_INCLUDE_S5P4418_INTERRUPT_H_ 
#define _ENV_INCLUDE_S5P4418_INTERRUPT_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum vic_reg{
    VIC_IRQSTATUS           = 0x000,
    VIC_FIQSTATUS           = 0x004,
    VIC_RAWINTR             = 0x008,
    VIC_INTSELECT           = 0x00c,
    VIC_INTENABLE           = 0x010,
    VIC_INTENCLEAR          = 0x014,
    VIC_SOFTINT             = 0x018,
    VIC_SOFTINTCLEAR        = 0x01c,
    VIC_PROTECTION          = 0x020,
    VIC_SWPRIORITYMASK      = 0x024,
    VIC_PRIORITYDAISY       = 0x028,
    VIC_VECTADDR0           = 0x100,
    VIC_VECPRIORITY0        = 0x200,
    VIC_ADDRESS             = 0xf00,
    VIC_PERID0              = 0xfe0,
    VIC_PERID1              = 0xfe4,
    VIC_PERID2              = 0xfe8,
    VIC_PERID3              = 0xfec,
    VIC_PCELLID0            = 0xff0,
    VIC_PCELLID1            = 0xff4,
    VIC_PCELLID2            = 0xff8,
    VIC_PCELLID3            = 0xffc,
};

enum irq_misc{
    IRQ_LINES                   = 64,
    IRQ_0_32                    = 32,
    IRQ_32_64                   = 64,
    IRQ_NOMBER_VIC0             = 32,
    IRQ_NOMBER_VIC1             = 64,
    IRQ_NOMBER_GPIOA            = 96,
    IRQ_NOMBER_GPIOB            = 128,
    IRQ_NOMBER_GPIOC            = 160,
    IRQ_NOMBER_GPIOD            = 192,
    IRQ_NOMBER_GPIOE            = 224,
    ALL_ZERO                    = 0x00000000,
    ALL_FFFF                    = 0xffffffff,
    INTER_ALL_CLEAR             = 0xffffffff,
};

#define GPIOA_IRQ_NO(n)                 (IRQ_NOMBER_VIC1 + (n))		//64  - 96
#define GPIOB_IRQ_NO(n)                 (IRQ_NOMBER_GPIOA + (n))	//96  - 128
#define GPIOC_IRQ_NO(n)                 (IRQ_NOMBER_GPIOB + (n))	//128 - 160
#define GPIOD_IRQ_NO(n)                 (IRQ_NOMBER_GPIOC + (n))	//160 - 192
#define GPIOE_IRQ_NO(n)                 (IRQ_NOMBER_GPIOD + (n))	//192 - 224

typedef void TIRQHandler (void *pParam);

int setIrqHandler(unsigned nIrq, TIRQHandler *pHandler, void *pParam);
TIRQHandler *getIrqHandler(unsigned nIrq);
int s5p4418_interrupt_init (void);
void s5p4418_InterruptHandler (void);

enum irq_no{
    MCUSTOP_IRQ_NO = 0,
    DMA0_IRQ_NO,
    DMA1_IRQ_NO,
    CLKPWR0_IRQ_NO,
    CLKPWR1_IRQ_NO,
    CLKPWR2_IRQ_NO,
    UART1_IRQ_NO,
    UART0_IRQ_NO,
    UART2_IRQ_NO,
    UART3_IRQ_NO,
    UART4_IRQ_NO,
    RESERVED,
    SSP0_IRQ_NO,
    SSP1_IRQ_NO,
    SSP2_IRQ_NO,
    I2C0_IRQ_NO,
    I2C1_IRQ_NO,
    I2C2_IRQ_NO,
    DEINTERLACE_IRQ_NO,
    SCALER_IRQ_NO,
    AC97_IRQ_NO,
    SPDIFRX_IRQ_NO,
    SPDIFTX_IRQ_NO,
    TIMER0_IRQ_NO,
    TIMER1_IRQ_NO,
    TIMER2_IRQ_NO,
    TIMER3_IRQ_NO,
    PWM0_IRQ_NO,
    PWM1_IRQ_NO,
    PWM2_IRQ_NO,
    PWM3_IRQ_NO,
    WDT_IRQ_NO,
    MPEGTSI_IRQ_NO,
    DISPLAYTOP0_IRQ_NO,
    DISPLAYTOP1_IRQ_NO,
    DISPLAYTOP2_IRQ_NO,
    DISPLAYTOP3_IRQ_NO,
    VIP0_IRQ_NO,
    VIP1_IRQ_NO,
    MIPI_IRQ_NO,
    GPU_IRQ_NO,
    ADC_IRQ_NO,
    PPM_IRQ_NO,
    SDMMC0_IRQ_NO,
    SDMMC1_IRQ_NO,
    SDMMC2_IRQ_NO,
    CODA9600_IRQ_NO,
    CODA9601_IRQ_NO,
    GMAC_IRQ_NO,
    USB20OTG_IRQ_NO,
    USB20HOST_IRQ_NO,
    GPIOA_IRQ_NO        = 53,
    GPIOB_IRQ_NO,
    GPIOC_IRQ_NO,
    GPIOD_IRQ_NO,
    GPIOE_IRQ_NO,
    CRYPTO_IRQ_NO,
    PDM_IRQ_NO,
    CPU0_PMU_IRQ_NO     = 64,
    CPU1_PMU_IRQ_NO,
    CPU2_PMU_IRQ_NO,
    CPU3_PMU_IRQ_NO,
    L2CCINTR_IRQ_NO,
    MCUINTR_IRQ_NO,
};

//kernel/arch/arm/mach-s5p4418/include/mach/s5p4418_irq.h
#define IRQ_PHY_MAX_COUNT	64
#define IRQ_GPIO_START		IRQ_PHY_MAX_COUNT
#define IRQ_GPIO_END		(IRQ_GPIO_START + 32 * 5)
#define IRQ_ALIVE_START     IRQ_GPIO_END
#define IRQ_ALIVE_END       (IRQ_ALIVE_START + 8)

enum gic_irqs{
	//IRQ_GIC_START		= ((IRQ_ALIVE_END & ~31) + 32),
	IRQ_GIC_START		= 0,
	IRQ_GIC_PPI_START	= (IRQ_GIC_START  +  16),
	IRQ_GIC_PPI_GT		= (IRQ_GIC_START  +  27),
	IRQ_GIC_PPI_PVT		= (IRQ_GIC_START  +  29),
	IRQ_GIC_PPI_WDT		= (IRQ_GIC_START  +  30),
	IRQ_GIC_PPI_VIC		= (IRQ_GIC_START  +  31),
	IRQ_GIC_END			= (IRQ_GIC_START  +  32),	
};

enum irq_type_t {
    IRQ_TYPE_NONE           = 0,
    IRQ_TYPE_LEVEL_LOW      = 1,
    IRQ_TYPE_LEVEL_HIGH     = 2,
    IRQ_TYPE_EDGE_FALLING   = 3,
    IRQ_TYPE_EDGE_RISING    = 4,
    IRQ_TYPE_EDGE_BOTH      = 5,
};

enum irq_to_cpu{
	DEFAULT_CPU = 0,
	CPU0_ID		= 0,
	CPU1_ID,
	CPU2_ID,
	CPU3_ID,
};

struct irq_handler_t {
    void (*func)(void *data);
    void *data;
};

struct irq_t {
    const char *name;
    const int no;
	int cpuid;

	struct irq_handler_t *handler;

    void (*enable)(struct irq_t *irq);
    void (*disable)(struct irq_t *irq);
    void (*set_type)(struct irq_t *irq, enum irq_type_t type);
};

boolean irq_register(struct irq_t *irq);
boolean irq_unregister(struct irq_t *irq);
boolean request_irq(	const char *name,
						enum irq_to_cpu to_cpu,		/*by duanshuai add*/ 
						void (*func)(void *), 
						enum irq_type_t type, 
						void *data);

boolean free_irq(const char *name);
//void enable_irq(const char *name);
//void disable_irq(const char *name);

#ifdef __cplusplus
}
#endif

#endif
