#ifndef _ENV_INCLUDE_S5P4418_TICK_DELAY_H_
#define _ENV_INCLUDE_S5P4418_TICK_DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <timer.h>
#include <config.h>

void s5p4418_tick_delay_initial(void);
void udelay(u32_t us);

#ifdef DELAY_SWITCH
#define mdelay(x) msleep(x)
#else
void mdelay(u32_t ms);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __S5P4418_TICK_DELAY_H__ */
