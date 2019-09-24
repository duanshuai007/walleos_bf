/* copyright(C) 2003 H.Kawai (under KL-01). */


#include <types.h>

s32 abs(s32 n)
{
	if (n < 0)
		n = - n;
	return n;
}
