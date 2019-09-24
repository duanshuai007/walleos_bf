/* copyright(C) 2003 H.Kawai (under KL-01). */

#ifndef _INCLUDE_STDLIB_H_
#define _INCLUDE_STDLIB_H_

#include <types.h>

#if (defined(__cplusplus))
	extern "C" {
#endif

#define EXIT_SUCCESS		0
#define EXIT_FAILURE		1

extern  u32  GO_rand_seed;
#define RAND_MAX			32767

s32 abs(s32 n);
s32 atoi(const s8 *s);
ssize_t strtol(const s8 *s, const s8 **endp, s32 base);
size_t strtoul(const s8 *s, const s8 **endp, s32 base);
s32 GO_rand(void);

#define rand()				GO_rand()
#define srand(seed)			(void) (GO_rand_seed = (seed))

#if (defined(__cplusplus))
	}
#endif

#endif
