#include "neverstoptask.h" 
#include <uspienv.h>
#include <graphic/graphic.h>
#include <task.h>
#include "tasksheetmanager.h"
#include <string.h>
#include "basicutil.h"
#include <stdlib.h>
#include <stdio.h>
#include <synchronize.h>
#include <alloc.h>
#include <s5p4418_led.h>
#include <config.h>

TNeverStopTask *new_NeverStopTask(unsigned nTaskID, unsigned nPriority)
{
	TNeverStopTask *pThis = (TNeverStopTask *)malloc(sizeof(TNeverStopTask));
    if(pThis == NULL)
    {
        printf("new_NeverStopTask malloc(%d) failed\r\n", sizeof(TNeverStopTask));
        return 0;
    }
    pThis->m_baseObj = new_Task(TASK_STACK_SIZE, 7);
	pThis->m_baseObj->m_derivedObj = pThis;
	pThis->m_baseObj->delete = delete_NeverStopTask;

	pThis->m_baseObj->Run = NeverStopTaskRun;
	pThis->m_baseObj->mPriority = nPriority;
	pThis->m_nTaskID = nTaskID;
	memset(pThis->m_baseObj->m_apSheets, 0, sizeof(SHEET *) * MAX_SHEET_SIZE_PER_TASK);
	
#if 1	
	if (1 == nTaskID)
	{
		create_win(&(pThis->m_baseObj->m_apSheets[0]), 410, 0, 100, 100, edit_off, "running1");
	}
	else if (2 == nTaskID)
	{
		create_win(&(pThis->m_baseObj->m_apSheets[0]), 410, 110, 100, 100, edit_off, "running2");
	}
	else if (3 == nTaskID)
	{
		create_win(&(pThis->m_baseObj->m_apSheets[0]), 520, 0, 100, 100, edit_off, "running3");
	}
	else
	{
		create_win(&(pThis->m_baseObj->m_apSheets[0]), 520, 110, 100, 100, edit_off, "running4");
	}
#endif	
	//printf("NeverStopTask: ID = %d, baseObj = %08x\r\n", nTaskID, pThis->m_baseObj);

	tsm_add_task(pThis->m_baseObj);
	return pThis;
}

void delete_NeverStopTask(TTask *pThis)
{
	TNeverStopTask *pNeverStopTask = (TNeverStopTask *)pThis->m_derivedObj;
	delete_Task(pThis);
	free(pNeverStopTask);
}

#ifdef SMP
#define MAX_NUMBER  5000000 
#else
#define MAX_NUMBER  1000000
#endif

void NeverStopTaskRun(TTask *pThis)
{
    UNUSED(pThis);

	u32 count = 0;
	u32 circle = 0;
	//TNeverStopTask *pNeverStopTask = DYNAMIC_CAST(pThis, TNeverStopTask);

	while (1)
	{
		count++;
        if(count == MAX_NUMBER)
        {
            count = 0;
            circle++;
        }

		if (circle > 50)
		{
            circle = 0;
            //if(pNeverStopTask->m_nTaskID == 3)
            {
                //EnterCritical();
				//printf("NTask[%d][%08x] - cpu[%d]\r\n", pNeverStopTask->m_nTaskID, pThis, get_cpuid());
                //printf("NeverStopTask(%d)[%08x] - count = (%d,%d)\r\n", pNeverStopTask->m_nTaskID, pThis,  circle, count);
//                printf("NeverStopTask(%d)[%08x] - count = (%d,%d)\r\n", pNeverStopTask->m_nTaskID, pThis,  circle, count);
                //LeaveCritical();
            }
		}
	}
}

