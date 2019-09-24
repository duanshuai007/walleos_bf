/*
	http-client-c
	Copyright (C) 2012-2013  Swen Kooij

	This file is part of http-client-c.

    http-client-c is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    http-client-c is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with http-client-c. If not, see <http://www.gnu.org/licenses/>.

	Warning:
	This library does not tend to work that stable nor does it fully implent the
	standards described by IETF. For more information on the precise implentation of the
	Hyper Text Transfer Protocol:

	http://www.ietf.org/rfc/rfc2616.txt
*/

#include "lwip/apps/http_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwip/apps/http_string.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/tcp_client.h"
#include "lwip/def.h"
#include <alloc.h>
#include <errno.h>

#define BUFSIZ 2*1024*1024

#define HTTP_CMD_TYPE_GET  0
#define HTTP_CMD_TYPE_POST 1
#define HTTP_CMD_TYPE_HEAD 2
#define HTTP_CMD_TYPE_NOT_SUPPORT_REDIRECT  100

int g_http_type = HTTP_CMD_TYPE_NOT_SUPPORT_REDIRECT;

char *ReceiveDataBuf = 0;
int DataMallocedMem = 0;
int DataLen = 0;

HTTP_RECEIVE_DATA_FUNC *g_http_data_rcv_callback;

static void http_response_free(struct http_response *hresp);
/*
	Handles redirect if needed for get requests
*/
static void handle_redirect_get(struct http_response* hresp, char* custom_headers)
{
	if(hresp->status_code_int > 300 && hresp->status_code_int < 399)
	{
		char *token = strtok_r(hresp->response_headers, "\r\n");
		while(token != NULL)
		{
			if(str_contains(token, "Location:"))
			{
				/* Extract url */
				char *location = str_replace("Location: ", "", token);
				http_get(location, custom_headers);
			}
			token = strtok_r(NULL, "\r\n");
		}
	}
	else
	{
		// int i = 0;
		// for(; i<hresp->bodyLen; i++)
		// {
		// 	printf("%02x ", (unsigned char)hresp->body[i]);

		// 	if ((i+1)%32 == 0)
		// 	{				
		// 		printf("\r\n");
		// 	}
		// }

		// printf("\r\n");
		//LoggerWrite(LoggerGet (), "http", LogDebug, "hresp->bodyLen = %d\r\n", hresp->bodyLen);
		printf("http: hresp->bodyLen = %d\r\n", hresp->bodyLen);

		(*g_http_data_rcv_callback)(hresp->body, hresp->bodyLen, hresp->content_type);


		g_http_type = HTTP_CMD_TYPE_NOT_SUPPORT_REDIRECT;
	}

	http_response_free(hresp);

	if (ReceiveDataBuf != NULL)
	{
		free(ReceiveDataBuf);
		ReceiveDataBuf = NULL;
	}

	DataMallocedMem = 0;
	DataLen = 0;
}

/*
	Handles redirect if needed for head requests
*/
static void handle_redirect_head(struct http_response* hresp, char* custom_headers)
{
	if(hresp->status_code_int > 300 && hresp->status_code_int < 399)
	{
		char *token = strtok_r(hresp->response_headers, "\r\n");
		while(token != NULL)
		{
			if(str_contains(token, "Location:"))
			{
				/* Extract url */
				char *location = str_replace("Location: ", "", token);
				http_head(location, custom_headers);
			}
			token = strtok_r(NULL, "\r\n");
		}
	}
	else
	{

	}
}

/*
	Handles redirect if needed for post requests
*/
static void handle_redirect_post(struct http_response* hresp, char* custom_headers, char *post_data)
{
	if(hresp->status_code_int > 300 && hresp->status_code_int < 399)
	{
		char *token = strtok_r(hresp->response_headers, "\r\n");
		while(token != NULL)
		{
			if(str_contains(token, "Location:"))
			{
				/* Extract url */
				char *location = str_replace("Location: ", "", token);
				return http_post(location, custom_headers, post_data);
			}
			token = strtok_r(NULL, "\r\n");
		}
	}
	else
	{

	}
}
void http_receive_packet(char *buffer, int buflen, int msg_type)
{

	if (msg_type == ES_RECEIVED)
	{
		if (NULL == ReceiveDataBuf)
		{
			ReceiveDataBuf = (char *) malloc(BUFSIZ);
            if(ReceiveDataBuf == NULL)
                return;
			memset(ReceiveDataBuf, 0, BUFSIZ);

			DataMallocedMem = BUFSIZ;
		}

		if (DataLen + buflen > DataMallocedMem)
		{
			DataMallocedMem += BUFSIZ;

			char *tempBuf = (char *) malloc(DataMallocedMem);
            if(tempBuf == NULL)
                return;
			memset(tempBuf, 0, DataMallocedMem);

			memcpy(tempBuf, ReceiveDataBuf, DataLen);

			free(ReceiveDataBuf);
			ReceiveDataBuf = tempBuf;
		}

		memcpy(ReceiveDataBuf + DataLen, buffer, buflen);
		DataLen += buflen;
	}

	else if(msg_type == ES_CLOSING)
	{

		/* Allocate memeory for htmlcontent */
		struct http_response *hresp = (struct http_response*)malloc(sizeof(struct http_response));
	 	if(hresp == NULL)
	 	{
	 		return;
	 	}
		hresp->body = NULL;
		hresp->response_headers = NULL;

		hresp->bodyLen = (DataLen - (strstr(ReceiveDataBuf, "\r\n\r\n") - ReceiveDataBuf) - 4);

		/* Parse status code and text */
		char *status_line = get_until(ReceiveDataBuf, "\r\n");
		status_line = str_replace("HTTP/1.1 ", "", status_line);
		char *status_code = str_ndup(status_line, 4);
		status_code = str_replace(" ", "", status_code);
		hresp->status_code_int = atoi(status_code);

		char *content_type = strstr(ReceiveDataBuf, "Content-Type");
		if (content_type == NULL)
		{
			sprintf(hresp->content_type, "non");
		}
		else
		{
			content_type = get_until(content_type, "\r\n");
			//LoggerWrite(LoggerGet (), "http", LogDebug, "content_type = %s\n", content_type);
			printf("http: content_type = %s\r\n", content_type);

			if (strstr(content_type, "bmp") || strstr(content_type, "BMP"))
			{
				sprintf(hresp->content_type, "bmp");
			}
			else if (strstr(content_type, "jpeg") || strstr(content_type, "JPEG"))
			{
				sprintf(hresp->content_type, "jpeg");
			}
			else if (strstr(content_type, "jpg") || strstr(content_type, "JPG"))
			{
				sprintf(hresp->content_type, "jpg");
			}
			else
			{
				sprintf(hresp->content_type, "non");
			}

			if (content_type != NULL)
			{
				free(content_type);
				content_type = NULL;
			}			
		}


		/* Parse response headers */
		char *headers = get_until(ReceiveDataBuf, "\r\n\r\n");
		hresp->response_headers = headers;

		/* Parse body */
		char *body = strstr(ReceiveDataBuf, "\r\n\r\n");
		hresp->body = body + strlen("\r\n\r\n");

		if(HTTP_CMD_TYPE_GET == g_http_type)
		{
			handle_redirect_get(hresp, NULL);
		}
		else if (HTTP_CMD_TYPE_POST == g_http_type)
		{	
			handle_redirect_post(hresp, NULL, NULL);
		}
		else if(HTTP_CMD_TYPE_HEAD == g_http_type)
		{
			handle_redirect_head(hresp, NULL);
		}
	}
}


/*
	Makes a HTTP request and returns the response
*/
static void http_req(char *http_headers, struct parsed_url *purl)
{
	/* Parse url */
	if(purl == NULL)
	{
		return;
	}

	//ip_addr_t ipaddr;
	//IP4_ADDR(&ipaddr, 192, 168, 150, 203);
	//ip4addr_aton(purl->host, &ipaddr);
	unsigned short port = atoi(purl->port);

	Tcp_Client_Init(purl->ip, port, http_receive_packet);

	Tcp_Client_Send(http_headers, strlen(http_headers));

	if (http_headers != NULL)
	{
		free(http_headers);
		http_headers = NULL;
	}
}


void http_set_data_rcv_callback(HTTP_RECEIVE_DATA_FUNC *callback)
{
	g_http_data_rcv_callback = callback;
}
/*
	Makes a HTTP GET request to the given url
*/
void http_get(char *url, char *custom_headers)
{
	/* Parse url */
	struct parsed_url *purl = parse_url(url);
	if(purl == NULL)
	{
		return;
	}

    if (purl->uri != 0)
    {
    	//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->uri = %s\n", purl->uri);
		printf("http: purl->uri = %s\r\n", purl->uri);
    }

    if (purl->scheme != 0)
    {
    	//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->scheme = %s\n", purl->scheme);
		printf("http: purl->scheme = %s\r\n", purl->scheme);
    }

	if (purl->host != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->host = %s\n", purl->host);
		printf("http: purl->host = %s\r\n", purl->host);
	}

	if (purl->ip != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->ip = %s\n", purl->ip);
		printf("http: purl->ip = %s\r\n", purl->ip);
	}

	if (purl->port != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->port = %s\n", purl->port);
		printf("http: purl->port = %s\r\n", purl->port);
	}

	if (purl->path != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->path = %s\n", purl->path);
		printf("http: purl->path = %s\r\n", purl->path);
	}

	if (purl->query != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->query = %s\n", purl->query);
		printf("http: purl->query = %s\r\n", purl->query);
	}

	if (purl->fragment != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->fragment = %s\n", purl->fragment);
		printf("http: purl->fragment = %s\r\n", purl->fragment);
	}

	if (purl->username != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->username = %s\n", purl->username);
		printf("http: purl->username = %s\r\n", purl->username);
	}

	if (purl->password != 0)
	{
		//LoggerWrite(LoggerGet (), "http", LogDebug, "purl->password = %s\n", purl->password);
		printf("http: purl->password = %s\r\n", purl->password);
	}

	//1.path
	char path[1024] = {0};
	/* Build query/headers */
	if(purl->path != NULL)
	{
		if(purl->query != NULL)
		{
			sprintf(path, "GET /%s?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->query, purl->host);
		}
		else
		{
			sprintf(path, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->host);
		}
	}
	else
	{
		if(purl->query != NULL)
		{
			sprintf(path, "GET /?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->query, purl->host);
		}
		else
		{
			sprintf(path, "GET / HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->host);
		}
	}	


	//2.username
	char user_name[1024] = {0};

	/* Handle authorisation if needed */
	if(purl->username != NULL)
	{
		/* Format username:password pair */
		char upwd[1024] = {0};
		sprintf(upwd, "%s:%s", purl->username, purl->password);

		/* Base64 encode */
		char *base64 = base64_encode(upwd);

		sprintf(user_name, "Authorization: Basic %s\r\n", base64);

		if (base64 != 0)
		{
			free(base64);
			base64 = 0;
		}
	}


	/* Declare variable */
	char *http_headers = 0;

	/* Add custom headers, and close */
	if(custom_headers != NULL)
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen(custom_headers) + strlen("\r\n") + 1);
        if(http_headers == NULL)
            return;
		sprintf(http_headers, "%s%s%s\r\n", path, user_name, custom_headers);
	}
	else
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen("\r\n") + 1);		
        if(http_headers == NULL)
            return;
		sprintf(http_headers, "%s%s\r\n", path, user_name);
	}


	http_req(http_headers, purl);

	g_http_type = HTTP_CMD_TYPE_GET;

}

/*
	Makes a HTTP POST request to the given url
*/
void http_post(char *url, char *custom_headers, char *post_data)
{
	/* Parse url */
	struct parsed_url *purl = parse_url(url);
	if(purl == NULL)
	{
		return;
	}

	//1.path
	char path[1024] = {0};
	/* Build query/headers */
	if(purl->path != NULL)
	{
		if(purl->query != NULL)
		{
			sprintf(path, "POST /%s?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\nContent-Length:%zu\r\nContent-Type:application/x-www-form-urlencoded\r\n", purl->path, purl->query, purl->host, strlen(post_data));
		}
		else
		{
			sprintf(path, "POST /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\nContent-Length:%zu\r\nContent-Type:application/x-www-form-urlencoded\r\n", purl->path, purl->host, strlen(post_data));
		}
	}
	else
	{
		if(purl->query != NULL)
		{
			sprintf(path, "POST /?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\nContent-Length:%zu\r\nContent-Type:application/x-www-form-urlencoded\r\n", purl->query, purl->host, strlen(post_data));
		}
		else
		{
			sprintf(path, "POST / HTTP/1.1\r\nHost:%s\r\nConnection:close\r\nContent-Length:%zu\r\nContent-Type:application/x-www-form-urlencoded\r\n", purl->host, strlen(post_data));
		}
	}

	//2.username
	char user_name[1024] = {0};

	/* Handle authorisation if needed */
	if(purl->username != NULL)
	{
		/* Format username:password pair */
		char upwd[1024] = {0};
		sprintf(upwd, "%s:%s", purl->username, purl->password);

		/* Base64 encode */
		char *base64 = base64_encode(upwd);

		sprintf(user_name, "Authorization: Basic %s\r\n", base64);

		if (base64 != 0)
		{
			free(base64);
			base64 = 0;
		}
	}

	/* Declare variable */
	char *http_headers = 0;

	/* Add custom headers, and close */
	if(custom_headers != NULL)
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen(custom_headers) + strlen("\r\n") + strlen(post_data) + 1);
        if(http_headers == NULL)
            return;
		sprintf(http_headers, "%s%s%s\r\n%s", path, user_name, custom_headers, post_data);
	}
	else
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen("\r\n") + strlen(post_data) + 1);		
        if(http_headers == NULL)
            return;
		sprintf(http_headers, "%s%s\r\n%s", path, user_name, post_data);
	}


	/* Make request and return response */
	http_req(http_headers, purl);

	g_http_type = HTTP_CMD_TYPE_POST;

}

/*
	Makes a HTTP HEAD request to the given url
*/
void http_head(char *url, char *custom_headers)
{
	/* Parse url */
	struct parsed_url *purl = parse_url(url);
	if(purl == NULL)
	{
		return;
	}

	//1.path
	char path[1024] = {0};
	/* Build query/headers */
	if(purl->path != NULL)
	{
		if(purl->query != NULL)
		{
			sprintf(path, "HEAD /%s?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->query, purl->host);
		}
		else
		{
			sprintf(path, "HEAD /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->host);
		}
	}
	else
	{
		if(purl->query != NULL)
		{
			sprintf(path, "HEAD/?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->query, purl->host);
		}
		else
		{
			sprintf(path, "HEAD / HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->host);
		}
	}

	//2.username
	char user_name[1024] = {0};

	/* Handle authorisation if needed */
	if(purl->username != NULL)
	{
		/* Format username:password pair */
		char upwd[1024] = {0};
		sprintf(upwd, "%s:%s", purl->username, purl->password);

		/* Base64 encode */
		char *base64 = base64_encode(upwd);

		sprintf(user_name, "Authorization: Basic %s\r\n", base64);

		if (base64 != 0)
		{
			free(base64);
			base64 = 0;
		}
	}

	/* Declare variable */
	char *http_headers = 0;

	/* Add custom headers, and close */
	if(custom_headers != NULL)
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen(custom_headers) + strlen("\r\n") + 1);
        if(http_headers == NULL) 
            return;
		sprintf(http_headers, "%s%s%s\r\n", path, user_name, custom_headers);
	}
	else
	{
		http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen("\r\n") + 1);		
        if(http_headers == NULL) 
            return;
		sprintf(http_headers, "%s%s\r\n", path, user_name);
	}


	/* Make request and return response */
	http_req(http_headers, purl);

	g_http_type = HTTP_CMD_TYPE_HEAD;
}

/*
	Do HTTP OPTIONs requests
*/
void http_options(char *url)
{
	/* Parse url */
	struct parsed_url *purl = parse_url(url);
	if(purl == NULL)
	{
		return ;
	}

	//1.path
	char path[1024] = {0};
	/* Build query/headers */
	if(purl->path != NULL)
	{
		if(purl->query != NULL)
		{
			sprintf(path, "OPTIONS /%s?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->query, purl->host);
		}
		else
		{
			sprintf(path, "OPTIONS /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->path, purl->host);
		}
	}
	else
	{
		if(purl->query != NULL)
		{
			sprintf(path, "OPTIONS/?%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->query, purl->host);
		}
		else
		{
			sprintf(path, "OPTIONS / HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n", purl->host);
		}
	}

	//2.username
	char user_name[1024] = {0};

	/* Handle authorisation if needed */
	if(purl->username != NULL)
	{
		/* Format username:password pair */
		char upwd[1024] = {0};
		sprintf(upwd, "%s:%s", purl->username, purl->password);

		/* Base64 encode */
		char *base64 = base64_encode(upwd);

		sprintf(user_name, "Authorization: Basic %s\r\n", base64);

		if (base64 != 0)
		{
			free(base64);
			base64 = 0;
		}
	}

	char *http_headers = (char*)malloc(strlen(path) + strlen(user_name) + strlen("\r\n") + 1);	
    if(http_headers == NULL) 
        return;
	sprintf(http_headers, "%s%s\r\n", path, user_name);	

	/* Make request and return response */
	http_req(http_headers, purl);

}

/*
	Free memory of http_response
*/
void http_response_free(struct http_response *hresp)
{
	if(hresp != NULL)
	{

		if(hresp->response_headers != NULL) 
		{
			free(hresp->response_headers);
		}

		free(hresp);
	}
}
