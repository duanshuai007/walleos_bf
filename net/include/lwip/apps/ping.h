#ifndef __PING_H__
#define __PING_H__

/**
 * PING_USE_SOCKETS: Set to 1 to use sockets, otherwise the raw api is used
 */
#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET
#endif

#include "lwip/ip_addr.h"

typedef void PING_RESULT_FUNC(char *respone);

void ping_init(PING_RESULT_FUNC *pCallback);
void ping_remove(void);

#if !PING_USE_SOCKETS
void ping_send_now(ip_addr_t *ipaddr);
#endif /* !PING_USE_SOCKETS */

#endif /* __PING_H__ */
