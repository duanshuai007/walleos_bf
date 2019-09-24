#include "dtouchtimertask.h" 
#include <uspios.h>
#include <uspienv.h>
#include <alloc.h>
#include <task.h>
#include <scheduler.h>
#include <input/messagequeue.h>
#include <picture_opt.h>
#include <s5p4418_serial_stdio.h>
#include <errno.h>
#include <config.h>
#include <stdio.h>
#include <global_timer.h>

TDTouchTimerTask *new_DTouchTimerTask(unsigned nPriority)
{
	TDTouchTimerTask *pThis = (TDTouchTimerTask *)malloc(sizeof(TDTouchTimerTask));
    if(pThis == NULL)
	{
		printf("new_DTouchTimerTask pThis == NULL\r\n");
        return NULL;
	}
	pThis->m_baseObj = new_Task(TASK_STACK_SIZE, MAIN_TASK_LEVEL);
	pThis->m_baseObj->m_derivedObj = pThis;
	pThis->m_baseObj->delete = delete_DTouchTimerTask;
	pThis->m_baseObj->Run = dTouchTimerTaskRun;
	pThis->m_baseObj->mPriority = nPriority;
	dtouchtimer_pMessageQueue = initMessageQueue(pThis->m_baseObj,DTouchTimerHandleMessage);

	DEBUG( TOUCH_ON, "DTouchTimerTask: baseObj = %08x, message = %08x\r\n", pThis->m_baseObj, dtouchtimer_pMessageQueue);
	return pThis;
}

void delete_DTouchTimerTask(TTask *pThis)
{
	TDTouchTimerTask *pDTouchTimerTask = (TDTouchTimerTask *)pThis->m_derivedObj;
	delete_Task(pThis);
	free(pDTouchTimerTask);
}

void dTouchTimerTaskRun(TTask *pThis)
{
    UNUSED(pThis);

	TTask *pTask = 0;
	while (1)
	{
		if (hasMessage(dtouchtimer_pMessageQueue))
	    {
            DEBUG( TOUCH_ON, "__in dTouchTimerTaskRun hasMessage\r\n");
		    handleTimeoutMessage(global_timer_get_counter(), dtouchtimer_pMessageQueue);
		}
		else
		{
            DEBUG( TOUCH_ON, "__in dTouchTimerTaskRun NOT hasMessage\r\n");
            SchedulerBlockTask(&pTask);
		}
	}
}

