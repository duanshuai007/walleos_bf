CPATH		:= $(CPATH)/net
X_INCDIR	+= -I ./include

#api
OBJS	= 	api/api_lib.o     	\
		api/err.o	    	\
		api/netdb.o	    	\
		api/sockets.o    	\
		api/api_msg.o     	\
		api/netbuf.o     	\
		api/netifapi.o     	\
		api/tcpip.o

#core ipv4
OBJS 	+= 	core/ipv4/autoip.o     	\
		core/ipv4/etharp.o     	\
		core/ipv4/igmp.o     	\
		core/ipv4/ip4_addr.o 	\
		core/ipv4/dhcp.o     	\
		core/ipv4/icmp.o     	\
		core/ipv4/ip4.o     	\
		core/ipv4/ip4_frag.o

#core ipv6
OBJS 	+= 	core/ipv6/dhcp6.o    	\
		core/ipv6/icmp6.o    	\
		core/ipv6/ip6.o    		\
		core/ipv6/ip6_frag.o    \
		core/ipv6/nd6.o 		\
		core/ipv6/ethip6.o		\
		core/ipv6/inet6.o		\
		core/ipv6/ip6_addr.o	\
		core/ipv6/mld6.o

OBJS	+=	include/arch/sys_arch.o

#core
OBJS	+= 	core/def.o				\
		core/ip.o				\
		core/memp.o				\
		core/stats.o			\
		core/tcp_out.o  		\
		core/dns.o				\
		core/tcp_in.o			\
		core/netif.o			\
		core/sys.o				\
		core/timeouts.o 		\
		core/inet_chksum.o		\
		core/raw.o				\
		core/pbuf.o				\
		core/tcp.o				\
		core/udp.o 				\
		core/init.o				\
		core/mem.o

#netif
OBJS	+= 	netif/ethernet.o 		\
		netif/ethernetif.o 		\
		netif/lowpan6.o 		\
		netif/ppp/auth.o 		\
		netif/ppp/ccp.o 		\
		netif/ppp/chap-md5.o 	\
		netif/ppp/chap-new.o 	\
		netif/ppp/chap_ms.o 	\
		netif/ppp/demand.o 		\
		netif/ppp/eap.o 		\
		netif/ppp/ecp.o 		\
		netif/ppp/eui64.o 		\
		netif/ppp/fsm.o 		\
		netif/ppp/ipcp.o 		\
		netif/ppp/ipv6cp.o  	\
		netif/ppp/lcp.o 		\
		netif/ppp/magic.o 		\
		netif/ppp/mppe.o 		\
		netif/ppp/multilink.o 	\
		netif/ppp/polarssl/arc4.o 	\
		netif/ppp/polarssl/des.o 	\
		netif/ppp/polarssl/md4.o 	\
		netif/ppp/polarssl/md5.o 	\
		netif/ppp/polarssl/sha1.o 	\
		netif/ppp/ppp.o 			\
		netif/ppp/pppapi.o 			\
		netif/ppp/pppcrypt.o 		\
		netif/ppp/pppoe.o 			\
		netif/ppp/pppol2tp.o 		\
		netif/ppp/pppos.o 			\
		netif/ppp/upap.o 			\
		netif/ppp/utils.o 			\
		netif/ppp/vj.o 				\
		netif/slipif.o
 
OBJS	+= 	apps/dhcpclient.o 			\
		apps/net_main.o 			\
		apps/ping.o 				\
		apps/tcp_client.o 			\
		apps/http_urlparser.o 		\
		apps/http_client.o 			\
		apps/http_string.o

all : $(OBJS)
$(OBJS) : %.o : %.c
	@echo [CC] $(CPATH)/$^
	@$(CC) $(X_CFLAGS) $(X_INCDIR) -c $^ -o $@

