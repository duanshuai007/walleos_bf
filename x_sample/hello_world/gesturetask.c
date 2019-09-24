#include "gesturetask.h"
#include <uspienv.h>
#include <graphic/graphic.h>
#include <string.h>
#include <input/messagequeue.h>
#include <task.h>
#include <scheduler.h>
#include <s5p4418_serial_stdio.h>
#include <alloc.h>
#include "tasksheetmanager.h"
#include <picture_opt.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <global_timer.h>

static SHEET * s_SelectedSheet = 0;
static int s_onFlingFlg = 0; 
static int s_onScrollFlg = 0;
static boolean gestureListener_onDown(TMotionEvent * pEvent)
{
	DEBUG( GESTURE_ON, "gestureListener_onDown(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);
    //选中窗口
    s_SelectedSheet = get_current_sheet(pEvent->nEventX, pEvent->nEventY);

    return FALSE;
}

static boolean gestureListener_onUp(TMotionEvent * pEvent)
{
    DEBUG( GESTURE_ON, "gestureListener_onUp(%d, %08x, %d)\r\n",s_onFlingFlg,s_SelectedSheet, s_SelectedSheet->zoom_status);

	//fling执行时取消onUp事件
   	if(s_onFlingFlg || !s_SelectedSheet || s_SelectedSheet->zoom_status != 0)
	{
		s_onFlingFlg = 0;
		return FALSE;
	}
    DEBUG( GESTURE_ON, "gestureListener_onUp(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);

    if (s_onScrollFlg == 1)
    {
 
		u64 nNowTime    = global_timer_get_counter();
		u64 delayMicros = MOVE_CYC*1000/MOVE_COUNT;
		for(int i = 0 ; i < MOVE_COUNT ; i++)
		{
		    enqueueMessage(1,nNowTime + i*delayMicros, dtouchtimer_pMessageQueue);
		}

		s_onScrollFlg = 0;
    }

    return FALSE;
}

static void gestureListener_onShowPress(TMotionEvent * pEvent)
{
    DEBUG( GESTURE_ON, "gestureListener_onShowPress(%u,%u)\r\n",pEvent->nEventX, pEvent->nEventY);
}

static boolean gestureListener_onSingleTapUp(TMotionEvent * pEvent)
{
    DEBUG( GESTURE_ON, "gestureListener_onSingleTapUp(%u,%u)\r\n",pEvent->nEventX, pEvent->nEventY);
	if (isWindowCloseArea(s_SelectedSheet, pEvent->nEventX, pEvent->nEventY))
    {
    	//判断窗口是否绑定任务，如果绑定，需要先将任务释放
	    TTask *pTask = tsm_get_task_by_sheet(s_SelectedSheet);
	    if (0 != pTask)
	    {
	        tsm_remove_sheet(pTask, s_SelectedSheet);

	        if (FALSE == tsm_task_have_sheet(pTask))
	        {
	            SchedulerRemoveTask(pTask);
	        }
	    }    
	    
	    //关闭当前窗口	
    	sheet_free(s_SelectedSheet);
    }    
    return FALSE;
}

/**
 * Notified when a scroll occurs with the initial on down {@link MotionEvent} and the
 * current move {@link MotionEvent}. The distance in x and y is also supplied for
 * convenience.
 *
 * @param e1 The first down motion event that started the scrolling.
 * @param e2 The move motion event that triggered the current onScroll.
 * @param distanceX The distance along the X axis that has been scrolled since the last
 *              call to onScroll. This is NOT the distance between {@code e1}
 *              and {@code e2}.
 * @param distanceY The distance along the Y axis that has been scrolled since the last
 *              call to onScroll. This is NOT the distance between {@code e1}
 *              and {@code e2}.
 * @return true if the event is consumed, else false
 */
static boolean gestureListener_onScroll(TMotionEvent * pE1, TMotionEvent * pE2,
            float distanceX, float distanceY)
{
    DEBUG( GESTURE_ON, "gestureListener_onScroll:pE1(%u, %u), PE2(%u, %u), (%f,%f)\r\n",pE1->nEventX, pE1->nEventY,pE2->nEventX, pE2->nEventY,distanceX,distanceY);
	if (0 != s_SelectedSheet)
    {
		if(s_SelectedSheet->zoom_status == -1)
		{
            sheet_slide_step(s_SelectedSheet, -distanceX, -distanceY);
		}
		else if(s_SelectedSheet->zoom_status == 0)
		{
    		s_onScrollFlg = 1;
    		
			if (s_SelectedSheet->current_scale != 1)
		    {    
				pic_opt_change_size_by_scale(1);    	
		    }

            pic_opt_switch_pic(-distanceX,-distanceY,0);
		}
		else
		{
		    pic_opt_move_image(-distanceX,-distanceY);
		}
    }
    return FALSE;
}

static void gestureListener_onLongPress(TMotionEvent * pEvent)
{
    DEBUG( GESTURE_ON, "gestureListener_onLongPress(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);
}

boolean gestureListener_onFling(TMotionEvent * pE1, TMotionEvent * pE2,
            float velocityX, float velocityY)
{
    UNUSED(pE1);
    UNUSED(pE2);

	if(!s_SelectedSheet || s_SelectedSheet->zoom_status != 0)
	{
		return FALSE;
	}
	s_onFlingFlg = 1;
  	DEBUG( GESTURE_ON, "gestureListener_onFling(%u,%u)\r\n",velocityX,velocityY);

	u64 nNowTime    = global_timer_get_counter();
	u64 delayMicros = MOVE_CYC*1000/MOVE_COUNT;
	for(int i = 0 ; i < MOVE_COUNT ; i++)
	{
		if(velocityX > 0)
		{
	        enqueueMessage(2,nNowTime + i*delayMicros,dtouchtimer_pMessageQueue);
		}
		else
		{
			enqueueMessage(3,nNowTime + i*delayMicros,dtouchtimer_pMessageQueue);
		}
	}
    return FALSE;
}



static boolean doubleTapListener_onSingleTapConfirmed(TMotionEvent * pEvent)
{
	DEBUG( GESTURE_ON, "doubleTapListener_onSingleTapConfirmed(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);
    return FALSE;
}

static boolean doubleTapListener_onDoubleTap(TMotionEvent * pEvent)
{
    DEBUG( GESTURE_ON, "doubleTapListener_onDoubleTap(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);
	if(s_SelectedSheet  == 0)
    {   
	    return FALSE;
	}
	if(s_SelectedSheet->zoom_status == 0)
	{
	    if(pEvent->nEventX < s_SelectedSheet->pvx0 || pEvent->nEventX > s_SelectedSheet->pvx0+s_SelectedSheet->srcWidth)
        {
	        return FALSE;
	    }
        if(pEvent->nEventY < s_SelectedSheet->pvy0 || pEvent->nEventY > s_SelectedSheet->pvy0+s_SelectedSheet->srcHeight)
        {
	        return FALSE;
        }
	}
	else if(s_SelectedSheet->zoom_status == 1 || s_SelectedSheet->zoom_status == 2)
	{
		int tmpx = s_SelectedSheet->srcWidth*(s_SelectedSheet->current_scale -1);
		int tmpy = s_SelectedSheet->srcHeight*(s_SelectedSheet->current_scale -1);
		int zoomed_x0 = pEvent->nEventX -tmpx;
		int zoomed_x1 = pEvent->nEventX+s_SelectedSheet->srcWidth+tmpx;
		int zoomed_y0 = pEvent->nEventY -tmpy;
	    int zoomed_y1 = pEvent->nEventY+s_SelectedSheet->srcHeight+tmpy;
	    if(pEvent->nEventX < zoomed_x0 || pEvent->nEventX > zoomed_x1)
		{
            return FALSE;
		}
		if(pEvent->nEventY < zoomed_y0 || pEvent->nEventY > zoomed_y1)
		{
		    return FALSE;
		}

	}
	if(s_SelectedSheet->current_scale>1)
    {
	    s_SelectedSheet->current_status = 1;
	}

	u64 nNowTime    = global_timer_get_counter();
	u64 delayMicros = MOVE_CYC*1000/ZOOM_COUNT;
	for(int i = 0 ; i < ZOOM_COUNT ; i++)
	{
	    enqueueMessage(0,nNowTime + i*delayMicros, dtouchtimer_pMessageQueue);
	}


    return FALSE;
}

static boolean doubleTapListener_onDoubleTapEvent(TMotionEvent * pEvent)
{
	DEBUG( GESTURE_ON, "doubleTapListener_onDoubleTapEvent(%u,%u)\r\n",pEvent->nEventX,pEvent->nEventY);
	if(s_SelectedSheet  == 0)
	{
		return FALSE;
	}
	if(s_SelectedSheet->current_status == 0)
	{
	    s_SelectedSheet->x_position = pEvent->nEventX;
	    s_SelectedSheet->y_position = pEvent->nEventY;
	}
    return FALSE;
}

static boolean scaleGestureListener_onScale(TScaleGestureDetector * pScaleGestureDetector)
{
    UNUSED(pScaleGestureDetector);

    float scaleFactor = ScaleGestureDetectorGetScaleFactor();
    float focusX = ScaleGestureDetectorGetFocusX();
    float focusY = ScaleGestureDetectorGetFocusY();
    
    DEBUG( GESTURE_ON, "scaleGestureListener_onScale(%u,%u)\r\n",focusX,focusY);
    if (scaleFactor != 1)
    {    
    	scaleFactor*=s_SelectedSheet->current_scale;
		pic_opt_change_size_by_scale(scaleFactor);    	
    }
    return TRUE;
}

static boolean scaleGestureListener_onScaleBegin(TScaleGestureDetector * pScaleGestureDetector)
{
    UNUSED(pScaleGestureDetector);

    float focusX = ScaleGestureDetectorGetFocusX();
    float focusY = ScaleGestureDetectorGetFocusY();
	DEBUG( GESTURE_ON, "scaleGestureListener_onScaleBegin(%u,%u)\r\n",focusX,focusY);
    return TRUE;
}

static void scaleGestureListener_onScaleEnd(TScaleGestureDetector * pScaleGestureDetector)
{
    UNUSED(pScaleGestureDetector);

	float scaleFactor = ScaleGestureDetectorGetScaleFactor();
    float focusX = ScaleGestureDetectorGetFocusX();
    float focusY = ScaleGestureDetectorGetFocusY();
	DEBUG( GESTURE_ON, "scaleGestureListener_onScaleEnd(%u,%u)\r\n",focusX,focusY);
	s_SelectedSheet->current_scale *= scaleFactor;
}

TGestureTask *new_GestureTask(unsigned nTaskID, unsigned nPriority)
{
	TGestureTask *pThis = (TGestureTask*)malloc(sizeof(TGestureTask));
    if(pThis == NULL)
        return NULL;
	pThis->m_baseObj = new_Task(TASK_STACK_SIZE, MAIN_TASK_LEVEL);
	pThis->m_baseObj->m_derivedObj = pThis;
	pThis->m_baseObj->delete = delete_GestureTask;
	pThis->m_baseObj->Run = GestureTaskRun;
	pThis->m_baseObj->mPriority = nPriority;
	pThis->m_nTaskID = nTaskID;

	DEBUG( GESTURE_ON, "GestureTask: baseObj = %08x\r\n", pThis->m_baseObj);

    TOnDoubleTapListener * pOnDoubleTapListener = (TOnDoubleTapListener*)malloc(sizeof(TOnDoubleTapListener));
    if(pOnDoubleTapListener == NULL)
    {
		DEBUG( GESTURE_ON, "new_GestureTask pOnDoubleTapListener == NULL");
        free(pThis);
        return NULL;
    }
    pOnDoubleTapListener->onSingleTapConfirmed = doubleTapListener_onSingleTapConfirmed;
    pOnDoubleTapListener->onDoubleTap = doubleTapListener_onDoubleTap;
    pOnDoubleTapListener->onDoubleTapEvent = doubleTapListener_onDoubleTapEvent;
    pThis->m_onDoubleTapListener = pOnDoubleTapListener;

    TOnScaleGestureListener * pOnScaleGestureListener = (TOnScaleGestureListener*)malloc(sizeof(TOnScaleGestureListener));
    if(pOnScaleGestureListener == NULL)
    {
		DEBUG( GESTURE_ON, "new_GestureTask pOnScaleGestureListener == NULL");
        free(pOnDoubleTapListener);
        free(pThis);
        return NULL;
    }
    pOnScaleGestureListener->onScale = scaleGestureListener_onScale;
    pOnScaleGestureListener->onScaleBegin = scaleGestureListener_onScaleBegin;
    pOnScaleGestureListener->onScaleEnd = scaleGestureListener_onScaleEnd;
    pThis->m_onScaleGestureListener = pOnScaleGestureListener;

    TOnGestureListener * pOnGestureListener = (TOnGestureListener*)malloc(sizeof(TOnGestureListener));
    if(pOnGestureListener == NULL)
    {
		DEBUG( GESTURE_ON, "new_GestureTask pOnGestureListener == NULL");
        free(pOnScaleGestureListener);
        free(pOnDoubleTapListener);
        free(pThis);
        return NULL;
    }
    pOnGestureListener->onDown = gestureListener_onDown;
    pOnGestureListener->onShowPress = gestureListener_onShowPress;
    pOnGestureListener->onSingleTapUp = gestureListener_onSingleTapUp;
    pOnGestureListener->onScroll = gestureListener_onScroll;
    pOnGestureListener->onLongPress = gestureListener_onLongPress;
    pOnGestureListener->onFling = gestureListener_onFling;
    pThis->m_onGestureListener = pOnGestureListener;
	pOnGestureListener->onUp = gestureListener_onUp;

    //手势识别消息队列
    g_pMessageQueue = initMessageQueue(pThis->m_baseObj, GestureHandleMessage);

    GestureSetGestureListener(pThis->m_onGestureListener);
    GestureSetDoubleTapListener(pThis->m_onDoubleTapListener);
    GestureSetScaleGestureListener(pThis->m_onScaleGestureListener);
	return pThis;
}

void delete_GestureTask(TTask *pThis)
{
	TGestureTask * pGestureTask = (TGestureTask *)pThis->m_derivedObj;
	delete_Task(pThis);
    free(pGestureTask->m_onScaleGestureListener);
    free(pGestureTask->m_onDoubleTapListener);
    free(pGestureTask->m_onGestureListener);
	free(pGestureTask);
}

void GestureTaskRun(TTask *pThis)
{
    UNUSED(pThis);

	TTask *pTask = 0;
	//监视键盘输入
    while (1)
    {
        if (hasMessage(g_pMessageQueue))
        {
            DEBUG( GESTURE_ON, "gestureTask : hasMessage 222222222\r\n");
            handleTimeoutMessage(global_timer_get_counter(), g_pMessageQueue);
        }
		else
		{
			//鼠标键盘无输入情况下，将任务block，等待向fifo中写入数据时将任务设置为ready状态
            DEBUG( GESTURE_ON, " --->in GestureTaskRun\r\n");
			SchedulerBlockTask(&pTask);
		}
	}
}
