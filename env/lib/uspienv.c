//
// uspienv.c
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
#include <uspienv.h>
#include <cache_cp15.h>
#include <headsmp.h>
#include <s5p4418_serial_stdio.h>

TUSPiEnv g_Env;

int USPiEnvInitialize (void)
{
    int ret;

    Timer(get_cpuid());
    printf("TimerInitialize\r\n");
    ret = !TimerInitialize (TimerGet(get_cpuid()));

    CTouchScreenDevice(&g_Env.m_TouchScreenDevice);
    printf("TouchScreenInitialize\r\n");
    ret |= !TouchScreenInitialize(&g_Env.m_TouchScreenDevice);

    CGestureDetector(&g_Env.m_GestureDetector);
    printf("GestureDetectorInitialize\r\n");
    ret |= !GestureDetectorInitialize(&g_Env.m_GestureDetector);

    if(ret)
	{
        printf("USPiENV Init Failed\r\n");

		_Timer(get_cpuid());
		_CTouchScreenDevice(&g_Env.m_TouchScreenDevice);
        _CGestureDetector(&g_Env.m_GestureDetector);

		return FALSE;
	}
    
    printf("USPiENV Init Success\r\n");
	
    return TRUE;
}

