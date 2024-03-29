/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2009 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/

#ifndef _ENV_INCLUDE_DRIVER_ETH_CFG_TYPE_H_
#define _ENV_INCLUDE_DRIVER_ETH_CFG_TYPE_H_

#include <s5p4418_gpio.h>

/*------------------------------------------------------------------------------
 * 	Display(DPC/MLC) type
 */
/*	the main screen. */
enum {
	MAIN_SCREEN_PRI = 0,
	MAIN_SCREEN_SEC = 1
};

/*f	the clock source of clock generator. */
enum {
	DPC_VCLK_SRC_PLL0		= 0,
	DPC_VCLK_SRC_PLL1		= 1,
	DPC_VCLK_SRC_PLL2		= 2,
	DPC_VCLK_SRC_SVCLK		= 2,
	DPC_VCLK_SRC_PSVCLK		= 3,
	DPC_VCLK_SRC_nPSVCLK	= 4,
	DPC_VCLK_SRC_XTI		= 5,	// AVCLK
	DPC_VCLK_SRC_nAVCLK		= 6,
	DPC_VCLK_SRC_VCLK2		= 7
};

/*	the data output format. */
enum {
	DPC_FORMAT_RGB555     	=  0UL,	///< RGB555 Format
	DPC_FORMAT_RGB565     	=  1UL,	///< RGB565 Format
	DPC_FORMAT_RGB666     	=  2UL,	///< RGB666 Format
	DPC_FORMAT_RGB888     	=  3UL,	///< RGB888 Format
	DPC_FORMAT_MRGB555A   	=  4UL,	///< MRGB555A Format
	DPC_FORMAT_MRGB555B   	=  5UL,	///< MRGB555B Format
	DPC_FORMAT_MRGB565    	=  6UL,	///< MRGB565 Format
	DPC_FORMAT_MRGB666    	=  7UL,	///< MRGB666 Format
	DPC_FORMAT_MRGB888A   	=  8UL,	///< MRGB888A Format
	DPC_FORMAT_MRGB888B   	=  9UL,	///< MRGB888B Format
	DPC_FORMAT_CCIR656    	= 10UL,	///< ITU-R BT.656 / 601(8-bit)
	DPC_FORMAT_CCIR601A		= 12UL,	///< ITU-R BT.601A
	DPC_FORMAT_CCIR601B		= 13UL,	///< ITU-R BT.601B
    DPC_FORMAT_4096COLOR    = 1UL,  ///< 4096 Color Format
    DPC_FORMAT_16GRAY       = 3UL   ///< 16 Level Gray Format
};

/*	the data output order in case of ITU-R BT.656 / 601. */
enum {
	DPC_YCORDER_CbYCrY		= 0UL,	///< Cb, Y, Cr, Y
	DPC_YCORDER_CrYCbY		= 1UL,	///< Cr, Y, Cb, Y
	DPC_YCORDER_YCbYCr		= 2UL,	///< Y, Cb, Y, Cr
	DPC_YCORDER_YCrYCb		= 3UL	///< Y, Cr, Y, Cb
};

/* the PAD output clock. */
enum {
	DPC_PADCLKSEL_VCLK	= 0UL,	///< VCLK
	DPC_PADCLKSEL_VCLK2	= 1UL	///< VCLK2
};

/* RGB dithering mode. */
enum {
	DPC_DITHER_BYPASS	= 0UL,	///< Bypass mode.
	DPC_DITHER_4BIT		= 1UL,	///< 8 bit -> 4 bit mode.
	DPC_DITHER_5BIT		= 2UL,	///< 8 bit -> 5 bit mode.
	DPC_DITHER_6BIT		= 3UL	///< 8 bit -> 6 bit mode.
};

/* priority of layers. */
enum {
	MLC_PRIORITY_VIDEOFIRST		= 0UL,	///< video layer > layer0 > layer1 > layer2
	MLC_PRIORITY_VIDEOSECOND	= 1UL,	///< layer0 > video layer > layer1 > layer2
	MLC_PRIORITY_VIDEOTHIRD		= 2UL,	///< layer0 > layer1 > video layer > layer2
    MLC_PRIORITY_VIDEOFOURTH	= 3UL	///< layer0 > layer1 > layer2 > video layer
};

/*	RGB layer pixel format. */
enum {
	MLC_RGBFMT_R5G6B5    = 0x44320000UL,   ///< 16bpp { R5, G6, B5 }.
	MLC_RGBFMT_B5G6R5    = 0xC4320000UL,   ///< 16bpp { B5, G6, R5 }.

	MLC_RGBFMT_X1R5G5B5  = 0x43420000UL,   ///< 16bpp { X1, R5, G5, B5 }.
	MLC_RGBFMT_X1B5G5R5  = 0xC3420000UL,   ///< 16bpp { X1, B5, G5, R5 }.
	MLC_RGBFMT_X4R4G4B4  = 0x42110000UL,   ///< 16bpp { X4, R4, G4, B4 }.
	MLC_RGBFMT_X4B4G4R4  = 0xC2110000UL,   ///< 16bpp { X4, B4, G4, R4 }.
	MLC_RGBFMT_X8R3G3B2  = 0x41200000UL,   ///< 16bpp { X8, R3, G3, B2 }.
	MLC_RGBFMT_X8B3G3R2  = 0xC1200000UL,   ///< 16bpp { X8, B3, G3, R2 }.

	MLC_RGBFMT_A1R5G5B5  = 0x33420000UL,   ///< 16bpp { A1, R5, G5, B5 }.
	MLC_RGBFMT_A1B5G5R5  = 0xB3420000UL,   ///< 16bpp { A1, B5, G5, R5 }.
	MLC_RGBFMT_A4R4G4B4  = 0x22110000UL,   ///< 16bpp { A4, R4, G4, B4 }.
	MLC_RGBFMT_A4B4G4R4  = 0xA2110000UL,   ///< 16bpp { A4, B4, G4, R4 }.
	MLC_RGBFMT_A8R3G3B2  = 0x11200000UL,   ///< 16bpp { A8, R3, G3, B2 }.
	MLC_RGBFMT_A8B3G3R2  = 0x91200000UL,   ///< 16bpp { A8, B3, G3, R2 }.

	MLC_RGBFMT_R8G8B8    = 0x46530000UL,   ///< 24bpp { R8, G8, B8 }.
	MLC_RGBFMT_B8G8R8    = 0xC6530000UL,   ///< 24bpp { B8, G8, R8 }.

	MLC_RGBFMT_X8R8G8B8  = 0x46530000UL,   ///< 32bpp { X8, R8, G8, B8 }.
	MLC_RGBFMT_X8B8G8R8  = 0xC6530000UL,   ///< 32bpp { X8, B8, G8, R8 }.
	MLC_RGBFMT_A8R8G8B8  = 0x06530000UL,   ///< 32bpp { A8, R8, G8, B8 }.
	MLC_RGBFMT_A8B8G8R8  = 0x86530000UL    ///< 32bpp { A8, B8, G8, R8 }.
};

/*	LVDS output format. */
enum {
	LVDS_LCDFORMAT_VESA		= 0,
	LVDS_LCDFORMAT_JEIDA	= 1,
	LVDS_LCDFORMAT_LOC		= 2,
};

/*------------------------------------------------------------------------------
 * 	Power and Clock type
 */
/*	pwm clock source */
enum {
	PWM_CLK_SRC_PLL0		= 0,
	PWM_CLK_SRC_PLL1		= 1,
	PWM_CLK_SRC_RESERVED	= 2,
	PWM_CLK_SRC_BITCLK		= 3,
	PWM_CLK_SRC_nBITCLK		= 4,
	PWM_CLK_SRC_AVCLK		= 5,
	PWM_CLK_SRC_nAVCLK		= 6,
	PWM_CLK_SRC_NONE		= 7
};

/*f Alive wakeup detect mode */
enum {
	PWR_DECT_ASYNC_LOWLEVEL 	= (0),
	PWR_DECT_ASYNC_HIGHLEVEL 	= (1),
	PWR_DECT_FALLINGEDGE 		= (2),
	PWR_DECT_RISINGEDGE  		= (3),
	PWR_DECT_SYNC_LOWLEVEL 		= (4),
	PWR_DECT_SYNC_HIGHLEVEL 	= (5),
	PWR_DECT_BOTHEDGE  			= (6),
};

#endif	/* __CFG_TYPE_H__ */

