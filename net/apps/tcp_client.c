#include "lwip/apps/tcp_client.h"
#include <alloc.h>
#include <errno.h>
#include <config.h>

struct tcp_pcb* tcp_client_pcb;


u8 lwip_client_buf[LWIP_CLIENT_BUF];		//定义用来发送和接收数据的缓存

TCP_RECEIVE_DATA_FUNC *rcv_data_callback; 

//客户端成功连接到远程主机时调用
err_t Tcp_Client_Connect(void *arg,struct tcp_pcb *tpcb, err_t err)
{
    UNUSED(tpcb);
    UNUSED(err);

	struct tcp_client_state* ts;
	ts = arg;	
	ts->state =   ES_RECEIVED;		//可以开始接收数据了

	return ERR_OK;
}


err_t Tcp_Client_Send(char *pBuf, int bufLen)
{	
	tcp_write(tcp_client_pcb, pBuf, bufLen, 1);

	return ERR_OK; 
}


//客户端接收到数据之后将要调用的函数
err_t Tcp_Client_Recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;

	int rev_len = 0;

	err_t ret_err;
	struct tcp_client_state* ts;
	ts = arg;			//TCP PCB状态
	if(p==NULL)
	{	
		(*rcv_data_callback)(NULL, 0, ES_CLOSING);
		Tcp_Client_Close(tpcb, ts);
		ret_err = ERR_OK;
	}
	else if(err!=ERR_OK)
	{	//位置错误释放pbuf
		if(p!=NULL)
		{
			pbuf_free(p);
		}
		ret_err = err;		//得到错误
	}
	else if(ts->state==ES_RECEIVED)
	{//连接收到了新的数据

		//	printf("服务器新接收的数据:%s\r\n",p->payload);

		char *pBuf = (char *) malloc(p->tot_len);
        if(pBuf == NULL)
            return ENOMEM;

	    for (q = p; q != NULL; q = q->next) {

	      	memcpy(&(pBuf[rev_len]), q->payload, q->len);
	      	rev_len += q->len;
	    }		

		(*rcv_data_callback)(pBuf, p->tot_len, ES_RECEIVED);

		tcp_recved(tpcb, p->tot_len);		//用于获取接收数据的长度,	表示可以获取更多的数据			
		pbuf_free(p);	//释放内存
		free(pBuf);
		ret_err = ERR_OK;

	}
	else if(ts->state==ES_CLOSING)//服务器关闭了
	{
		tcp_recved(tpcb, p->tot_len);		//远程端口关闭两次，垃圾数据
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


//初始化TCP客户端
err_t Tcp_Client_Init(ip_addr_t *pIpaddr, u16_t uPort, TCP_RECEIVE_DATA_FUNC *callback)
{

	rcv_data_callback = callback;

	struct tcp_client_state* ts;
	
	tcp_client_pcb = tcp_new();				//新建一个PCB
	if(tcp_client_pcb!=NULL)
	{	
		ts = mem_malloc(sizeof(struct tcp_client_state));	 //申请内存

		tcp_arg(tcp_client_pcb, ts);  				//将程序的协议控制块的状态传递给多有的回调函数
		//设定TCP的回调函数
		err_t ret = tcp_connect(tcp_client_pcb, pIpaddr, uPort, Tcp_Client_Connect);
		if (ret == ERR_OK)
		{
			tcp_recv(tcp_client_pcb, Tcp_Client_Recv);	//指定连接接收到新的数据之后将要调用的回调函数
		}

		return ret;
//		tcp_poll(tcp_client_pcb, Tcp_Client_Poll, 0); //指定轮询时将要调用的回调函数
	}

	return ERR_ARG;
}





















