#include "lwip/apps/net_main.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/ip4_frag.h"
#include "lwip/ip4.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "netif/ethernetif.h"
#include "arch/sys_arch.h" 
#include <s5p4418_tick_delay.h>
#include "lwip/apps/dhcpclient.h"
#include <scheduler.h>
#include "lwip/apps/tcp_client.h"
#include "lwip/apps/http_client.h"
#include "lwip/dns.h"

#include <s5p4418_tick_delay.h>
#include <synchronize.h>

extern boolean NX_DWC_GMAC_DRV_RX(u32 ModuleIndex);

struct netif walle_default_netif;  			//定义网络接口
u32_t input_time = 0;
u32_t last_arp_time = 0;			
u32_t last_tcp_time = 0;	
u32_t last_ipreass_time = 0;

u32_t last_dhcp_fine_time = 0;			
u32_t last_dhcp_coarse_time = 0; 

u32_t last_dns_coarse_time = 0; 

//LWIP查询
void LWIP_Polling(void)
{
    //间隔第二个参数制定的时间轮训读取网卡
	if(timer_expired(&input_time,1)) //接收包，周期处理函数
	{
		NX_DWC_GMAC_DRV_RX(0);
	}

	if(timer_expired(&last_tcp_time, TCP_TMR_INTERVAL))//TCP处理定时器处理函数
	{
		tcp_tmr();
	}

	if(timer_expired(&last_arp_time, ARP_TMR_INTERVAL))//ARP处理定时器
	{
		etharp_tmr();
	}
	if(timer_expired(&last_ipreass_time, IP_TMR_INTERVAL))//IP重新组装定时器
	{ 
		ip_reass_tmr();
	}			   					
	if(timer_expired(&last_dhcp_fine_time, DHCP_FINE_TIMER_MSECS))
	{
		dhcp_fine_tmr();
	}
	if(timer_expired(&last_dhcp_coarse_time, DHCP_COARSE_TIMER_MSECS))
	{
		dhcp_coarse_tmr();
	}

	if(timer_expired(&last_dns_coarse_time, DNS_TMR_INTERVAL))
	{
		dhcp_coarse_tmr();
	}  
}

void net_input(void *data, u32 len)
{
	if (len > 0)
	{
		ethernetif_input(&walle_default_netif, data, len);
	}	
}

void dhcp_auto_config_ip(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
	dhcpclient_reg_poll_callback(LWIP_Polling);

	dhcpclient_start(&walle_default_netif, ipaddr, netmask, gw);
}

void pollingData(void)
{
	LWIP_Polling();
}

int start_ping(char *ip, int count, PING_RESULT_FUNC *pCallback)
{
	ping_init(pCallback);

	ip_addr_t ipdddr;

	if(!ipaddr_aton(ip, &ipdddr))
	{
		return 0;
	}

	for (int i = 0; i < count; ++i)
	{
		ping_send_now(&ipdddr);

		mdelay(1000);
	}

	ping_remove();
	return 1;
}

void SendHttpGetReq(char *url, HTTP_RECEIVE_DATA_FUNC *callback)
{
	http_set_data_rcv_callback(callback);

	if (url != NULL)
	{		
		http_get(url, 0);
	}
}

void initNetModule(void)
{
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw; 						//定义IP地址

	IP4_ADDR(&ipaddr,	0, 0, 0, 0);  		//设置本地ip地址
	IP4_ADDR(&gw,		0, 0, 0, 0);			//网关
	IP4_ADDR(&netmask,	0, 0, 0, 0);		//子网掩码	

	//初始化LWIP协议栈,执行检查用户所有可配置的值，初始化所有的模块
	lwip_init();

	//添加网络接口
	while((netif_add(&walle_default_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input)==NULL))
	{
		mdelay(200);
	}

	//注册默认的网络接口
	netif_set_default(&walle_default_netif);
	//建立网络接口用于处理通信
	netif_set_up(&walle_default_netif); 		
}
