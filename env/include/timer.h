//
// timer.h
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#ifndef _ENV_INCLUDE_TIMER_H_ 
#define _ENV_INCLUDE_TIMER_H_

#include <interrupt.h>
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void TKernelTimerHandler (unsigned hTimer, void *pParam, void *pContext);

typedef struct TKernelTimer
{
	TKernelTimerHandler *m_pHandler;
	unsigned	     m_nElapsesAt;
	void 		    *m_pParam;
	void 		    *m_pContext;
}
TKernelTimer;

typedef struct TTimer
{
	volatile unsigned	 m_nTicks;
	volatile unsigned	 m_nTime;
	volatile TKernelTimer	 m_KernelTimer[KERNEL_TIMERS];	// TODO: should be linked list
	unsigned		 m_nMsDelay;
	unsigned		 m_nusDelay;
}
TTimer;

void Timer (u8 cpu);
void _Timer (u8 cpu);

boolean TimerInitialize (TTimer *pThis);

void TimerInterruptHandler (void *pParam);
//unsigned TimerGetClockTicks (TTimer *pThis);		// 1 MHz counter
//unsigned TimerGetTicks (TTimer *pThis);			// 1/HZ seconds since system boot
unsigned TimerGetTicks(void);			        // ms seconds since system boot
unsigned TimerGetTime (TTimer *pThis);			// Seconds since system boot

/*
 *  返回时间字符串的地址,失败返回NULL
 *  “HH:MM:SS.ss”
 *  在使用完之后必须使用free进行内存释放
 */
char* TimerGetTimeString (TTimer *pThis);		// CString object must be deleted by caller

boolean IsLeapYear (unsigned nYear);
unsigned GetDaysOfMonth (unsigned nMonth, unsigned nYear);

// returns timer handle (0 on failure)
unsigned TimerStartKernelTimer (TTimer *pThis,
				unsigned nDelay,		// in HZ units
				TKernelTimerHandler *pHandler,
				void *pParam,
				void *pContext);
void TimerCancelKernelTimer (TTimer *pThis, unsigned hTimer);
void TimerUpdateKernelTimer (TTimer *pThis, unsigned hTimer, unsigned nDelay);

// when a CTimer object is available better use these methods
//void TimerMsDelay (TTimer *pThis, unsigned nMilliSeconds);
//void TimerusDelay (TTimer *pThis, unsigned nMicroSeconds);

TTimer *TimerGet (u8 cpu_id);

// can be used before Timer is constructed
//void TimerSimpleMsDelay (unsigned nMilliSeconds);
//void TimerSimpleusDelay (unsigned nMicroSeconds);

u32_t get_system_hz(void);
u64_t clock_gettime(void);
u64_t get_timer(u64_t base);
void msleep(u32 msecond);

#ifdef __cplusplus
}
#endif

#endif
