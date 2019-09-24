#ifndef _ENV_INCLUDE_S5P4418_PWM_H_
#define _ENV_INCLUDE_S5P4418_PWM_H_

#include <types.h>
#include <list.h>

enum pwm{
    S5P4418_PWM_BASE    = 0xC0018000,
    S5P4418_PWM_TCFG0   = 0xC0018000,
    S5P4418_PWM_TCFG1   = 0xC0018004,
    S5P4418_PWM_TCON    = 0xC0018008,
    S5P4418_PWM_TSTAT   = 0xC0018044,
    S5P4418_PWM0_BASE   = 0xC001800C,
    S5P4418_PWM1_BASE   = 0xC0018018,
    S5P4418_PWM2_BASE   = 0xC0018024,
    S5P4418_PWM3_BASE   = 0xC0018030,
    PWM_TCNTB           = 0x00,
    PWM_TCMPB           = 0x04,
    PWM_TCNTO           = 0x08,
};

void s5p4418_pwm_init(void);
void s5p4418_pwm_exit(void);

struct pwm_t
{
    const char * name;

    boolean __enable;
    u32_t  __duty;
    u32_t  __period;
    boolean __polarity;

    void (*config)(struct pwm_t * pwm, u32_t duty, u32_t period, boolean polarity);
    void (*enable)(struct pwm_t * pwm);
    void (*disable)(struct pwm_t * pwm);

    void * priv;
};

struct pwm_list_t
{
    struct pwm_t * pwm;
    struct list_head entry;
};

extern struct pwm_list_t __pwm_list;

struct pwm_t *search_pwm(const char *name);
boolean register_pwm(struct pwm_t *pwm);
boolean unregister_pwm(struct pwm_t *pwm);
void pwm_config(struct pwm_t *pwm, u32_t duty, u32_t period, boolean polarity);
void pwm_enable(struct pwm_t *pwm);
void pwm_disable(struct pwm_t *pwm);

#endif /* __S5P4418_REG_PWM_H__ */

