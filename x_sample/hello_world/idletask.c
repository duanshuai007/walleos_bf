#include "idletask.h" 
#include <uspienv.h>
#include <graphic/graphic.h>
#include <task.h>
#include <alloc.h>
#include <stdlib.h>
#include <errno.h>
#include <config.h>

TIdleTask *new_IdleTask()
{
	TIdleTask *pThis = (TIdleTask *)malloc(sizeof(TIdleTask));
    if(pThis == NULL)
        return NULL;
	pThis->m_baseObj = new_Task(TASK_STACK_SIZE, 1);
	pThis->m_baseObj->m_derivedObj = pThis;
	pThis->m_baseObj->delete = delete_IdleTask;

	pThis->m_baseObj->Run = IdleTaskRun;
	pThis->m_baseObj->mPriority = 1;
	pThis->m_baseObj->mLevel = 0;
	
	return pThis;
}

void delete_IdleTask(TTask *pThis)
{
	TIdleTask *pIdleTask = (TIdleTask *)pThis->m_derivedObj;
	delete_Task(pThis);
	free(pIdleTask);
}

void IdleTaskRun(TTask *pThis)
{
    UNUSED(pThis);

	while (1)
	{
		//write_uart("io_hlt()\n");
		io_hlt();
	}
}
