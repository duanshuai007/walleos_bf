//
// touchscreen.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2016  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _ENV_USPIENV_INPUT_TOUCHSCREEN_H_
#define _ENV_USPIENV_INPUT_TOUCHSCREEN_H_

#include <types.h>

typedef enum TTouchScreenEvent
{
	TouchScreenEventFingerDown,
	TouchScreenEventFingerUp,
	TouchScreenEventFingerMove,	
	TouchScreenEventFingerNoMove,
	TouchScreenEventUnknown
}TTouchScreenEvent;

typedef struct TTouchScreenAction
{
    unsigned nPosX;
    unsigned nPosY;
    TTouchScreenEvent nEvent;
}TTouchScreenAction;

typedef void TTouchScreenEventHandler (unsigned Event, unsigned nPointerId,
				       unsigned nClockTicks, unsigned nPosX, unsigned nPosY,
                       TTouchScreenAction* pMotionEvent);

typedef void TTouchScreenActionHandler(struct TTouchScreenAction* pMotionEvent);

enum touch_screen{
    TOUCH_SCREEN_MAX_POINTS     = 5,
    TOUCH_SCREEN_MAX_ID         = 5-1,
};

typedef struct TFT5406Buffer
{
	u8	DeviceMode;
	u8	GestureID;
	u8	NumPoints;

	struct
	{
		u8	xh;
		u8	xl;
		u8	yh;
		u8	yl;
		u8	Reserved[2];
	}Point[TOUCH_SCREEN_MAX_POINTS];
}TFT5406Buffer;


typedef struct TouchScreenDevice{

	TFT5406Buffer *m_pFT5406Buffer;

    TTouchScreenActionHandler * m_pActionHandler;

	unsigned m_nKnownIDs;

	unsigned m_nPosX[TOUCH_SCREEN_MAX_POINTS];
	unsigned m_nPosY[TOUCH_SCREEN_MAX_POINTS];	

}TouchScreenDevice;

void CTouchScreenDevice (TouchScreenDevice *pThis);
void _CTouchScreenDevice (TouchScreenDevice *pThis);

boolean TouchScreenInitialize (TouchScreenDevice *pThis);

boolean TouchScreenUpdate (TouchScreenDevice *pThis);		// call this about 60 times per second
void TouchUpdateMotionEvent (TouchScreenDevice *pThis);

void TouchScreenRegisterActionHandler (TouchScreenDevice *pThis, TTouchScreenActionHandler *pActionHandler);

TouchScreenDevice *TouchScreenDeviceGet (void);
#endif
