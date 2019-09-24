#ifndef _NET_MAIN_H_
#define _NET_MAIN_H_


#include "lwip/ip_addr.h"

#include "lwip/apps/ping.h"

#include "lwip/apps/http_client.h"

void dhcp_auto_config_ip(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
void initNetModule(void);

void pollingData(void);

int start_ping(char *ip, int count, PING_RESULT_FUNC *pCallback);

void sendTcpData(char *buf);

void SendHttpGetReq(char *url, HTTP_RECEIVE_DATA_FUNC *callback);
#endif