//
// debug.cpp
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
#include <debug.h>
#include <stdio.h>
#include <types.h>

#ifndef NDEBUG

static const s8 s_FromDebug[] = "debug";

//void debug_hexdump (const void *pStart, u32 nBytes, const s8 *pSource)
//{
//    if((pStart == NULL) || (pSource == NULL))
//    {
//        return;
//    }
//
//	u8 *pOffset = (u8 *) pStart;
//
//	if (pSource == 0)
//	{
//		pSource = s_FromDebug;
//	}
//
//	printf ("Dumping 0x%X bytes starting at 0x%X", nBytes, (u32) pOffset);
//	
//	while (nBytes > 0)
//	{
//		printf ("%04X: %02X %02X %02X %02X %02X %02X %02X %02X\r\n%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
//				(u32) pOffset & 0xFFFF,
//				(u32) pOffset[0],  (u32) pOffset[1],  (u32) pOffset[2],  (u32) pOffset[3],
//				(u32) pOffset[4],  (u32) pOffset[5],  (u32) pOffset[6],  (u32) pOffset[7],
//				(u32) pOffset[8],  (u32) pOffset[9],  (u32) pOffset[10], (u32) pOffset[11],
//				(u32) pOffset[12], (u32) pOffset[13], (u32) pOffset[14], (u32) pOffset[15]);
//
//		pOffset += 16;
//
//		if (nBytes >= 16)
//		{
//			nBytes -= 16;
//		}
//		else
//		{
//			nBytes = 0;
//		}
//	}
//}

void debug_stacktrace (const u32 *pStackPtr, const s8 *pSource)
{
    if(pStackPtr == NULL)
    {
        return;
    }
	if (pSource == NULL)
	{
		pSource = s_FromDebug;
	}
	
	u32 i = 0;
	for (; i < 64; i++, pStackPtr++)
	{
        if(i%8 == 0) printf("\r\n");
        printf ("stack[%u] is 0x%X", i, (u32) *pStackPtr);
	}
    printf("\r\n");
}

#endif
