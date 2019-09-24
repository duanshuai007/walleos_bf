/* copyright(C) 2003 H.Kawai (under KL-01). */

#include <errno.h>
#include <limits.h>
#include <types.h>

size_t strtoul0(const s8 **ps, s32 base, u8 *errflag);

ssize_t strtol(const s8 *s, const s8 **endp, s32 base)
{
	const s8 *s0 = s, *s1;
	s8 sign = 0, errflag;
	size_t val;
	while (*s != '\0' && *s <= ' ')
		s++;
	if (*s == '-') {
		sign = 1;
		s++;
	}
	while (*s != '\0' && *s <= ' ')
		s++;
	s1 = s;
	val = strtoul0(&s, base, (u8 *)&errflag);
	if (s == s1)
		s = s0;
	if (endp)
		*endp = s;
	if (val > LONG_MAX) {
		errflag = 1;
		val = LONG_MAX;
		if (sign)
			val = LONG_MIN;
	}
	if (errflag == 0 && sign != 0)
		val = - val;
	if (errflag)
		errno = ERANGE;
	return val;
}
