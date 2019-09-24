//
// types.h
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
#ifndef _INCLUDE_TYPE_H_
#define _INCLUDE_TYPE_H_

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef char                s8;
typedef short               s16;
typedef int                 s32;
typedef long long           s64;

typedef signed char         s8_t;
typedef signed short        s16_t;
typedef signed int          s32_t;
typedef signed long long    s64_t;

typedef unsigned char       u8_t;
typedef unsigned short      u16_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;

typedef unsigned char       unchar;
typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;


typedef int		            boolean;
typedef int					bool_t;

enum {
    DISABLE = 0,
    ENABLE  = 1,

    FALSE   = 0,
    TRUE    = 1,

	CFALSE	= 0,
	CTRUE	= 1,
};

#define NULL                ((void *)0)

typedef unsigned long	        size_t;
typedef long		            ssize_t;

typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;

typedef signed int              int32_t;
typedef signed long long        int64_t;

typedef signed int				bool_t;

typedef signed long long        intmax_t;
typedef unsigned long long      uintmax_t;

typedef signed int              ptrdiff_t;
typedef signed int              intptr_t;
typedef unsigned int            uintptr_t;

typedef unsigned long           lbaint_t;

typedef __signed__ char         __s8;
typedef unsigned char           __u8;

typedef __signed__ short        __s16;
typedef unsigned short          __u16;

typedef __signed__ int          __s32;
typedef unsigned int            __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef unsigned int            phys_addr_t;
typedef unsigned int            phys_size_t;
typedef unsigned int            virtual_addr_t;
typedef unsigned int            physical_addr_t;

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
#if defined(__GNUC__)
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
#endif

typedef s32	    CBOOL;							///< boolean type is 32bits signed integer

#endif
