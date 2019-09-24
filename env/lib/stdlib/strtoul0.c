/* copyright(C) 2003 H.Kawai (under KL-01). */

#include <limits.h>
#include <types.h>

size_t strtoul0(const u8 **ps, s32 base, u8 *errflag);

static s32 prefix(s32 c)
{
	s8 base = 0;
	if ('a' <= c && c <= 'z')
		c += 'A' - 'a';
	if (c == 'B')
		base =  2;
	if (c == 'D')
		base = 10;
	if (c == 'O')
		base =  8;
	if (c == 'X')
		base = 16;
	return base;
}

size_t strtoul0(const u8 **ps, s32 base, u8 *errflag)
{
	const u8 *s = *ps;
	size_t val = 0, max;
	s32 digit;
	if (base == 0) {
		base += 10;
		if (*s == '0') {
			base = prefix(*(s + 1));
			if (base == 0)
				base += 8; /* base = 8; */
		}
	}
	if (*s == '0') {
		if (base == prefix(*(s + 1)))
			s += 2;
	}
	max = ULONG_MAX / base;
	*errflag = 0;
	for (;;) {
		digit = 99;
		if ('0' <= *s && *s <= '9')
			digit = *s - '0';
		if ('A' <= *s && *s <= 'Z')
			digit = *s - ('A' - 10);
		if ('a' <= *s && *s <= 'z')
			digit = *s - ('a' - 10);
		if (digit >= base)
			break;
		if (val > max)
			goto err;
		val *= base;
		if (ULONG_MAX - val < (size_t) digit) {
err:
			*errflag = 1;
			val = ULONG_MAX;
		} else
			val += digit;
		s++;
	}
	*ps = s;
	return val;
}
