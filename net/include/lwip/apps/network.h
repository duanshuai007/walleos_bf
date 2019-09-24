#ifndef __NETWORK_H_
#define __NETWORK_H_

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "string.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/ip4_frag.h"
#include "lwip/prot/dns.h"
#include "arch/sys_arch.h"


#define LWIP_CONNECTED  0X80 //连接成功
#define LWIP_NEW_DATA   0x40 //有新的数据
#define LWIP_SEND_DATA  0x20 //有数据需要发送


#endif
















