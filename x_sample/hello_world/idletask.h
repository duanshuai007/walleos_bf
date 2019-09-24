#ifndef _X_SAMPLE_HELLO_WORLD_IDLE_TASK_H_
#define _X_SAMPLE_HELLO_WORLD_IDLE_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TIdleTask
{
	struct TTask *m_baseObj;
	unsigned    m_nTaskID;
	struct SHEET * m_pSheet;
}TIdleTask;

TIdleTask *new_IdleTask(void);
void delete_IdleTask(struct TTask *pThis);
void IdleTaskRun(struct TTask *pThis);

#ifdef __cplusplus
}
#endif

#endif
