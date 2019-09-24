//
// timer.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <alloc.h>
#include <timer.h>
#include <synchronize.h>
#include <debug.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <s5p4418_timer.h>
#include <config.h>
#include <errno.h>
#include <headsmp.h>
#include <s5p4418_serial_stdio.h>
#include <headsmp.h>

extern void __delay(u32 ss);

volatile u32_t jiffies = 0;

void TimerPollKernelTimers (TTimer *pThis);
void TimerInterruptHandler (void *pParam);

static TTimer s_Timer[CPU_MAX_NUM];

void Timer (u8 cpu)
{
	TTimer *pThis = TimerGet(cpu);
	assert (pThis != NULL);

    memset(s_Timer, 0, sizeof(TTimer)*CPU_MAX_NUM);

	pThis->m_nTicks = 0;
	pThis->m_nTime = 0;
#ifdef ARM_DISABLE_MMU
	pThis->m_nMsDelay = 12500;
#else
	pThis->m_nMsDelay = 350000;
#endif
	pThis->m_nusDelay = pThis->m_nMsDelay / 1000;

	for (unsigned hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		pThis->m_KernelTimer[hTimer].m_pHandler = 0;
	}
}

void _Timer (u8 cpu)
{
    //int cpu = get_cpuid();
    assert(cpu < CPU_MAX_NUM);

    memset(&(s_Timer[cpu]), 0, sizeof(TTimer));	
}

boolean TimerInitialize (TTimer *pThis)
{
	assert(pThis != NULL);

    request_irq("TIMER1", CPU0_ID, TimerInterruptHandler, IRQ_TYPE_NONE, pThis);

    return TRUE;
}

unsigned TimerGetTicks(void)
{
    return jiffies;
}

unsigned TimerGetTime (TTimer *pThis)
{
	assert (pThis != NULL);

	return pThis->m_nTime;
}

char *TimerGetTimeString (TTimer *pThis)
{
	assert (pThis != NULL);

	EnterCritical ();

	unsigned nTime = pThis->m_nTime;
	//unsigned nTicks = pThis->m_nTicks;
	unsigned nTicks = jiffies;

	LeaveCritical ();

	if (nTicks == 0)
	{
		return NULL;
	}

	unsigned nSecond = nTime % 60;
	nTime /= 60;
	unsigned nMinute = nTime % 60;
	nTime /= 60;
	unsigned nHours = nTime;

	nTicks %= HZ;
#if (HZ != 100)
	nTicks = nTicks * 100 / HZ;
#endif
    //在程序调用TimerGetTimeString处有对应的free
    char * pString = (char *)malloc(32);
    if(pString == NULL)
        return NULL;
    memset(pString, 0, 32);

	sprintf (pString, "%02u:%02u:%02u.%02lu", nHours, nMinute, nSecond, nTicks);

	return pString;
}

boolean IsLeapYear (unsigned nYear)
{
	if (nYear % 100 == 0)
	{
		return nYear % 400 == 0;
	}

	return nYear % 4 == 0;
}

const unsigned s_nDaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

unsigned GetDaysOfMonth (unsigned nMonth, unsigned nYear)
{
	if ((nMonth == 1) && (IsLeapYear (nYear)))
	{
		return 29;
	}

	assert (nMonth <= 11);
	return s_nDaysOfMonth[nMonth];
}

unsigned TimerStartKernelTimer (TTimer *pThis, unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext)
{
	assert (pThis != NULL);

	EnterCritical ();

	unsigned hTimer;
	for (hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		if (pThis->m_KernelTimer[hTimer].m_pHandler == 0)
		{
			break;
		}
	}

	if (hTimer >= KERNEL_TIMERS)
	{
		LeaveCritical ();
		printf("timer System limit of kernel timers exceeded\r\n");
        return FALSE;
	}

	assert (pHandler != NULL);
	pThis->m_KernelTimer[hTimer].m_pHandler    = pHandler;
	pThis->m_KernelTimer[hTimer].m_nElapsesAt  = jiffies + nDelay;
	//pThis->m_KernelTimer[hTimer].m_nElapsesAt  = pThis->m_nTicks+nDelay;
	pThis->m_KernelTimer[hTimer].m_pParam      = pParam;
	pThis->m_KernelTimer[hTimer].m_pContext    = pContext;

	LeaveCritical ();

	return hTimer+1;
}

void TimerCancelKernelTimer (TTimer *pThis, unsigned hTimer)
{
	assert (pThis != NULL);

	assert (1 <= hTimer && hTimer <= KERNEL_TIMERS);
	pThis->m_KernelTimer[hTimer-1].m_pHandler = 0;
}

void TimerUpdateKernelTimer (TTimer *pThis, unsigned hTimer, unsigned nDelay)
{
	assert (pThis != NULL);
	assert (1 <= hTimer && hTimer <= KERNEL_TIMERS);

	pThis->m_KernelTimer[hTimer-1].m_nElapsesAt = jiffies + nDelay;
}

//void TimerMsDelay (TTimer *pThis, unsigned nMilliSeconds)
//{
//	assert (pThis != NULL);
//
//	if (nMilliSeconds > 0)
//	{
//		unsigned nCycles =  pThis->m_nMsDelay * nMilliSeconds;
//
//        __delay(nCycles);
//	}
//}
//
//void TimerusDelay (TTimer *pThis, unsigned nMicroSeconds)
//{
//	assert (pThis != NULL);
//
//	if (nMicroSeconds > 0)
//	{
//		unsigned nCycles =  pThis->m_nusDelay * nMicroSeconds;
//
//        __delay(nCycles);
//	}
//}

TTimer *TimerGet(u8 cpu_id)
{
    //int cpu = get_cpuid();	
    assert(cpu_id < CPU_MAX_NUM);

	return &(s_Timer[cpu_id]);
}


void TimerPollKernelTimers (TTimer *pThis)
{
	assert (pThis != NULL);

	EnterCritical ();

	for (unsigned hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		volatile TKernelTimer *pTimer = &pThis->m_KernelTimer[hTimer];

		TKernelTimerHandler *pHandler = pTimer->m_pHandler;
		//printf("TimerPollKernelTimers pHandler = %08x %d %d\r\n", pHandler, pTimer->m_nElapsesAt, jiffies);
		if (pHandler != NULL)
		{
            //if ((int) (pTimer->m_nElapsesAt - pThis->m_nTicks) <= 0)
			if ((int) (pTimer->m_nElapsesAt - jiffies) <= 0)
			{
				pTimer->m_pHandler = 0;
				(*pHandler) (hTimer+1, pTimer->m_pParam, pTimer->m_pContext);
			}
		}
	}

	LeaveCritical ();
}

void TimerInterruptHandler (void *pParam)
{
    const int TIMER_CHANNEL = 1;
	TTimer *pThis = (TTimer *) pParam;
	assert (pThis != NULL);

	if(get_cpuid() == DO_TICK_CPU)
	{
		if (++jiffies % HZ == 0)
		{
			pThis->m_nTime++;   //1Sec inc
		}
	}
	TimerPollKernelTimers (pThis);
    timer_irq_clear(TIMER_CHANNEL);
}

u32_t get_system_hz(void)
{
    return HZ;
}

u64_t clock_gettime(void)
{
    //系统时间为us级，将其放大到秒级别，需要乘1000 000
    if(get_system_hz() > 0)
        return (u64_t)jiffies * 1000000 / get_system_hz();

    return 0;
}

u64_t get_timer(u64_t base)
{
    u64_t time = get_system_hz();
    u64_t hz = HZ;

    return (time/hz - base);
}

void msleep(u32 msecond)
{
    u32 timeout;

    timeout = jiffies + msecond;
    while (time_before(jiffies, timeout));
}

