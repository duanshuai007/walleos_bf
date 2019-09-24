#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "timer.h"
#include "arch/sys_arch.h"
#include "stdlib.h" 
#include <uspios.h>

#include <synchronize.h>
#include <s5p4418_tick_delay.h>

u32_t sys_now(void)
{
	//return TimerGetTicks(TimerGet())*10;
	return TimerGetTicks();
}

u8_t timer_expired(u32_t *last_time, u32_t tmr_interval)
{
    u32_t lwip_timer, time;
    //lwip_timer = TimerGetTicks(TimerGet())*10;
    lwip_timer = TimerGetTicks();

    if(lwip_timer == 0)
        return 0;

	time = *last_time;	
	
    if((lwip_timer - time) >= tmr_interval)
    {
	    *last_time = lwip_timer;
        return 1;
	}

	return 0;
}

