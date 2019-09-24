#ifndef _ENV_USPIENV_INPUT_SCALE_GESTURE_DETECTOR_H
#define _ENV_USPIENV_INPUT_SCALE_GESTURE_DETECTOR_H

#include <types.h>
#include "touchscreen.h"

typedef struct TScaleGestureDetector
{
    boolean mInProgress;
    u64 mCurrTime;
    struct TOnScaleGestureListener * mListener;
    float mInitialSpan;

    float mFocusX;
    float mFocusY;
    int mMinSpan;
    float mPrevSpanX;
    float mPrevSpanY;
    float mCurrSpanX;
    float mCurrSpanY;

    float mCurrSpan;
    float mPrevSpan;

    int mSpanSlop;
    u64 mPrevTime;

}TScaleGestureDetector;

typedef boolean TScaleGestureDetectorHandler (TScaleGestureDetector * pScaleGestureDetector);
typedef void TScaleGestureDetectorHandler1 (TScaleGestureDetector * pScaleGestureDetector);

typedef struct TOnScaleGestureListener
{
    /*
    * 返回值代表本次缩放事件是否已被处理。如果已被处理，那么detector就会重置缩放事件；
    * 如果未被处理，detector会继续进行计算，修改getScaleFactor()的返回值，直到被处理为止。
    * 因此，它常用在判断只有缩放值达到一定数值时才进行缩放。
    */
    TScaleGestureDetectorHandler * onScale;

    /*
    *  缩放开始。该detector是否处理后继的缩放事件。返回false时，不会执行onScale()
    */
    TScaleGestureDetectorHandler * onScaleBegin;

    /*
    * 缩放结束
    */
    TScaleGestureDetectorHandler1 * onScaleEnd;
}TOnScaleGestureListener;

void CScaleGestureDetector (TScaleGestureDetector *pThis);
void _CScaleGestureDetector (TScaleGestureDetector *pThis);
boolean ScaleGestureDetectorInitialize (TScaleGestureDetector *pThis);
float ScaleGestureDetectorGetFocusX(void);
float ScaleGestureDetectorGetFocusY(void);
void ScaleGestureDetectorOnTouchEvent(unsigned Event, u64 nClockTicks, unsigned nPosX,
        unsigned nPosY, TTouchScreenAction* pMotionEvent);
float ScaleGestureDetectorGetScaleFactor(void);
boolean ScaleGestureIsInProgress(void);
#endif
