#include <alloc.h>
#include <scheduler.h>
#include <task.h>
#include <timer.h>
#include <s5p4418_timer.h>
#include <uspienv.h>
#include <string.h>
#include <assert.h>
#include <uspios.h>
#include <timeslicetimerhandler.h>
#include <stdlib.h>
#include <stdio.h>
#include <synchronize.h>
#include <errno.h>
#include <config.h>
#include <stdio.h>
#include <headsmp.h>

static const char s_FromScheduler[] = "sched";
static TScheduler *g_pScheduler[CPU_MAX_NUM] = {0};
static unsigned g_nSchedulerTimer[CPU_MAX_NUM] = {0};

void Scheduler(void)
{

	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);	

	g_pScheduler[cpu] = malloc(sizeof(TScheduler));
	if(g_pScheduler[cpu] == NULL)
	{
		printf("Scheduler g_pScheduler[%d] == NULL\r\n", cpu);
        return;
	}

	TScheduler *pThis = g_pScheduler[cpu];

	memset(pThis->m_nTasks, 0, sizeof(pThis->m_nTasks)); //各个level当前任务数
    memset(pThis->m_pTask, 0, sizeof(pThis->m_pTask));
    memset(pThis->m_nLevelCurrent, -1, sizeof(pThis->m_nLevelCurrent));
	pThis->m_pCurrent = 0;
	pThis->m_nCurrentLevel = 0;
	pThis->m_nCurrent = 0;

	pThis->m_pCurrent = new_Task(TASK_STACK_SIZE, MAIN_TASK_LEVEL);
    pThis->m_nCurrentLevel = MAIN_TASK_LEVEL;
	assert(pThis->m_pCurrent != 0);

	return;
}

void _Scheduler(void)
{
	for (int i = 0; i < CPU_MAX_NUM; ++i)
	{
		TScheduler *pTemp = g_pScheduler[i];

		unsigned nTotalTasks = 0;
		for (unsigned i = 0; i < MAX_TASK_LEVELS; i ++)
		{
			nTotalTasks += pTemp->m_nTasks[i];
		}
		assert (nTotalTasks == 1); //必须除了main task没有其它任务
		assert (MAIN_TASK_LEVEL == pTemp->m_nCurrentLevel); // main task level
		assert (0 == pTemp->m_nCurrent); // mail task index iniitial 0
		assert (pTemp->m_pTask[MAIN_TASK_LEVEL][0] == pTemp->m_pCurrent); //当前任务必须是第一个任务，同样是保证除了main task没有其它任务了
		SchedulerRemoveTask (pTemp->m_pCurrent);
		delete_Task(pTemp->m_pCurrent);
		pTemp->m_pCurrent = 0;

	    if(pTemp != NULL)
	    {
	        free(pTemp);
			pTemp = 0;
	    }
	}
}

void SchedulerYield (void)
{
	TScheduler *pScheduler = ScheduerGet();

    DisableInterrupts();

    DEBUG( SCHEDUER_ON, "SchedulerYield old cpu(%d), m_nCurrent = %u, %u, %08x\r\n",get_cpuid(), pScheduler->m_nCurrentLevel, pScheduler->m_nCurrent, pScheduler->m_pTask[pScheduler->m_nCurrentLevel][pScheduler->m_nCurrent]);
	//获取同等级下一个准备好的任务 
	while ((pScheduler->m_nCurrent = SchedulerGetNextTask (&(pScheduler->m_nCurrentLevel))) == MAX_TASKS)	
	{
		// no task is ready
		assert (pScheduler->m_nTasks[pScheduler->m_nCurrentLevel] > 0);
	}

	//get ready task
	assert (pScheduler->m_nCurrentLevel < MAX_TASK_LEVELS);
	assert (pScheduler->m_nCurrent < MAX_TASKS);
	pScheduler->m_nLevelCurrent[pScheduler->m_nCurrentLevel] = pScheduler->m_nCurrent;

    DEBUG( SCHEDUER_ON, "SchedulerYield new cpu(%d), m_nCurrent = %u, %u, %08x\r\n",get_cpuid(), pScheduler->m_nCurrentLevel, pScheduler->m_nCurrent, pScheduler->m_pTask[pScheduler->m_nCurrentLevel][pScheduler->m_nCurrent]);

	TTask *pNext = pScheduler->m_pTask[pScheduler->m_nCurrentLevel][pScheduler->m_nCurrent];
	//assert (pNext != 0);
	if(pNext == NULL)
	{
		printf("SchedulerYield pNext == NULL\r\n");
		EnableInterrupts();
		return;
	}
	if (pScheduler->m_pCurrent == pNext)
	{
        EnableInterrupts();
		return;
	}
	
	TTaskRegisters *pOldRegs = TaskGetRegs (pScheduler->m_pCurrent);
	pScheduler->m_pCurrent = pNext;
	TTaskRegisters *pNewRegs = TaskGetRegs (pScheduler->m_pCurrent);
//    printf("pOldRegs: %x %x %x %x %x \r\n%x %x %x %x %x\r\n%x %x %x %x %x\r\n%x %x\r\n",
//            pOldRegs->r0, pOldRegs->r1, pOldRegs->r2, pOldRegs->r3, pOldRegs->r4,
//            pOldRegs->r5, pOldRegs->r6, pOldRegs->r7, pOldRegs->r8, pOldRegs->r9,
//            pOldRegs->r10,pOldRegs->r11,pOldRegs->r12,pOldRegs->pc,pOldRegs->sp,
//            pOldRegs->lr, pOldRegs->spsr);
//
//    printf("pNewRegs: %x %x %x %x %x \r\n%x %x %x %x %x\r\n%x %x %x %x %x\r\n%x %x\r\n",
//            pNewRegs->r0, pNewRegs->r1, pNewRegs->r2, pNewRegs->r3, pNewRegs->r4,
//            pNewRegs->r5, pNewRegs->r6, pNewRegs->r7, pNewRegs->r8, pNewRegs->r9,
//            pNewRegs->r10,pNewRegs->r11,pNewRegs->r12,pNewRegs->pc,pNewRegs->sp,
//            pNewRegs->lr, pNewRegs->spsr);

	assert (pOldRegs != 0);
	assert (pNewRegs != 0);

    UpdateKernelTimer(getSchedulerTimer(), pNext->mPriority + 1); //这里如果不+1，则第一次主动放权任务切换之后会马上被定时器中断打断，还需要调整
	TaskSwitch (pOldRegs, pNewRegs);

    //理应运行不到这里
	DEBUG( SCHEDUER_ON, "SchedulerYield end\r\n");
    EnableInterrupts();
}

void SchedulerYieldInterrupt (void)
{
    DEBUG( SCHEDUER_ON, "SchedulerYieldInterrupt begin cpu(%d)\r\n",get_cpuid());

	TScheduler *pScheduler = ScheduerGet();

	while ((pScheduler->m_nCurrent = SchedulerGetNextTask (&(pScheduler->m_nCurrentLevel))) == MAX_TASKS)
	{
		// no task is ready
		DEBUG( SCHEDUER_ON, "no task is ready\r\n");
        assert (pScheduler->m_nTasks[pScheduler->m_nCurrentLevel] > 0);
	}

    assert (pScheduler->m_nCurrentLevel < MAX_TASK_LEVELS);
	assert (pScheduler->m_nCurrent < MAX_TASKS);
	pScheduler->m_nLevelCurrent[pScheduler->m_nCurrentLevel] = pScheduler->m_nCurrent;

	TTask *pNext = pScheduler->m_pTask[pScheduler->m_nCurrentLevel][pScheduler->m_nCurrent];
	assert (pNext != 0);

	if (pScheduler->m_pCurrent == pNext)
	{
		//0.01秒
        DEBUG( SCHEDUER_ON, "in SchedulerYieldInterrupt: pScheduler->m_pCurrent == pNext\r\n");

        setSchedulerTimer(StartKernelTimer(TASK_SWTICH_TIME * HZ, TimeSliceTimerHandler, 0, 0));

		return;
	}
	
	TTaskRegisters *pOldRegs = TaskGetRegs (pScheduler->m_pCurrent);
	pScheduler->m_pCurrent = pNext;
	TTaskRegisters *pNewRegs = TaskGetRegs (pScheduler->m_pCurrent);
	// if(get_cpuid() == 2)
	// printf("pOldRegs = %08x, pNewRegs = %08x\r\n", pOldRegs, pNewRegs);

	assert (pOldRegs != 0);
	assert (pNewRegs != 0);

    TaskSwitchInterrupt (pOldRegs, pNewRegs);

    DEBUG( SCHEDUER_ON, "pOldRegs: \t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x\r\n",
            pOldRegs->r0, pOldRegs->r1, pOldRegs->r2, pOldRegs->r3, pOldRegs->r4,
            pOldRegs->r5, pOldRegs->r6, pOldRegs->r7, pOldRegs->r8, pOldRegs->r9,
            pOldRegs->r10,pOldRegs->r11,pOldRegs->r12,pOldRegs->pc,pOldRegs->sp,
            pOldRegs->lr, pOldRegs->spsr);

    DEBUG( SCHEDUER_ON, "pNewRegs: \t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x %08x %08x %08x\r\n\t\t%08x %08x\r\n",
            pNewRegs->r0, pNewRegs->r1, pNewRegs->r2, pNewRegs->r3, pNewRegs->r4,
            pNewRegs->r5, pNewRegs->r6, pNewRegs->r7, pNewRegs->r8, pNewRegs->r9,
            pNewRegs->r10,pNewRegs->r11,pNewRegs->r12,pNewRegs->pc,pNewRegs->sp,
            pNewRegs->lr, pNewRegs->spsr);

	setSchedulerTimer(StartKernelTimer(pNext->mPriority, TimeSliceTimerHandler, 0, 0));

    DEBUG( SCHEDUER_ON, "SchedulerYieldInterrupt end\r\n");
}

void SchedulerSleep(unsigned nSeconds)
{
	// be sure the clock does not run over taken as signed int
	const unsigned nSleepMax = 1800;	// normally 2147 but to be sure
	while (nSeconds > nSleepMax)
	{
		SchedulerUsSleep (nSleepMax * 1000000);

		nSeconds -= nSleepMax;
	}
    //乘 1000 000 转换为秒延时
	SchedulerUsSleep (nSeconds * 1000000);
}

void SchedulerMsSleep (unsigned nMilliSeconds)
{
	if (nMilliSeconds > 0)
	{
        //乘1000 转换为ms延时
		SchedulerUsSleep (nMilliSeconds * 1000);
	}
}

void SchedulerUsSleep (unsigned nMicroSeconds)
{

	TScheduler *pScheduler = ScheduerGet();

	if (nMicroSeconds > 0)
	{
		unsigned nTicks = nMicroSeconds * (CLOCKHZ / 1000000);

		unsigned nStartTicks = TimerGetTicks();
		//nStartTicks *= (10 * 1000);
		nStartTicks *= 1000;

		assert (pScheduler->m_pCurrent != 0);
		assert (TaskGetState (pScheduler->m_pCurrent) == TaskStateReady);
		TaskSetWakeTicks (pScheduler->m_pCurrent, nStartTicks + nTicks);
		TaskSetState (pScheduler->m_pCurrent, TaskStateSleeping);

		SchedulerYield ();
	}
}

void SchedulerAddTask(TTask *pTask)
{
	assert (pTask != 0);
	assert (pTask->mLevel < MAX_TASK_LEVELS);

	TScheduler *pScheduler = ScheduerGet();

	unsigned i;
	for (i = 0; i < pScheduler->m_nTasks[pTask->mLevel]; i++)
	{
		if (pScheduler->m_pTask[pTask->mLevel][i] == 0)
		{
			pScheduler->m_pTask[pTask->mLevel][i] = pTask;
            DEBUG( SCHEDUER_ON, "AddTask - 11111 i = (%u, %u), task = %x\r\n", pTask->mLevel, i, pTask);

			return;
		}
	}

	if (pScheduler->m_nTasks[pTask->mLevel] >= MAX_TASKS)
	{
	}
    DEBUG( SCHEDUER_ON, "AddTask - 2222 i = (%u, %u), task = %x\r\n", 
            pTask->mLevel, pScheduler->m_nTasks[pTask->mLevel], pTask);

	pScheduler->m_pTask[pTask->mLevel][pScheduler->m_nTasks[pTask->mLevel]] = pTask;
	pScheduler->m_nTasks[pTask->mLevel] += 1;
}


void SchedulerRemoveTask(TTask *pTask)
{
	assert (pTask != 0);
	assert (pTask->mLevel < MAX_TASK_LEVELS);

	TScheduler *pScheduler = ScheduerGet();

	for (unsigned i = 0; i < pScheduler->m_nTasks[pTask->mLevel]; i++)
	{
		if (pScheduler->m_pTask[pTask->mLevel][i] == pTask)
		{
			pScheduler->m_pTask[pTask->mLevel][i] = 0;
            DEBUG( SCHEDUER_ON, "RemoveTask - i = (%u, %u), task = %u\r\n", pTask->mLevel, i, pTask);
			if (i == pScheduler->m_nTasks[pTask->mLevel]-1)
			{
				pScheduler->m_nTasks[pTask->mLevel]--;
			}

			return;
		}
	}

	assert (0);	
}

void SchedulerKillSelfTask(TTask *pTask)
{
	TScheduler *pScheduler = ScheduerGet();

	assert (pTask != 0);
	assert (pTask == pScheduler->m_pCurrent);
	assert (TaskGetState (pTask) == TaskStateReady);

	TaskSetState (pTask, TaskStateTerminated);
    SchedulerYield ();
}

void SchedulerBlockTask(TTask **ppTask)
{
	TScheduler *pScheduler = ScheduerGet();

	assert (ppTask != 0);
	*ppTask = pScheduler->m_pCurrent;

	assert (pScheduler->m_pCurrent != 0);
	assert (TaskGetState (pScheduler->m_pCurrent) == TaskStateReady);
	
    TaskSetState (pScheduler->m_pCurrent, TaskStateBlocked);
	SchedulerYield ();
}

void SchedulerWakeTask(TTask **ppTask)
{
	assert (ppTask != 0);
	TTask *pTask = *ppTask;

	*ppTask = 0;

#ifdef NDEBUG
	if (   pTask == 0
	    || TaskGetState (pTask) != TaskStateBlocked)
	{
		LogWrite (s_FromScheduler, LogPanic, "Tried to wake non-blocked task");
	}
#else
	assert (pTask != 0);
	assert (TaskGetState (pTask) == TaskStateBlocked);
#endif

	TaskSetState (pTask, TaskStateReady);
}

unsigned SchedulerGetNextTask(unsigned * pLevel)
{
	TScheduler *pScheduler = ScheduerGet();

	for (int nLevel = MAX_TASK_LEVELS - 1; nLevel >= 0 ; nLevel --)
	{
		if (0 == pScheduler->m_nTasks[nLevel])
		{
			continue;
		}

		*pLevel = nLevel;

		//当前level中正在执行的task
		unsigned nTask = pScheduler->m_nLevelCurrent[nLevel] < MAX_TASKS? pScheduler->m_nLevelCurrent[nLevel] : 0;
		
		unsigned nTicks = TimerGetTicks(); 
		nTicks *= 1000;

		for (unsigned i = 1; i <= pScheduler->m_nTasks[nLevel]; i++) //除了main task 以外的任务中查找
		{
			if (++nTask >= pScheduler->m_nTasks[nLevel]) //下一个
			{
				nTask = 0;
			}

			TTask *pTask = pScheduler->m_pTask[nLevel][nTask];
			if (pTask == 0)
			{
				continue;
			}

			switch (TaskGetState (pTask))
			{
			case TaskStateReady:
		        DEBUG( SCHEDUER_ON, "enter SchedulerGetNextTask: task %08x nTask %d in TaskStateReady\r\n", pTask, nTask);
				return nTask;

			case TaskStateBlocked:
				continue;

			case TaskStateSleeping:
		        DEBUG( SCHEDUER_ON, "enter SchedulerGetNextTask: in TaskStateSleeping\r\n");
                if ((int) (TaskGetWakeTicks (pTask) - nTicks) > 0) //还没到时间
				{
					continue;
				}
				TaskSetState (pTask, TaskStateReady); //sleep结束了
				return nTask;

			case TaskStateTerminated:
                DEBUG( SCHEDUER_ON, "enter SchedulerGetNextTask: in TaskStateTerminated\r\n");
				SchedulerRemoveTask(pTask);
				pTask->delete(pTask);
				return MAX_TASKS;

			default:
				printk("TaskGetState (pTask) = %d\r\n", TaskGetState (pTask));
				assert (0);
				break;
			}
		}
	}

	return MAX_TASKS;	
}

void setSchedulerTimer(int timer)
{
    int cpu = get_cpuid();
    assert(cpu <= CPU_MAX_NUM);

    g_nSchedulerTimer[cpu] = timer; 

}

int getSchedulerTimer(void)
{
    int cpu = get_cpuid();
    assert(cpu <= CPU_MAX_NUM);

    return g_nSchedulerTimer[cpu];
}

TScheduler *ScheduerGet(void)
{
	int cpu = get_cpuid();

	assert(cpu <= CPU_MAX_NUM);
	assert(g_pScheduler[cpu] != 0);

	return g_pScheduler[cpu];	
}
