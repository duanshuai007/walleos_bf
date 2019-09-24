//
// util.cpp
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
#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void *memset (void *pBuffer, s32 nValue, size_t nLength)
{
    s8 *p = (s8 *) pBuffer;

    while (nLength--)
    {
        *p++ = (s8) nValue;
    }

    return pBuffer;
}

void *memcpy (void *pDest, const void *pSrc, size_t nLength)
{
    s8 *pd = (s8 *) pDest;
    s8 *ps = (s8 *) pSrc;

    while (nLength--)
    {
        *pd++ = *ps++;
    }

    return pDest;
}

s32 memcmp (const void *pBuffer1, const void *pBuffer2, size_t nLength)
{
   	const u8 *p1 = (const u8 *) pBuffer1;
    const u8 *p2 = (const u8 *) pBuffer2;

    while (nLength-- > 0)
    {
        if (*p1 > *p2)
        {
            return 1;
        }
        else if (*p1 < *p2)
        {
            return -1;
        }

        p1++;
        p2++;
    }

    return 0;
}

size_t strlen (const s8 *pString)
{
    size_t nResult = 0;

    while (*pString++)
    {
        nResult++;
    }

    return nResult;
}

s32 strcmp (const s8 *pString1, const s8 *pString2)
{
    while (   *pString1 != '\0'
           && *pString2 != '\0')
    {
        if (*pString1 > *pString2)
        {
            return 1;
        }
        else if (*pString1 < *pString2)
        {
            return -1;
        }

        pString1++;
        pString2++;
    }

    if (*pString1 > *pString2)
    {
        return 1;
    }
    else if (*pString1 < *pString2)
    {
        return -1;
    }

    return 0;
}

s8 *strcpy (s8 *pDest, const s8 *pSrc)
{
    s8 *p = pDest;

    while (*pSrc)
    {
        *p++ = *pSrc++;
    }

    *p = '\0';

    return pDest;
}

s8 *strncpy (s8 *pDest, const s8 *pSrc, size_t nMaxLen)
{
    s8 *pResult = pDest;

    while (nMaxLen > 0)
    {
        if (*pSrc == '\0')
        {
            break;
        }

        *pDest++ = *pSrc++;
        nMaxLen--;
    }

    if (nMaxLen > 0)
    {
        *pDest = '\0';
    }

    return pResult;
}

s8 *strcat (s8 *pDest, const s8 *pSrc)
{
    s8 *p = pDest;

    while (*p)
    {
        p++;
    }

    while (*pSrc)
    {
        *p++ = *pSrc++;
    }

    *p = '\0';

    return pDest;
}

s32 char2int (s8 chValue)
{
    s32 nResult = chValue;

    if (nResult > 0x7F)
    {
        nResult |= -0x100;
    }

    return nResult;
}

u16 le2be16 (u16 usValue)
{
    return    ((usValue & 0x00FF) << 8)
        | ((usValue & 0xFF00) >> 8);
}

u32 le2be32 (u32 ulValue)
{
    return    ((ulValue & 0x000000FF) << 24)
        | ((ulValue & 0x0000FF00) << 8)
        | ((ulValue & 0x00FF0000) >> 8)
        | ((ulValue & 0xFF000000) >> 24);
}

void* memmove (void *d, void *s, size_t sz)
{
    void *tmp = d;
    s8 *dp = (s8*)d;
    s8 *sp = (s8*)s;

    if (dp > sp) {
        dp += sz;
        sp += sz;
        while (sz--)
            *--dp = *--sp;
    } else {
        while (sz--)
            *dp++ = *sp++;
    }

    return tmp;
}

s8* strncat (s8 *d, const s8 *s, size_t sz)
{
    s8 *tmp = d;

    while ('\0' != *d)
        d++;

    while ('\0' != *s) {
        if (0 == sz)
            break;
        sz--;
        *d++ = *s++;
    }

    if (sz)
        *d = '\0';

    return tmp;
}

s8* strchr (s8 *d, s32 c)
{
    while (c != *d) {
        if ('\0' == *d++)
            return 0;
    }

    return d;
}


s32 strncmp (const s8 *d, const s8 *s, size_t sz)
{

#if 0
    /* Gaku's code */

    while ( sz-- && '\0' != *d ) {
        if (*d != *s)
            return *d - *s;
        d++;
        s++;
    }
    return *d - *s; /* bug? (if sz == 0) */

#endif

#if 1
    /* Kawai's code */

    while (sz--) {
        if (*d == '\0')
            goto ret;
        if (*d != *s) {
ret:
            return *d - *s;
        }
        d++;
        s++;
    }
    return 0;
#endif

}

s8* strrchr (s8 *d, s32 c)
{
    s8 *tmp = d;

    while ('\0' != *d)
        d++;

    while (tmp <= d) {
        if (c == *d)
            return d;
        d--;
    }

    return 0;
}

s8* strstr (s8 *d, const s8 *s)
{
    s8 *tmp;
    const s8 *bgn = s;

    while ('\0' != *d) {
        tmp = d;
        while (*d++ == *s++)
            if ('\0' == *d || '\0' == *s)
                return tmp;
        d = tmp+1;
        s = bgn;
    }

    return 0;
}

s8*  strtok_r(s8* string_org,const s8* demial) 
{
    static u8* last; 
    u8* str;         
    const u8* ctrl = (const u8*)demial;
    u8 map[32]; 
    s32 count;
    
    for (count =0; count <32; count++)
    {
        map[count] = 0;
    }   
    
    do 
    {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);     
    
    if (string_org)
    {
        str = (u8*)string_org;
    } 
    else
    {
        str = last;
    }
    
    while ((map[*str >> 3] & (1 << (*str & 7)))  && *str)
    {
        str++;
    } 
    
    string_org = (s8*)str; 
    for(; *str; str++)
    {
        if ( map[*str >> 3] & (1 << (*str & 7)))
        {
            *str++ = '\0';
            break;         
        }         
    }    
    last =str;    
    if (string_org == (s8*)str)
    {
        return 0; 
    }
    else
    {
        return string_org;
    }
}

/*
 * Allocates and duplicates a string
 */
char * strdup(const char * s)
{
    char * p;

    if(!s)
        return NULL;

    p = malloc(strlen(s) + 1);
    if(p)
        return(strcpy(p, s));

    return NULL;
}

     
