#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_
#include "lwip/apps/network.h"
#include "lwip/tcp.h"


//连接状态
enum tcp_client_states			
{
  ES_NONE = 0,			
  ES_RECEIVED,		//接收到了数据
  ES_CLOSING			//连接关闭
};

 //TCP服务器状态
struct tcp_client_state
{
  u8_t state;
};

typedef void TCP_RECEIVE_DATA_FUNC(char *buffer, int buflen, int msg_type);

#define LWIP_CLIENT_BUF   536	//TCP链接缓存

extern u8 lwip_client_buf[LWIP_CLIENT_BUF];		//定义用来发送和接收数据的缓存


//客户端成功连接到远程主机时调用
err_t Tcp_Client_Connect(void *arg,struct tcp_pcb *tpcb,err_t err);

//连接轮询时将要调用的函数
//err_t Tcp_Client_Poll(void *arg, struct tcp_pcb *tpcb);

//用于连接远程主机
void Tcp_Client_Connect_Remotehost(void);

//客户端接收到数据之后将要调用的函数
err_t Tcp_Client_Recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

//关闭连接
void Tcp_Client_Close(struct tcp_pcb *tpcb, struct tcp_client_state* ts);
	
//初始化TCP客户端
err_t Tcp_Client_Init(ip_addr_t *pIpaddr, u16_t uPort, TCP_RECEIVE_DATA_FUNC *callback);

err_t Tcp_Client_Send(char *pBuf, int bufLen);



















#endif













