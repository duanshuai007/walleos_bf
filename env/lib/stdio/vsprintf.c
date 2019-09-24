/* copyright(C) 2003 H.Kawai (under KL-01). */

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include <types.h>

size_t strtoul0(const s8 **ps, s32 base, u8 *errflag);

static u8 *setdec(u8 *s, u32 ui)
{
	do {
		*--s = (ui % 10) + '0';
	} while (ui /= 10);
	return s;
}

s32 vsprintf(s8 *s, const s8 *format, va_list arg)
{
	u8 c, *t = (u8 *)s, *p, flag_left, flag_zero /* , flag_sign, flag_space */;
	u8 temp[32] /* êîéöóp */, *q;
	temp[31] = '\0';
	s32 field_min, field_max, i;
	ssize_t l;
	static s8 hextable_X[16] = "0123456789ABCDEF";
	static s8 hextable_x[16] = "0123456789abcdef";
	for (;;) {
		c = *format++;
		if (c != '%') {
put1char:
			*t++ = c;
			if (c)
				continue;
			return t - (u8 *) s - 1;
		}
		flag_left = flag_zero = /* flag_sign = flag_space = flag_another = */ 0;
		c = *format++;
		for (;;) {
			if (c == '-')
				flag_left = 1;
			else if (c == '0')
				flag_zero = 1;
			else
				break;
			c = *format++;
		}
		field_min = 0;
		if ('1' <= c && c <= '9') {
			format--;
			field_min = (s32) strtoul0(&format, 10, &c);
			c = *format++;
		} else if (c == '*') {
			field_min = va_arg(arg, s32);
			c = *format++;
		}
		field_max = INT_MAX;
		if (c == '.') {
			c = *format++;
			if ('1' <= c && c <= '9') {
				format--;
				field_min = (s32) strtoul0(&format, 10, &c);
				c = *format++;
			} else if (c == '*') {
				field_max = va_arg(arg, s32);
				c = *format++;
			}
		}
		if (c == 's') {
			if (field_max != INT_MAX)
				goto mikan;
			p = (u8 *)va_arg(arg, s8 *);
			l = strlen((s8 *)p);
			if (*p) {
				c = ' ';
copy_p2t:
				if (flag_left == 0) {
					while (l < field_min) {
						*t++ = c;
						field_min--;
					}
				}
				do {
					*t++ = *p++;
				} while (*p);
			}
			while (l < field_min) {
				*t++ = ' ';
				field_min--;
			}
			continue;
		}
		if (c == 'l') {
			c = *format++;
			if (c != 'd' && c != 'x' && c != 'u') {
				format--;
				goto mikan;
			}
		}
		if (c == 'u') {
			l = va_arg(arg, u32);
			goto printf_u;
		}
		if (c == 'd') {
printf_d:
			l = va_arg(arg, ssize_t);
			if (l < 0) {
				*t++ = '-';
				l = - l;
				field_min--;
			}
printf_u:
			if (field_max != INT_MAX)
				goto mikan;
			if (field_min <= 0)
				field_min = 1;
			p = setdec(&temp[31], l);
printf_x2:
			c = ' ';
			l = &temp[31] - p;
			if (flag_zero)
				c = '0';
			goto copy_p2t;
		}
		if (c == 'i')
			goto printf_d;
		if (c == '%')
			goto put1char;
		if (c == 'x') {
			q = (u8 *)hextable_x;
printf_x:
			l = va_arg(arg, u32);
			p = &temp[31];
			do {
				*--p = q[l & 0x0f];
			} while ((*(u32 *) (void *)&l) >>= 4);
			goto printf_x2;
		}
		if (c == 'X') {
			q = (u8 *)hextable_X;
			goto printf_x;
		}
		if (c == 'p') {
			i = (s32) va_arg(arg, void *);
			p = &temp[31];
			for (l = 0; l < 8; l++) {
				*--p = hextable_X[i & 0x0f];
				i >>= 4;
			}
			goto copy_p2t;
		}
		if (c == 'o') {
			l = va_arg(arg, u32);
			p = &temp[31];
			do {
				*--p = hextable_x[l & 0x07];
			} while ((*(u32 *) (void *)&l) >>= 3);
			goto printf_x2;
		}
		if (c == 'f') {
			if (field_max == INT_MAX)
				field_max = 6;
			/* for ese */
			if (field_min < field_max + 2)
				field_min = field_max + 2;
			do {
				*t++ = '#';
			} while (--field_min);
			continue;
		}
mikan:
		for(;;);
	}
}
