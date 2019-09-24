#include "lwip/apps/tcp_client.h"
#include <alloc.h>
#include <errno.h>
#include <config.h>

struct tcp_pcb* tcp_client_pcb;


u8 lwip_client_buf[LWIP_CLIENT_BUF];		//�����������ͺͽ������ݵĻ���

TCP_RECEIVE_DATA_FUNC *rcv_data_callback; 

//�ͻ��˳ɹ����ӵ�Զ������ʱ����
err_t Tcp_Client_Connect(void *arg,struct tcp_pcb *tpcb, err_t err)
{
    UNUSED(tpcb);
    UNUSED(err);

	struct tcp_client_state* ts;
	ts = arg;	
	ts->state =   ES_RECEIVED;		//���Կ�ʼ����������

	return ERR_OK;
}


err_t Tcp_Client_Send(char *pBuf, int bufLen)
{	
	tcp_write(tcp_client_pcb, pBuf, bufLen, 1);

	return ERR_OK; 
}


//�ͻ��˽��յ�����֮��Ҫ���õĺ���
err_t Tcp_Client_Recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;

	int rev_len = 0;

	err_t ret_err;
	struct tcp_client_state* ts;
	ts = arg;			//TCP PCB״̬
	if(p==NULL)
	{	
		(*rcv_data_callback)(NULL, 0, ES_CLOSING);
		Tcp_Client_Close(tpcb, ts);
		ret_err = ERR_OK;
	}
	else if(err!=ERR_OK)
	{	//λ�ô����ͷ�pbuf
		if(p!=NULL)
		{
			pbuf_free(p);
		}
		ret_err = err;		//�õ�����
	}
	else if(ts->state==ES_RECEIVED)
	{//�����յ����µ�����

		//	printf("�������½��յ�����:%s\r\n",p->payload);

		char *pBuf = (char *) malloc(p->tot_len);
        if(pBuf == NULL)
            return ENOMEM;

	    for (q = p; q != NULL; q = q->next) {

	      	memcpy(&(pBuf[rev_len]), q->payload, q->len);
	      	rev_len += q->len;
	    }		

		(*rcv_data_callback)(pBuf, p->tot_len, ES_RECEIVED);

		tcp_recved(tpcb, p->tot_len);		//���ڻ�ȡ�������ݵĳ���,	��ʾ���Ի�ȡ���������			
		pbuf_free(p);	//�ͷ��ڴ�
		free(pBuf);
		ret_err = ERR_OK;

	}
	else if(ts->state==ES_CLOSING)//�������ر���
	{
		tcp_recved(tpcb, p->tot_len);		//Զ�̶˿ڹر����Σ���������
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	else
	{	
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		ret_err = ERR_OK;
	}

	return ret_err;	
}

void Tcp_Client_Close(struct tcp_pcb *tpcb, struct tcp_client_state* ts)
{
    UNUSED(tpcb);

	tcp_arg(tcp_client_pcb, NULL);  			
	tcp_recv(tcp_client_pcb, NULL);
	tcp_poll(tcp_client_pcb, NULL, 0); 
	if(ts!=NULL)
	{
		mem_free(ts);
	}
	tcp_close(tcp_client_pcb);

	rcv_data_callback = NULL;
}


//��ʼ��TCP�ͻ���
err_t Tcp_Client_Init(ip_addr_t *pIpaddr, u16_t uPort, TCP_RECEIVE_DATA_FUNC *callback)
{

	rcv_data_callback = callback;

	struct tcp_client_state* ts;
	
	tcp_client_pcb = tcp_new();				//�½�һ��PCB
	if(tcp_client_pcb!=NULL)
	{	
		ts = mem_malloc(sizeof(struct tcp_client_state));	 //�����ڴ�

		tcp_arg(tcp_client_pcb, ts);  				//�������Э����ƿ��״̬���ݸ����еĻص�����
		//�趨TCP�Ļص�����
		err_t ret = tcp_connect(tcp_client_pcb, pIpaddr, uPort, Tcp_Client_Connect);
		if (ret == ERR_OK)
		{
			tcp_recv(tcp_client_pcb, Tcp_Client_Recv);	//ָ�����ӽ��յ��µ�����֮��Ҫ���õĻص�����
		}

		return ret;
//		tcp_poll(tcp_client_pcb, Tcp_Client_Poll, 0); //ָ����ѯʱ��Ҫ���õĻص�����
	}

	return ERR_ARG;
}





















