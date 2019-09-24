#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "lwip/apps/http_urlparser.h"

/*
	Represents an HTTP html response
*/
struct http_response
{
	char *body;
	int status_code_int;
	char *response_headers;
	int bodyLen;
	char content_type[12];
};

typedef void HTTP_RECEIVE_DATA_FUNC(char *buffer, int buflen, char *fileType);

void http_set_data_rcv_callback(HTTP_RECEIVE_DATA_FUNC *callback);
/*
	Prototype functions
*/
void http_get(char *url, char *custom_headers);
void http_head(char *url, char *custom_headers);
void http_post(char *url, char *custom_headers, char *post_data);


#endif