#ifndef _ENV_INCLUDE_S5P4418_TIMER_H_
#define _ENV_INCLUDE_S5P4418_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <s5p4418_rstcon.h>
#include <types.h>
#include <io.h>
#include <config.h>

enum{
    TIMER_CFG0 = 0x00,
    TIMER_CFG1 = 0x04,
    TIMER_TCON = 0x08,
    TIMER_CNTB = 0x0c,
    TIMER_CMPB = 0x10,
    TIMER_CNTO = 0x14,
    TIMER_STAT = 0x44,
};

enum tcon{
    TCON_AUTO   = 8,
    TCON_INVT   = 4,
    TCON_UP     = 2,
    TCON_RUN    = 1,
};

enum timer_misc{
    MAX_DIVISION    = 4,
    TINT_CS_OFFS    = 5,
    TIMER_CH_OFFS   = 0xc,
    TCON_MASK       = 0xe,
    PRESCALER_MASK  = 0xff,
};

enum timer_reg{
    S5P4418_TIMER_TCFG0 = 0xC0017000,
    S5P4418_TIMER_TCFG1 = 0xC0017004,
    S5P4418_TIMER_TCON  = 0xC0017008,
    S5P4418_TIMER_TSTAT = 0xC0017044,
};

enum timer_count{
    S5P4418_TIMER0_BASE = 0xC001700C,
    S5P4418_TIMER1_BASE = 0xC0017018,
    S5P4418_TIMER2_BASE = 0xC0017024,
    S5P4418_TIMER3_BASE = 0xC0017030,
};

enum timer_base{
    TIMER_BASE          = 0xC0017000,
};

#define TIMER_CH_IS_0_1(ch)     (ch < 2)

enum{
    IRQ_OFF = 0,
    IRQ_ON,
};
enum arm_irq_timer{
    ARM_IRQ_TIMER0 = 23,
    ARM_IRQ_TIMER1,
    ARM_IRQ_TIMER2,
    ARM_IRQ_TIMER3,
};

/*
    * When HZ = 1000, The Max delay is 24 days
     */
#define time_after(a, b)        (((s32_t)(b) - (s32_t)(a) < 0))
#define time_before(a, b)       time_after(b, a)
#define time_after_eq(a,b)      (((s32_t)(a) - (s32_t)(b) >= 0))
#define time_before_eq(a,b)     time_after_eq(b, a)

enum tick{
	TICK_TIMER_CHANNEL  = 0,
};        

extern volatile u32_t jiffies;

void timer_start(int ch, int irqon);
void timer_stop(int ch, int irqon);
void timer_clock(int ch, int mux, int scl);
void timer_count(int ch, unsigned int cnt);
void timer_reset(void);
void timer_irq_clear(int ch);
u64_t timer_calc_tin(int ch, u32_t period);
void s5p4418_timer_init(void);
void s5p4418_timer_Deinit(void);

#ifdef __cplusplus
}
#endif

#endif
