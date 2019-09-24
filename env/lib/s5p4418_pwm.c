/*
 * s5p4418-pwm.c
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
#include <s5p4418_pwm.h>
#include <io.h>
#include <s5p4418_gpio.h>
#include <s5p4418_rstcon.h>
#include <s5p4418_clk.h>
#include <string.h>
#include <sizes.h>

#define TCON_CHANNEL(ch)        (ch ? ch * 4 + 4 : 0)
#define TCON_START(ch)          (0x1 << TCON_CHANNEL(ch))
#define TCON_MANUALUPDATE(ch)   (0x2 << TCON_CHANNEL(ch))
#define TCON_INVERT(ch)         (0x4 << TCON_CHANNEL(ch))
#define TCON_AUTORELOAD(ch)     (0x8 << TCON_CHANNEL(ch))
#define IS_CHANNEL_0_OR_1(x)    (x < 2)

struct s5p4418_pwm_data_t
{
    const char * name;
    int id;
    int gpio;
    int iocfg;
    physical_addr_t regbase;
};

static struct s5p4418_pwm_data_t pwm_datas[] = {
    {
        .name       = "pwm0",
        .id         = 0,
        .gpio       = S5P4418_GPIOD(1),
        .iocfg      = ALTFH1,
        .regbase    = S5P4418_PWM0_BASE,
    }, {
        .name       = "pwm1",
        .id         = 1,
        .gpio       = S5P4418_GPIOC(13),
        .iocfg      = ALTFH2,
        .regbase    = S5P4418_PWM1_BASE,
    },
    {
        .name       = "pwm2",
        .id         = 2,
        .gpio       = S5P4418_GPIOC(14),
        .iocfg      = ALTFH2,
        .regbase    = S5P4418_PWM2_BASE,
    },
    {
        .name       = "pwm3",
        .id         = 3,
        .gpio       = S5P4418_GPIOD(0),
        .iocfg      = ALTFH2,
        .regbase    = S5P4418_PWM3_BASE,
    },
};

static u64_t s5p4418_pwm_calc_tin(struct pwm_t * pwm, u32_t period)
{
    if(!pwm)
        return FALSE;

    struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
    u64_t rate, freq = 1000000000L / period;
    u8_t div, shift;

    if(IS_CHANNEL_0_OR_1(dat->id))
        rate = clk_get_rate("DIV-PWM-PRESCALER0");
    else
        rate = clk_get_rate("DIV-PWM-PRESCALER1");

    for(div = 0; div < 4; div++)
    {
        if((rate >> div) <= freq)
            break;
    }

    shift = dat->id * 4;
    write32(phys_to_virt(S5P4418_PWM_TCFG1), (read32(phys_to_virt(S5P4418_PWM_TCFG1)) & ~(0xf<<shift)) | (div<<shift));

    return (rate >> div);
}

static void s5p4418_pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period, boolean polarity)
{
    if(!pwm)
        return;

    struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
    u64_t rate;
    u32_t tcnt, tcmp;
    u32_t tcon;

    if((pwm->__duty != duty) || (pwm->__period != period))
    {
        rate = s5p4418_pwm_calc_tin(pwm, period);

        if(pwm->__duty != duty)
        {
            tcmp = rate * duty / 1000000000L;
            write32(phys_to_virt(dat->regbase + PWM_TCMPB), tcmp);
        }

        if(pwm->__period != period)
        {
            tcnt = rate * period / 1000000000L;
            write32(phys_to_virt(dat->regbase + PWM_TCNTB), tcnt);
        }

        tcon = read32(phys_to_virt(S5P4418_PWM_TCON));
        tcon |= TCON_MANUALUPDATE(dat->id);
        write32(phys_to_virt(S5P4418_PWM_TCON), tcon);

        tcon &= ~TCON_MANUALUPDATE(dat->id);
        write32(phys_to_virt(S5P4418_PWM_TCON), tcon);
    }

    if(pwm->__polarity != polarity)
    {
        tcon = read32(phys_to_virt(S5P4418_PWM_TCON));
        if(polarity)
            tcon |= TCON_INVERT(dat->id);
        else
            tcon &= ~TCON_INVERT(dat->id);
        write32(phys_to_virt(S5P4418_PWM_TCON), tcon);
    }
}

static void s5p4418_pwm_enable(struct pwm_t * pwm)
{
    if(!pwm)
        return;

    struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
    u32_t tcon;

    gpio_set_cfg(dat->gpio, dat->iocfg);
    if(IS_CHANNEL_0_OR_1(dat->id))
        clk_enable("DIV-PWM-PRESCALER0");
    else
        clk_enable("DIV-PWM-PRESCALER1");

    tcon = read32(phys_to_virt(S5P4418_PWM_TCON));
    tcon &= ~(TCON_AUTORELOAD(dat->id) | TCON_START(dat->id));
    tcon |= TCON_MANUALUPDATE(dat->id);
    write32(phys_to_virt(S5P4418_PWM_TCON), tcon);

    tcon = read32(phys_to_virt(S5P4418_PWM_TCON));
    tcon &= ~TCON_MANUALUPDATE(dat->id);
    tcon |= TCON_AUTORELOAD(dat->id) | TCON_START(dat->id);
    write32(phys_to_virt(S5P4418_PWM_TCON), tcon);
}

static void s5p4418_pwm_disable(struct pwm_t * pwm)
{
    if(!pwm)
        return;

    struct s5p4418_pwm_data_t * dat = (struct s5p4418_pwm_data_t *)pwm->priv;
    u32_t tcon;

    tcon = read32(phys_to_virt(S5P4418_PWM_TCON));
    tcon &= ~(TCON_AUTORELOAD(dat->id) | TCON_START(dat->id));
    write32(phys_to_virt(S5P4418_PWM_TCON), tcon);

    if(IS_CHANNEL_0_OR_1(dat->id))
        clk_disable("DIV-PWM-PRESCALER0");
    else
        clk_disable("DIV-PWM-PRESCALER1");
}

void s5p4418_pwm_init(void)
{
    struct pwm_t * pwm;
    unsigned int i;

    s5p4418_ip_reset(RESET_ID_PWM, 0);

    for(i = 0; i < ARRAY_SIZE(pwm_datas); i++)
    {
        pwm = malloc(sizeof(struct pwm_t));
        if(!pwm)
            continue;

        pwm->name = pwm_datas[i].name;
        pwm->config = s5p4418_pwm_config;
        pwm->enable = s5p4418_pwm_enable;
        pwm->disable = s5p4418_pwm_disable;
        pwm->priv = &pwm_datas[i];

        register_pwm(pwm);
    }
}

void s5p4418_pwm_exit(void)
{
    struct pwm_t * pwm;
    unsigned int i;

    for(i = 0; i < ARRAY_SIZE(pwm_datas); i++)
    {
        pwm = search_pwm(pwm_datas[i].name);
        if(!pwm)
            continue;
        unregister_pwm(pwm);
        free(pwm);
    }
}

struct pwm_list_t __pwm_list = {
    .entry = {
        .next   = &(__pwm_list.entry),
        .prev   = &(__pwm_list.entry),
    },
};

struct pwm_t * search_pwm(const char * name)
{
    struct pwm_list_t * pos, * n;

    if(!name)
        return NULL;

    list_for_each_entry_safe(pos, n, &(__pwm_list.entry), entry)
    {
        if(strcmp(pos->pwm->name, name) == 0)
            return pos->pwm;
    }

    return NULL;
}

boolean register_pwm(struct pwm_t * pwm)
{
    struct pwm_list_t * pl;

    if(!pwm || !pwm->name)
        return FALSE;

    if(search_pwm(pwm->name))
        return FALSE;

    pl = malloc(sizeof(struct pwm_list_t));
    if(!pl)
        return FALSE;

    pwm->__enable = FALSE;
    pwm->__duty = 0;
    pwm->__period = 0;
    pwm->__polarity = FALSE;
    pl->pwm = pwm;
    list_add_tail(&pl->entry, &(__pwm_list.entry));

    return TRUE;
}

boolean unregister_pwm(struct pwm_t * pwm)
{
    struct pwm_list_t * pos, * n;

    if(!pwm || !pwm->name)
        return FALSE;

    list_for_each_entry_safe(pos, n, &(__pwm_list.entry), entry)
    {
        if(pos->pwm == pwm)
        {
            list_del(&(pos->entry));
            free(pos);
            return TRUE;
        }
    }

    return FALSE;
}

void pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period, boolean polarity)
{
    if(pwm && pwm->config)
    {
        if(duty > period)
            duty = period;
        if((pwm->__duty != duty) || (pwm->__period != period) || (pwm->__polarity != polarity))
        {
            pwm->config(pwm, duty, period, polarity);
            pwm->__duty = duty;
            pwm->__period = period;
            pwm->__polarity = polarity;
        }
    }
}

void pwm_enable(struct pwm_t * pwm)
{
    if(pwm && pwm->enable && !pwm->__enable)
    {
        pwm->enable(pwm);
        pwm->__enable = TRUE;
    }
}

void pwm_disable(struct pwm_t * pwm)
{
    if(pwm && pwm->disable && pwm->__enable)
    {
        pwm->disable(pwm);
        pwm->__enable = FALSE;
    }
}
