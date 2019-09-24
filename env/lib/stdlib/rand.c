/* copyright(C) 2002 H.Kawai (under KL-01). */

#include <types.h>

u32 GO_rand_seed = 1;

s32 GO_rand(void)
{
	GO_rand_seed = GO_rand_seed * 1103515245 + 12345;
	return (u32) (GO_rand_seed >> 16) & 32767;
}
