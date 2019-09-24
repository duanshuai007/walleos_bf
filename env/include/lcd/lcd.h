#ifndef _ENV_INCLUDE_LCD_LCD_H_
#define _ENV_INCLUDE_LCD_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <config.h>
#include <s5p4418_gpio.h>

#define CFG_IO_LCD_PWR_ENB		((PAD_GPIO_C + 24) | PAD_FUNC_ALT1)/* GPIO */

/*	layer pixel format. */
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

/*	LVDS output format. */
enum {
	LVDS_LCDFORMAT_VESA		= 0,
	LVDS_LCDFORMAT_JEIDA	= 1,
	LVDS_LCDFORMAT_LOC		= 2,
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

enum cfg_disp{
    cfg_disp_output_modole         = 0,
    cfg_disp_lvds_lcd_format       = LVDS_LCDFORMAT_JEIDA,
    cfg_disp_pri_resol_width       = 800, //800
    cfg_disp_pri_resol_height      = 1280, //1280
    cfg_disp_pri_screen_pixel_byte = 4,
    cfg_disp_pri_screen_layer      = 0,
    cfg_disp_pri_video_priority    = 2, // 0, 1, 2, 3;
    cfg_disp_pri_screen_rgb_format = MLC_RGBFMT_A8R8G8B8,
    cfg_disp_pri_back_ground_color = 0x000000,
    cfg_disp_pri_mlc_interlace     = FALSE,
    
    cfg_disp_pri_out_format        = DPC_FORMAT_RGB666,
    cfg_disp_pri_out_invert_field  = FALSE,
    cfg_disp_pri_out_swaprb        = FALSE,
    cfg_disp_pri_out_ycorder       = DPC_YCORDER_CbYCrY,
    cfg_disp_pri_padclksel         = DPC_PADCLKSEL_VCLK, /* VCLK=CLKGEN1, VCLK12=CLKGEN0 */
    cfg_disp_pri_clkgen0_delay     = 0,
    cfg_disp_pri_clkgen0_invert    = 0,
    cfg_disp_pri_clkgen1_delay     = 0,
    cfg_disp_pri_clkgen1_invert    = 0,
    cfg_disp_pri_clksel1_select    = 0,
    
    cfg_disp_pri_hsync_sync_width  = 20,
    cfg_disp_pri_hsync_back_porch  = 160,
    cfg_disp_pri_hsync_front_porch = 160,
    cfg_disp_pri_hsync_active_high = TRUE,
    cfg_disp_pri_vsync_sync_width  = 3,
    cfg_disp_pri_vsync_back_porch  = 23,
    cfg_disp_pri_vsync_front_porch = 12,
    cfg_disp_pri_vsync_active_high = TRUE,
    cfg_disp_pri_pixel_clock       = 800000000/12,
    cfg_disp_pri_clkgen0_source    = 2,
    cfg_disp_pri_clkgen0_div       = 12, // even divide;
    cfg_disp_pri_clkgen1_source    = 7,
    cfg_disp_pri_clkgen1_div       = 1,
};

/*
 * LVDS
 */
enum lvds_pclk{
    LVDS_PCLK_L_MIN = 40000000,
    LVDS_PCLK_L_MAX = 80000000,
    LVDS_PCLK_H_MIN = 80000000,
    LVDS_PCLK_H_MAX = 160000000,
};

struct disp_lvds_param {
	unsigned int lcd_format;		/* 0:VESA, 1:JEIDA, 2: Location Setting */
	int			 inv_hsync;			/* hsync polarity invert for VESA, JEIDA */
	int			 inv_vsync;			/* bsync polarity invert for VESA, JEIDA */
	int			 inv_de;			/* de polarity invert for VESA, JEIDA */
	int			 inv_inclk_pol;		/* input clock(pixel clock) polarity invert */
	/* Location settting */
	unsigned int loc_map[9];		/* when lcd format is "Location Setting", LCDn = 8bit * 35 = 35byte := 9dword */
	unsigned int loc_mask[2];		/* when lcd format is "Location Setting", 0 ~ 34 */
	unsigned int loc_pol[2];		/* when lcd format is "Location Setting", 0 ~ 34 */
};


#define INIT_PARAM_LVDS(name)                                                   \
		struct disp_lvds_param name = {                                         \
		.lcd_format     = cfg_disp_lvds_lcd_format,                             \
	};

#define	INIT_VIDEO_SYNC(name)								\
	struct disp_vsync_info name = { 						\
		.h_active_len	= cfg_disp_pri_resol_width, 		\
		.h_sync_width	= cfg_disp_pri_hsync_sync_width,	\
		.h_back_porch	= cfg_disp_pri_hsync_back_porch,	\
		.h_front_porch	= cfg_disp_pri_hsync_front_porch,	\
		.h_sync_invert	= cfg_disp_pri_hsync_active_high,	\
		.v_active_len	= cfg_disp_pri_resol_height,		\
		.v_sync_width	= cfg_disp_pri_vsync_sync_width,	\
		.v_back_porch	= cfg_disp_pri_vsync_back_porch,	\
		.v_front_porch	= cfg_disp_pri_vsync_front_porch,	\
		.v_sync_invert	= cfg_disp_pri_vsync_active_high,	\
		.pixel_clock_hz = cfg_disp_pri_pixel_clock, 		\
		.clk_src_lv0	= cfg_disp_pri_clkgen0_source,		\
		.clk_div_lv0	= cfg_disp_pri_clkgen0_div, 		\
		.clk_src_lv1	= cfg_disp_pri_clkgen1_source,		\
		.clk_div_lv1	= cfg_disp_pri_clkgen1_div, 		\
	};

#define	INIT_PARAM_SYNCGEN(name)							\
	struct disp_syncgen_param name = {						\
		.interlace		= cfg_disp_pri_mlc_interlace,		\
		.out_format 	= cfg_disp_pri_out_format,			\
		.lcd_mpu_type	= 0,								\
		.invert_field	= cfg_disp_pri_out_invert_field,	\
		.swap_RB		= cfg_disp_pri_out_swaprb,			\
		.yc_order		= cfg_disp_pri_out_ycorder, 		\
		.delay_mask 	= 0,								\
		.vclk_select	= cfg_disp_pri_padclksel,			\
		.clk_delay_lv0	= cfg_disp_pri_clkgen0_delay,		\
		.clk_inv_lv0	= cfg_disp_pri_clkgen0_invert,		\
		.clk_delay_lv1	= cfg_disp_pri_clkgen1_delay,		\
		.clk_inv_lv1	= cfg_disp_pri_clkgen1_invert,		\
		.clk_sel_div1	= cfg_disp_pri_clksel1_select,		\
	};

#define	INIT_PARAM_MULTILY(name)					\
	struct disp_multily_param name = {						\
		.x_resol		= cfg_disp_pri_resol_width, 		\
		.y_resol		= cfg_disp_pri_resol_height,		\
		.pixel_byte 	= cfg_disp_pri_screen_pixel_byte,	\
		.fb_layer		= cfg_disp_pri_screen_layer,		\
		.video_prior	= cfg_disp_pri_video_priority,		\
		.mem_lock_size	= 16,								\
		.rgb_format 	= cfg_disp_pri_screen_rgb_format,	\
		.bg_color		= cfg_disp_pri_back_ground_color,	\
		.interlace		= cfg_disp_pri_mlc_interlace,		\
	};

/*
 *	display device number
 */
enum {
	DISP_DEVICE_RESCONV 	= 0,
	DISP_DEVICE_LCDIF		= 1,
	DISP_DEVICE_HDMI		= 2,
	DISP_DEVICE_MIPI		= 3,
	DISP_DEVICE_LVDS		= 4,
	DISP_DEVICE_SYNCGEN0	= 5,
	DISP_DEVICE_SYNCGEN1	= 6,
	DISP_DEVICE_END 		,
};

/*
 * multilayer control (MLC)
 */
struct disp_multily_param {
	int 		 x_start;
	int 		 y_start;
	int			 x_resol;
	int			 y_resol;
	int 		 pixel_byte;
	unsigned int rgb_format;
	int 		 fb_layer;
	int			 video_prior;		/* 0 = video > RGB0.., 1 = RGB0 > vidoe > RGB1 .., 2 = RGB0 > RGB1 > vidoe .. */
	int			 interlace;
	unsigned int bg_color;
	unsigned int mem_lock_size;		/* lock size for memory access, 4, 8, 16 only valid (default 8byter) */
};

struct disp_vsync_info {
	int	interlace;
	int	h_active_len;
	int	h_sync_width;
	int	h_back_porch;
	int	h_front_porch;
	int	h_sync_invert;		/* default active low */
	int	v_active_len;
	int	v_sync_width;
	int	v_back_porch;
	int	v_front_porch;
	int	v_sync_invert;		/* default active low */
	int pixel_clock_hz;		/* HZ */
	/* clock gen */
	int clk_src_lv0;
	int clk_div_lv0;
	int clk_src_lv1;
	int clk_div_lv1;
};

struct disp_syncgen_param {
	/* scan format */
	int 		 interlace;
	/* syncgen format */
	unsigned int out_format;
	int 		 lcd_mpu_type;		/* set when lcd type is mpu */
	int			 invert_field;		/* 0= Normal Field(Low is odd field), 1: Invert Field(low is even field) */
	int			 swap_RB;
	unsigned int yc_order;			/* for CCIR output */
	/* exten sync delay  */
	int			delay_mask;			/* if 0, set defalut delays (rgb_pvd, hsync_cp1, vsync_fram, de_cp2 */
	int 		d_rgb_pvd;			/* the delay value for RGB/PVD signal   , 0 ~ 16, default  0 */
	int			d_hsync_cp1;		/* the delay value for HSYNC/CP1 signal , 0 ~ 63, default 12 */
	int			d_vsync_fram;		/* the delay value for VSYNC/FRAM signal, 0 ~ 63, default 12 */
	int			d_de_cp2;			/* the delay value for DE/CP2 signal    , 0 ~ 63, default 12 */
	/* exten sync delay  */
	int			vs_start_offset;	/* start veritcal sync offset, defatult 0 */
	int			vs_end_offset;		/* end veritcla sync offset  , defatult 0 */
	int			ev_start_offset;	/* start even veritcal sync offset, defatult 0 */
	int			ev_end_offset;		/* end even veritcal sync offset  , defatult 0 */
	/* pad clock seletor */
	int			vclk_select;		/* 0=vclk0, 1=vclk2 */
	int			clk_inv_lv0;		/* OUTCLKINVn */
	int			clk_delay_lv0;		/* OUTCLKDELAYn */
	int			clk_inv_lv1;		/* OUTCLKINVn */
	int			clk_delay_lv1;		/* OUTCLKDELAYn */
	int			clk_sel_div1;		/* 0=clk1_inv, 1=clk1_div_2_ns */
};

//extern void gpio_initialize();
void bd_display_run(void );

#ifdef __cplusplus
}
#endif

#endif	/* _LCD_H_ */





