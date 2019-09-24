#include <input/scalegesturedetector.h>
#include <input/gesturedetector.h>
#include <assert.h>
#include <config.h>

static TScaleGestureDetector *s_pThis = 0;

float InvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x; 
    i = 0x5f375a86 - (i>>1); 
    x = *(float*)&i;
    x = x*(1.5f-xhalf*x*x); 
    x = x*(1.5f-xhalf*x*x); 
    x = x*(1.5f-xhalf*x*x);

    return 1/x;
}

static float float_abs(float number)
{
    return (number>= 0 ? number : -number);
}

void ScaleGestureDetectorOnTouchEvent(unsigned Event, u64 nClockTicks, unsigned nPosX,
        unsigned nPosY, TTouchScreenAction* pMotionEvent)
{
    UNUSED(nPosX);
    UNUSED(nPosY);

    s_pThis->mCurrTime = nClockTicks;
    const boolean streamComplete = Event == GESTURE_ACTION_TYPE_ACTION_UP
                || Event == GESTURE_ACTION_TYPE_ACTION_CANCEL;
    if (Event == GESTURE_ACTION_TYPE_ACTION_DOWN || streamComplete)
    {
        if (s_pThis->mInProgress)
        {
            s_pThis->mListener->onScaleEnd(s_pThis);
            s_pThis->mInProgress = FALSE;
            s_pThis->mInitialSpan = 0;
        }

        if (streamComplete)
        {
            return ;
        }
    }

    const boolean configChanged = Event == GESTURE_ACTION_TYPE_ACTION_DOWN||
                Event== GESTURE_ACTION_TYPE_ACTION_POINTER_UP ||
                Event == GESTURE_ACTION_TYPE_ACTION_POINTER_DOWN;

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

        sumX += pAction->nPosX;
        sumY += pAction->nPosY;
        div ++; 
    }   
    const float focusX = sumX / div;
    const float focusY = sumY / div; 

    // Determine average deviation from focal point
    float devSumX = 0;
    float devSumY = 0;
    for (unsigned i = 0; i < TOUCH_SCREEN_MAX_POINTS; i ++) 
    {   
        TTouchScreenAction * pAction = pMotionEvent + i;
        if (pAction->nEvent == TouchScreenEventUnknown
                || pAction->nEvent == TouchScreenEventFingerUp)
        {   
            continue;
        }   

        devSumX += float_abs(pAction->nPosX - focusX);
        devSumY += float_abs(pAction->nPosY - focusY);
    }

    const float devX = devSumX / div;
    const float devY = devSumY / div;

    // Span is the average distance between touch points through the focal point;
    // i.e. the diameter of the circle with a radius of the average deviation from
    // the focal point.
    const float spanX = devX * 2;
    const float spanY = devY * 2;
    const float span = InvSqrt(spanX * spanX + spanY * spanY);//两个点的直线距离

    // Dispatch begin/end events as needed.
    // If the configuration changes, notify the app to reset its current state by beginning
    // a fresh scale event stream.
    const boolean wasInProgress = s_pThis->mInProgress;
    s_pThis->mFocusX = focusX;
    s_pThis->mFocusY = focusY;
    if (s_pThis->mInProgress && (span < s_pThis->mMinSpan || configChanged))
    {
        s_pThis->mListener->onScaleEnd(s_pThis);
        s_pThis->mInProgress = FALSE;
        s_pThis->mInitialSpan = span;
    }
    if (configChanged)
    {
        s_pThis->mPrevSpanX = s_pThis->mCurrSpanX = spanX;
        s_pThis->mPrevSpanY = s_pThis->mCurrSpanY = spanY;
        s_pThis->mInitialSpan = s_pThis->mPrevSpan = s_pThis->mCurrSpan = span;
    }
    if (!s_pThis->mInProgress && span >= s_pThis->mMinSpan &&
            (wasInProgress || float_abs(span - s_pThis->mInitialSpan) > s_pThis->mSpanSlop))
    {
        s_pThis->mPrevSpanX = s_pThis->mCurrSpanX = spanX;
        s_pThis->mPrevSpanY = s_pThis->mCurrSpanY = spanY;
        s_pThis->mPrevSpan = s_pThis->mCurrSpan = span;
        s_pThis->mPrevTime = s_pThis->mCurrTime;
        s_pThis->mInProgress = s_pThis->mListener->onScaleBegin(s_pThis);
    }

    // Handle motion; focal point and span/scale factor are changing.
    if (Event == GESTURE_ACTION_TYPE_ACTION_POINTER_MOVE) 
    {
        s_pThis->mCurrSpanX = spanX;
        s_pThis->mCurrSpanY = spanY;
        s_pThis->mCurrSpan = span;

        boolean updatePrev = TRUE;
        if (s_pThis->mInProgress)
        {
            updatePrev = s_pThis->mListener->onScale(s_pThis);
        }

        if (updatePrev)
        {
            s_pThis->mPrevSpanX = s_pThis->mCurrSpanX;
            s_pThis->mPrevSpanY = s_pThis->mCurrSpanY;
            s_pThis->mPrevSpan = s_pThis->mCurrSpan;
            s_pThis->mPrevTime = s_pThis->mCurrTime;
        }
    }
}

void CScaleGestureDetector (TScaleGestureDetector *pThis)
{
    assert(pThis != 0);
    assert (s_pThis == 0);
    s_pThis = pThis;
}

boolean ScaleGestureDetectorInitialize (TScaleGestureDetector *pThis)
{
	assert (pThis != 0);
    
    pThis->mSpanSlop = TOUCH_SLOP * 2;
    pThis->mMinSpan = 140;

	return TRUE;
}

void _CScaleGestureDetector (TScaleGestureDetector *pThis)
{
    assert(pThis != 0);
}

float ScaleGestureDetectorGetScaleFactor()
{
    return s_pThis->mPrevSpan > 0 ? s_pThis->mCurrSpan / s_pThis->mPrevSpan : 1;
}

float ScaleGestureDetectorGetFocusX()
{
    return s_pThis->mFocusX;
}

float ScaleGestureDetectorGetFocusY()
{
    return s_pThis->mFocusY;
}

boolean ScaleGestureIsInProgress()
{
    return s_pThis->mInProgress;
}
