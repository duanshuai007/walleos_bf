//
// synchronize.cpp
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2016  R. Stange <rsta2@o2online.de>
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
#include <synchronize.h>
#include <types.h>
#include <stddef.h>
#include <assert.h>
#include <headsmp.h>

static volatile unsigned s_nCriticalLevel[4] = {0, 0, 0, 0};
static volatile boolean s_bWereEnabled[4] = {FALSE, FALSE, FALSE, FALSE};

void EnterCritical (void)
{
	u32 nFlags, cpuid;
	//查询cpsr寄存器
    //__asm volatile ("mrs %0, cpsr" : "=r" (nFlags));
    cpuid = get_cpuid();
	assert(cpuid < 4);

    if (s_nCriticalLevel[cpuid] == 0)
	{   //第7位时中断屏蔽位，=1禁止中断，=0允许中断
        nFlags = getcpsr();
        s_bWereEnabled[cpuid] = nFlags & 0x80 ? FALSE : TRUE;
        DisableInterrupts();    
	}
    
    s_nCriticalLevel[cpuid]++;

    //DataMemBarrier();
    DataSyncBarrier();
}

void LeaveCritical (void)
{
    u32 cpuid = get_cpuid();
	assert(cpuid < 4);	
    //DataMemBarrier ();
    DataSyncBarrier();

    if(s_nCriticalLevel[cpuid] > 0)
        s_nCriticalLevel[cpuid]--;

    if(s_nCriticalLevel[cpuid] == 0)
    {
        if (s_bWereEnabled[cpuid])
        {
            EnableInterrupts();
        }
    }
}

u32 get_cpu_critical(u8 cpu)
{
    return s_nCriticalLevel[cpu];
}

boolean get_cpu_enable(u8 cpu)
{
    return s_bWereEnabled[cpu];
}
// Cache maintenance operations for ARMv7-A
//
// See: ARMv7-A Architecture Reference Manual, Section B4.2.1
//
// NOTE: The following functions should hold all variables in CPU registers. Currently this will be
//	 ensured using the register keyword and maximum optimation (see uspienv/synchronize.h).
//
//	 The following numbers can be determined (dynamically) using CTR, CSSELR, CCSIDR and CLIDR.
//	 As long we use the Cortex-A7 implementation in the BCM2836 or the Cortex-A53 implementation
//	 in the BCM2837 these static values will work:
//

void InvalidateDataCache (void)
{
	// invalidate L1 data cache
	for (register unsigned nSet = 0; nSet < L1_DATA_CACHE_SETS; nSet++)
	{
		for (register unsigned nWay = 0; nWay < L1_DATA_CACHE_WAYS; nWay++)
		{
			register u32 nSetWayLevel =   nWay << L1_SETWAY_WAY_SHIFT
						    | nSet << L1_SETWAY_SET_SHIFT
						    | 0 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("mcr p15, 0, %0, c7, c6,  2" : : "r" (nSetWayLevel) : "memory");	// DCISW
		}
	}

	// invalidate L2 unified cache
	for (register unsigned nSet = 0; nSet < L2_CACHE_SETS; nSet++)
	{
		for (register unsigned nWay = 0; nWay < L2_CACHE_WAYS; nWay++)
		{
			register u32 nSetWayLevel =   nWay << L2_SETWAY_WAY_SHIFT
						    | nSet << L2_SETWAY_SET_SHIFT
						    | 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("mcr p15, 0, %0, c7, c6,  2" : : "r" (nSetWayLevel) : "memory");	// DCISW
		}
	}
}

void CleanDataCache (void)
{
	// clean L1 data cache
	for (register unsigned nSet = 0; nSet < L1_DATA_CACHE_SETS; nSet++)
	{
		for (register unsigned nWay = 0; nWay < L1_DATA_CACHE_WAYS; nWay++)
		{
			register u32 nSetWayLevel =   nWay << L1_SETWAY_WAY_SHIFT
						    | nSet << L1_SETWAY_SET_SHIFT
						    | 0 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("mcr p15, 0, %0, c7, c10,  2" : : "r" (nSetWayLevel) : "memory");	// DCCSW
		}
	}

	// clean L2 unified cache
	for (register unsigned nSet = 0; nSet < L2_CACHE_SETS; nSet++)
	{
		for (register unsigned nWay = 0; nWay < L2_CACHE_WAYS; nWay++)
		{
			register u32 nSetWayLevel =   nWay << L2_SETWAY_WAY_SHIFT
						    | nSet << L2_SETWAY_SET_SHIFT
						    | 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("mcr p15, 0, %0, c7, c10,  2" : : "r" (nSetWayLevel) : "memory");	// DCCSW
		}
	}
}

void InvalidateDataCacheRange (u32 nAddress, u32 nLength)
{
	nLength += DATA_CACHE_LINE_LENGTH_MIN;

	while (1)
	{
		__asm volatile ("mcr p15, 0, %0, c7, c6,  1" : : "r" (nAddress) : "memory");	// DCIMVAC

		if (nLength < DATA_CACHE_LINE_LENGTH_MIN)
		{
			break;
		}

		nAddress += DATA_CACHE_LINE_LENGTH_MIN;
		nLength  -= DATA_CACHE_LINE_LENGTH_MIN;
	}
}

void CleanDataCacheRange (u32 nAddress, u32 nLength)
{
	nLength += DATA_CACHE_LINE_LENGTH_MIN;

	while (1)
	{
		__asm volatile ("mcr p15, 0, %0, c7, c10,  1" : : "r" (nAddress) : "memory");	// DCCMVAC

		if (nLength < DATA_CACHE_LINE_LENGTH_MIN)
		{
			break;
		}

		nAddress += DATA_CACHE_LINE_LENGTH_MIN;
		nLength  -= DATA_CACHE_LINE_LENGTH_MIN;
	}
}

void CleanAndInvalidateDataCacheRange (u32 nAddress, u32 nLength)
{
	nLength += DATA_CACHE_LINE_LENGTH_MIN;

	while (1)
	{
		__asm volatile ("mcr p15, 0, %0, c7, c14,  1" : : "r" (nAddress) : "memory");	// DCCIMVAC

		if (nLength < DATA_CACHE_LINE_LENGTH_MIN)
		{
			break;
		}

		nAddress += DATA_CACHE_LINE_LENGTH_MIN;
		nLength  -= DATA_CACHE_LINE_LENGTH_MIN;
	}
}

