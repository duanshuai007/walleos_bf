#ifndef _ENV_INCLUDE_TASK_H_ 
#define _ENV_INCLUDE_TASK_H_

#include <config.h>
#include <taskswitch.h>

#define DYNAMIC_CAST(p, TYPE) ((TYPE*)p->m_derivedObj) //得到子类
#define STATIC_CAST(p, TYPE) ((TYPE*)p->m_baseObj) //得到父类

typedef enum TTaskState
{
	TaskStateReady,
	TaskStateBlocked,
	TaskStateSleeping,
	TaskStateTerminated,
	TaskStateUnknown
}TTaskState;

typedef struct TTask
{
	void *m_derivedObj; //子类实例
	void (*Run)(struct TTask*);
	void (*delete)(struct TTask*);

	volatile TTaskState m_State; //任务状态
	unsigned m_nWakeTicks; // 唤醒tick
	TTaskRegisters m_Regs; // 用来保存任务运行现场
	unsigned	    m_nStackSize; //栈大小，不小于1024，4的整数倍
	u8		   *m_pStack; //栈空间字节数组，数组大小m_nStackSize
	unsigned mPriority; //优先级
	unsigned mLevel; //level
	struct SHEET * m_apSheets[MAX_SHEET_SIZE_PER_TASK]; //task对应窗口
}TTask;

void TaskInitializeRegs(TTask *pThis);
void TaskRun(TTask *pThis);
void delete_Task(TTask *pThis);
void TaskTaskEntry(void *pParam);
TTask* new_Task(unsigned nStackSize, unsigned nLevel);
TTaskRegisters *TaskGetRegs (TTask *pThis);
TTaskState TaskGetState(TTask *pThis);
void TaskSetWakeTicks (TTask *pThis, unsigned nTicks);
void TaskSetState(TTask *pThis, TTaskState State);
unsigned TaskGetWakeTicks(TTask *pThis);

#endif
