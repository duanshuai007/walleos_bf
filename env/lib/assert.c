//
// assert.cpp
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
#include <assert.h>
#include <debug.h>
#include <stdio.h>

#include <config.h>

#ifndef NDEBUG

void assertion_failed (const s8 *pExpr, const s8 *pFile, u32 nLine)
{
	u32 ulStackPtr;
	asm volatile ("mov %0,sp" : "=r" (ulStackPtr));

	s8 Source[1024];
	sprintf (Source, "%s(%u)", pFile, nLine);

	debug_stacktrace ((u32 *) ulStackPtr, Source);
	
	printf("assertion failed: %s, file:%s(line:%d)\r\n", pExpr, pFile, nLine);
}

#endif
