//
// util.h
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
#ifndef _INCLUDE_STRING_H_
#define _INCLUDE_STRING_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memset (void *pBuffer, s32 nValue, size_t nLength);

void *memcpy (void *pDest, const void *pSrc, size_t nLength);

s32 memcmp (const void *pBuffer1, const void *pBuffer2, size_t nLength);

size_t strlen (const s8 *pString);

s32 strcmp (const s8 *pString1, const s8 *pString2);

s8 *strcpy (s8 *pDest, const s8 *pSrc);

s8 *strncpy (s8 *pDest, const s8 *pSrc, size_t nMaxLen);

s8 *strcat (s8 *pDest, const s8 *pSrc);

s32 char2int (s8 chValue);			// with sign extension

u16 le2be16 (u16 usValue);

u32 le2be32 (u32 ulValue);

// util_fast
void *memcpyblk (void *pDest, const void *pSrc, size_t nLength);	// nLength must be multiple of 16

void* memmove (void *d, void *s, size_t sz);

s8* strncat (s8 *d, const s8 *s, size_t sz);

s8* strchr (s8 *d, s32 c);

s32 strncmp (const s8 *d, const s8 *s, size_t sz);

s8* strrchr (s8 *d, s32 c);

s8* strstr (s8 *d, const s8 *s);

s8*  strtok_r(s8* string_org,const s8* demial);

char * strdup(const char * s);

#ifdef __cplusplus
}
#endif

#endif
