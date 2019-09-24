#ifndef _ENV_INCLUDE_S5P4418_CLK_H_
#define _ENV_INCLUDE_S5P4418_CLK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <io.h>

/*
 * Clock Generator Level 0
 */
enum clk_level_0{
    S5P4418_CLK_CODA960CLKENB = 0xC00C0000 + 0x7000,
    S5P4418_CLK_CRYPTOCLKENB    = 0xC00C0000 + 0x6000,
    S5P4418_CLK_I2C0CLKENB      = 0xC00A0000 + 0xE000,
    S5P4418_CLK_I2C1CLKENB      = 0xC00A0000 + 0xF000,
    S5P4418_CLK_I2C2CLKENB      = 0xC00B0000 + 0x0000,
    S5P4418_CLK_GPUCLKENB       = 0xC00C0000 + 0x3000,
    S5P4418_CLK_MPEGTSICLKENB   = 0xC00C0000 + 0xB700,
    S5P4418_CLK_PDMCLKENB       = 0xC00C0000 + 0xB000,
    S5P4418_CLK_SCALERCLKENB    = 0xC00B0000 + 0x6000,
    S5P4418_CLK_DEINTERLACECLKENB = 0xC00B0000 + 0x5000,
    S5P4418_CLK_MLC0CLKENB      = 0xC0100000 + 0x23C0,
    S5P4418_CLK_MLC1CLKENB      = 0xC0100000 + 0x27C0,
};

/*
 * Clock Generator Level 1
 */
enum clk_level_1{
    S5P4418_CLK_MIPICSICLKENB           = 0xC00C0000 + 0xA000,
    S5P4418_CLK_MIPICSICLKGEN0L         = 0xC00C0000 + 0xA004,
    S5P4418_CLK_PPMCLKENB               = 0xC00C0000 + 0x4000,
    S5P4418_CLK_PPMCLKGEN0L             = 0xC00C0000 + 0x4004,
    S5P4418_CLK_PWM0CLKENB              = 0xC00B0000 + 0xA000,
    S5P4418_CLK_PWM1CLKENB              = 0xC00B0000 + 0xE000,
    S5P4418_CLK_PWM2CLKENB              = 0xC00B0000 + 0xF000,
    S5P4418_CLK_PWM3CLKENB              = 0xC00C0000 + 0x0000,
    S5P4418_CLK_TIMER0CLKENB            = 0xC00B0000 + 0x9000,
    S5P4418_CLK_TIMER1CLKENB            = 0xC00B0000 + 0xB000,
    S5P4418_CLK_TIMER2CLKENB            = 0xC00B0000 + 0xC000,
    S5P4418_CLK_TIMER3CLKENB            = 0xC00B0000 + 0xD000,
    S5P4418_CLK_PWM0CLKGEN0L            = 0xC00B0000 + 0xA004,
    S5P4418_CLK_PWM1CLKGEN0L            = 0xC00B0000 + 0xE004,
    S5P4418_CLK_PWM2CLKGEN0L            = 0xC00B0000 + 0xF004,
    S5P4418_CLK_PWM3CLKGEN0L            = 0xC00C0000 + 0x0004,
    S5P4418_CLK_TIMER0CLKGEN0L          = 0xC00B0000 + 0x9004,
    S5P4418_CLK_TIMER1CLKGEN0L          = 0xC00B0000 + 0xB004,
    S5P4418_CLK_TIMER2CLKGEN0L          = 0xC00B0000 + 0xC004,
    S5P4418_CLK_TIMER3CLKGEN0L          = 0xC00B0000 + 0xD004,
    S5P4418_CLK_SDMMC0CLKENB            = 0xC00C0000 + 0x5000,
    S5P4418_CLK_SDMMC1CLKENB            = 0xC00C0000 + 0xC000,
    S5P4418_CLK_SDMMC2CLKENB            = 0xC00C0000 + 0xD000,
    S5P4418_CLK_SDMMC0CLKGEN0L          = 0xC00C0000 + 0x5004,
    S5P4418_CLK_SDMMC1CLKGEN0L          = 0xC00C0000 + 0xC004,
    S5P4418_CLK_SDMMC2CLKGEN0L          = 0xC00C0000 + 0xD004,
    S5P4418_CLK_SPDIFTXCLKENB           = 0xC00B0000 + 0x8000,
    S5P4418_CLK_SPDIFTXCLKGEN0L         = 0xC00B0000 + 0x8004,
    S5P4418_CLK_SSP0CLKENB              = 0xC00A0000 + 0xC000,
    S5P4418_CLK_SSP1CLKENB              = 0xC00A0000 + 0xD000,
    S5P4418_CLK_SSP2CLKENB              = 0xC00A0000 + 0x7000,
    S5P4418_CLK_SSP0CLKGEN0L            = 0xC00A0000 + 0xC004,
    S5P4418_CLK_SSP1CLKGEN0L            = 0xC00A0000 + 0xD004,
    S5P4418_CLK_SSP2CLKGEN0L            = 0xC00A0000 + 0x7004,
    S5P4418_CLK_UART0CLKENB             = 0xC00A0000 + 0x9000,
    S5P4418_CLK_UART1CLKENB             = 0xC00A0000 + 0x8000,
    S5P4418_CLK_UART2CLKENB             = 0xC00A0000 + 0xA000,
    S5P4418_CLK_UART3CLKENB             = 0xC00A0000 + 0xB000,
    S5P4418_CLK_UART4CLKENB             = 0xC0060000 + 0xE000,
    S5P4418_CLK_UART5CLKENB             = 0xC0080000 + 0x4000,
    S5P4418_CLK_UART0CLKGEN0L           = 0xC00A0000 + 0x9004,
    S5P4418_CLK_UART1CLKGEN0L           = 0xC00A0000 + 0x8004,
    S5P4418_CLK_UART2CLKGEN0L           = 0xC00A0000 + 0xA004,
    S5P4418_CLK_UART3CLKGEN0L           = 0xC00A0000 + 0xB004,
    S5P4418_CLK_UART4CLKGEN0L           = 0xC0060000 + 0xE004,
    S5P4418_CLK_UART5CLKGEN0L           = 0xC0080000 + 0x4004,
    S5P4418_CLK_VIP0CLKENB              = 0xC00C0000 + 0x1000,
    S5P4418_CLK_VIP1CLKENB              = 0xC00C0000 + 0x2000,
    S5P4418_CLK_VIP0CLKGEN0L            = 0xC00C0000 + 0x1004,
    S5P4418_CLK_VIP1CLKGEN0L            = 0xC00C0000 + 0x2004,
};
/*
 * Clock Generator Level 2
 */
enum clk_level_2{
    S5P4418_CLK_GMACCLKENB              = 0xC00C0000 + 0x8000,
    S5P4418_CLK_GMACCLKGEN0L            = 0xC00C0000 + 0x8004,
    S5P4418_CLK_GMACCLKGEN1L            = 0xC00C0000 + 0x800C,
    S5P4418_CLK_I2S0CLKENB              = 0xC00B0000 + 0x2000,
    S5P4418_CLK_I2S1CLKENB              = 0xC00B0000 + 0x3000,
    S5P4418_CLK_I2S2CLKENB              = 0xC00B0000 + 0x4000,
    S5P4418_CLK_I2S0CLKGEN0L            = 0xC00B0000 + 0x2004,
    S5P4418_CLK_I2S1CLKGEN0L            = 0xC00B0000 + 0x3004,
    S5P4418_CLK_I2S2CLKGEN0L            = 0xC00B0000 + 0x4004,
    S5P4418_CLK_I2S0CLKGEN1L            = 0xC00B0000 + 0x200C,
    S5P4418_CLK_I2S1CLKGEN1L            = 0xC00B0000 + 0x300C,
    S5P4418_CLK_I2S2CLKGEN1L            = 0xC00B0000 + 0x400C,
    S5P4418_CLK_USBHOSTOTGCLKENB        = 0xC0060000 + 0xB000,
    S5P4418_CLK_USBHOSTOTGCLKGEN0L      = 0xC0060000 + 0xB004,
    S5P4418_CLK_USBHOSTOTGCLKGEN1L      = 0xC0060000 + 0xB00C,
    S5P4418_CLK_DPC0CLKENB              = 0xC0100000 + 0x2BC0,
    S5P4418_CLK_DPC1CLKENB              = 0xC0100000 + 0x2FC0,
    S5P4418_CLK_DPC0CLKGEN0L            = 0xC0100000 + 0x2BC4,
    S5P4418_CLK_DPC1CLKGEN0L            = 0xC0100000 + 0x2FC4,
    S5P4418_CLK_DPC0CLKGEN1L            = 0xC0100000 + 0x2BCC,
    S5P4418_CLK_DPC1CLKGEN1L            = 0xC0100000 + 0x2FCC,
    S5P4418_CLK_LVDSCLKENB              = 0xC0100000 + 0x8000,
    S5P4418_CLK_LVDSCLKGEN0L            = 0xC0100000 + 0x8004,
    S5P4418_CLK_LVDSCLKGEN1L            = 0xC0100000 + 0x800C,
    S5P4418_CLK_HDMICLKENB              = 0xC0100000 + 0x9000,
    S5P4418_CLK_HDMICLKGEN0L            = 0xC0100000 + 0x9004,
    S5P4418_CLK_HDMICLKGEN1L            = 0xC0100000 + 0x900C,
    S5P4418_CLK_MIPIDSICLKENB           = 0xC0100000 + 0x5000,
    S5P4418_CLK_MIPIDSICLKGEN0L         = 0xC0100000 + 0x5004,
    S5P4418_CLK_MIPIDSICLKGEN1L         = 0xC0100000 + 0x500C,
    S5P4418_SYS_CLKMODEREG0             = 0xC0010000 + 0x0000,
    S5P4418_SYS_CLKMODEREG1             = 0xC0010000 + 0x0004,
    S5P4418_SYS_PLLSETREG0              = 0xC0010000 + 0x0008,
    S5P4418_SYS_PLLSETREG1              = 0xC0010000 + 0x000C,
    S5P4418_SYS_PLLSETREG2              = 0xC0010000 + 0x0010,
    S5P4418_SYS_PLLSETREG3              = 0xC0010000 + 0x0014,
    S5P4418_SYS_CLKDIVREG0              = 0xC0010000 + 0x0020,
    S5P4418_SYS_CLKDIVREG1              = 0xC0010000 + 0x0024,
    S5P4418_SYS_CLKDIVREG2              = 0xC0010000 + 0x0028,
    S5P4418_SYS_CLKDIVREG3              = 0xC0010000 + 0x002C,
    S5P4418_SYS_CLKDIVREG4              = 0xC0010000 + 0x0030,
    S5P4418_SYS_PLLSETREG0_SSCG         = 0xC0010000 + 0x0048,
    S5P4418_SYS_PLLSETREG1_SSCG         = 0xC0010000 + 0x004C,
    S5P4418_SYS_PLLSETREG2_SSCG         = 0xC0010000 + 0x0050,
    S5P4418_SYS_PLLSETREG3_SSCG         = 0xC0010000 + 0x0054,

	S5P4418_SYS_GPIOWAKEUP_RISEENB		= 0xC0010000 + 0x200,
	S5P4418_SYS_GPIOWAKEUP_FALLENB		= 0xC0010000 + 0x204,
	S5P4418_SYS_GPIORSTENB				= 0xC0010000 + 0x208,
	S5P4418_SYS_GPIOWKENB				= 0xC0010000 + 0x20C,
	S5P4418_SYS_INTENB					= 0xC0010000 + 0x210,
	S5P4418_SYS_GPIOINTPEND				= 0xC0010000 + 0x214,
	S5P4418_SYS_RESETSTATUS				= 0xC0010000 + 0x218,
	S5P4418_SYS_INTENABLE				= 0xC0010000 + 0x21C,
	S5P4418_SYS_INTPEND					= 0xC0010000 + 0x220,
	S5P4418_SYS_PWRCONT					= 0xC0010000 + 0x224,
	S5P4418_SYS_PWRMODE					= 0xC0010000 + 0x228,
};

enum clk_type_t {
    CLK_TYPE_FIXED,
    CLK_TYPE_FIXED_FACTOR,
    CLK_TYPE_PLL,
    CLK_TYPE_MUX,
    CLK_TYPE_DIVIDER,
    CLK_TYPE_GATE,
    CLK_TYPE_LINK,
};

enum clk_src{
    PLL0 = 0,
    PLL1,
    PLL2,
};

struct clk_t
{
    const char * name;
    enum clk_type_t type;
    int count;

    void (*set_parent)(struct clk_t * clk, const char * pname);
    const char * (*get_parent)(struct clk_t * clk);
    void (*set_enable)(struct clk_t * clk, boolean enable);
    boolean (*get_enable)(struct clk_t * clk);
    void (*set_rate)(struct clk_t * clk, u64_t prate, u64_t rate);
    u64_t (*get_rate)(struct clk_t * clk, u64_t prate);

    void * priv;
};

struct clk_t * clk_search(const char * name);
boolean clk_register(struct clk_t * clk);
boolean clk_unregister(struct clk_t * clk);

void clk_set_parent(const char * name, const char * pname);
const char * clk_get_parent(const char * name);
void clk_enable(const char * name);
void clk_disable(const char * name);
boolean clk_status(const char * name);
void clk_set_rate(const char * name, u64_t rate);
u64_t clk_get_rate(const char * name);

struct clk_fixed_t {
    const char * name;
    u64_t rate;
};

void s5p4418_clk_init(void);
void s5p4418_clk_exit(void);

boolean clk_fixed_register(struct clk_fixed_t * fclk);
boolean clk_fixed_unregister(struct clk_fixed_t * fclk);

struct clk_pll_t {
    const char * name;
    const char * parent;
    void (*set_rate)(struct clk_pll_t * pclk, u64_t prate, u64_t rate);
    u64_t (*get_rate)(struct clk_pll_t * pclk, u64_t prate);
};

boolean clk_pll_register(struct clk_pll_t * pclk);
boolean clk_pll_unregister(struct clk_pll_t * pclk);

struct clk_mux_table_t {
    const char * name;
    int val;
};

struct clk_mux_t {
    const char * name;
    struct clk_mux_table_t * parent;
    physical_addr_t reg;
    int shift;
    int width;
};

boolean clk_mux_register(struct clk_mux_t * mclk);
boolean clk_mux_unregister(struct clk_mux_t * mclk);

enum clk_divider_type_t {
    CLK_DIVIDER_ZERO_BASED      = 0x0,
    CLK_DIVIDER_ONE_BASED       = 0x1,
};

struct clk_divider_t {
    const char * name;
    const char * parent;
    physical_addr_t reg;
    enum clk_divider_type_t type;
    int shift;
    int width;
};

boolean clk_divider_register(struct clk_divider_t * dclk);
boolean clk_divider_unregister(struct clk_divider_t * dclk);

struct clk_gate_t {
    const char * name;
    const char * parent;
    physical_addr_t reg;
    int shift;
    int invert;
};

boolean clk_gate_register(struct clk_gate_t * gclk);
boolean clk_gate_unregister(struct clk_gate_t * gclk);

//add by duanshuai
void s5p4418_set_softwareReset_Enable(boolean enable);
void s5p4418_do_softwareReset(void);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */


