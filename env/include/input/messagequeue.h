#ifndef _ENV_USPIENV_INPUT_MESSAGE_QUEUE_H
#define _ENV_USPIENV_INPUT_MESSAGE_QUEUE_H
#include <types.h>

typedef void TMessageHandler (int what);

typedef struct TNode
{
    u64 when;
    int what;
    struct TNode * pNext;
} TNode;

typedef struct TList
{
    TNode * pHead;
    struct TTask *task;
    TMessageHandler * messageHandler;
} TList;

TList * initMessageQueue(struct TTask *task, TMessageHandler *messageHandler);
void LinkedListDelete(int data, TList * pList);
void enqueueMessage(int what, u64 when, TList * pList);
void removeMessages(int what, TList * pList);
int hasMessage(TList * pList);
int hasSpecificMessage(int what, TList * pList);
void handleTimeoutMessage(u64 when, TList * pList);

extern TList * g_pMessageQueue;

extern TList * dtouchtimer_pMessageQueue;


#endif
