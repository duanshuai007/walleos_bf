#ifndef _ENV_USPIENV_INPUT_GESTURE_DETECTOR_H
#define _ENV_USPIENV_INPUT_GESTURE_DETECTOR_H

#include <types.h>
#include "touchscreen.h"
#include "scalegesturedetector.h"
//时间单位10ms
enum touch_screen_parmter{
    TOUCH_SLOP              = 8,    //像素大小
    DOUBLE_TAP_SLOP         = 100,  //像素大小

    DOUBLE_TAP_TIMEOUT      = 300000,  //ms
    DOUBLE_TAP_MIN_TIME     = 40000,   //ms
    TAP_TIMEOUT             = 100000,  //ms
    LONGPRESS_TIMEOUT       = 500000,  //ms
    MINIMUM_FLING_VELOCITY  = 50,
};

//#define DOUBLE_TAP_TIMEOUT 300000
//#define DOUBLE_TAP_MIN_TIME 40000
//#define TAP_TIMEOUT 100000
//#define LONGPRESS_TIMEOUT 500000
typedef enum TGestureMessageType
{
    GESTURE_MESSAGE_TYPE_SHOW_PRESS,
    GESTURE_MESSAGE_TYPE_LONG_PRESS,
    GESTURE_MESSAGE_TYPE_TAP,
    GESTURE_MESSAGE_TYPE_MAX_NUMBER
}TGestureMessageType;

typedef enum TGestureActionType
{
    GESTURE_ACTION_TYPE_ACTION_POINTER_DOWN,
    GESTURE_ACTION_TYPE_ACTION_POINTER_MOVE,
    GESTURE_ACTION_TYPE_ACTION_POINTER_UP,
    GESTURE_ACTION_TYPE_ACTION_DOWN,
    GESTURE_ACTION_TYPE_ACTION_MOVE,
    GESTURE_ACTION_TYPE_ACTION_UP,
    GESTURE_ACTION_TYPE_ACTION_CANCEL,
}TGestureActionType;

typedef struct TMotionEvent
{
    u64 nEventTime;
    int nEventX;
    int nEventY;
}TMotionEvent;

typedef boolean TGestureDetectorHandler (TMotionEvent * pEvent);
typedef void TGestureDetectorHandler1 (TMotionEvent * pEvent);
typedef boolean TGestureDetectorHandler2 (TMotionEvent * e1, TMotionEvent * e2,
        float distanceX, float distanceY);


typedef struct TOnGestureListener
{
    TGestureDetectorHandler * onDown;
	TGestureDetectorHandler * onUp;//add by lizg 鼠标抬起事件追加
    TGestureDetectorHandler1 * onShowPress;
    TGestureDetectorHandler * onSingleTapUp;
    TGestureDetectorHandler2 * onScroll;
    TGestureDetectorHandler1 * onLongPress;
    TGestureDetectorHandler2 * onFling;
}TOnGestureListener;

typedef struct TOnDoubleTapListener
{
    TGestureDetectorHandler * onSingleTapConfirmed;
    TGestureDetectorHandler * onDoubleTap;
    TGestureDetectorHandler * onDoubleTapEvent;
}TOnDoubleTapListener;

typedef struct TGestureDetector
{
    int mTouchSlopSquare;
    int mDoubleTapSlopSquare;
    int mDoubleTapTouchSlopSquare;
    int mMinimumFlingVelocity;

    TMotionEvent mCurrentDownEvent;
    TMotionEvent mPreviousUpEvent;

    boolean mAlwaysInTapRegion;
    boolean mAlwaysInBiggerTapRegion;
    boolean mStillDown;
    boolean mInLongPress;
    boolean mDeferConfirmSingleTap;
    boolean mIsLongpressEnabled;
    boolean mIsDoubleTapping;
    boolean mInContextClick;
    boolean mIgnoreNextUpEvent;
    boolean mScrollingFlg;

    unsigned mDownTouchID;

    float mLastFocusX;
    float mLastFocusY;
    float mDownFocusX;
    float mDownFocusY;

    int mFingerStatus; //10个手指的压下状态

    //VelocityTracker
	unsigned m_nDownPosX[TOUCH_SCREEN_MAX_POINTS];
	unsigned m_nDownPosY[TOUCH_SCREEN_MAX_POINTS];
	u64 m_nDownTime[TOUCH_SCREEN_MAX_POINTS];

    TOnGestureListener *mListener;
    TOnDoubleTapListener *mDoubleTapListener;
    TOnScaleGestureListener *mScaleGestureListener;

    boolean mInScale;
    TScaleGestureDetector mScaleGestureDetector;
}TGestureDetector;

void CGestureDetector (TGestureDetector *pThis);
void _CGestureDetector (TGestureDetector *pThis);
TGestureDetector *GestureDetectorGet (void);
boolean GestureDetectorInitialize (TGestureDetector *pThis);

void GestureSetGestureListener(TOnGestureListener * gestureListener);
void GestureSetDoubleTapListener(TOnDoubleTapListener* doubleTapListener);
void GestureSetScaleGestureListener(TOnScaleGestureListener* scaleGestureListener);
void GestureHandleMessage(int msgType);

void TouchScreenActionHandler(TTouchScreenAction* pMotionEvent);

#endif
