#ifndef  _ENV_INCLUDE_SCHEDULER_H_ 
#define  _ENV_INCLUDE_SCHEDULER_H_

#include <task.h>
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TScheduler
{
	TTask *m_pTask[MAX_TASK_LEVELS][MAX_TASKS]; // 所有任务数组，分级存储
	unsigned m_nTasks[MAX_TASK_LEVELS]; // 当前级别任务数
	int m_nLevelCurrent[MAX_TASK_LEVELS]; // 当前级别正在执行任务index

	TTask *m_pCurrent; // 当前正在执行的任务
	unsigned m_nCurrentLevel; // 当前正在执行任务的级别
	unsigned m_nCurrent;			// 当前正在执行任务index
}TScheduler;


void SchedulerYield (void);			// 任务主动放权切进行任务切换
void SchedulerYieldInterrupt (void); // 定时器中断导致任务切换
void SchedulerSleep (unsigned nSeconds); // 任务进入睡眠状态，时间到了恢复为ready状态
void SchedulerMsSleep (unsigned nMilliSeconds);
void SchedulerUsSleep (unsigned nMicroSeconds);
void SchedulerRemoveTask(TTask *pTask); // 将任务从调度中移除
/*
* 获取下一个等待调度任务
* return : 任务index
* pLevel : 做返回值，任务级别
*/
unsigned SchedulerGetNextTask(unsigned * pLevel);
void SchedulerWakeTask(TTask **ppTask); // 唤醒任务,任务状态由block到ready状态
void SchedulerBlockTask(TTask **ppTask); // 中断任务，任务状态由ready到block状态
void SchedulerAddTask(TTask *pTask); // 将任务增加到调度中
void Scheduler(void);
void _Scheduler(void);
void SchedulerKillSelfTask(TTask *pTask);
TScheduler *ScheduerGet(void);
void setSchedulerTimer(int timer);
int getSchedulerTimer(void);


#ifdef __cplusplus
}
#endif

#endif
