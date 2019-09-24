#include "basicutil.h"

char* _itoa(int value, char* string)
{
	char tmp[33];
	char* tp = tmp;
	int i;
	unsigned v;
	char* sp;
	v = (unsigned)value;
	while (v || tp == tmp)
	{
		i = v % 10;
		v = v / 10;
		*tp++ = i+'0';
	}
	sp = string;
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	return string;
}
