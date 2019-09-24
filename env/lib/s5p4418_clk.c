/*
 * s5p4418-clk.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <alloc.h>
#include <s5p4418_clk.h>
#include <s5p4418_timer.h>
#include <s5p4418_pwm.h>
#include <s5p4418_rstcon.h>
#include <list.h>
#include <string.h>
#include <sizes.h>
#include <stdlib.h>
#include <io.h>
#include <config.h>

/*
 * [CORE CLK]
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-FCLKCPU0 --> DIV-FCLKCPU0 --> DIV-HCLKCPU0
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-BCLK --> DIV-BCLK --> DIV-PCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-MDCLK --> DIV-MDCLK --> DIV-MCLK --> DIV-MBCLK --> DIV-MPCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-GR3DBCLK --> DIV-GR3DBCLK --> DIV-GR3DPCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-MPEGBCLK --> DIV-MPEGBCLK --> DIV-MPEGPCLK
 *
 * [TIMER / PWM CLK]
 * DIV-PCLK --> DIV-TIMER-PRESCALER0
 * DIV-PCLK --> DIV-TIMER-PRESCALER1
 * DIV-PCLK --> DIV-PWM-PRESCALER0
 * DIV-PCLK --> DIV-PWM-PRESCALER1
 *
 * [UART CLK]
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART0 --> DIV-UART0 --> GATE-UART0
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART1 --> DIV-UART1 --> GATE-UART1
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART2 --> DIV-UART2 --> GATE-UART2
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART3 --> DIV-UART3 --> GATE-UART3
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART4 --> DIV-UART4 --> GATE-UART4
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX->UART5 --> DIV-UART5 --> GATE-UART5
 *
 */

/*
 * CORE CLK
 */
static struct clk_fixed_t core_fixed_clks[] = {
    {
        .name = "PLLXTI",
        .rate = 24 * 1000 * 1000,
    }, {
        .name = "RTCXTI",
        .rate = 32768,
    }
};

static void core_pll_clk_set_rate(struct clk_pll_t * pclk, u64_t prate, u64_t rate)
{
    UNUSED(prate);
    UNUSED(rate);
    if(strcmp(pclk->name, "PLL0") == 0)
    {
    }
    else if(strcmp(pclk->name, "PLL1") == 0)
    {
    }
    else if(strcmp(pclk->name, "PLL2") == 0)
    {
    }
    else if(strcmp(pclk->name, "PLL3") == 0)
    {
    }
}

static u64_t core_pll_clk_get_rate(struct clk_pll_t * pclk, u64_t prate)
{
    u32_t r, p, m, s, k;
    u64_t fout = 0;

    if(!pclk)
        return FALSE;

    if(strcmp(pclk->name, "PLL0") == 0)
    {
        /*
         * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
         */
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG0));
        p = (r >> 18) & 0x3f;
        m = (r >> 8) & 0x3ff;
        s = (r >> 0) & 0xff;
        fout = m * (prate / (p * (1 << s)));
    }
    else if(strcmp(pclk->name, "PLL1") == 0)
    {
        /*
         * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
         */
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG1));
        p = (r >> 18) & 0x3f;
        m = (r >> 8) & 0x3ff;
        s = (r >> 0) & 0xff;
        fout = m * (prate / (p * (1 << s)));
    }
    else if(strcmp(pclk->name, "PLL2") == 0)
    {
        /*
         * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
         */
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG2));
        p = (r >> 18) & 0x3f;
        m = (r >> 8) & 0x3ff;
        s = (r >> 0) & 0xff;
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG2_SSCG));
        k = (r >> 16) & 0xffff;
        fout = (m + k / 65536) * (prate / (p * (1 << s)));
    }
    else if(strcmp(pclk->name, "PLL3") == 0)
    {
        /*
         * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
         */
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG3));
        p = (r >> 18) & 0x3f;
        m = (r >> 8) & 0x3ff;
        s = (r >> 0) & 0xff;
        r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG3_SSCG));
        k = (r >> 16) & 0xffff;
        fout = (m + k / 65536) * (prate / (p * (1 << s)));
    }

    return fout;
}

static struct clk_pll_t core_pll_clks[] = {
    {
        .name = "PLL0",
        .parent = "PLLXTI",
        .set_rate = core_pll_clk_set_rate,
        .get_rate = core_pll_clk_get_rate,
    }, {
        .name = "PLL1",
        .parent = "PLLXTI",
        .set_rate = core_pll_clk_set_rate,
        .get_rate = core_pll_clk_get_rate,
    }, {
        .name = "PLL2",
        .parent = "PLLXTI",
        .set_rate = core_pll_clk_set_rate,
        .get_rate = core_pll_clk_get_rate,
    }, {
        .name = "PLL3",
        .parent = "PLLXTI",
        .set_rate = core_pll_clk_set_rate,
        .get_rate = core_pll_clk_get_rate,
    },
};

static struct clk_mux_table_t pll_mux_tables[] = {
    { .name = "PLL0",   .val = 0 },
    { .name = "PLL1",   .val = 1 },
    { .name = "PLL2",   .val = 2 },
    { .name = "PLL3",   .val = 3 },
    { 0, 0 },
};

static struct clk_mux_t core_mux_clks[] = {
    {
        .name = "MUX-FCLKCPU0",
        .parent = pll_mux_tables,
        .reg = S5P4418_SYS_CLKDIVREG0,
        .shift = 0,
        .width = 3,
    }, {
        .name = "MUX-BCLK",
        .parent = pll_mux_tables,
        .reg = S5P4418_SYS_CLKDIVREG1,
        .shift = 0,
        .width = 3,
    }, {
        .name = "MUX-MDCLK",
        .parent = pll_mux_tables,
        .reg = S5P4418_SYS_CLKDIVREG2,
        .shift = 0,
        .width = 3,
    }, {
        .name = "MUX-GR3DBCLK",
        .parent = pll_mux_tables,
        .reg = S5P4418_SYS_CLKDIVREG3,
        .shift = 0,
        .width = 3,
    }, {
        .name = "MUX-MPEGBCLK",
        .parent = pll_mux_tables,
        .reg = S5P4418_SYS_CLKDIVREG4,
        .shift = 0,
        .width = 3,
    }
};

static struct clk_divider_t core_div_clks[] = {
    {
        .name = "DIV-FCLKCPU0",
        .parent = "MUX-FCLKCPU0",
        .reg = S5P4418_SYS_CLKDIVREG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 3,
        .width = 6,
    }, {
        .name = "DIV-HCLKCPU0",
        .parent = "DIV-FCLKCPU0",
        .reg = S5P4418_SYS_CLKDIVREG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 9,
        .width = 6,
    }, {
        .name = "DIV-BCLK",
        .parent = "MUX-BCLK",
        .reg = S5P4418_SYS_CLKDIVREG1,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 3,
        .width = 6,
    }, {
        .name = "DIV-PCLK",
        .parent = "DIV-BCLK",
        .reg = S5P4418_SYS_CLKDIVREG1,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 9,
        .width = 6,
    }, {
        .name = "DIV-MDCLK",
        .parent = "MUX-MDCLK",
        .reg = S5P4418_SYS_CLKDIVREG2,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 3,
        .width = 6,
    }, {
        .name = "DIV-MCLK",
        .parent = "DIV-MDCLK",
        .reg = S5P4418_SYS_CLKDIVREG2,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 9,
        .width = 6,
    }, {
        .name = "DIV-MBCLK",
        .parent = "DIV-MCLK",
        .reg = S5P4418_SYS_CLKDIVREG2,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 15,
        .width = 6,
    }, {
        .name = "DIV-MPCLK",
        .parent = "DIV-MBCLK",
        .reg = S5P4418_SYS_CLKDIVREG2,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 21,
        .width = 6,
    }, {
        .name = "DIV-GR3DBCLK",
        .parent = "MUX-GR3DBCLK",
        .reg = S5P4418_SYS_CLKDIVREG3,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 3,
        .width = 6,
    }, {
        .name = "DIV-GR3DPCLK",
        .parent = "DIV-GR3DBCLK",
        .reg = S5P4418_SYS_CLKDIVREG3,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 9,
        .width = 6,
    }, {
        .name = "DIV-MPEGBCLK",
        .parent = "MUX-MPEGBCLK",
        .reg = S5P4418_SYS_CLKDIVREG4,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 3,
        .width = 6,
    }, {
        .name = "DIV-MPEGPCLK",
        .parent = "DIV-MPEGBCLK",
        .reg = S5P4418_SYS_CLKDIVREG4,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 9,
        .width = 6,
    }, {
        .name = "DIV-TIMER-PRESCALER0",
        .parent = "DIV-PCLK",
        .reg = S5P4418_TIMER_TCFG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 0,
        .width = 8,
    }, {
        .name = "DIV-TIMER-PRESCALER1",
        .parent = "DIV-PCLK",
        .reg = S5P4418_TIMER_TCFG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 8,
        .width = 8,
    }, {
        .name = "DIV-PWM-PRESCALER0",
        .parent = "DIV-PCLK",
        .reg = S5P4418_PWM_TCFG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 0,
        .width = 8,
    }, {
        .name = "DIV-PWM-PRESCALER1",
        .parent = "DIV-PCLK",
        .reg = S5P4418_PWM_TCFG0,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 8,
        .width = 8,
    },
};

static struct clk_gate_t core_gate_clks[] = {
    {
        .name = NULL,
        .parent = NULL,
        //reg
        //shift
        //invert
    },
};

static void clk_register_core(void)
{
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(core_fixed_clks); i++)
        clk_fixed_register(&core_fixed_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_pll_clks); i++)
        clk_pll_register(&core_pll_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_mux_clks); i++)
        clk_mux_register(&core_mux_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_div_clks); i++)
        clk_divider_register(&core_div_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_gate_clks); i++)
        clk_gate_register(&core_gate_clks[i]);
}

static void clk_unregister_core(void)
{
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(core_fixed_clks); i++)
        clk_fixed_unregister(&core_fixed_clks[i]);
    for(i = 0; i< ARRAY_SIZE(core_pll_clks); i++)
        clk_pll_unregister(&core_pll_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_mux_clks); i++)
        clk_mux_unregister(&core_mux_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_div_clks); i++)
        clk_divider_unregister(&core_div_clks[i]);
    for(i = 0; i < ARRAY_SIZE(core_gate_clks); i++)
        clk_gate_unregister(&core_gate_clks[i]);
}

/*
 * UART CLK
 */
static struct clk_mux_table_t uart_mux_tables[] = {
    { .name = "PLL0",   .val = 0 },
    { .name = "PLL1",   .val = 1 },
    { .name = "PLL2",   .val = 2 },
    { 0, 0 },
};

static struct clk_mux_t uart_mux_clks[] = {
    {
        .name = "MUX-UART0",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART0CLKGEN0L,
        .shift = 2,
        .width = 3,
    }, {
        .name = "MUX-UART1",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART1CLKGEN0L,
        .shift = 2,
        .width = 3,
    }, {
        .name = "MUX-UART2",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART2CLKGEN0L,
        .shift = 2,
        .width = 3,
    }, {
        .name = "MUX-UART3",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART3CLKGEN0L,
        .shift = 2,
        .width = 3,
    }, {
        .name = "MUX-UART4",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART4CLKGEN0L,
        .shift = 2,
        .width = 3,
    }, {
        .name = "MUX-UART5",
        .parent = uart_mux_tables,
        .reg = S5P4418_CLK_UART5CLKGEN0L,
        .shift = 2,
        .width = 3,
    }
};

static struct clk_divider_t uart_div_clks[] = {
    {
        .name = "DIV-UART0",
        .parent = "MUX-UART0",
        .reg = S5P4418_CLK_UART0CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }, {
        .name = "DIV-UART1",
        .parent = "MUX-UART1",
        .reg = S5P4418_CLK_UART1CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }, {
        .name = "DIV-UART2",
        .parent = "MUX-UART2",
        .reg = S5P4418_CLK_UART2CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }, {
        .name = "DIV-UART3",
        .parent = "MUX-UART3",
        .reg = S5P4418_CLK_UART3CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }, {
        .name = "DIV-UART4",
        .parent = "MUX-UART4",
        .reg = S5P4418_CLK_UART4CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }, {
        .name = "DIV-UART5",
        .parent = "MUX-UART5",
        .reg = S5P4418_CLK_UART5CLKGEN0L,
        .type = CLK_DIVIDER_ONE_BASED,
        .shift = 5,
        .width = 8,
    }
};

static struct clk_gate_t uart_gate_clks[] = {
    {
        .name = "GATE-UART0",
        .parent = "DIV-UART0",
        .reg = S5P4418_CLK_UART0CLKENB,
        .shift = 1,
        .invert = 0,
    }, {
        .name = "GATE-UART1",
        .parent = "DIV-UART1",
        .reg = S5P4418_CLK_UART1CLKENB,
        .shift = 1,
        .invert = 0,
    }, {
        .name = "GATE-UART2",
        .parent = "DIV-UART2",
        .reg = S5P4418_CLK_UART2CLKENB,
        .shift = 1,
        .invert = 0,
    }, {
        .name = "GATE-UART3",
        .parent = "DIV-UART3",
        .reg = S5P4418_CLK_UART3CLKENB,
        .shift = 1,
        .invert = 0,
    }, {
        .name = "GATE-UART4",
        .parent = "DIV-UART4",
        .reg = S5P4418_CLK_UART4CLKENB,
        .shift = 1,
        .invert = 0,
    }, {
        .name = "GATE-UART5",
        .parent = "DIV-UART5",
        .reg = S5P4418_CLK_UART5CLKENB,
        .shift = 1,
        .invert = 0,
    }
};

static void clk_register_uart(void)
{
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(uart_mux_clks); i++)
        clk_mux_register(&uart_mux_clks[i]);
    for(i = 0; i < ARRAY_SIZE(uart_div_clks); i++)
        clk_divider_register(&uart_div_clks[i]);
    for(i = 0; i < ARRAY_SIZE(uart_gate_clks); i++)
        clk_gate_register(&uart_gate_clks[i]);
}

static void clk_unregister_uart(void)
{
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(uart_mux_clks); i++)
        clk_mux_unregister(&uart_mux_clks[i]);
    for(i = 0; i < ARRAY_SIZE(uart_div_clks); i++)
        clk_divider_unregister(&uart_div_clks[i]);
    for(i = 0; i < ARRAY_SIZE(uart_gate_clks); i++)
        clk_gate_unregister(&uart_gate_clks[i]);
}

static const char * default_off_clks[] = {
    NULL,
};

static const char * default_on_clks[] = {
    NULL,
};

void s5p4418_clk_init(void)
{
    unsigned int i;

    /*
     * Reset IP modules, for timer and pwm clock.
     */
    s5p4418_ip_reset(RESET_ID_TIMER, 0);
    s5p4418_ip_reset(RESET_ID_PWM, 0);

    clk_register_core();
    clk_register_uart();

    clk_set_rate("DIV-TIMER-PRESCALER0", 100 * 1000 * 1000);
    clk_set_rate("DIV-TIMER-PRESCALER1", 100 * 1000 * 1000);
    clk_set_rate("DIV-PWM-PRESCALER0", 100 * 1000 * 1000);
    clk_set_rate("DIV-PWM-PRESCALER1", 100 * 1000 * 1000);
    clk_set_rate("DIV-UART0", 11 * 1000 * 1000);
    clk_set_rate("DIV-UART1", 11 * 1000 * 1000);
    clk_set_rate("DIV-UART2", 11 * 1000 * 1000);
    clk_set_rate("DIV-UART3", 11 * 1000 * 1000);

    for(i = 0; i < ARRAY_SIZE(default_off_clks); i++)
        clk_disable(default_off_clks[i]);
    for(i = 0; i < ARRAY_SIZE(default_on_clks); i++)
        clk_enable(default_on_clks[i]);
}

void s5p4418_clk_exit(void)
{
    clk_unregister_core();
    clk_unregister_uart();
}

struct clk_list_t
{
    struct clk_t * clk;
    struct list_head entry;
};

static struct clk_list_t __clk_list = {
    .entry = {
        .next   = &(__clk_list.entry),
        .prev   = &(__clk_list.entry),
    },
};

struct clk_t * clk_search(const char * name)
{
    struct clk_list_t * pos, * n;

    if(!name)
        return NULL;

    list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
    {
        if(strcmp(pos->clk->name, name) == 0)
            return pos->clk;
    }

    return NULL;
}

boolean clk_register(struct clk_t * clk)
{
    struct clk_list_t * cl;

    if(!clk || !clk->name)
        return FALSE;

    if(clk_search(clk->name))
        return FALSE;

    cl = malloc(sizeof(struct clk_list_t));
    if(!cl)
        return FALSE;

    cl->clk = clk;
    list_add_tail(&cl->entry, &(__clk_list.entry));

    return TRUE;
}

boolean clk_unregister(struct clk_t * clk)
{
    struct clk_list_t * pos, * n;

    if(!clk || !clk->name)
        return FALSE;

    list_for_each_entry_safe(pos, n, &(__clk_list.entry), entry)
    {
        if(pos->clk == clk)
        {
            list_del(&(pos->entry));
            free(pos);
            return TRUE;
        }
    }

    return FALSE;
}

void clk_set_parent(const char * name, const char * pname)
{
    if((name == NULL) || (pname == NULL))
        return;

    struct clk_t * clk = clk_search(name);
    struct clk_t * pclk = clk_search(pname);

    if(pclk && clk && clk->set_parent)
        clk->set_parent(clk, pname);
}

const char * clk_get_parent(const char * name)
{
    if(name == NULL)
        return NULL;

    struct clk_t * clk = clk_search(name);

    if(clk && clk->get_parent)
        return clk->get_parent(clk);
    return NULL;
}

void clk_enable(const char * name)
{
    if(name == NULL)
        return;

    struct clk_t * clk = clk_search(name);

    if(!clk)
        return;

    if(clk->get_parent)
        clk_enable(clk->get_parent(clk));

    if(clk->set_enable)
        clk->set_enable(clk, TRUE);

    clk->count++;
}

void clk_disable(const char * name)
{
    if(name == NULL)
        return;

    struct clk_t * clk = clk_search(name);

    if(!clk)
        return;

    if(clk->count > 0)
        clk->count--;

    if(clk->count == 0)
    {
        if(clk->get_parent)
            clk_disable(clk->get_parent(clk));

        if(clk->set_enable)
            clk->set_enable(clk, FALSE);
    }
}

boolean clk_status(const char * name)
{
    if(name == NULL)
        return FALSE;

    struct clk_t * clk = clk_search(name);

    if(!clk)
        return FALSE;

    if(!clk->get_parent(clk))
        return clk->get_enable(clk);

    if(clk->get_enable(clk))
        return clk_status(clk->get_parent(clk));

    return FALSE;
}

void clk_set_rate(const char * name, u64_t rate)
{
    if(name == NULL)
        return;

    struct clk_t * clk = clk_search(name);
    u64_t prate;

    if(!clk)
        return;

    if(clk->get_parent)
        prate = clk_get_rate(clk->get_parent(clk));
    else
        prate = 0;

    if(clk->set_rate)
        clk->set_rate(clk, prate, rate);
}

u64_t clk_get_rate(const char * name)
{
    if(name == NULL)
        return FALSE;

    struct clk_t * clk = clk_search(name);
    u64_t prate;

    if(!clk)
        return 0;

    if(clk->get_parent)
        prate = clk_get_rate(clk->get_parent(clk));
    else
        prate = 0;

    if(clk->get_rate)
        return clk->get_rate(clk, prate);

    return 0;
}

static void clk_fixed_set_parent(struct clk_t * clk, const char * pname)
{
    UNUSED(clk);
    UNUSED(pname);
}

static const char * clk_fixed_get_parent(struct clk_t * clk)
{
    UNUSED(clk);
    return NULL;
}

static void clk_fixed_set_enable(struct clk_t * clk, boolean enable)
{
    UNUSED(clk);
    UNUSED(enable);
}

static boolean clk_fixed_get_enable(struct clk_t * clk)
{
    UNUSED(clk);
    return TRUE;
}

static void clk_fixed_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
    UNUSED(clk);
    UNUSED(prate);
    UNUSED(rate);
}

static u64_t clk_fixed_get_rate(struct clk_t * clk, u64_t prate)
{
    UNUSED(prate);
    if(clk == NULL)
        return FALSE;

    struct clk_fixed_t * fclk = (struct clk_fixed_t *)clk->priv;
    return fclk->rate;
}

boolean clk_fixed_register(struct clk_fixed_t * fclk)
{
    struct clk_t * clk;

    if(!fclk || !fclk->name)
        return FALSE;

    if(clk_search(fclk->name))
        return FALSE;

    clk = malloc(sizeof(struct clk_t));
    if(!clk)
        return FALSE;

    clk->name = fclk->name;
    clk->type = CLK_TYPE_FIXED;
    clk->count = 0;
    clk->set_parent = clk_fixed_set_parent;
    clk->get_parent = clk_fixed_get_parent;
    clk->set_enable = clk_fixed_set_enable;
    clk->get_enable = clk_fixed_get_enable;
    clk->set_rate = clk_fixed_set_rate;
    clk->get_rate = clk_fixed_get_rate;
    clk->priv = fclk;

    if(!clk_register(clk))
    {
        free(clk);
        return FALSE;
    }

    return TRUE;
}

boolean clk_fixed_unregister(struct clk_fixed_t * fclk)
{
    struct clk_t * clk;

    if(!fclk || !fclk->name)
        return FALSE;

    clk = clk_search(fclk->name);
    if(!clk)
        return FALSE;

    if(clk_unregister(clk))
    {
        free(clk);
        return TRUE;
    }

    return FALSE;
}

static void clk_pll_set_parent(struct clk_t * clk, const char * pname)
{
    UNUSED(clk);
    UNUSED(pname);
}

static const char * clk_pll_get_parent(struct clk_t * clk)
{
	if(clk == NULL)
		return NULL;
    struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
    return pclk->parent;
}

static void clk_pll_set_enable(struct clk_t * clk, boolean enable)
{
    UNUSED(clk);
    UNUSED(enable);
}

static boolean clk_pll_get_enable(struct clk_t * clk)
{
    UNUSED(clk);
    return TRUE;
}

static void clk_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
    if(clk == NULL)
        return;

    struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
    if(pclk->set_rate)
        pclk->set_rate(pclk, prate, rate);
}

static u64_t clk_pll_get_rate(struct clk_t * clk, u64_t prate)
{
    if(clk == NULL)
        return FALSE;

    struct clk_pll_t * pclk = (struct clk_pll_t *)clk->priv;
    if(pclk->get_rate)
        return pclk->get_rate(pclk, prate);
    return 0;
}

boolean clk_pll_register(struct clk_pll_t * pclk)
{
    struct clk_t * clk;

    if(!pclk || !pclk->name)
        return FALSE;

    if(clk_search(pclk->name))
        return FALSE;

    clk = malloc(sizeof(struct clk_t));
    if(!clk)
        return FALSE;

    clk->name = pclk->name;
    clk->type = CLK_TYPE_PLL;
    clk->count = 0;
    clk->set_parent = clk_pll_set_parent;
    clk->get_parent = clk_pll_get_parent;
    clk->set_enable = clk_pll_set_enable;
    clk->get_enable = clk_pll_get_enable;
    clk->set_rate = clk_pll_set_rate;
    clk->get_rate = clk_pll_get_rate;
    clk->priv = pclk;

    if(!clk_register(clk))
    {
        free(clk);
        return FALSE;
    }

    return TRUE;
}

boolean clk_pll_unregister(struct clk_pll_t * pclk)
{
    struct clk_t * clk;

    if(!pclk || !pclk->name)
        return FALSE;

    clk = clk_search(pclk->name);
    if(!clk)
        return FALSE;

    if(clk_unregister(clk))
    {
        free(clk);
        return TRUE;
    }

    return FALSE;
}

static void clk_mux_set_parent(struct clk_t * clk, const char * pname)
{
    struct clk_mux_t * mclk = (struct clk_mux_t *)clk->priv;
    struct clk_mux_table_t * table = mclk->parent;
    u32_t val;

    for(table = mclk->parent; table && table->name; table++)
    {
        if(strcmp(table->name, pname) == 0)
        {
            val = read32(phys_to_virt(mclk->reg));
            val &= ~(((1 << mclk->width) - 1) << mclk->shift);
            val |= table->val << mclk->shift;
            write32(phys_to_virt(mclk->reg), val);
            return;
        }
    }
}

static const char * clk_mux_get_parent(struct clk_t * clk)
{
    struct clk_mux_t * mclk = (struct clk_mux_t *)clk->priv;
    struct clk_mux_table_t * table = mclk->parent;
    int val = read32(phys_to_virt(mclk->reg)) >> mclk->shift & ((1 << mclk->width) - 1);

    for(table = mclk->parent; table && table->name; table++)
    {
        if(table->val == val)
            return table->name;
    }
    return NULL;
}

static void clk_mux_set_enable(struct clk_t * clk, boolean enable)
{
    UNUSED(clk);
    UNUSED(enable);
}

static boolean clk_mux_get_enable(struct clk_t * clk)
{
    UNUSED(clk);
    return TRUE;
}

static void clk_mux_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
    UNUSED(clk);
    UNUSED(prate);
    UNUSED(rate);
}

static u64_t clk_mux_get_rate(struct clk_t * clk, u64_t prate)
{
    UNUSED(clk);
    return prate;
}

boolean clk_mux_register(struct clk_mux_t * mclk)
{
    struct clk_t * clk;

    if(!mclk || !mclk->name)
        return FALSE;

    if(clk_search(mclk->name))
        return FALSE;

    clk = malloc(sizeof(struct clk_t));
    if(!clk)
        return FALSE;

    clk->name = mclk->name;
    clk->type = CLK_TYPE_MUX;
    clk->count = 0;
    clk->set_parent = clk_mux_set_parent;
    clk->get_parent = clk_mux_get_parent;
    clk->set_enable = clk_mux_set_enable;
    clk->get_enable = clk_mux_get_enable;
    clk->set_rate = clk_mux_set_rate;
    clk->get_rate = clk_mux_get_rate;
    clk->priv = mclk;

    if(!clk_register(clk))
    {
        free(clk);
        return FALSE;
    }

    return TRUE;
}

boolean clk_mux_unregister(struct clk_mux_t * mclk)
{
    struct clk_t * clk;

    if(!mclk || !mclk->name)
        return FALSE;

    clk = clk_search(mclk->name);
    if(!clk)
        return FALSE;

    if(clk_unregister(clk))
    {
        free(clk);
        return TRUE;
    }

    return FALSE;
}

#define div_mask(d)     ((1 << (d->width)) - 1)

static void clk_divider_set_parent(struct clk_t * clk, const char * pname)
{
    UNUSED(clk);
    UNUSED(pname);
}

static const char * clk_divider_get_parent(struct clk_t * clk)
{
    struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
    return dclk->parent;
}

static void clk_divider_set_enable(struct clk_t * clk, boolean enable)
{
    UNUSED(clk);
    UNUSED(enable);
}

static boolean clk_divider_get_enable(struct clk_t * clk)
{
    UNUSED(clk);
    return TRUE;
}

static void clk_divider_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
    struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
    u32_t div = prate / rate;
    u32_t val;

    if(dclk->type == CLK_DIVIDER_ONE_BASED)
        div--;

    if(div > (u32_t)div_mask(dclk))
        div = div_mask(dclk);

    val = read32(phys_to_virt(dclk->reg));
    val &= ~(div_mask(dclk) << dclk->shift);
    val |= div << dclk->shift;
    write32(phys_to_virt(dclk->reg), val);
}

static u64_t clk_divider_get_rate(struct clk_t * clk, u64_t prate)
{
    struct clk_divider_t * dclk = (struct clk_divider_t *)clk->priv;
    u32_t div;

    div = read32(phys_to_virt(dclk->reg)) >> dclk->shift;
    div &= div_mask(dclk);

    if(dclk->type == CLK_DIVIDER_ONE_BASED)
        div++;

    return prate / div;
}

boolean clk_divider_register(struct clk_divider_t * dclk)
{
    struct clk_t * clk;

    if(!dclk || !dclk->name)
        return FALSE;

    if(clk_search(dclk->name))
        return FALSE;

    clk = malloc(sizeof(struct clk_t));
    if(!clk)
        return FALSE;

    clk->name = dclk->name;
    clk->type = CLK_TYPE_DIVIDER;
    clk->count = 0;
    clk->set_parent = clk_divider_set_parent;
    clk->get_parent = clk_divider_get_parent;
    clk->set_enable = clk_divider_set_enable;
    clk->get_enable = clk_divider_get_enable;
    clk->set_rate = clk_divider_set_rate;
    clk->get_rate = clk_divider_get_rate;
    clk->priv = dclk;

    if(!clk_register(clk))
    {
        free(clk);
        return FALSE;
    }

    return TRUE;
}

boolean clk_divider_unregister(struct clk_divider_t * dclk)
{
    struct clk_t * clk;

    if(!dclk || !dclk->name)
        return FALSE;

    clk = clk_search(dclk->name);
    if(!clk)
        return FALSE;

    if(clk_unregister(clk))
    {
        free(clk);
        return TRUE;
    }

    return FALSE;
}

static void clk_gate_set_parent(struct clk_t * clk, const char * pname)
{
    UNUSED(clk);
    UNUSED(pname);
}

static const char * clk_gate_get_parent(struct clk_t * clk)
{
    struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;
    return gclk->parent;
}

static void clk_gate_set_enable(struct clk_t * clk, boolean enable)
{
    struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;

    if(enable)
        write32(phys_to_virt(gclk->reg), (read32(phys_to_virt(gclk->reg)) & ~(0x1 << gclk->shift)) | ((gclk->invert ? 0x0 : 0x1) << gclk->shift));
    else
        write32(phys_to_virt(gclk->reg), (read32(phys_to_virt(gclk->reg)) & ~(0x1 << gclk->shift)) | ((gclk->invert ? 0x1 : 0x0) << gclk->shift));
}

static boolean clk_gate_get_enable(struct clk_t * clk)
{
    struct clk_gate_t * gclk = (struct clk_gate_t *)clk->priv;

    if(read32(phys_to_virt(gclk->reg)) & (0x1 << gclk->shift))
        return gclk->invert ? FALSE : TRUE;
    return gclk->invert ? TRUE : FALSE;
}

static void clk_gate_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
    UNUSED(clk);
    UNUSED(prate);
    UNUSED(rate);
}

static u64_t clk_gate_get_rate(struct clk_t * clk, u64_t prate)
{
    UNUSED(clk);
    UNUSED(prate);
    return prate;
}

boolean clk_gate_register(struct clk_gate_t * gclk)
{
    struct clk_t * clk;

    if(!gclk || !gclk->name)
        return FALSE;

    if(clk_search(gclk->name))
        return FALSE;

    clk = malloc(sizeof(struct clk_t));
    if(!clk)
        return FALSE;

    clk->name = gclk->name;
    clk->type = CLK_TYPE_GATE;
    clk->count = 0;
    clk->set_parent = clk_gate_set_parent;
    clk->get_parent = clk_gate_get_parent;
    clk->set_enable = clk_gate_set_enable;
    clk->get_enable = clk_gate_get_enable;
    clk->set_rate = clk_gate_set_rate;
    clk->get_rate = clk_gate_get_rate;
    clk->priv = gclk;

    if(!clk_register(clk))
    {
        free(clk);
        return FALSE;
    }

    return TRUE;
}

boolean clk_gate_unregister(struct clk_gate_t * gclk)
{
    struct clk_t * clk;

    if(!gclk || !gclk->name)
        return FALSE;

    clk = clk_search(gclk->name);
    if(!clk)
        return FALSE;

    if(clk_unregister(clk))
    {
        free(clk);
        return TRUE;
    }

    return FALSE;
}


//add by duanshuai
void s5p4418_set_softwareReset_Enable(boolean enable)
{
	const u32 SWRSTENB_BITPOS   = 3;  
	const u32 SWRSTENB_MASK     = 1 << SWRSTENB_BITPOS;

	int val;

	val = readl(S5P4418_SYS_PWRCONT);
	val &= ~SWRSTENB_MASK;
	val |= enable << SWRSTENB_BITPOS;
	writel(S5P4418_SYS_PWRCONT, val);
}

void s5p4418_do_softwareReset(void)
{
	const u32 SWREST_BITPOS = 12; 
	const u32 SWREST_MASK   = 1 << SWREST_BITPOS;

	writel(S5P4418_SYS_PWRMODE, SWREST_MASK);
}
