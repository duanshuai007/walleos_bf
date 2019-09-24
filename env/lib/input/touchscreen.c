//
// touchscreen.cpp
//
// This is based on the the Raspbian Linux driver:
//
//   drivers/input/touchscreen/rpi-ft5406.c
//
//   Driver for memory based ft5406 touchscreen
//
//   Copyright (C) 2015 Raspberry Pi
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
#include <input/touchscreen.h>
#include <string.h>
#include <assert.h>
#include <alloc.h>
#include <stdlib.h>
#include <errno.h>
#include <synchronize.h>
#include <input/gesturedetector.h>
#include <input/motioneventfifo.h>
#include <config.h>

static const char FromFT5406[] = "ft5406";

static TouchScreenDevice *s_pThis = 0;

void CTouchScreenDevice (TouchScreenDevice *pThis)
{
	assert(pThis != 0);
	pThis->m_pFT5406Buffer = 0;
	pThis->m_nKnownIDs = 0;

	assert (s_pThis == 0);
	s_pThis = pThis;
}
void _CTouchScreenDevice (TouchScreenDevice *pThis)
{
	assert(pThis != 0);

	pThis->m_pFT5406Buffer = 0;
}


TouchScreenDevice *TouchScreenDeviceGet (void)
{
	assert (s_pThis != 0);
	//printf("TouchScreenDeviceGet: %08x\r\n", s_pThis);
    return s_pThis;
}

boolean TouchScreenInitialize (TouchScreenDevice *pThis)
{
    UNUSED(pThis);

    s_pThis->m_pFT5406Buffer  = (TFT5406Buffer *) malloc(sizeof(TFT5406Buffer));
    if(s_pThis->m_pFT5406Buffer == NULL)
        return ENOMEM;
	s_pThis->m_pFT5406Buffer->NumPoints = 99;

    initMotionEventFifo();

    return TRUE;
}

void TouchUpdateMotionEvent (TouchScreenDevice *pThis)
{ 

	TFT5406Buffer Regs;
	assert (pThis->m_pFT5406Buffer != 0);
	memcpy (&Regs, pThis->m_pFT5406Buffer, sizeof (TFT5406Buffer));
	*(volatile u8 *) &(pThis->m_pFT5406Buffer->NumPoints) = 99;

    //return;
	// Do not output if theres no new information (NumPoints is 99)
	// or we have no touch points and don't need to release any
    //printf("numpoints = %d, id = %d\r\n", Regs.NumPoints , pThis->m_nKnownIDs);
    if (   Regs.NumPoints == 99
	    || (   Regs.NumPoints == 0
		&& pThis->m_nKnownIDs == 0))
	{
		return;
	}

	unsigned nModifiedIDs = 0;
	assert (Regs.NumPoints <= TOUCH_SCREEN_MAX_POINTS);

    TTouchScreenAction * pMotionEvent = (TTouchScreenAction*)malloc(sizeof(TTouchScreenAction) * TOUCH_SCREEN_MAX_POINTS);
    for (unsigned i = 0; i < TOUCH_SCREEN_MAX_POINTS; i ++)
    {
        pMotionEvent[i].nEvent = TouchScreenEventUnknown;
    }

    int addFlg = 0;
	for (unsigned i = 0; i < Regs.NumPoints; i++)
	{
		unsigned x = (((unsigned) Regs.Point[i].xh & 0xF) << 8) | Regs.Point[i].xl;
		unsigned y = (((unsigned) Regs.Point[i].yh & 0xF) << 8) | Regs.Point[i].yl;
		
	#ifdef SCREEN_HORIZONTAL
		unsigned temp = x;
		x = y;
		y = CFG_DISP_PRI_RESOL_HEIGHT - temp;
	#endif

		//unsigned touchEvent = (Regs.Point[i].xh >> 6) & 0x3;
		unsigned nTouchID = (Regs.Point[i].yh >> 4) & 0xF;
		if (nTouchID >= TOUCH_SCREEN_MAX_POINTS)
		{
			return;
		}
		nModifiedIDs |= 1 << nTouchID;
		if (!((1 << nTouchID) & pThis->m_nKnownIDs))
		{
			pThis->m_nPosX[nTouchID] = x;
			pThis->m_nPosY[nTouchID] = y;
            pMotionEvent[nTouchID].nPosX = x;
            pMotionEvent[nTouchID].nPosY = y;
            pMotionEvent[nTouchID].nEvent = TouchScreenEventFingerDown;

            addFlg = 1;
		}
		else
		{
            const int deltaX = (int) (pThis->m_nPosX[nTouchID] - x);
            const int deltaY = (int) (pThis->m_nPosY[nTouchID] - y);
            int distance = (deltaX * deltaX) + (deltaY * deltaY);

			if ( distance > 64)
			{
				pThis->m_nPosX[nTouchID] = x;
				pThis->m_nPosY[nTouchID] = y;
                pMotionEvent[nTouchID].nPosX = x;
                pMotionEvent[nTouchID].nPosY = y;
                pMotionEvent[nTouchID].nEvent = TouchScreenEventFingerMove;

                addFlg = 1;
			}
			else
			{
				pThis->m_nPosX[nTouchID] = x;
				pThis->m_nPosY[nTouchID] = y;
                pMotionEvent[nTouchID].nPosX = x;
                pMotionEvent[nTouchID].nPosY = y;
                pMotionEvent[nTouchID].nEvent = TouchScreenEventFingerNoMove;
			}
		}
	}

	unsigned nReleasedIDs = pThis->m_nKnownIDs & ~nModifiedIDs;
	for (unsigned i = 0; nReleasedIDs != 0 && i < TOUCH_SCREEN_MAX_POINTS; i++)
	{
		if (nReleasedIDs & (1 << i))
		{
            pMotionEvent[i].nPosX = pThis->m_nPosX[i];
            pMotionEvent[i].nPosY = pThis->m_nPosY[i];
            pMotionEvent[i].nEvent = TouchScreenEventFingerUp;
			nModifiedIDs &= ~(1 << i);

			addFlg = 1;
		}
	}

	pThis->m_nKnownIDs = nModifiedIDs;

	if (addFlg == 1)
	{
	    putMotionEventFifo((u8 *)pMotionEvent);
	}


    return;
}		// call this about 60 times per second

boolean TouchScreenUpdate (TouchScreenDevice *pThis)
{

	if(getMotionEventStatus() == 0)
	{
		return FALSE;
	}
    EnterCritical();

	if (pThis->m_pActionHandler != 0)
    {

    	TTouchScreenAction * action = (TTouchScreenAction *)getMotionEventFifo();


        (*(pThis->m_pActionHandler)) (action);

        free(action);	
    }


    LeaveCritical();

    return TRUE;
}

void TouchScreenRegisterActionHandler (TouchScreenDevice *pThis, TTouchScreenActionHandler *pActionHandler)
{
	assert(pThis != 0);
	assert (pThis->m_pActionHandler == 0);
    //printf("TouchScreenRegisterActionHandler: pThis = %08x\r\n", pThis);
    //printf(("pActionHandler = %08x\r\n", pActionHandler);

    pThis->m_pActionHandler = pActionHandler;
	assert (pThis->m_pActionHandler != 0);
}
