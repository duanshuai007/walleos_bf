#include <alloc.h>
#include <task.h>
#include <assert.h>
#include <string.h>
#include <scheduler.h>
#include <uspienv.h>
#include <stdlib.h>
#include <timer.h>
#include <s5p4418_timer.h>
#include <synchronize.h>
#include <s5p4418_serial_stdio.h>
#include <errno.h>

static const unsigned int MIN_STACK_SIZE    = 1024;
static const unsigned int STACK_ALIGN_MASK  = (4-1);

TTask* new_Task(unsigned nStackSize, unsigned nLevel)
{
	TTask *pThis = (TTask *)malloc(sizeof(TTask));
    if(pThis == NULL)
    {
        printf("new_Task pThis==NULL\r\n");
        return NULL;
    }
    pThis->m_State = TaskStateReady;
	pThis->m_nStackSize = nStackSize;
	pThis->m_pStack = 0;

	if (pThis->m_nStackSize != 0)
	{
		assert (pThis->m_nStackSize >= MIN_STACK_SIZE);
		assert ((pThis->m_nStackSize & STACK_ALIGN_MASK) == 0);
		pThis->m_pStack =  malloc(pThis->m_nStackSize);
        if(pThis->m_pStack == NULL)
        {
            printf("new_Task pThis->m_pStack == NULL\r\n");
            free(pThis);
            return NULL;
        }	
		TaskInitializeRegs (pThis);
	}

	pThis->Run = TaskRun;
	pThis->m_derivedObj = 0; 
	pThis->delete = delete_Task;
	pThis->mPriority = 0.02 * HZ; //0.02秒
	pThis->mLevel = nLevel;

	SchedulerAddTask (pThis);

    return pThis;
}

void delete_Task(TTask *pThis)
{
    if(pThis == NULL)
        return;
	//write_uart("delete_Task()");
	assert (pThis->m_State == TaskStateTerminated); //状态必须为terminated才能释放
	pThis->m_State = TaskStateUnknown;

    if (pThis->m_pStack)
    {
	    free(pThis->m_pStack);
	    pThis->m_pStack = 0;
    }
    free(pThis);
}

void TaskRun(TTask *pThis)
{
    if(pThis == NULL)
        return;

	assert(0);
}

void TaskInitializeRegs(TTask *pThis)
{
    if(pThis == NULL)
	{
		printf("TaskInitializeRegs pThis == NULL\r\n");
        return;
	}
	memset (&pThis->m_Regs, 0, sizeof pThis->m_Regs);

	pThis->m_Regs.r0 = (u32) pThis;		// pParam for TaskEntry()

	assert (pThis->m_pStack != 0);
	pThis->m_Regs.sp = (u32) pThis->m_pStack + pThis->m_nStackSize; //栈顶指针

	pThis->m_Regs.lr = (u32) &TaskTaskEntry; //TaskSwitch的汇编最后一句是bx lr
	pThis->m_Regs.pc = (u32) &TaskTaskEntry; //TaskSwitch的汇编最后一句是bx lr
    u32 nFlags;
    __asm volatile ("mrs %0, cpsr" : "=r" (nFlags));
    pThis->m_Regs.spsr = nFlags;
}

void TaskTaskEntry(void *pParam)
{
    if(pParam == NULL)
	{
        return;
	}

	TTask *pThis = (TTask*)pParam;
    if(pThis == NULL)
    {
        printf("TaskTaskEntry pThis == NULL\r\n");
        return;
    }
	//assert(pThis !=0);

	pThis->Run(pThis);

	pThis->m_State = TaskStateTerminated;
	SchedulerYield ();

	//assert (0);
}

TTaskState TaskGetState(TTask *pThis)
{
    if(pThis == NULL)
	{
		printf("TaskGetState pThis==NULL\r\n`");
        return TaskStateUnknown;
	}
	return pThis->m_State;
}

void TaskSetState(TTask *pThis, TTaskState State)
{
    if(pThis == NULL)
	{
		printf("TaskSetState pThis == NULL\r\n");
        return;
	}
	pThis->m_State = State;
}

unsigned TaskGetWakeTicks(TTask *pThis)
{
    if(pThis == NULL)
	{
		printf("TaskGetWakeTicks pThis==NULL\r\n");
		return FALSE;
	}
	return pThis->m_nWakeTicks;
}

void TaskSetWakeTicks (TTask *pThis, unsigned nTicks)
{
    if(pThis == NULL)
	{
		printf("TaskSetWakeTicks pThis==NULL\r\n");
        return;
	}

	pThis->m_nWakeTicks = nTicks;
}

TTaskRegisters *TaskGetRegs (TTask *pThis)
{
    if(pThis == NULL)
	{
		printf("TaskGetRegs pThis==NULL\r\n");
        return NULL;
	}
	return &pThis->m_Regs;
}

