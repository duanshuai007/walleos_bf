#ifndef _HTTP_URLPARSER_H_
#define _HTTP_URLPARSER_H_

#include "types.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"

struct parsed_url 
{
	char *uri;					/* mandatory */
    char *scheme;               /* mandatory */
    char *host;                 /* mandatory */
	ip_addr_t *ip; 					/* mandatory */
    char *port;                 /* optional */
    char *path;                 /* optional */
    char *query;                /* optional */
    char *fragment;             /* optional */
    char *username;             /* optional */
    char *password;             /* optional */
};

/*
	Free memory of parsed url
*/
void parsed_url_free(struct parsed_url *purl);

struct parsed_url *parse_url(char *url);

#endif