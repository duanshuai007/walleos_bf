#ifndef __SYS_RTXC_H__
#define __SYS_RTXC_H__

#define CLOCKTICKS_PER_MS 10    //定义时钟节拍

void init_lwip_timer(void);  //初始化LWIP定时器
u8_t timer_expired(u32_t *last_time,u32_t tmr_interval);	//定时器超时判断

#endif /* __SYS_RTXC_H__ */

