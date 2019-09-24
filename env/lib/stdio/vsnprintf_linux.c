#include <types.h>
#include <stdarg.h>
#include <string.h>
#include <config.h>
#include <div64.h>

#define ULLONG_MAX		((unsigned long long)~0ULL)	

#define ZEROPAD 1       /* pad with zero */
#define SIGN    2       /* unsigned/signed long */
#define PLUS    4       /* show plus */
#define SPACE   8       /* space if plus */
#define LEFT    16      /* left justified */
#define SMALL   32      /* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL 64      /* prefix hex with "0x", octal with "0" */

#define _U  0x01    /* upper */
#define _L  0x02    /* lower */
#define _D  0x04    /* digit */
#define _C  0x08    /* cntrl */
#define _P  0x10    /* punct */
#define _S  0x20    /* white space (space/lf/tab) */
#define _X  0x40    /* hex digit */
#define _SP 0x80    /* hard space (0x20) */

struct printf_spec {
	u8  type;       /* format_type enum */
	u8  flags;      /* flags to number() */
	u8  base;       /* number base, 8, 10 or 16 only */
	u8  qualifier;  /* number qualifier, one of 'hHlLtzZ' */
	s16 field_width;    /* width of output field */
	s16 precision;  /* # of digits/chars */
};

enum format_type {
	FORMAT_TYPE_NONE, /* Just a string part */
	FORMAT_TYPE_WIDTH,
	FORMAT_TYPE_PRECISION,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_PERCENT_CHAR,
	FORMAT_TYPE_INVALID,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_ULONG,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_UBYTE,
	FORMAT_TYPE_BYTE,
	FORMAT_TYPE_USHORT,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_UINT,
	FORMAT_TYPE_INT,
	FORMAT_TYPE_NRCHARS,
	FORMAT_TYPE_SIZE_T,
	FORMAT_TYPE_PTRDIFF
};

static inline char _tolower(const char c)
{
    return c | 0x20;
}

const unsigned char _ctype[] = { 
	_C,_C,_C,_C,_C,_C,_C,_C,                /* 0-7 */
	_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,         /* 8-15 */
	_C,_C,_C,_C,_C,_C,_C,_C,                /* 16-23 */
	_C,_C,_C,_C,_C,_C,_C,_C,                /* 24-31 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,                /* 32-39 */
	_P,_P,_P,_P,_P,_P,_P,_P,                /* 40-47 */
	_D,_D,_D,_D,_D,_D,_D,_D,                /* 48-55 */
	_D,_D,_P,_P,_P,_P,_P,_P,                /* 56-63 */
	_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,      /* 64-71 */
	_U,_U,_U,_U,_U,_U,_U,_U,                /* 72-79 */
	_U,_U,_U,_U,_U,_U,_U,_U,                /* 80-87 */
	_U,_U,_U,_P,_P,_P,_P,_P,                /* 88-95 */
	_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,      /* 96-103 */
	_L,_L,_L,_L,_L,_L,_L,_L,                /* 104-111 */
	_L,_L,_L,_L,_L,_L,_L,_L,                /* 112-119 */
	_L,_L,_L,_P,_P,_P,_P,_C,                /* 120-127 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            /* 128-143 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,            /* 144-159 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,    /* 160-175 */
	_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,    /* 176-191 */
	_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,    /* 192-207 */
	_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,    /* 208-223 */
	_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,    /* 224-239 */
	_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};   /* 240-255 */

#define GIT_SPACE       0x01
#define GIT_DIGIT       0x02
#define GIT_ALPHA       0x04
#define GIT_GLOB_SPECIAL    0x08
#define GIT_REGEX_SPECIAL   0x10
#define GIT_PRINT_EXTRA     0x20
#define GIT_PRINT       0x3E

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)  ((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)  ((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)  ((__ismask(c)&(_C)) != 0)
#define isdigit(c)  ((__ismask(c)&(_D)) != 0)
#define isgraph(c)  ((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)  ((__ismask(c)&(_L)) != 0)
#define isprint(c)  ((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)  ((__ismask(c)&(_P)) != 0)
/* Note: isspace() must return FALSE for %NUL-terminator */
#define isspace(c)  ((__ismask(c)&(_S)) != 0)
#define isupper(c)  ((__ismask(c)&(_U)) != 0)
#define isxdigit(c) ((__ismask(c)&(_D|_X)) != 0)

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

#ifndef dereference_function_descriptor
#define dereference_function_descriptor(p) (p)
#endif

#define KSTRTOX_OVERFLOW	(1U << 31)

#define noinline            __attribute__((noinline))
#define noinline_for_stack  noinline

static noinline_for_stack int skip_atoi(const char **s)
{
	int i = 0;

	while (isdigit(**s))
		i = i*10 + *((*s)++) - '0';

	return i;
}

int strnlen(const char *s, int count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

const char *_parse_integer_fixup_radix(const char *s, unsigned int *base)
{
	if (*base == 0) {
		if (s[0] == '0') {
			if (_tolower(s[1]) == 'x' && isxdigit(s[2]))
				*base = 16; 
			else
				*base = 8;
		} else
			*base = 10; 
	}   
	if (*base == 16 && s[0] == '0' && _tolower(s[1]) == 'x')
		s += 2;
	return s;
}

unsigned int _parse_integer(const char *s, unsigned int base, unsigned long long *p) 
{
	unsigned long long res;
	unsigned int rv; 
	int overflow;

	res = 0;
	rv = 0;
	overflow = 0;
	while (*s) {
		unsigned int val;

		if ('0' <= *s && *s <= '9')
			val = *s - '0';
		else if ('a' <= _tolower(*s) && _tolower(*s) <= 'f')
			val = _tolower(*s) - 'a' + 10; 
		else
			break;

		if (val >= base)
			break;
		/*  
		 * Check for overflow only if we are within range of
		 * it in the max base we support (16)
		 */
		if (res & (~0ull << 60)) {
			if (res > div_u64(ULLONG_MAX - val, base))
				overflow = 1;
		}
		res = res * base + val;
		rv++;
		s++;
	}
	*p = res;
	if (overflow)
		rv |= KSTRTOX_OVERFLOW;
	return rv;
}

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
	unsigned long long result;
	unsigned int rv;

	cp = _parse_integer_fixup_radix(cp, &base);
	rv = _parse_integer(cp, base, &result);
	/* FIXME */
	cp += (rv & ~KSTRTOX_OVERFLOW);

	if (endp)
		*endp = (char *)cp;

	return result;
}

/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
	return simple_strtoull(cp, endp, base);
}

static noinline_for_stack char *put_dec_trunc(char *buf, unsigned q)
{
	unsigned d3, d2, d1, d0;
	d1 = (q>>4) & 0xf;
	d2 = (q>>8) & 0xf;
	d3 = (q>>12);

	d0 = 6*(d3 + d2 + d1) + (q & 0xf);
	q = (d0 * 0xcd) >> 11;
	d0 = d0 - 10*q;
	*buf++ = d0 + '0'; /* least significant digit */
	d1 = q + 9*d3 + 5*d2 + d1;
	if (d1 != 0) {
		q = (d1 * 0xcd) >> 11;
		d1 = d1 - 10*q;
		*buf++ = d1 + '0'; /* next digit */

		d2 = q + 2*d2;
		if ((d2 != 0) || (d3 != 0)) {
			q = (d2 * 0xd) >> 7;
			d2 = d2 - 10*q;
			*buf++ = d2 + '0'; /* next digit */

			d3 = q + 4*d3;
			if (d3 != 0) {
				q = (d3 * 0xcd) >> 11;
				d3 = d3 - 10*q;
				*buf++ = d3 + '0';  /* next digit */
				if (q != 0)
					*buf++ = q + '0'; /* most sign. digit */
			}
		}
	}

	return buf;
}

static noinline_for_stack char *put_dec_full(char *buf, unsigned q)
{
	/* BTW, if q is in [0,9999], 8-bit ints will be enough, */
	/* but anyway, gcc produces better code with full-sized ints */
	unsigned d3, d2, d1, d0;
	d1 = (q>>4) & 0xf;
	d2 = (q>>8) & 0xf;
	d3 = (q>>12);

	/*   
	 * Possible ways to approx. divide by 10
	 * gcc -O2 replaces multiply with shifts and adds
	 * (x * 0xcd) >> 11: 11001101 - shorter code than * 0x67 (on i386)
	 * (x * 0x67) >> 10:  1100111
	 * (x * 0x34) >> 9:    110100 - same
	 * (x * 0x1a) >> 8:     11010 - same
	 * (x * 0x0d) >> 7:      1101 - same, shortest code (on i386)
	 */
	d0 = 6*(d3 + d2 + d1) + (q & 0xf);
	q = (d0 * 0xcd) >> 11;
	d0 = d0 - 10*q;
	*buf++ = d0 + '0'; 
	d1 = q + 9*d3 + 5*d2 + d1;
	q = (d1 * 0xcd) >> 11;
	d1 = d1 - 10*q;
	*buf++ = d1 + '0'; 

	d2 = q + 2*d2;
	q = (d2 * 0xd) >> 7;
	d2 = d2 - 10*q;
	*buf++ = d2 + '0'; 

	d3 = q + 4*d3;
	q = (d3 * 0xcd) >> 11; /* - shorter code */
	/* q = (d3 * 0x67) >> 10; - would also work */
	d3 = d3 - 10*q;
	*buf++ = d3 + '0'; 
	*buf++ = q + '0'; 

	return buf; 
}

static noinline_for_stack char *put_dec(char *buf, unsigned long long num) 
{
	while (1) {
		unsigned rem; 
		if (num < 100000)
			return put_dec_trunc(buf, num);
		rem = do_div((num), 100000);
		buf = put_dec_full(buf, rem);
	}    
}

static noinline_for_stack char *number(char *buf, char *end, unsigned long long num,
		struct printf_spec spec)
{
	/* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
	static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

	char tmp[66];
	char sign;
	char locase;
	int need_pfx = ((spec.flags & SPECIAL) && spec.base != 10);
	int i;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (spec.flags & SMALL);
	if (spec.flags & LEFT)
		spec.flags &= ~ZEROPAD;
	sign = 0;
	if (spec.flags & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = -(signed long long)num;
			spec.field_width--;
		} else if (spec.flags & PLUS) {
			sign = '+';
			spec.field_width--;
		} else if (spec.flags & SPACE) {
			sign = ' ';
			spec.field_width--;
		}
	}
	if (need_pfx) {
		spec.field_width--;
		if (spec.base == 16)
			spec.field_width--;
	}

	/* generate full string in tmp[], in reverse order */
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	/* Generic code, for any base:
	   else do {
	   tmp[i++] = (digits[do_div(num,base)] | locase);
	   } while (num != 0);
	 */
	else if (spec.base != 10) { /* 8 or 16 */
		int mask = spec.base - 1;
		int shift = 3;

		if (spec.base == 16)
			shift = 4;
		do {
			tmp[i++] = (digits[((unsigned char)num) & mask] | locase);
			num >>= shift;
		} while (num);
	} else { /* base 10 */
		i = put_dec(tmp, num) - tmp;
	}

	/* printing 100 using %2d gives "100", not "00" */
	if (i > spec.precision)
		spec.precision = i;
	/* leading space padding */
	spec.field_width -= spec.precision;
	if (!(spec.flags & (ZEROPAD+LEFT))) {
		while (--spec.field_width >= 0) {
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	/* sign */
	if (sign) {
		if (buf < end)
			*buf = sign;
		++buf;
	}
	/* "0x" / "0" prefix */
	if (need_pfx) {
		if (buf < end)
			*buf = '0';
		++buf;
		if (spec.base == 16) {
			if (buf < end)
				*buf = ('X' | locase);
			++buf;
		}
	}
	/* zero or space padding */
	if (!(spec.flags & LEFT)) {
		char c = (spec.flags & ZEROPAD) ? '0' : ' ';
		while (--spec.field_width >= 0) {
			if (buf < end)
				*buf = c;
			++buf;
		}
	}
	/* hmm even more zero padding? */
	while (i <= --spec.precision) {
		if (buf < end)
			*buf = '0';
		++buf;
	}
	/* actual digits of result */
	while (--i >= 0) {
		if (buf < end)
			*buf = tmp[i];
		++buf;
	}
	/* trailing space padding */
	while (--spec.field_width >= 0) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}

	return buf;
}

static noinline_for_stack char *symbol_string(char *buf, char *end, void *ptr,
		            struct printf_spec spec, char ext) 
{
    UNUSED(ext);
	unsigned long value = (unsigned long) ptr; 
	spec.field_width = 2 * sizeof(void *);
	spec.flags |= SPECIAL | SMALL | ZEROPAD;
	spec.base = 16;

	return number(buf, end, value, spec);
}

static noinline_for_stack char *string(char *buf, char *end, const char *s, struct printf_spec spec)
{
	int len, i;

	if ((unsigned long)s < PAGE_SIZE)
		s = "(null)";

	len = strnlen(s, spec.precision);

	if (!(spec.flags & LEFT)) {
		while (len < spec.field_width--) {
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	for (i = 0; i < len; ++i) {
		if (buf < end)
			*buf = *s;
		++buf; ++s;
	}
	while (len < spec.field_width--) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}

	return buf;
}

static noinline_for_stack char *pointer(const char *fmt, char *buf, char *end, void *ptr,
		struct printf_spec spec)
{
	if (!ptr && *fmt != 'K') {
		/*   
		 * Print (null) with the same width as a pointer so it makes
		 * tabular output look nice.
		 */
		if (spec.field_width == -1)
			spec.field_width = 2 * sizeof(void *);
		return string(buf, end, "(null)", spec);
	}    

	switch (*fmt) {
		case 'F': 
		case 'f': 
			ptr = dereference_function_descriptor(ptr);
			/* Fallthrough */
		case 'S': 
		case 's': 
		case 'B': 
			return symbol_string(buf, end, ptr, spec, *fmt);
		//case 'R': 
		//case 'r': 
		//	return resource_string(buf, end, ptr, spec, fmt);
		//case 'M':           /* Colon separated: 00:01:02:03:04:05 */
		//case 'm':           /* Contiguous: 000102030405 */
			/* [mM]F (FDDI, bit reversed) */
			//return mac_address_string(buf, end, ptr, spec, fmt);
		//case 'I':          
							/* Formatted IP supported
							 * 4:   1.2.3.4
							 * 6:   0001:0203:...:0708
							 * 6c:  1::708 or 1::1.2.3.4
							 */
		//case 'i':           /* Contiguous:
		//					 * 4:   001.002.003.004
		//					 * 6:   000102...0f
		//					 */
		//	switch (fmt[1]) {
		//		case '6': 
		//			//return ip6_addr_string(buf, end, ptr, spec, fmt);
		//		case '4': 
		//			//return ip4_addr_string(buf, end, ptr, spec, fmt);
		//	}    
		//	break;
		//case 'U': 
			//return uuid_string(buf, end, ptr, spec, fmt);
		//case 'V': 
		//	{    
		//		va_list va;

		//		va_copy(va, *((struct va_format *)ptr)->va);
		//		buf += vsnprintf(buf, end > buf ? end - buf : 0, 
		//				((struct va_format *)ptr)->fmt, va);
		//		va_end(va);
		//		return buf;
		//	}
		//case 'K':
		//	/*
		//	 * %pK cannot be used in IRQ context because its test
		//	 * for CAP_SYSLOG would be meaningless.
		//	 */
		//	if (in_irq() || in_serving_softirq() || in_nmi()) {
		//		if (spec.field_width == -1)
		//			spec.field_width = 2 * sizeof(void *);
		//		return string(buf, end, "pK-error", spec);
		//	}
		//	if (!((kptr_restrict == 0) ||
		//				(kptr_restrict == 1 &&
		//				 has_capability_noaudit(current, CAP_SYSLOG))))
		//		ptr = NULL;
		//	break;
		//case 'N':
		//	switch (fmt[1]) {
		//		case 'F':
		//			return netdev_feature_string(buf, end, ptr, spec);
		//	}
		//	break;
	}
	spec.flags |= SMALL;
	if (spec.field_width == -1) {
		spec.field_width = 2 * sizeof(void *);
		spec.flags |= ZEROPAD;
	}
	spec.base = 16;

	return number(buf, end, (unsigned long) ptr, spec);
}

static noinline_for_stack int format_decode(const char *fmt, struct printf_spec *spec)
{
	const char *start = fmt; 

	/* we finished early by reading the field width */
	if (spec->type == FORMAT_TYPE_WIDTH) {
		if (spec->field_width < 0) { 
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}    
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}    

	/* we finished early by reading the precision */
	if (spec->type == FORMAT_TYPE_PRECISION) {
		if (spec->precision < 0) 
			spec->precision = 0; 

		spec->type = FORMAT_TYPE_NONE;
		goto qualifier;
	}    

	/* By default */
	spec->type = FORMAT_TYPE_NONE;

	for (; *fmt ; ++fmt) {
		if (*fmt == '%') 
			break;
	}    

	/* Return the current non-format string */
	if (fmt != start || !*fmt)
		return fmt - start;

	/* Process flags */
	spec->flags = 0; 

	while (1) { /* this also skips first '%' */
		boolean found = TRUE;

		++fmt;

		switch (*fmt) {
			case '-': spec->flags |= LEFT;    break;
			case '+': spec->flags |= PLUS;    break;
			case ' ': spec->flags |= SPACE;   break;
			case '#': spec->flags |= SPECIAL; break;
			case '0': spec->flags |= ZEROPAD; break;
			default:  found = FALSE;
		}

		if (!found)
			break;
	}

	/* get field width */
	spec->field_width = -1;

	if (isdigit(*fmt))
		spec->field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
		/* it's the next argument */
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

precision:
	/* get the precision */
	spec->precision = -1;
	if (*fmt == '.') {
		++fmt;
		if (isdigit(*fmt)) {
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
				spec->precision = 0;
		} else if (*fmt == '*') {
			/* it's the next argument */
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

qualifier:
	/* get the conversion qualifier */
	spec->qualifier = -1;
	if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
			_tolower(*fmt) == 'z' || *fmt == 't') {
		spec->qualifier = *fmt++;
		if (spec->qualifier == *fmt)
		{
			if (spec->qualifier == 'l') 
			{
				spec->qualifier = 'L';
				++fmt;
			} 
			else if (spec->qualifier == 'h') 
			{
				spec->qualifier = 'H';
				++fmt;
			}
		}
	}

	/* default base */
	spec->base = 10;
	switch (*fmt) {
		case 'c':
			spec->type = FORMAT_TYPE_CHAR;
			return ++fmt - start;

		case 's':
			spec->type = FORMAT_TYPE_STR;
			return ++fmt - start;

		case 'p':
			spec->type = FORMAT_TYPE_PTR;
			return fmt - start;
			/* skip alnum */

		case 'n':
			spec->type = FORMAT_TYPE_NRCHARS;
			return ++fmt - start;

		case '%':
			spec->type = FORMAT_TYPE_PERCENT_CHAR;
			return ++fmt - start;

			/* integer number formats - set up the flags and "break" */
		case 'o':
			spec->base = 8;
			break;

		case 'x':
			spec->flags |= SMALL;

		case 'X':
			spec->base = 16;
			break;

		case 'd':
		case 'i':
			spec->flags |= SIGN;
		case 'u':
			break;

		default:
			spec->type = FORMAT_TYPE_INVALID;
			return fmt - start;
	}

	if (spec->qualifier == 'L')
	{
		spec->type = FORMAT_TYPE_LONG_LONG;
	}
	else if (spec->qualifier == 'l') 
	{
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_LONG;
		else
			spec->type = FORMAT_TYPE_ULONG;
	}
   	else if (_tolower(spec->qualifier) == 'z') 
	{
		spec->type = FORMAT_TYPE_SIZE_T;
	} 
	else if (spec->qualifier == 't') 
	{
		spec->type = FORMAT_TYPE_PTRDIFF;
	} 
	else if (spec->qualifier == 'H') 
	{
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_BYTE;
		else
			spec->type = FORMAT_TYPE_UBYTE;
	} 
	else if (spec->qualifier == 'h') 
	{
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_SHORT;
		else
			spec->type = FORMAT_TYPE_USHORT;
	} 
	else 
	{
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_INT;
		else
			spec->type = FORMAT_TYPE_UINT;
	}

	return ++fmt - start;
}


int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	unsigned long long num; 
	char *str, *end;
	struct printf_spec spec; 
    //start 指向printk传入参数的第一个地址
    u32 start = (u32)args - 4; 
	/* Reject out-of-range values early.  Large positive sizes are
	   used for unknown buffer sizes. */
	if ( (int)size < 0)
		return 0;

    memset(&spec, 0, sizeof( struct printf_spec));

	str = buf; 
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = ((void *)-1);
		size = end - buf; 
	}    

	while (*fmt) {
		const char *old_fmt = fmt; 
		int read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) {
			case FORMAT_TYPE_NONE: 
				{
					int copy = read;
					if (str < end) {
						if (copy > end - str) 
							copy = end - str; 
						memcpy(str, old_fmt, copy);
					}    
					str += read;
					break;
				}    

			case FORMAT_TYPE_WIDTH:   
				spec.field_width = va_arg(args, int);
				break;

			case FORMAT_TYPE_PRECISION:
				spec.precision = va_arg(args, int);
				break;

			case FORMAT_TYPE_CHAR: 
				{
					char c;

					if (!(spec.flags & LEFT)) {
						while (--spec.field_width > 0) { 
							if (str < end) 
								*str = ' '; 
							++str;

						}    
					}    
					c = (unsigned char) va_arg(args, int);
					if (str < end) 
						*str = c; 
					++str;
					while (--spec.field_width > 0) { 
						if (str < end) 
							*str = ' '; 
						++str;
					}
					break;
				}

			case FORMAT_TYPE_STR:
				str = string(str, end, va_arg(args, char *), spec);
				break;

			case FORMAT_TYPE_PTR:
				str = pointer(fmt+1, str, end, va_arg(args, void *), spec);
				while (isalnum(*fmt))
					fmt++;
				break;

			case FORMAT_TYPE_PERCENT_CHAR:
				if (str < end)
					*str = '%';
				++str;
				break;

			case FORMAT_TYPE_INVALID:
				if (str < end)
					*str = '%';
				++str;
				break;

			case FORMAT_TYPE_NRCHARS: 
				{
					u8 qualifier = spec.qualifier;

					if (qualifier == 'l') {
						long *ip = va_arg(args, long *);
						*ip = (str - buf);
					} else if (_tolower(qualifier) == 'z') {
						size_t *ip = va_arg(args, size_t *);
						*ip = (str - buf);
					} else {
						int *ip = va_arg(args, int *);
						*ip = (str - buf);
					}
					break;
				}
			default:

				switch (spec.type) {
					case FORMAT_TYPE_LONG_LONG:
						num = va_arg(args, unsigned long long);
                        u32 new_loca = (u32)args - 8;
                        if((new_loca - start)%8)
                        {
                            args -= 4;
                            num = va_arg(args, unsigned long long);
                        }
						break;
					case FORMAT_TYPE_ULONG:
						num = va_arg(args, unsigned long);
						break;
					case FORMAT_TYPE_LONG:
						num = va_arg(args, long);
						break;
					case FORMAT_TYPE_SIZE_T:
						num = va_arg(args, size_t);
						break;
					case FORMAT_TYPE_PTRDIFF:
						num = va_arg(args, ptrdiff_t);
						break;
					case FORMAT_TYPE_UBYTE:
						num = (unsigned char) va_arg(args, int);
						break;
					case FORMAT_TYPE_BYTE:
						num = (signed char) va_arg(args, int);
						break;
					case FORMAT_TYPE_USHORT:
						num = (unsigned short) va_arg(args, int);
						break;
					case FORMAT_TYPE_SHORT:
						num = (short) va_arg(args, int);
						break;
					case FORMAT_TYPE_INT:
						num = (int) va_arg(args, int);
						break;
					default:
						num = va_arg(args, unsigned int);
				}

				str = number(str, end, num, spec);
		}
	}

	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}

	/* the trailing null byte doesn't count towards the total */
	return str-buf;
}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	size_t i;

	i = (size_t)vsnprintf(buf, size, fmt, args);

	if (i < size)
		return i;
	if (size != 0)
		return size - 1; 
	return 0;
}

