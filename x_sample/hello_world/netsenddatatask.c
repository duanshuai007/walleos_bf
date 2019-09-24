#include "netsenddatatask.h" 
#include <uspienv.h>
#include <task.h>
#include "basicutil.h"
#include "lwip/apps/net_main.h"
#include <alloc.h>

TNetSendDataTask *new_NetSendDataTask(unsigned nTaskID, unsigned nPriority)
{
	TNetSendDataTask *pThis = (TNetSendDataTask *)malloc(sizeof(TNetSendDataTask));
	pThis->m_baseObj = new_Task(TASK_STACK_SIZE, 7);
	pThis->m_baseObj->m_derivedObj = pThis;
	pThis->m_baseObj->delete = delete_NetSendDataTask;

	pThis->m_baseObj->Run = NetSendDataTaskRun;
	pThis->m_baseObj->mPriority = nPriority;
	pThis->m_nTaskID = nTaskID;

	return pThis;
}

void delete_NetSendDataTask(TTask *pThis)
{
	TNetSendDataTask *pNeverStopTask = (TNetSendDataTask *)pThis->m_derivedObj;
	delete_Task(pThis);
	free(pNeverStopTask);
}

extern void cmd_download_file_callback(char *buffer, int buflen, char *fileType);
int g_http_sw = 1;
void NetSendDataTaskRun(TTask *pThis)
{
	printf("NetSendDataTaskRun start\r\n");

	while(1)
	{
		if (g_http_sw == 1)
		{
    		SendHttpGetReq("http://192.168.200.26/1.txt", cmd_download_file_callback);
			g_http_sw = 0;
		}
	}

	printf("NetSendDataTaskRun end\r\n");
}

