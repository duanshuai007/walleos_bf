/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ENV_INCLUDE_DRIVERS_COMMON_H_
#define _ENV_INCLUDE_DRIVERS_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define ARCH_DMA_MINALIGN	64

#define min(X, Y)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		(__x > __y) ? __x : __y; })

#define MIN(x, y)  min(x, y)
#define MAX(x, y)  max(x, y)

#define min3(X, Y, Z)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		typeof(Z) __z = (Z);		\
		__x < __y ? (__x < __z ? __x : __z) :	\
		(__y < __z ? __y : __z); })

#define max3(X, Y, Z)				\
	({ typeof(X) __x = (X);			\
		typeof(Y) __y = (Y);		\
		typeof(Z) __z = (Z);		\
		__x > __y ? (__x > __z ? __x : __z) :	\
		(__y > __z ? __y : __z); })

#define MIN3(x, y, z)  min3(x, y, z)
#define MAX3(x, y, z)  max3(x, y, z)

#define ROUND(a,b)		(((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND(n,d)		(((n) + ((d)/2)) / (d))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define roundup(x, y)		((((x) + ((y) - 1)) / (y)) * (y))

#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))

#define PAD_COUNT(s, pad) (((s) - 1) / (pad) + 1)
#define PAD_SIZE(s, pad) (PAD_COUNT(s, pad) * pad)
#define ALLOC_ALIGN_BUFFER_PAD(type, name, size, align, pad)		\
	char __##name[ROUND(PAD_SIZE((size) * sizeof(type), pad), align)  \
		      + (align - 1)];					\
									\
	type *name = (type *) ALIGN((uintptr_t)__##name, align)
#define ALLOC_ALIGN_BUFFER(type, name, size, align)		\
	ALLOC_ALIGN_BUFFER_PAD(type, name, size, align, 1)
#define ALLOC_CACHE_ALIGN_BUFFER_PAD(type, name, size, pad)		\
	ALLOC_ALIGN_BUFFER_PAD(type, name, size, ARCH_DMA_MINALIGN, pad)
#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size)			\
	ALLOC_ALIGN_BUFFER(type, name, size, ARCH_DMA_MINALIGN)


#define DEFINE_ALIGN_BUFFER(type, name, size, align)			\
	static char __##name[roundup(size * sizeof(type), align)]	\
			__aligned(align);				\
									\
	static type *name = (type *)__##name
#define DEFINE_CACHE_ALIGN_BUFFER(type, name, size)			\
	DEFINE_ALIGN_BUFFER(type, name, size, ARCH_DMA_MINALIGN)

#ifdef __cplusplus
}
#endif

#endif	/* __COMMON_H_ */
