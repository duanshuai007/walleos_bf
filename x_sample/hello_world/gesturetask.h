#ifndef _X_SAMPLE_HELLO_WORLD_GESTURE_TASK_H_ 
#define _X_SAMPLE_HELLO_WORLD_GESTURE_TASK_H_


typedef struct TGestureTask
{
	struct TTask *m_baseObj;
	unsigned    m_nTaskID;
    struct TOnDoubleTapListener * m_onDoubleTapListener;
    struct TOnGestureListener* m_onGestureListener;
    struct TOnScaleGestureListener* m_onScaleGestureListener;
}TGestureTask;

TGestureTask *new_GestureTask(unsigned nTaskID, unsigned nPriority);
void delete_GestureTask(struct TTask *pThis);
void GestureTaskRun(struct TTask *pThis);

void gestureTask(void);

#endif
