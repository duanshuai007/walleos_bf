//
// debug.h
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
#ifndef _ENV_INCLUDE_DEBUG_H_
#define _ENV_INCLUDE_DEBUG_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG

//void debug_hexdump (const void *pStart, u32 nBytes, const s8 *pSource /* = 0 */);

void debug_stacktrace (const u32 *pStackPtr, const s8 *pSource /* = 0 */);

#endif

#ifdef __cplusplus
}
#endif

#endif
