//
// synchronize.h
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
#ifndef _ENV_INCLUDE_SYNCHRONIZE_H_ 
#define _ENV_INCLUDE_SYNCHRONIZE_H_

#include <macros.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum cache{
    SETWAY_LEVEL_SHIFT = 1,
    L1_DATA_CACHE_WAYS = 4,
    L1_DATA_CACHE_SETS = 128,
    L1_SETWAY_WAY_SHIFT = 30,
    L1_DATA_CACHE_LINE_LENGTH = 64,
    L1_SETWAY_SET_SHIFT = 6,
    L2_CACHE_SETS = 512,
    L2_CACHE_WAYS = 16,
    L2_SETWAY_WAY_SHIFT = 28,
    L2_CACHE_LINE_LENGTH = 64,
    L2_SETWAY_SET_SHIFT = 6,
    DATA_CACHE_LINE_LENGTH_MIN = 64,
};
//
// Interrupt control
//
inline void EnableInterrupts(void)
{
    asm volatile ("cpsie i"
            :
            :
            : "memory", "cc");
}

inline void DisableInterrupts(void)
{
    asm volatile ("cpsid i"
            :
            :
            : "memory", "cc");
}
//
// Cache control
//
inline void InvalidateInstructionCache(void)
{
        __asm volatile ("mcr p15, 0, %0, c7, c5,  0" : : "r" (0) : "memory");
}

inline void FlushPrefetchBuffer(void)
{
        __asm volatile ("isb" ::: "memory");
}

inline void FlushBranchTargetCache(void)
{
        __asm volatile ("mcr p15, 0, %0, c7, c5,  6" : : "r" (0) : "memory");
}

//
// Barriers
//
inline void DataSyncBarrier(void)
{
        __asm volatile ("dsb" ::: "memory");
}

inline void DataMemBarrier(void)
{
        __asm volatile ("dmb" ::: "memory");
}

inline void InstructionSyncBarrier(void)
{
        __asm volatile ("isb" ::: "memory");
}

inline void InstructionMemBarrier(void)
{
        __asm volatile ("isb" ::: "memory");
}

inline void CompilerBarrier(void)
{
        __asm volatile ("" ::: "memory");
}

void EnterCritical (void);
void LeaveCritical (void);

void InvalidateDataCache (void) MAXOPT;
void CleanDataCache (void) MAXOPT;

void InvalidateDataCacheRange (u32 nAddress, u32 nLength) MAXOPT;
void CleanDataCacheRange (u32 nAddress, u32 nLength) MAXOPT;
void CleanAndInvalidateDataCacheRange (u32 nAddress, u32 nLength) MAXOPT;

#ifdef __cplusplus
}
#endif

#endif
