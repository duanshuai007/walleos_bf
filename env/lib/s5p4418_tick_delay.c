/*
 * s5p4418-tick-delay.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <s5p4418_tick_delay.h>
#include <timer.h>
#include <s5p4418_serial_stdio.h>
#include <string.h>
#include <config.h>

extern void __delay(u32 ss);
extern void __udelay(u32 us);

static const u32_t VALUE_1M = 1000000;
static const u32_t VALUE_1K = 1000;

//该值保存了每0.5个tick时间范围内CPU可以执行空指令的条数
volatile u32_t loops_per_jiffy = 0;

//void __attribute__ ((noinline)) __delay(volatile u32_t loop)
//{
//	for(; loop > 0; loop--);
//}

//void udelay(u32_t us)
//{
//	u32_t hz = get_system_hz();
//
//	if(hz)
//		__delay(us * loops_per_jiffy / (VALUE_1M/ hz));
//	else
//		__delay(us);
//}

//void mdelay(u32_t ms)
//{
//	u32_t hz = get_system_hz();
//
//	//printf("[cpu-%d][%s]:hz = %d\r\n", cpuid, __func__,  hz);
//
//	if(hz)
//		__delay(ms * loops_per_jiffy / (VALUE_1K / hz));
//	else
//		__delay(ms * 1000);
//}

void udelay(u32_t us)
{
    __udelay(us);
}

#ifndef DELAY_SWITCH
void mdelay(u32_t ms)
{
    while(ms--)
    {
        __udelay(1000);
    }
}
#endif

extern volatile u32_t jiffies;

static void calibrate_delay(void)
{
	u32_t ticks, loopbit;
	s32_t lps_precision = 8;
	u32_t hz = get_system_hz();

    printf("System : Hz = %d\r\n", hz);

	if(hz > 0)
	{
        //设置一个大概的范围
		loops_per_jiffy = (1<<12);
        //在较大范围内确定loops_per_jiffy的范围值
		while((loops_per_jiffy <<= 1) != 0)
		{
			/* wait for "start of" clock tick */
			ticks = jiffies;
	        //死等中断
            while (ticks == jiffies)
            {
                //printf("wait\r\n");
            }
			/* go ... */
			ticks = jiffies;
			__delay(loops_per_jiffy);
			//DelayLoop(loops_per_jiffy);
			ticks = jiffies - ticks;

			if(ticks)
				break;
		}

		loops_per_jiffy >>= 1;
		loopbit = loops_per_jiffy;
        //采用逐次逼近的方式获得一个比较准确的数值
		while(lps_precision-- && (loopbit >>= 1))
		{
			loops_per_jiffy |= loopbit;
			ticks = jiffies;
			while(ticks == jiffies);

			ticks = jiffies;
			__delay(loops_per_jiffy);
			//DelayLoop(loops_per_jiffy);

			/* longer than 1 tick */
			if(jiffies != ticks)
				loops_per_jiffy &= ~loopbit;
		}
	}
	else
	{
		loops_per_jiffy = 0;
	}
    //printf("exit calibrate_delay, loops_per_jiffy = %d\r\n", loops_per_jiffy);
	printf("loops_per_jiffy = %d\r\n", loops_per_jiffy);
}

void s5p4418_tick_delay_initial(void)
{
	calibrate_delay();
}
