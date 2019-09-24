#ifndef _DHCPCLIENT_H_
#define _DHCPCLIENT_H_

#include "lwip/netif.h"
#include "lwip/ip_addr.h"

typedef void dhcpclient_polling(void);



void dhcpclient_reg_poll_callback(dhcpclient_polling *callback);


void dhcpclient_start(struct netif *curnetif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);

#endif