#ifndef _ENV_INCLUDE_NXP_NX_DISPLAY_H_
#define _ENV_INCLUDE_NXP_NX_DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <nxp/nx_clockcontrol.h>
#include <nxp/nx_chip.h>
#include <s5p4418_gpio.h>

enum{
    NUMBER_OF_DISPTOP_CLKGEN_MODULE     = 5,
    NUMBER_OF_DPC_MODULE                = 2,
    NUMBER_OF_MLC_MODULE                = 2,
    NUMBER_OF_LVDS_MODULE 		        = 1,
};

enum{
OTHER_ADDR_OFFSET		                   =  (((PHY_BASEADDR_DISPLAYTOP_MODULE/0x00100000)%2)?0x000000:0x100000),
PHY_BASEADDR_DISPLAYTOP_MODULE_OFFSET      =  (OTHER_ADDR_OFFSET + 0x001000),
PHY_BASEADDR_DUALDISPLAY_MODULE		       =  (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x002000),

RESETINDEX_OF_DUALDISPLAY_MODULE_i_nRST    =  RESETINDEX_OF_DISPLAYTOP_MODULE_i_DualDisplay_nRST,

PHY_BASEADDR_DPC1_MODULE				   =  ( PHY_BASEADDR_DUALDISPLAY_MODULE + 3072 ),
PHY_BASEADDR_DPC0_MODULE				   =  ( PHY_BASEADDR_DUALDISPLAY_MODULE + 2048 ),
PHY_BASEADDR_LVDS_MODULE				   = ( PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x00A000),
RESETINDEX_OF_LVDS_MODULE_I_RESETN		   =  RESETINDEX_OF_DISPLAYTOP_MODULE_i_LVDS_nRST,
};

//------------------------------------------------------------------------------
// Reset Controller : Number of Reset
//------------------------------------------------------------------------------
enum{
    PHY_BASEADDR_MLC0_MODULE            = ( PHY_BASEADDR_DUALDISPLAY_MODULE +    0 ),
    PHY_BASEADDR_MLC1_MODULE            = ( PHY_BASEADDR_DUALDISPLAY_MODULE + 1024 ),
    
    PHY_BASEADDR_DISPTOP_CLKGEN0_MODULE = (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x006000),
    PHY_BASEADDR_DISPTOP_CLKGEN1_MODULE = (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x007000),
    PHY_BASEADDR_DISPTOP_CLKGEN2_MODULE = (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x005000),
    PHY_BASEADDR_DISPTOP_CLKGEN3_MODULE = (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x008000),
    PHY_BASEADDR_DISPTOP_CLKGEN4_MODULE = (PHY_BASEADDR_DISPLAYTOP_MODULE + OTHER_ADDR_OFFSET + 0x009000),
};

/* syncgen control (DPC) */
enum dpc_ctrl{
    DISP_SYNCGEN_DELAY_RGB_PVD          = 1<<0,
    DISP_SYNCGEN_DELAY_HSYNC_CP1        = 1<<1,
    DISP_SYNCGEN_DELAY_VSYNC_FRAM       = 1<<2,
    DISP_SYNCGEN_DELAY_DE_CP            = 1<<3,
};

typedef enum
{
	NX_DPC_YCORDER_CbYCrY		= 0UL,	///< Cb, Y, Cr, Y
	NX_DPC_YCORDER_CrYCbY		= 1UL,	///< Cr, Y, Cb, Y
	NX_DPC_YCORDER_YCbYCr		= 2UL,	///< Y, Cb, Y, Cr
	NX_DPC_YCORDER_YCrYCb		= 3UL	///< Y, Cr, Y, Cb

}	NX_DPC_YCORDER;

///	@brief the PAD output clock.
typedef enum
{
	NX_DPC_PADCLK_VCLK		= 0UL,	///< VCLK
	NX_DPC_PADCLK_VCLK2		= 1UL,	///< VCLK2
	NX_DPC_PADCLK_VCLK3		= 2UL	///< VCLK3
}	NX_DPC_PADCLK;

typedef enum
{
	NX_DPC_DITHER_BYPASS	= 0UL,	///< Bypass mode.
	NX_DPC_DITHER_4BIT		= 1UL,	///< 8 bit -> 4 bit mode.
	NX_DPC_DITHER_5BIT		= 2UL,	///< 8 bit -> 5 bit mode.
	NX_DPC_DITHER_6BIT		= 3UL	///< 8 bit -> 6 bit mode.

} NX_DPC_DITHER;

typedef enum
{
	NX_MLC_PRIORITY_VIDEOFIRST	= 0UL,	///< video layer > layer0 > layer1 > layer2
	NX_MLC_PRIORITY_VIDEOSECOND	= 1UL,	///< layer0 > video layer > layer1 > layer2
	NX_MLC_PRIORITY_VIDEOTHIRD	= 2UL,	///< layer0 > layer1 > video layer > layer2
	NX_MLC_PRIORITY_VIDEOFOURTH	= 3UL	///< layer0 > layer1 > layer2 > video layer

} NX_MLC_PRIORITY ;

enum {
	DISP_CLOCK_RESCONV		= 0,
	DISP_CLOCK_LCDIF		= 1,
	DISP_CLOCK_MIPI  		= 2,
	DISP_CLOCK_LVDS  		= 3,
	DISP_CLOCK_HDMI  		= 4,
	DISP_CLOCK_END			,
};

//------------------------------------------------------------------------------
///	@name	reset Interface
//------------------------------------------------------------------------------
//@{
enum HDMI_RESET{
	i_nRST       = 0,
	i_nRST_VIDEO = 1,
	i_nRST_SPDIF = 2,
	i_nRST_TMDS  = 3,
	i_nRST_PHY   = 4,
};

//------------------------------------------------------------------------------
///	@name	Clock Control types
//------------------------------------------------------------------------------
//@{
/// @brief	RGB layer pixel format.
typedef enum
{
	NX_MLC_RGBFMT_R5G6B5	= 0x44320000UL,	///< 16bpp { R5, G6, B5 }.
	NX_MLC_RGBFMT_B5G6R5	= 0xC4320000UL,	///< 16bpp { B5, G6, R5 }.

	NX_MLC_RGBFMT_X1R5G5B5	= 0x43420000UL,	///< 16bpp { X1, R5, G5, B5 }.
	NX_MLC_RGBFMT_X1B5G5R5	= 0xC3420000UL,	///< 16bpp { X1, B5, G5, R5 }.
	NX_MLC_RGBFMT_X4R4G4B4	= 0x42110000UL,	///< 16bpp { X4, R4, G4, B4 }.
	NX_MLC_RGBFMT_X4B4G4R4	= 0xC2110000UL,	///< 16bpp { X4, B4, G4, R4 }.
	NX_MLC_RGBFMT_X8R3G3B2	= 0x41200000UL,	///< 16bpp { X8, R3, G3, B2 }.
	NX_MLC_RGBFMT_X8B3G3R2	= 0xC1200000UL,	///< 16bpp { X8, B3, G3, R2 }.

	NX_MLC_RGBFMT_A1R5G5B5	= 0x33420000UL,	///< 16bpp { A1, R5, G5, B5 }.
	NX_MLC_RGBFMT_A1B5G5R5	= 0xB3420000UL,	///< 16bpp { A1, B5, G5, R5 }.
	NX_MLC_RGBFMT_A4R4G4B4	= 0x22110000UL,	///< 16bpp { A4, R4, G4, B4 }.
	NX_MLC_RGBFMT_A4B4G4R4	= 0xA2110000UL,	///< 16bpp { A4, B4, G4, R4 }.
	NX_MLC_RGBFMT_A8R3G3B2	= 0x11200000UL,	///< 16bpp { A8, R3, G3, B2 }.
	NX_MLC_RGBFMT_A8B3G3R2	= 0x91200000UL,	///< 16bpp { A8, B3, G3, R2 }.

	NX_MLC_RGBFMT_R8G8B8	= 0x46530000UL,	///< 24bpp { R8, G8, B8 }.
	NX_MLC_RGBFMT_B8G8R8	= 0xC6530000UL,	///< 24bpp { B8, G8, R8 }.

	NX_MLC_RGBFMT_X8R8G8B8	= 0x46530000UL,	///< 32bpp { X8, R8, G8, B8 }.
	NX_MLC_RGBFMT_X8B8G8R8	= 0xC6530000UL,	///< 32bpp { X8, B8, G8, R8 }.
	NX_MLC_RGBFMT_A8R8G8B8	= 0x06530000UL,	///< 32bpp { A8, R8, G8, B8 }.
	NX_MLC_RGBFMT_A8B8G8R8	= 0x86530000UL	///< 32bpp { A8, B8, G8, R8 }.

}	NX_MLC_RGBFMT ;

typedef enum
{
	NX_DPC_FORMAT_RGB555		= 0UL,	///< RGB555 Format
	NX_DPC_FORMAT_RGB565		= 1UL,	///< RGB565 Format
	NX_DPC_FORMAT_RGB666		= 2UL,	///< RGB666 Format
	NX_DPC_FORMAT_RGB666B		= 18UL, ///< RGB666 Format

	NX_DPC_FORMAT_RGB888		= 3UL,	///< RGB888 Format
	NX_DPC_FORMAT_MRGB555A		= 4UL,	///< MRGB555A Format
	NX_DPC_FORMAT_MRGB555B		= 5UL,	///< MRGB555B Format
	NX_DPC_FORMAT_MRGB565		= 6UL,	///< MRGB565 Format
	NX_DPC_FORMAT_MRGB666		= 7UL,	///< MRGB666 Format
	NX_DPC_FORMAT_MRGB888A		= 8UL,	///< MRGB888A Format
	NX_DPC_FORMAT_MRGB888B		= 9UL,	///< MRGB888B Format
	NX_DPC_FORMAT_CCIR656		= 10UL, ///< ITU-R BT.656 / 601(8-bit)
	NX_DPC_FORMAT_CCIR601A		= 12UL, ///< ITU-R BT.601A
	NX_DPC_FORMAT_CCIR601B		= 13UL, ///< ITU-R BT.601B
	NX_DPC_FORMAT_SRGB888		= 14UL, ///< SRGB888 Format
	NX_DPC_FORMAT_SRGBD8888 	= 15UL, ///< SRGBD8888 Format
	NX_DPC_FORMAT_4096COLOR 	= 1UL,	///< 4096 Color Format
	NX_DPC_FORMAT_16GRAY		= 3UL	///< 16 Level Gray Format

}	NX_DPC_FORMAT;

//==============================================================================
/// @name NULL data type
///
///	NULL is the null-pointer value used with many pointer operations \n
/// and functions. and this value is signed integer zero
//==============================================================================

//------------------------------------------------------------------------------
/// @defgroup	GPIO GPIO
//------------------------------------------------------------------------------
//@{

/// @brief	DPC Module's Register List
struct	NX_DPC_RegisterSet
{
	volatile u32 __Reserved[(0x07C-0x00)/2];///< 0x00-0x078*2 Reserved Regions
	volatile u32 DPCHTOTAL		;			///< 0x07C*2 : DPC Horizontal Total Length Register
	volatile u32 DPCHSWIDTH 	;			///< 0x07E*2 : DPC Horizontal Sync Width Register
	volatile u32 DPCHASTART 	;			///< 0x080*2 : DPC Horizontal Active Video Start Register
	volatile u32 DPCHAEND		;			///< 0x082*2 : DPC Horizontal Active Video End Register
	volatile u32 DPCVTOTAL		;			///< 0x084*2 : DPC Vertical Total Length Register
	volatile u32 DPCVSWIDTH 	;			///< 0x086*2 : DPC Vertical Sync Width Register
	volatile u32 DPCVASTART 	;			///< 0x088*2 : DPC Vertical Active Video Start Register
	volatile u32 DPCVAEND		;			///< 0x08A*2 : DPC Vertical Active Video End Register
	volatile u32 DPCCTRL0		;			///< 0x08C*2 : DPC Control 0 Register
	volatile u32 DPCCTRL1		;			///< 0x08E*2 : DPC Control 1 Register
	volatile u32 DPCEVTOTAL 	;			///< 0x090*2 : DPC Even Field Vertical Total Length Register
	volatile u32 DPCEVSWIDTH	;			///< 0x092*2 : DPC Even Field Vertical Sync Width Register
	volatile u32 DPCEVASTART	;			///< 0x094*2 : DPC Even Field Vertical Active Video Start Register
	volatile u32 DPCEVAEND		;			///< 0x096*2 : DPC Even Field Vertical Active Video End Register
	volatile u32 DPCCTRL2		;			///< 0x098*2 : DPC Control 2 Register
	volatile u32 DPCVSEOFFSET	;			///< 0x09A*2 : DPC Vertical Sync End Offset Register
	volatile u32 DPCVSSOFFSET	;			///< 0x09C*2 : DPC Vertical Sync Start Offset Register
	volatile u32 DPCEVSEOFFSET	;			///< 0x09E*2 : DPC Even Field Vertical Sync End Offset Register
	volatile u32 DPCEVSSOFFSET	;			///< 0x0A0*2 : DPC Even Field Vertical Sync Start Offset Register
	volatile u32 DPCDELAY0		;			///< 0x0A2*2 : DPC Delay 0 Register
	volatile u32 DPCUPSCALECON0 ;			///< 0x0A4*2 : DPC Sync Upscale Control Register 0
	volatile u32 DPCUPSCALECON1 ;			///< 0x0A6*2 : DPC Sync Upscale Control Register 1
	volatile u32 DPCUPSCALECON2 ;			///< 0x0A8*2 : DPC Sync Upscale Control Register 2
	//@modified choiyk 2012-10-09 ���� 11:10:34 : ( reference - NXP3200, LF2000, mes_disp03.h )
	volatile u32 DPCRNUMGENCON0 ;			///< 0x0AA*2 : DPC Sync Random number generator control register 0
	volatile u32 DPCRNUMGENCON1 ;			///< 0x0AC*2 : DPC Sync Random number generator control register 1
	volatile u32 DPCRNUMGENCON2 ;			///< 0x0AE*2 : DPC Sync Random number generator control register 2
	volatile u32 DPCRNDCONFORMULA_L;		///< 0x0B0*2
	volatile u32 DPCRNDCONFORMULA_H;		///< 0x0B2*2
	volatile u32 DPCFDTAddr 	;			///< 0x0B4*2 : DPC Sync frame dither table address register
	volatile u32 DPCFRDITHERVALUE;			///< 0x0B6*2 : DPC Sync frame red dither table value register
	volatile u32 DPCFGDITHERVALUE;			///< 0x0B8*2 : DPC Sync frame green dither table value register
	volatile u32 DPCFBDITHERVALUE;			///< 0x0BA*2 : DPC Sync frame blue table value register
	volatile u32 DPCDELAY1		 ;			///< 0x0BC*2 : DPC Delay 1 Register
	volatile u32 DPCMPUTIME0	 ;			///< 0x0BE*2   @todo Find Databook of these registers..
	volatile u32 DPCMPUTIME1	 ;			///< 0x0C0*2
	volatile u32 DPCMPUWRDATAL	 ;			///< 0x0C2*2
	volatile u32 DPCMPUINDEX	 ;			///< 0x0C4*2
	volatile u32 DPCMPUSTATUS	 ;			///< 0x0C6*2
	volatile u32 DPCMPUDATAH	 ;			///< 0x0C8*2
	volatile u32 DPCMPURDATAL	 ;			///< 0x0CA*2
	volatile u32 DPCDummy12 	 ;			///< 0x0CC*2
	volatile u32 DPCCMDBUFFERDATAL; 		///< 0x0CE*2
	volatile u32 DPCCMDBUFFERDATAH; 		///< 0x0D0*2
	volatile u32 DPCPOLCTRL 	 ;			///< 0x0D2*2
	volatile u32 DPCPADPOSITION[8]; 		///< 0x0D4*2~0x0E2*2(4,6,8,A,C,E,0,2)
	volatile u32 DPCRGBMASK[2]	 ;			///< 0x0E4*2~0x0E6*2(4,6)
	volatile u32 DPCRGBSHIFT	 ;			///< 0x0E8*2
	volatile u32 DPCDATAFLUSH	 ;			///< 0x0EA*2
	volatile u32 __Reserved06[( (0x3C0) - (2*0x0EC) )/4];		///< 0x0EC*2 ~ (0x3C0)
	// CLKGEN
	volatile u32 DPCCLKENB		;			///< 0x3C0 : DPC Clock Generation Enable Register
	volatile u32 DPCCLKGEN[2][2];			///< 0x3C4 : DPC Clock Generation Control 0 Low Register
											///< 0x3C8 : DPC Clock Generation Control 0 High Register
											///< 0x3CC : DPC Clock Generation Control 1 Low Register
											///< 0x3D0 : DPC Clock Generation Control 1 High Register
	// odd index is Reserved Region.
};

struct	NX_DISPTOP_CLKGEN_RegisterSet
{
	volatile u32	CLKENB; 		///< 0x40 : Clock Enable Register
	volatile u32	CLKGEN[4];			///< 0x44 : Clock Generate Register
};

//--------------------------------------------------------------------------
/// @brief	INTC register set structure.
struct NX_DISPLAYTOP_RegisterSet
{
	volatile u32 RESCONV_MUX_CTRL;		// 0x00
	volatile u32 INTERCONV_MUX_CTRL;	// 0x04
	volatile u32 MIPI_MUX_CTRL;			// 0x08
	volatile u32 LVDS_MUX_CTRL;			// 0x0C

	volatile u32 HDMIFIXCTRL0;			// 0x10
	volatile u32 HDMISYNCCTRL0;			// 0x14
	volatile u32 HDMISYNCCTRL1;			// 0x18
	volatile u32 HDMISYNCCTRL2;			// 0x1C
	volatile u32 HDMISYNCCTRL3;			// 0x20

	volatile u32 TFTMPU_MUX;            // 0x24
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// @defgroup	MLC MLC
//------------------------------------------------------------------------------
//@{

/// @brief	MLC Module's Register List
struct	NX_MLC_RegisterSet
{
	volatile u32 MLCCONTROLT;				///< 0x00 : MLC Top Control Register
	volatile u32 MLCSCREENSIZE;				///< 0x04 : MLC Screen Size Register
	volatile u32 MLCBGCOLOR;				///< 0x08 : MLC Background Color Register

	struct {
	volatile u32 MLCLEFTRIGHT;				///< 0x0C, 0x40 : MLC RGB Layer Left Right Register 0/1
	volatile u32 MLCTOPBOTTOM;				///< 0x10, 0x44 : MLC RGB Layer Top Bottom Register 0/1
	volatile u32 MLCINVALIDLEFTRIGHT0;		///< 0x14, 0x48 : MLC RGB Invalid Area0 Left, Right Register 0/1
	volatile u32 MLCINVALIDTOPBOTTOM0;		///< 0x18, 0x4C : MLC RGB Invalid Area0 Top, Bottom Register 0/1
	volatile u32 MLCINVALIDLEFTRIGHT1;		///< 0x1C, 0x50 : MLC RGB Invalid Area1 Left, Right Register 0/1
	volatile u32 MLCINVALIDTOPBOTTOM1;		///< 0x20, 0x54 : MLC RGB Invalid Area1 Top, Bottom Register 0/1
	volatile u32 MLCCONTROL;				///< 0x24, 0x58 : MLC RGB Layer Control Register 0/1
	volatile s32 MLCHSTRIDE;				///< 0x28, 0x5C : MLC RGB Layer Horizontal Stride Register 0/1
	volatile s32 MLCVSTRIDE;				///< 0x2C, 0x60 : MLC RGB Layer Vertical Stride Register 0/1
	volatile u32 MLCTPCOLOR;				///< 0x30, 0x64 : MLC RGB Layer Transparency Color Register 0/1
	volatile u32 MLCINVCOLOR;				///< 0x34, 0x68 : MLC RGB Layer Inversion Color Register 0/1
	volatile u32 MLCADDRESS;				///< 0x38, 0x6C : MLC RGB Layer Base Address Register 0/1
	volatile u32 __Reserved0;				///< 0x3C, 0x70 : Reserved Region

	} MLCRGBLAYER[2];

	struct {
	volatile u32 MLCLEFTRIGHT;				///< 0x74 : MLC Video Layer Left Right Register
	volatile u32 MLCTOPBOTTOM;				///< 0x78 : MLC Video Layer Top Bottom Register
	volatile u32 MLCCONTROL;				///< 0x7C : MLC Video Layer Control Register
	volatile u32 MLCVSTRIDE;				///< 0x80 : MLC Video Layer Y Vertical Stride Register
	volatile u32 MLCTPCOLOR;				///< 0x84 : MLC Video Layer Transparency Color Register
	//volatile u32 __Reserved1[1];			///< 0x88 : Reserved Region
	volatile u32 MLCINVCOLOR;				///< 0x88 : MLCINVCOLOR3 ( )
	volatile u32 MLCADDRESS;				///< 0x8C : MLC Video Layer Y Base Address Register
	volatile u32 MLCADDRESSCB;				///< 0x90 : MLC Video Layer Cb Base Address Register
	volatile u32 MLCADDRESSCR;				///< 0x94 : MLC Video Layer Cr Base Address Register
	volatile s32 MLCVSTRIDECB;				///< 0x98 : MLC Video Layer Cb Vertical Stride Register
	volatile s32 MLCVSTRIDECR;				///< 0x9C : MLC Video Layer Cr Vertical Stride Register
	volatile u32 MLCHSCALE;					///< 0xA0 : MLC Video Layer Horizontal Scale Register
	volatile u32 MLCVSCALE;					///< 0xA4 : MLC Video Layer Vertical Scale Register
	volatile u32 MLCLUENH;					///< 0xA8 : MLC Video Layer Luminance Enhancement Control Register
	volatile u32 MLCCHENH[4];				///< 0xAC, 0xB0, 0xB4, 0xB8 : MLC Video Layer Chrominance Enhancement Control Register 0/1/2/3

	} MLCVIDEOLAYER;

	struct {
	volatile u32 MLCLEFTRIGHT;				///< 0xBC : MLC RGB Layer Left Right Register 2
	volatile u32 MLCTOPBOTTOM;				///< 0xC0 : MLC RGB Layer Top Bottom Register 2
	volatile u32 MLCINVALIDLEFTRIGHT0;		///< 0xC4 : MLC RGB Invalid Area0 Left, Right Register 2
	volatile u32 MLCINVALIDTOPBOTTOM0;		///< 0xC8 : MLC RGB Invalid Area0 Top, Bottom Register 2
	volatile u32 MLCINVALIDLEFTRIGHT1;		///< 0xCC : MLC RGB Invalid Area1 Left, Right Register 2
	volatile u32 MLCINVALIDTOPBOTTOM1;		///< 0xD0 : MLC RGB Invalid Area1 Top, Bottom Register 2
	volatile u32 MLCCONTROL;				///< 0xD4 : MLC RGB Layer Control Register 2
	volatile s32 MLCHSTRIDE;				///< 0xD8 : MLC RGB Layer Horizontal Stride Register 2
	volatile s32 MLCVSTRIDE;				///< 0xDC : MLC RGB Layer Vertical Stride Register 2
	volatile u32 MLCTPCOLOR;				///< 0xE0 : MLC RGB Layer Transparency Color Register 2
	volatile u32 MLCINVCOLOR;				///< 0xE4 : MLC RGB Layer Inversion Color Register 2
	volatile u32 MLCADDRESS;				///< 0xE8 : MLC RGB Layer Base Address Register 2

	} MLCRGBLAYER2;

	volatile u32 MLCPALETETABLE2;               ///< 0xEC
	volatile u32 MLCGAMMACONT;					///< 0xF0 : MLC Gama Control Register
	volatile u32 MLCRGAMMATABLEWRITE;			///< 0xF4 : MLC Red Gamma Table Write Register
	volatile u32 MLCGGAMMATABLEWRITE;			///< 0xF8 : MLC Green Gamma Table Write Register
	volatile u32 MLCBGAMMATABLEWRITE;			///< 0xFC : MLC Blue Gamma Table Write Register
	volatile u32 YUVLAYERGAMMATABLE_RED;        ///< 0x100:
	volatile u32 YUVLAYERGAMMATABLE_GREEN;      ///< 0x104:
	volatile u32 YUVLAYERGAMMATABLE_BLUE;       ///< 0x108:
	// @modified charles 2010/01/28 Dynamic Dimming �߰�
	volatile u32 DIMCTRL;                       ///< 0x10C
	volatile u32 DIMLUT0;                       ///< 0x110
	volatile u32 DIMLUT1;                       ///< 0x114
	volatile u32 DIMBUSYFLAG;                   ///< 0x118
	volatile u32 DIMPRDARRR0;                   ///< 0x11C
	volatile u32 DIMPRDARRR1;                   ///< 0x120
	volatile u32 DIMRAM0RDDATA;                 ///< 0x124
	volatile u32 DIMRAM1RDDATA;                 ///< 0x128
	volatile u32 __Reserved2[(0x3C0-0x12C)/4];	///< 0x12C ~ 0x3BC : Reserved Region
	volatile u32 MLCCLKENB;						///< 0x3C0 : MLC Clock Enable Register

};

typedef struct
{
	volatile u32 LVDSCTRL0;       //'h00
    volatile u32 LVDSCTRL1;       //'h04
    volatile u32 LVDSCTRL2;       //'h08
    volatile u32 LVDSCTRL3;       //'h0c
    volatile u32 LVDSCTRL4;       //'h10
    volatile u32 _Reserved0[3];   //'h14--'h1c
    volatile u32 LVDSLOC0			;   //'h20
    volatile u32 LVDSLOC1		  ;   //'h24
    volatile u32 LVDSLOC2		  ;   //'h28
    volatile u32 LVDSLOC3		  ;   //'h2C
    volatile u32 LVDSLOC4		  ;   //'h30
    volatile u32 LVDSLOC5		  ;   //'h34
    volatile u32 LVDSLOC6		  ;   //'h38
    volatile u32 _Reserved1   ;   //'h3c
    volatile u32 LVDSLOCMASK0 ;   //'h40
    volatile u32 LVDSLOCMASK1 ;   //'h44
    volatile u32 LVDSLOCPOL0  ;   //'h48
    volatile u32 LVDSLOCPOL1  ;   //'h4c
    volatile u32 LVDSTMODE0   ;   //'h50
    volatile u32 LVDSTMODE1   ;   //'h54

    volatile u32 _Reserved2[2];   //'h58--'h5C
	//========================================
	// ECO for LVDS self test.
	// Never Use i_PCLK in ECO Script!!!!!!
	//@modified choiyk 2013-01-29 ���� 3:55:56
	//========================================

} NX_LVDS_RegisterSet;


boolean NX_DISPLAYTOP_Initialize( void );
u32 NX_DISPLAYTOP_GetNumberOfModule( void );
void NX_DISPTOP_CLKGEN_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable );
u32 NX_LVDS_GetResetNumber ( u32 ModuleIndex );
void NX_DISPTOP_CLKGEN_SetClockSource( u32 ModuleIndex, u32 Index, u32 ClkSrc );
void NX_DISPTOP_CLKGEN_SetClockDivisor( u32 ModuleIndex, u32 Index, u32 Divisor );
void NX_LVDS_SetLVDSCTRL0       ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSCTRL1       ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSCTRL2       ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSCTRL3       ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSCTRL4       ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSTMODE0      ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC0        ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC1		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC2		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC3		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC4		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC5		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOC6		( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOCMASK0 ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOCMASK1 ( u32 ModuleIndex, u32 regvalue );
void NX_LVDS_SetLVDSLOCPOL0  ( u32 ModuleIndex, u32 regvalue );
void NX_DISPLAYTOP_SetLVDSMUX   ( boolean bEnb, u32 SEL );
void NX_DPC_SetDPCEnable        ( u32 ModuleIndex, boolean bEnb );
void NX_DPC_SetClockDivisorEnable( u32 ModuleIndex, boolean Enable );
void NX_DPC_SetClockSource      ( u32 ModuleIndex, u32 Index, u32 ClkSrc );
void NX_DPC_SetClockDivisor     ( u32 ModuleIndex, u32 Index, u32 Divisor );
void NX_DPC_SetClockOutDelay    ( u32 ModuleIndex, u32 Index, u32 delay );
void NX_DPC_SetMode( u32 ModuleIndex, NX_DPC_FORMAT format,
					boolean bInterlace, boolean bInvertField,
					boolean bRGBMode, boolean bSwapRB,
					NX_DPC_YCORDER ycorder, boolean bClipYC, boolean bEmbeddedSync,
					NX_DPC_PADCLK clock, boolean bInvertClock);
void NX_DPC_SetHSync( u32 ModuleIndex, u32 AVWidth, u32 HSW, u32 HFP, u32 HBP, boolean bInvHSYNC );
void NX_DPC_SetVSync( u32 ModuleIndex, u32 AVHeight, u32 VSW, u32 VFP, u32 VBP, boolean bInvVSYNC,
					u32 EAVHeight, u32 EVSW, u32 EVFP, u32 EVBP );
void NX_DPC_SetVSyncOffset( u32 ModuleIndex, u32 VSSOffset, u32 VSEOffset, u32 EVSSOffset, u32 EVSEOffset );
void NX_DPC_SetDelay( u32 ModuleIndex, u32 DelayRGB_PVD, u32 DelayHS_CP1, u32 DelayVS_FRAM, u32 DelayDE_CP2 );
void NX_DPC_SetDither( u32 ModuleIndex, NX_DPC_DITHER DitherR, NX_DPC_DITHER DitherG, NX_DPC_DITHER DitherB );
void NX_MLC_SetScreenSize( u32 ModuleIndex, u32 width, u32 height );
void NX_MLC_SetMLCEnable( u32 ModuleIndex, boolean bEnb );
void NX_MLC_SetTopDirtyFlag( u32 ModuleIndex );
void NX_MLC_SetLayerEnable( u32 ModuleIndex, u32 layer, boolean bEnb );
void NX_MLC_SetDirtyFlag( u32 ModuleIndex, u32 layer );
void NX_MLC_SetLayerPriority( u32 ModuleIndex, NX_MLC_PRIORITY priority );
void NX_MLC_SetBackground( u32 ModuleIndex, u32 color );
void NX_MLC_SetFieldEnable( u32 ModuleIndex, boolean bEnb );
void NX_MLC_SetRGBLayerGamaTablePowerMode( u32 ModuleIndex, boolean bRed, boolean bGreen, boolean bBlue );
void NX_MLC_SetRGBLayerGamaTableSleepMode( u32 ModuleIndex, boolean bRed, boolean bGreen, boolean bBlue );
void NX_MLC_SetRGBLayerGammaEnable( u32 ModuleIndex, boolean bEnable );
void NX_MLC_SetDitherEnableWhenUsingGamma( u32 ModuleIndex, boolean bEnable );
void NX_MLC_SetGammaPriority( u32 ModuleIndex, boolean bVideoLayer );
void NX_MLC_SetTopPowerMode( u32 ModuleIndex, boolean bPower );
void NX_MLC_SetTopSleepMode( u32 ModuleIndex, boolean bSleep );
void NX_MLC_SetLockSize( u32 ModuleIndex, u32 layer, u32 locksize );
void NX_MLC_SetAlphaBlending( u32 ModuleIndex, u32 layer, boolean bEnb, u32 alpha );
void NX_MLC_SetTransparency( u32 ModuleIndex, u32 layer, boolean bEnb, u32 color );
void NX_MLC_SetColorInversion( u32 ModuleIndex, u32 layer, boolean bEnb, u32 color );
void NX_MLC_SetRGBLayerInvalidPosition( u32 ModuleIndex, u32 layer, u32 region, s32 sx, s32 sy, s32 ex, s32 ey, boolean bEnb );
void NX_MLC_SetFormatRGB( u32 ModuleIndex, u32 layer, NX_MLC_RGBFMT format );
void NX_MLC_SetPosition( u32 ModuleIndex, u32 layer, s32 sx, s32 sy, s32 ex, s32 ey );
void NX_MLC_SetRGBLayerStride( u32 ModuleIndex, u32 layer, s32 hstride, s32 vstride );
void NX_MLC_SetRGBLayerAddress( u32 ModuleIndex, u32 layer, u32 addr );
void NX_DISPTOP_CLKGEN_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32 NX_DISPTOP_CLKGEN_GetPhysicalAddress( u32 ModuleIndex );
boolean NX_LVDS_Initialize( void );
u32 NX_LVDS_GetNumberOfModule( void );
void NX_LVDS_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32 NX_LVDS_GetPhysicalAddress( u32 ModuleIndex );
void NX_DISPTOP_CLKGEN_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode );
boolean NX_DISPTOP_CLKGEN_Initialize( void );
boolean NX_DPC_Initialize( void );
boolean NX_MLC_Initialize( void );
void NX_DISPLAYTOP_SetBaseAddress( u32 BaseAddress );
u32 NX_DISPLAYTOP_GetPhysicalAddress( void );
boolean NX_DISPLAYTOP_OpenModule( void );
u32 NX_DISPLAYTOP_GetResetNumber ( void );
u32 NX_DUALDISPLAY_GetResetNumber ( u32 ModuleIndex );
void NX_MLC_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32 NX_MLC_GetPhysicalAddress( u32 ModuleIndex );
void NX_MLC_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode );
void NX_MLC_SetClockBClkMode( u32 ModuleIndex, NX_BCLKMODE mode );
void NX_DPC_SetBaseAddress( u32 ModuleIndex, u32 BaseAddress );
u32 NX_DPC_GetPhysicalAddress( u32 ModuleIndex );
void NX_DPC_SetClockPClkMode( u32 ModuleIndex, NX_PCLKMODE mode );

#ifdef __cplusplus
}
#endif

#endif	/* _NX_DISPLAY_H_*/


