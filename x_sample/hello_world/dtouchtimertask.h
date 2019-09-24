#ifndef _X_SAMPLE_HELLO_WORLD_DTOUCHTIMER_TASK_H_
#define _X_SAMPLE_HELLO_WORLD_DTOUCHTIMER_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDTouchTimerTask
{
	struct TTask *m_baseObj;
}TDTouchTimerTask;

TDTouchTimerTask *new_DTouchTimerTask(unsigned nPriority);

void delete_DTouchTimerTask(struct TTask *pThis);

void dTouchTimerTaskRun(struct TTask *pThis);

void touchTask(void);

#ifdef __cplusplus
}
#endif

#endif
