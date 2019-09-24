#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_
#include "lwip/apps/network.h"
#include "lwip/tcp.h"


//����״̬
enum tcp_client_states			
{
  ES_NONE = 0,			
  ES_RECEIVED,		//���յ�������
  ES_CLOSING			//���ӹر�
};

 //TCP������״̬
struct tcp_client_state
{
  u8_t state;
};

typedef void TCP_RECEIVE_DATA_FUNC(char *buffer, int buflen, int msg_type);

#define LWIP_CLIENT_BUF   536	//TCP���ӻ���

extern u8 lwip_client_buf[LWIP_CLIENT_BUF];		//�����������ͺͽ������ݵĻ���


//�ͻ��˳ɹ����ӵ�Զ������ʱ����
err_t Tcp_Client_Connect(void *arg,struct tcp_pcb *tpcb,err_t err);

//������ѯʱ��Ҫ���õĺ���
//err_t Tcp_Client_Poll(void *arg, struct tcp_pcb *tpcb);

//��������Զ������
void Tcp_Client_Connect_Remotehost(void);

//�ͻ��˽��յ�����֮��Ҫ���õĺ���
err_t Tcp_Client_Recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

//�ر�����
void Tcp_Client_Close(struct tcp_pcb *tpcb, struct tcp_client_state* ts);
	
//��ʼ��TCP�ͻ���
err_t Tcp_Client_Init(ip_addr_t *pIpaddr, u16_t uPort, TCP_RECEIVE_DATA_FUNC *callback);

err_t Tcp_Client_Send(char *pBuf, int bufLen);



















#endif













