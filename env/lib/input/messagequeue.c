#include <input/messagequeue.h>
#include <stdlib.h>
#include <task.h>
#include <uspienv.h>
#include <synchronize.h>
#include <alloc.h>
#include <errno.h>
#include <s5p4418_serial_stdio.h>

TList * g_pMessageQueue = 0; //手势识别消息队列
TList * dtouchtimer_pMessageQueue = 0; //双击缩放定时器用消息队列

static TNode * createnode(int what, u64 when)
{
    TNode * pNewNode = malloc(sizeof(TNode));
    if(pNewNode == NULL)
    {
        printf("createnode: pNewNode == NULL\r\n");
        return NULL;
    }//duanshui modify
    pNewNode->what = what;
    pNewNode->when = when;
    pNewNode->pNext = 0;
    return pNewNode;
}

TList * initMessageQueue(struct TTask *task, TMessageHandler *messageHandler)
{
    TList * pList = malloc(sizeof(TList));
    if(pList == NULL)
    {
        printf("initMessageQueue: pList == NULL\r\n");
        return NULL;
    }
    pList->pHead = createnode(0xff,0);//链表头
    pList->task = task;
    pList->messageHandler = messageHandler;
    return pList;
}

void enqueueMessage(int what, u64 when, TList * pList)
{
    //duanshui modify
    if(pList == NULL)
    {
        printf("enqueueMessage: pList == NULL\r\n");
        return;
    }
    EnterCritical();
    TNode * pCurrent = 0;
    TNode * pPrevious = 0;
    TNode * pTmp = 0;

    if (pList->pHead == NULL)
    {
        printf("removeMessages: pList == NULL\r\n");
        LeaveCritical();
        return;
    }

    pPrevious = pList->pHead;
    pCurrent = pList->pHead->pNext; 

    while (pCurrent != NULL)
    {
        if (when < pCurrent->when)
        {           
            //在pre与current之间添加
            pTmp = createnode(what, when);

            pPrevious->pNext = pTmp;
            pTmp->pNext = pCurrent;

            break;
        }

        pPrevious = pCurrent;
        pCurrent = pCurrent->pNext;

    }

    if (pCurrent == NULL)
    {
        pTmp = createnode(what, when);
        pPrevious->pNext = pTmp;
    }

	if (pList->task != 0)
    {
		if (TaskGetState(pList->task) == TaskStateBlocked) 
        { /*如果任务处于休眠状态*/
			TaskSetState(pList->task, TaskStateReady); /*将任务唤醒*/
        }
    }
    LeaveCritical();
}

void removeMessages(int what, TList *pList)
{
    //duanshui modify
    if(pList == NULL)
    {
        printf("removeMessages: pList == NULL\r\n");
        return;
    }
    EnterCritical();
    TNode * pPrevious = pList->pHead;
    TNode * pCurrent = pPrevious->pNext;

    while (pCurrent != 0)
    {           
        if (pCurrent->what == what)	//如果类型匹配
        {
            pPrevious->pNext = pCurrent->pNext;

            free(pCurrent);

            pCurrent = pPrevious->pNext;


        }
        else
        {
            pPrevious = pCurrent;
            pCurrent = pPrevious->pNext;     
        } 
    }
    LeaveCritical();
} 

int hasSpecificMessage(int what, TList *pList)
{
    EnterCritical();
    //duanshui modify
    if(pList == NULL)
    {
        printf("hasSpecificMessage: pList == NULL\r\n");
        LeaveCritical();
        return FALSE;
    }

    TNode * pCurrent = pList->pHead->pNext;

    while (pCurrent != 0)
    {
        if (pCurrent->what == what)
        {
            LeaveCritical();
            return TRUE;
        }
        pCurrent = pCurrent->pNext;
    }

    LeaveCritical();
    return FALSE;
}

int hasMessage(TList * pList)
{
    EnterCritical();
    //duanshui modify
    if(pList == NULL)
    {
        printf("hasMessage: pList == NULL\r\n");
        LeaveCritical();
        return FALSE;
    }
    if (pList->pHead->pNext == 0)
    {
        //printf("hasMessage: pList->Head == 0\r\n");
        LeaveCritical();
        return FALSE;
    }

    LeaveCritical();
    return TRUE;
}

void handleTimeoutMessage(u64 when, TList * pList)
{
    //duanshui modify
    if(pList == NULL)
    {
        printf("handleTimeoutMessage: pList == NULL\r\n");
        return;
    }
    EnterCritical();
    TNode * pCurrent = pList->pHead->pNext;
    TNode * pFreeNode;
    while (pCurrent != 0)
    {
        if (when >= pCurrent->when)
        {
            pFreeNode = pCurrent;
            pCurrent = pCurrent->pNext;
            pList->messageHandler(pFreeNode->what);
            free(pFreeNode);
        }
        else
        {
            break;
        }
    }
    pList->pHead->pNext = pCurrent;
    LeaveCritical();
}
