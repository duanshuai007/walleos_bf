/* copyright(C) 2002 H.Kawai (under KL-01). */

#include <stdarg.h>
#include <stdio.h>

s32 sprintf(s8 *s, const s8 *format, ...)
{
	s32 i;
	va_list ap;

	va_start(ap, format);
	i = vsprintf(s, format, ap);
	va_end(ap);
	return i;
}

