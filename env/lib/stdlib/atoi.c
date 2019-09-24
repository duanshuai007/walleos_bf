/* copyright(C) 2003 H.Kawai (under KL-01). */

#include <stdlib.h>	/* strtol */
#include <stdio.h>	/* NULL */
#include <stddef.h>

s32 atoi(const s8 *s)
{
	return (s32) strtol(s, (const s8 **) NULL, 10);
}
