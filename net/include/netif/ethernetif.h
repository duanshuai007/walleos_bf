#ifndef __ETHERNETIF_H
#define __ETHERNETIF_H	 

#include "lwip/opt.h"
#include <lwip/stats.h>


void ethernetif_input(struct netif *netif, unsigned char *buf, unsigned int len);

err_t ethernetif_init(struct netif *netif);

#endif

