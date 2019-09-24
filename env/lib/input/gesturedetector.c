#include <input/gesturedetector.h>
#include <input/touchscreen.h>
#include <uspienv.h>
#include <uspios.h>
#include <assert.h>
#include <string.h>
#include <input/messagequeue.h>
#include <s5p4418_serial_stdio.h>
#include <stdio.h>
#include <config.h>
#include <stdio.h>
#include <global_timer.h>

static TGestureDetector *s_pThis = 0;

static void RemoveMessage(TGestureMessageType msgType)
{
    assert (msgType < GESTURE_MESSAGE_TYPE_MAX_NUMBER);
    removeMessages(msgType, g_pMessageQueue);
}

static void sendMessageDelayed(TGestureMessageType msgType, u64 delayMicros)
{
    assert (msgType < GESTURE_MESSAGE_TYPE_MAX_NUMBER);
    u64 nNowTime = global_timer_get_counter();

    enqueueMessage(msgType, nNowTime + delayMicros, g_pMessageQueue);
}

static void sendMessageAtTime(TGestureMessageType msgType, u64 uptimeMicros)
{
    assert (msgType < GESTURE_MESSAGE_TYPE_MAX_NUMBER);
    enqueueMessage(msgType, uptimeMicros, g_pMessageQueue);
}

static boolean IsValidEvent(TMotionEvent * pEvent)
{
    if (pEvent->nEventTime > 0)
    {
        return TRUE;
    }

    return FALSE;
}

static void cancelTaps(void)
{
    RemoveMessage(GESTURE_MESSAGE_TYPE_SHOW_PRESS);
    RemoveMessage(GESTURE_MESSAGE_TYPE_LONG_PRESS);
    RemoveMessage(GESTURE_MESSAGE_TYPE_TAP);
    s_pThis->mIsDoubleTapping = FALSE;
    s_pThis->mAlwaysInTapRegion = FALSE;
    s_pThis->mAlwaysInBiggerTapRegion = FALSE;
    s_pThis->mDeferConfirmSingleTap = FALSE;
    s_pThis->mInLongPress = FALSE;
    s_pThis->mInContextClick = FALSE;
    s_pThis->mIgnoreNextUpEvent = FALSE;
}

static boolean isConsideredDoubleTap(TMotionEvent * pFirstDown, TMotionEvent * pFirstUp,
            TMotionEvent * pSecondDown) 
{
    if (!s_pThis->mAlwaysInBiggerTapRegion) 
    {
        return FALSE;
    }

    u64 deltaTime = pSecondDown->nEventTime - pFirstUp->nEventTime;
    if (deltaTime > DOUBLE_TAP_TIMEOUT || deltaTime < DOUBLE_TAP_MIN_TIME) 
    {
        return FALSE;
    }

    int deltaX = pFirstDown->nEventX - pSecondDown->nEventX;
    int deltaY = pFirstDown->nEventY - pSecondDown->nEventY;
    return (deltaX * deltaX + deltaY * deltaY < s_pThis->mDoubleTapSlopSquare);
}

static void copyMotionEvent(TMotionEvent *pDstMotionEvent, TMotionEvent *pSrcMotionEvent)
{
    memcpy(pDstMotionEvent, pSrcMotionEvent, sizeof(TMotionEvent));
}

static void getVelocity(unsigned nPointerId, u64 nClockTicks, unsigned nPosX,
        unsigned nPosY, float * pVelocityX, float * pVelocityY)
{
    if (0 == s_pThis->m_nDownTime[nPointerId])
    {
        *pVelocityX = 0.0f;
        *pVelocityY = 0.0f;
        return;
    }

    u64 nDiffTime = nClockTicks - s_pThis->m_nDownTime[nPointerId];

    float nDiffPosX = (float)nPosX - (float)s_pThis->m_nDownPosX[nPointerId];
    float nDiffPosY = (float)nPosY - (float)s_pThis->m_nDownPosY[nPointerId];

    *pVelocityX = nDiffPosX * 1000000 / nDiffTime;
    *pVelocityY = nDiffPosY * 1000000 / nDiffTime;
}

static void TouchScreenEventHandler(unsigned Event, unsigned nPointerId,
        u64 nClockTicks, unsigned nPosX, unsigned nPosY,
        float focusX, float focusY, TTouchScreenAction* pMotionEvent)
{
    unsigned int i;
    TMotionEvent ev;
    ev.nEventTime = nClockTicks;
    ev.nEventX = nPosX;
    ev.nEventY = nPosY;

    ScaleGestureDetectorOnTouchEvent(Event, nClockTicks, nPosX, nPosY, pMotionEvent);
    if (ScaleGestureIsInProgress())
    {
        return ;
    }
 
    switch (Event)
    {
        case GESTURE_ACTION_TYPE_ACTION_POINTER_DOWN:
            s_pThis->mDownFocusX = s_pThis->mLastFocusX = focusX;
            s_pThis->mDownFocusY = s_pThis->mLastFocusY = focusY;
            cancelTaps();
            break;

        case GESTURE_ACTION_TYPE_ACTION_POINTER_UP:
            s_pThis->mDownFocusX = s_pThis->mLastFocusX = focusX;
            s_pThis->mDownFocusY = s_pThis->mLastFocusY = focusY;
            // Check the dot product of current velocities.
            // If the pointer that left was opposing another velocity vector, clear.
            float x1 = 0;
            float y1 = 0;
            getVelocity(nPointerId, nClockTicks, nPosX, nPosY, &x1, &y1);
            for (i = 0; i < TOUCH_SCREEN_MAX_POINTS; i++)
            {
                TTouchScreenAction * pAction = pMotionEvent + i;
                if (i == nPointerId || pAction->nEvent == TouchScreenEventUnknown)
                {
                    continue;
                }

                float x = 0;
                float y = 0;
                getVelocity(i, nClockTicks, pAction->nPosX, pAction->nPosY, &x, &y);
                x = x1 * x;
                y = y1 * y;

                const float dot = x + y;
                if (dot < 0)
                {
                    //clear velocity tracker
                    memset(s_pThis->m_nDownTime, 0, sizeof(s_pThis->m_nDownTime));
                    break;
                }
            }
            break;

        case GESTURE_ACTION_TYPE_ACTION_DOWN:
            //printf_nolock("TouchScreenEventFingerDown\r\n");
            if (0 != s_pThis->mDoubleTapListener) //监测双击事件
            {
                int hadTapMessage = hasSpecificMessage(GESTURE_MESSAGE_TYPE_TAP, g_pMessageQueue);
                if (hadTapMessage)
                {
                    RemoveMessage(GESTURE_MESSAGE_TYPE_TAP);
                }
                DEBUG( TOUCH_ON, "GESTURE_ACTION_TYPE_ACTION_DOWN: hadTapMessage = %d\r\n", hadTapMessage);
                if (IsValidEvent(&s_pThis->mCurrentDownEvent)
                        && IsValidEvent(&s_pThis->mPreviousUpEvent)
                        && hadTapMessage
                        && isConsideredDoubleTap(&s_pThis->mCurrentDownEvent, 
                            &s_pThis->mPreviousUpEvent, &ev))
                {
                    // This is a second tap
                    s_pThis->mIsDoubleTapping = TRUE;
                    // Give a callback with the first tap of the double-tap
                    s_pThis->mDoubleTapListener->onDoubleTap(&s_pThis->mCurrentDownEvent);
                    // Give a callback with down event of the double-tap
                    s_pThis->mDoubleTapListener->onDoubleTapEvent(&ev);
                }
                else
                {
                    sendMessageDelayed(GESTURE_MESSAGE_TYPE_TAP, DOUBLE_TAP_TIMEOUT);
                }   
            }  
            s_pThis->mDownFocusX = s_pThis->mLastFocusX = focusX;
            s_pThis->mDownFocusY = s_pThis->mLastFocusY = focusY;

            copyMotionEvent(&s_pThis->mCurrentDownEvent, &ev);

            s_pThis->mAlwaysInTapRegion = TRUE;
            s_pThis->mAlwaysInBiggerTapRegion = TRUE;
            s_pThis->mStillDown = TRUE;
            s_pThis->mInLongPress = FALSE;
            s_pThis->mDeferConfirmSingleTap = FALSE;
 
            if (s_pThis->mIsLongpressEnabled)
            {
                //DEBUG( TOUCH_ON, "GESTURE_ACTION_TYPE_ACTION_DOWN: remove longpress\r\n");
                RemoveMessage(GESTURE_MESSAGE_TYPE_LONG_PRESS);
                sendMessageAtTime(GESTURE_MESSAGE_TYPE_LONG_PRESS, 
                        s_pThis->mCurrentDownEvent.nEventTime + TAP_TIMEOUT + LONGPRESS_TIMEOUT);
            }

            sendMessageAtTime(GESTURE_MESSAGE_TYPE_SHOW_PRESS,
                    s_pThis->mCurrentDownEvent.nEventTime + TAP_TIMEOUT);
            s_pThis->mListener->onDown(&ev);
            break;

        case GESTURE_ACTION_TYPE_ACTION_POINTER_MOVE:    
            s_pThis->mAlwaysInTapRegion = FALSE;
            RemoveMessage(GESTURE_MESSAGE_TYPE_SHOW_PRESS);
            RemoveMessage(GESTURE_MESSAGE_TYPE_LONG_PRESS);
            s_pThis->mInLongPress = FALSE;            
            s_pThis->mScrollingFlg = FALSE;

            float velocityX = 0;
            float velocityY = 0;
            getVelocity(nPointerId, nClockTicks, nPosX, nPosY, &velocityX, &velocityY);
            if (velocityY > s_pThis->mMinimumFlingVelocity
                    || velocityY < -s_pThis->mMinimumFlingVelocity
                    || velocityX > s_pThis->mMinimumFlingVelocity
                    || velocityX < -s_pThis->mMinimumFlingVelocity)
            {
                s_pThis->mListener->onFling(&s_pThis->mCurrentDownEvent, &ev,
                        velocityX, velocityY);
            }            

            break;

        case GESTURE_ACTION_TYPE_ACTION_MOVE:
            if (s_pThis->mInLongPress || s_pThis->mInContextClick)
            {
                break;
            }
            const float scrollX = s_pThis->mLastFocusX - focusX;
            const float scrollY = s_pThis->mLastFocusY - focusY;
            if (s_pThis->mIsDoubleTapping)
            {
                // Give the move events of the double-tap
                s_pThis->mDoubleTapListener->onDoubleTapEvent(&ev);
            }
            else if (s_pThis->mAlwaysInTapRegion)
            {
                //move超范围则不能认定为tap动作
                const int deltaX = (int) (focusX - s_pThis->mDownFocusX);
                const int deltaY = (int) (focusY - s_pThis->mDownFocusY);
                int distance = (deltaX * deltaX) + (deltaY * deltaY);
                if (distance > s_pThis->mTouchSlopSquare)
                {
                    s_pThis->mLastFocusX = focusX;
                    s_pThis->mLastFocusY = focusY;
                    s_pThis->mAlwaysInTapRegion = FALSE;
                    RemoveMessage(GESTURE_MESSAGE_TYPE_SHOW_PRESS);
                    RemoveMessage(GESTURE_MESSAGE_TYPE_LONG_PRESS);
                    s_pThis->mInLongPress = FALSE;

                    s_pThis->mScrollingFlg = TRUE;
                    s_pThis->mListener->onScroll(&s_pThis->mCurrentDownEvent, &ev,
                            scrollX, scrollY);

                }
                if (distance > s_pThis->mDoubleTapTouchSlopSquare)
                {
                    s_pThis->mAlwaysInBiggerTapRegion = FALSE;
                }
            }
            else if (scrollX >= 1 || scrollX <= -1 || scrollY >= 1 || scrollY <= -1)
            {
                s_pThis->mScrollingFlg = TRUE;
                s_pThis->mListener->onScroll(&s_pThis->mCurrentDownEvent, &ev,
                        scrollX, scrollY);
                s_pThis->mLastFocusX = focusX;
                s_pThis->mLastFocusY = focusY;
            }
            break;
        case GESTURE_ACTION_TYPE_ACTION_UP:
            //printf_nolock("TouchScreenEventFingerUp\r\n");
            s_pThis->mStillDown = FALSE;
            if (s_pThis->mIsDoubleTapping)
            {
                // Finally, give the up event of the double-tap
                s_pThis->mDoubleTapListener->onDoubleTapEvent(&ev);
            }
            else if (s_pThis->mInLongPress)
            {
                RemoveMessage(GESTURE_MESSAGE_TYPE_TAP);
                s_pThis->mInLongPress = FALSE;
            }
            else if (s_pThis->mAlwaysInTapRegion && !s_pThis->mIgnoreNextUpEvent)
            {
                s_pThis->mListener->onSingleTapUp(&ev);
                if (s_pThis->mDeferConfirmSingleTap && s_pThis->mDoubleTapListener != 0)
                {
                    s_pThis->mDoubleTapListener->onSingleTapConfirmed(&ev);
                }
            }
            else if (!s_pThis->mIgnoreNextUpEvent)
            {
                // A fling must travel the minimum tap distance
                float velocityX = 0;
                float velocityY = 0;
                getVelocity(nPointerId, nClockTicks, nPosX, nPosY, &velocityX, &velocityY);
                if (velocityY > s_pThis->mMinimumFlingVelocity
                        || velocityY < -s_pThis->mMinimumFlingVelocity
                        || velocityX > s_pThis->mMinimumFlingVelocity
                        || velocityX < -s_pThis->mMinimumFlingVelocity)
                {
                    s_pThis->mListener->onFling(&s_pThis->mCurrentDownEvent, &ev,
                            velocityX, velocityY);
                }
            }
            copyMotionEvent(&s_pThis->mPreviousUpEvent, &ev);

            memset(s_pThis->m_nDownTime, 0, sizeof(s_pThis->m_nDownTime));
            s_pThis->mIsDoubleTapping = FALSE;
            s_pThis->mDeferConfirmSingleTap = FALSE;
            s_pThis->mIgnoreNextUpEvent = FALSE;
            s_pThis->mScrollingFlg = FALSE;

            RemoveMessage(GESTURE_MESSAGE_TYPE_SHOW_PRESS);
            RemoveMessage(GESTURE_MESSAGE_TYPE_LONG_PRESS);
            break;
        default:
            break;
    }
}

void TouchScreenActionHandler(TTouchScreenAction* pMotionEvent)
{
    u64 nClockTicks = global_timer_get_counter();
    DEBUG( TOUCH_ON, "in TouchScreenActionHandler\r\n");

    float sumX = 0;
    float sumY = 0;
    unsigned div = 0;
    for (unsigned i = 0; i < TOUCH_SCREEN_MAX_POINTS; i ++)
    {
        TTouchScreenAction * pAction = pMotionEvent + i;
        if (pAction->nEvent == TouchScreenEventUnknown
                || pAction->nEvent == TouchScreenEventFingerUp)
        {
            continue;
        }

        if (pAction->nEvent == TouchScreenEventFingerDown)
        {
            s_pThis->m_nDownPosX[i] = pAction->nPosX;
            s_pThis->m_nDownPosY[i] = pAction->nPosY;
            s_pThis->m_nDownTime[i] = nClockTicks;
        }

        sumX += pAction->nPosX;
        sumY += pAction->nPosY;
        div ++;
    }
    const float focusX = sumX / div;
    const float focusY = sumY / div;

    for (unsigned i = 0; i < TOUCH_SCREEN_MAX_POINTS; i ++)
    {
        TTouchScreenAction * pAction = pMotionEvent + i;
        if (pAction->nEvent == TouchScreenEventUnknown)
        {
            continue;
        }
    
        switch (pAction->nEvent)
        {
            case TouchScreenEventFingerDown:
                if (0 == s_pThis->mFingerStatus)
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_DOWN, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);
                }
                else
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_POINTER_DOWN, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);
                }
                s_pThis->mFingerStatus |= (1 << i);
                break;
            case TouchScreenEventFingerUp:
                if (s_pThis->mFingerStatus == (1 << i))
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_UP, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);
                }
                else
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_POINTER_UP, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);
                }
                s_pThis->mFingerStatus &= ~(1 << i);
				//add by lizg start 鼠标up事件追加
				TMotionEvent ev;
				ev.nEventTime = nClockTicks;
				s_pThis->mListener->onUp(&ev);
				//add by lizg end 鼠标up事件追加
                break;
            case TouchScreenEventFingerMove:
                if (s_pThis->mFingerStatus == (1 << i))
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_MOVE, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);                    
                }
                else
                {
                    TouchScreenEventHandler(GESTURE_ACTION_TYPE_ACTION_POINTER_MOVE, i,
                            nClockTicks, pAction->nPosX, pAction->nPosY, focusX, focusY,
                            pMotionEvent);                                        
                }
                break;
            default:
                break;
        }
    }
}

static void dispatchLongPress(void)
{
    RemoveMessage(GESTURE_MESSAGE_TYPE_TAP);
    s_pThis->mDeferConfirmSingleTap = FALSE;
    s_pThis->mInLongPress = TRUE;
    s_pThis->mListener->onLongPress(&s_pThis->mCurrentDownEvent);
}

void CGestureDetector (TGestureDetector *pThis)
{
	assert(pThis != 0);
	assert(s_pThis == 0);
	s_pThis = pThis;
    CScaleGestureDetector(&pThis->mScaleGestureDetector);
}

void _CGestureDetector (TGestureDetector *pThis)
{
	assert(pThis != 0);
    _CScaleGestureDetector(&pThis->mScaleGestureDetector);
}


TGestureDetector *GestureDetectorGet (void)
{
	assert (s_pThis != 0);
	return s_pThis;
}

boolean GestureDetectorInitialize (TGestureDetector *pThis)
{
	assert (pThis != 0);

    DEBUG( TOUCH_ON, "GestureDetectorInitialize\r\n");

    pThis->mTouchSlopSquare = TOUCH_SLOP * TOUCH_SLOP;
    pThis->mDoubleTapSlopSquare = DOUBLE_TAP_SLOP * DOUBLE_TAP_SLOP;
    pThis->mDoubleTapTouchSlopSquare = TOUCH_SLOP * TOUCH_SLOP;
    pThis->mMinimumFlingVelocity = MINIMUM_FLING_VELOCITY;
    pThis->mIsLongpressEnabled = TRUE;
    s_pThis->mScrollingFlg = FALSE;

    TouchScreenRegisterActionHandler(TouchScreenDeviceGet(), TouchScreenActionHandler);
    ScaleGestureDetectorInitialize(&pThis->mScaleGestureDetector);

	return TRUE;
}

void GestureSetGestureListener(TOnGestureListener * gestureListener)
{
    s_pThis->mListener = gestureListener;
}

void GestureSetDoubleTapListener(TOnDoubleTapListener* doubleTapListener)
{
    s_pThis->mDoubleTapListener = doubleTapListener;
}

void GestureSetScaleGestureListener(TOnScaleGestureListener* scaleGestureListener)
{
    s_pThis->mScaleGestureDetector.mListener = scaleGestureListener;
}

void GestureHandleMessage(int msgType)
{
    switch (msgType)
    {
        case GESTURE_MESSAGE_TYPE_SHOW_PRESS:
            DEBUG( TOUCH_ON, "GestureHandleMessage: GESTURE_MESSAGE_TYPE_SHOW_PRESS\r\n");
            s_pThis->mListener->onShowPress(&s_pThis->mCurrentDownEvent);
            break;
        case GESTURE_MESSAGE_TYPE_LONG_PRESS:
            dispatchLongPress();
            break;
        case GESTURE_MESSAGE_TYPE_TAP:
            // If the user's finger is still down, do not count it as a tap
            if (s_pThis->mDoubleTapListener != 0)
            {
                if (!s_pThis->mStillDown)
                {
                    s_pThis->mDoubleTapListener->onSingleTapConfirmed(&s_pThis->mCurrentDownEvent);
                }
                else
                {
                    s_pThis->mDeferConfirmSingleTap = TRUE;
                }
            }
            break;
        default:
            break;
    }
}
