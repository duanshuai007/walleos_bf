#include "lwip/apps/dhcpclient.h"
#include "lwip/dhcp.h"
#include "lwip/ip4.h"
#include "string.h"

dhcpclient_polling *polling_callback;

void dhcpclient_reg_poll_callback(dhcpclient_polling *callback)
{
	polling_callback = callback;
}

void dhcpclient_start(struct netif *curnetif, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
	dhcp_start(curnetif);             //为网卡创建一个新的DHCP客户端	

	while(1)
	{
		(*polling_callback)();

		struct dhcp* dhcp = netif_dhcp_data(curnetif);
		if (dhcp == 0)
		{
			printf("dhcp: dhcp = 0\r\n");
		}

		if(	dhcp != NULL &&
			dhcp->offered_ip_addr.addr != 0 &&
			dhcp->offered_sn_mask.addr != 0 &&
			dhcp->offered_gw_addr.addr != 0 )
		{

			memcpy(ipaddr, &(dhcp->offered_ip_addr), sizeof(ip_addr_t));
			memcpy(netmask, &(dhcp->offered_sn_mask), sizeof(ip_addr_t));
			memcpy(gw, &(dhcp->offered_gw_addr), sizeof(ip_addr_t));

			netif_set_addr(curnetif, &(dhcp->offered_ip_addr)
							, &(dhcp->offered_sn_mask)
							, &(dhcp->offered_gw_addr));

			netif_set_default(curnetif);    
			netif_set_up(curnetif);     

			ip_init();             

			dhcp_release(curnetif);
			dhcp_stop(curnetif);

			return;
		}
	}

	return;	
}
