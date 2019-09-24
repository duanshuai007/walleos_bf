#include <io.h>
#include <types.h>
#include <s5p4418_timer.h>
#include <s5p4418_interrupt.h>
#include <s5p4418_clk.h>
#include <s5p4418_interrupt.h>
#include <s5p4418_serial_stdio.h>

static inline u8 CFG0_CH(u8 ch)
{
    return ((ch == 0) || (ch == 1)) ? 0 : 8;
}

static inline u8 CFG1_CH(u8 ch)
{
    return ch * 4;
}

static inline u32 DIVIDER_MUX_MASK(u8 n)
{
    return 0xf << (n * 4);
}

static inline u8 DIVIDER_MUX_OFFSET(u8 n)
{
    return n*4;
}

static inline u8 TCON_CH(u8 ch)
{
    return (ch ? ch * 4 + 4 : 0);
}

static inline u8 TINT_CH(u8 ch)
{
    return ch;
}

static inline u8 TINT_CS_CH(u8 ch)
{
    return ch + 5;
}

void timer_stop(int ch, int irqon)
{
    volatile u32 val; 
    int on = irqon ? BIT_SET : BIT_CLEAR;  

    val  = readl(TIMER_BASE + TIMER_STAT);
    val &= ~((RESET_ID_MASK << TINT_CS_OFFS) | (BIT_SET << TINT_CH(ch)));
    val |=  ((BIT_SET << TINT_CS_CH(ch)) | (on << TINT_CH(ch)));
    __writel(val, TIMER_BASE + TIMER_STAT);

    val  = readl(TIMER_BASE + TIMER_TCON);
    val &= ~(TCON_RUN << TCON_CH(ch));
    __writel(val, TIMER_BASE + TIMER_TCON);
}

void timer_clock(int ch, int mux, int scl)
{
    volatile u32 val;

    val  = readl(TIMER_BASE + TIMER_CFG0);
    val &= ~((PRESCALER_MASK   << CFG0_CH(ch)));
    val |=  ((scl-1)<< CFG0_CH(ch));
     __writel(val, TIMER_BASE + TIMER_CFG0);
    
    val  = readl(TIMER_BASE + TIMER_CFG1);
    val &= ~(0xf << CFG1_CH(ch));
    val |=  (mux << CFG1_CH(ch));   
    __writel(val, TIMER_BASE + TIMER_CFG1);
}

u64_t timer_calc_tin(int ch, u32_t period)
{
    u64_t rate, freq = 1000000000L / period;
    u8_t div;
    //u8_t  shift;
    u32_t val;

    if(TIMER_CH_IS_0_1(ch))
        rate = clk_get_rate("DIV-TIMER-PRESCALER0");
    else
        rate = clk_get_rate("DIV-TIMER-PRESCALER1");

    for(div = 0; div < MAX_DIVISION; div++)
    {
        if((rate >> div) <= freq)
            break;
    }

    val = read32(phys_to_virt(S5P4418_TIMER_TCFG1));
    val &= ~DIVIDER_MUX_MASK(ch);
    val |= (div << DIVIDER_MUX_OFFSET(ch));
    __writel(val, phys_to_virt(S5P4418_TIMER_TCFG1));

    return (rate >> div);
}

void timer_count(int ch, unsigned int cnt)
{
    __writel((cnt-1), TIMER_BASE + TIMER_CNTB + (TIMER_CH_OFFS * ch));
    __writel((cnt-1), TIMER_BASE + TIMER_CMPB + (TIMER_CH_OFFS * ch));
}

void timer_start(int ch, int irqon)
{
    volatile u32 val;
    int on = irqon ? BIT_SET : BIT_CLEAR;

    val  = readl(TIMER_BASE + TIMER_STAT);
    val &= ~((RESET_ID_MASK << TINT_CS_OFFS) | (BIT_SET << TINT_CH(ch)));
    val |=  ((BIT_SET << TINT_CS_CH(ch)) | (on << TINT_CH(ch)));
    __writel(val, TIMER_BASE + TIMER_STAT);

    val = readl(TIMER_BASE + TIMER_TCON);
    val &= ~(TCON_MASK << TCON_CH(ch));
    val |=  (TCON_UP << TCON_CH(ch));
    __writel(val, TIMER_BASE + TIMER_TCON);

    val &= ~(TCON_UP << TCON_CH(ch));
    val |=  ((TCON_AUTO | TCON_RUN)  << TCON_CH(ch));
    __writel(val, TIMER_BASE + TIMER_TCON);
}
void timer_reset(void)
{
    s5p4418_ip_reset(RESET_ID_TIMER, 0);
}

void timer_irq_clear(int ch) 
{
    volatile u32 val;
    val  = readl(TIMER_BASE + TIMER_STAT);
    val &= ~(RESET_ID_MASK << TINT_CS_OFFS);
    val |= (BIT_SET << TINT_CS_CH(ch));
    __writel(val, TIMER_BASE + TIMER_STAT);
}

void s5p4418_timer_init(void)
{
    printf("TIMER1_IRQ_NO = %d\r\n", TIMER1_IRQ_NO);
    const int ch = 1;
    const int tmux = 0;
    const int tscl = 150;
    //关闭定时器ch
    timer_stop(ch, IRQ_ON); 
    timer_clock(ch, tmux, tscl);
    timer_stop(ch, IRQ_OFF); 
    timer_stop(ch, IRQ_OFF); 
    timer_count(ch, CLOCKHZ / HZ);
    timer_start(ch, IRQ_ON); 
    //s5p4418_enable_interrupt(24);
}

void s5p4418_timer_Deinit(void)
{
    const int ch = 1;
    timer_stop(ch, IRQ_OFF);
}

