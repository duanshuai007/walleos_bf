#ifndef _X_SAMPLE_HELLO_WORLD_NEVERSTOPTASK_H_
#define _X_SAMPLE_HELLO_WORLD_NEVERSTOPTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TNeverStopTask
{
	struct TTask *m_baseObj;
	unsigned    m_nTaskID;
}TNeverStopTask;

TNeverStopTask *new_NeverStopTask(unsigned nTaskID, unsigned nPriority);
void delete_NeverStopTask(struct TTask *pThis);
void NeverStopTaskRun(struct TTask *pThis);

#ifdef __cplusplus
}
#endif

#endif
