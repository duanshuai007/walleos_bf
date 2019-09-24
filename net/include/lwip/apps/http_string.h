#ifndef _HTTP_STRING_H_
#define _HTTP_STRING_H_

//#include "types.h"

int str_contains(char *haystack, const char *needle);
char *str_ndup (const char *str, size_t max);
char *str_replace(char *search , char *replace , char *subject);
char* get_until(char *haystack, char *until);
char* base64_encode(char *clrstr); 

#endif
