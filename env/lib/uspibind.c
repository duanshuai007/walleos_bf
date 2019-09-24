//
// uspibind.cpp
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
#include <uspios.h>
#include <timer.h>
#include <interrupt.h>
#include <debug.h>
#include <string.h>
#include <assert.h>
#include <headsmp.h>
#include <input/touchscreen.h>

unsigned StartKernelTimer (unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext)
{
	TTimer *this = TimerGet(get_cpuid());

	//printf("StartKernelTimer: %08x, ndelay = %d, pHandler = %08x, pParam = %08x, cCpntex = %08x\r\n",
	//		this, nDelay, pHandler, pParam, pContext);
	return TimerStartKernelTimer (this, nDelay, pHandler, pParam, pContext);
}

void CancelKernelTimer (unsigned hTimer)
{
	TimerCancelKernelTimer (TimerGet(get_cpuid()), hTimer);
}

void UpdateKernelTimer(unsigned hTimer, unsigned nDelay)
{
    TimerUpdateKernelTimer (TimerGet(get_cpuid()), hTimer, nDelay);
}

boolean USPiTouchScreenUpdate(void)
{
	return TouchScreenUpdate(TouchScreenDeviceGet());
}
