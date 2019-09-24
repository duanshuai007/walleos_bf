#ifndef _ENV_INCLUDE_GRAPHIC_GRAPHIC_H_ 
#define _ENV_INCLUDE_GRAPHIC_GRAPHIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <font.h>
#include <types.h>
#include <config.h>

#define DEPTH 32   // can be: 8, 16 or 32
 
// really ((green) & 0x3F) << 5, but to have a 0-31 range for all colors
#define COLOR16(red, green, blue)     (((red) & 0x1F) << 11 \
		             | ((green) & 0x1F) << 6 \
			         | ((blue) & 0x1F))

#define COLOR32(red, green, blue, alpha)  (((red) & 0xFF)        \
			         | ((green) & 0xFF) << 8  \
			         | ((blue) & 0xFF) << 16  \
			         | ((alpha) & 0xFF) << 24)

#if DEPTH == 8
    typedef u8 TScreenColor;
#elif DEPTH == 16
    typedef u16 TScreenColor;
#elif DEPTH == 32
    typedef u32 TScreenColor;
#else
    #error DEPTH must be 8, 16 or 32
#endif

#define MAX_SHEETS      256
#define CURSOR_C_TIME   0.5

enum win{
    WIN_TOP_PADDING     = 21,
    WIN_LEFT_PADDING    = 3,
    WIN_RIGHT_PADDING   = 3,
    WIN_BOTTOM_PADDING  = 2,
};

enum word_dis{
    WORD_WIDTH  = 8,
    WORD_HEIGHT = 16,
};

enum mouse{
    MOUSE_BUTTON_LEFT   = 1<<0,
    MOUSE_BUTTON_RIGHT  = 1<<1,
    MOUSE_BUTTON_MIDDLE = 1<<2,
};

enum scaleType
{
    CENTER,       //按图片的原来size居中显示，当图片长/宽超过sheet的长/宽，则截取图片的居中部分显示。当图片小于ImageView的宽高：直接居中显示该图片。
	CENTER_CROP,  //将图片等比例缩放，让图像的短边与ImageView的边长度相同，即不能留有空白，缩放后截取中间部分进行显示
	CENTER_INSIDE,//将图片大小大于ImageView的图片进行等比例缩小，直到整幅图能够居中显示在ImageView中，小于ImageView的图片不变，直接居中显示
	FIT_CENTER,   //大图等比例缩小，使整幅图能够居中显示在ImageView中，小图等比例放大，同样要整体居中显示在ImageView中
	FIT_END ,     //缩放方式同FIT_CENTER，只是将图片显示在右方或下方，而不是居中
	FIT_START,    //缩放方式同FIT_CENTER，只是将图片显示在左方或上方，而不是居中
	FIT_XY,       //将图片非等比例缩放到大小与ImageView相
	MATRIX,       //不改变原图的大小，从sheet的左上角开始绘制原图，原图超过sheet的部分作裁剪处理
	SWITCH        //图片切换
};

enum TMouseEvent
{
    MouseEventMouseMove,
	MouseEventMouseDown,
	MouseEventMouseUp,
	MouseEventUnknown
};

enum edit
{
    edit_off,
    edit_on,
	edit_pic
};

typedef enum ESheetEvent
{
    SheetSelected, //选中有效sheet
    SheetClosed, //关闭有效sheet
    SheetInvalid //选中背景，无有效sheet
}ESheetEvent;

typedef enum ETouchScreenEvent
{
    FingerDown,
    FingerUp,
    FingerMove,
    Unknown
}ETouchScreenEvent;

typedef struct SHEET {
    TScreenColor *buf;
	unsigned char *picBuf;//源图片缓冲
	float current_scale,quotiety;//当前缩放比例,放大系数
	int srcPitch,srcWidth,srcHeight,dstWidth,dstHeight;
	int scaleType,picType;
    int bxsize; // x轴像素个数
	int bysize; // y轴像素个数
    int	col_inv, height, flags,editFlag;
	int current_status;//0:原图 1:完全放大
	int zoom_status;//-1:非图片sheet 0:未超出sheet 1:宽超出 2:高超出 3:宽高都超出
	int cursorx; //光标位置
	int cursory; // 光标位置
	int vx0; //当前窗口左上角x坐标
	int vy0; //当前窗口左上角y坐标
	int pvx0;//当前图片的左上角x坐标
	int pvy0;//当前图片的左上角y坐标
	int pic_position;
	int x_position;
	int y_position;
	int n_pos_x;//图片切换时的显示位置
	int create_flg;//是否首次创建
	int style;//0:带标题栏 1:不带标题栏
	int start_x,start_y,end_x,end_y;//图片相对中点坐标
	struct SHEET *next_pic_sht_win,*pre_pic_sht_win,*current_pic_sht_win;
    struct SHTCTL *ctl;
}SHEET;

typedef struct SHTCTL {
    TScreenColor *vram,*map;
    int xsize, ysize, top;
    SHEET *sheets[MAX_SHEETS];
    SHEET sheets0[MAX_SHEETS];
}SHTCTL;

extern SHEET *m_tmp_sht_win;
/**
 *  屏幕打印
 *    
 *  @param vramc 窗口缓存
 *  
 *  @param xsize 窗口宽度
 *       
 *  @param x，y  文字相对窗口左上角坐标
 *         
 *  @param cccc  文字背景色
 *       
 *  @param s     打印内容
 *                 
 *  @return void
 */

void putfonts_asc(TScreenColor *vram, int xsize, int x, int y, TScreenColor c,const char *s);

/**
 *  图形接口初期化
 *    
 *  @param bWithMouse 是否需要支持鼠标
 *  
 *  @return 0:成功 1:失败
 */
int graphic_init(boolean bWithMouse);

SHTCTL *shtctl_init(TScreenColor *vram, int xsize, int ysize);

SHEET *sheet_alloc(SHTCTL *ctl);

void sheet_setbuf(SHEET *sht, TScreenColor *buf, int xsize, int ysize, int col_inv);

void mouse_sheet_slide(SHEET *sht,int vx0, int vy0);

/**
 *    鼠标事件处理
 *       
 *    @param nEvent       鼠标事件
 *
 *    @param nButtons     鼠标按键事件
 *  
 *    @param nPosX，nPosY 鼠标相对位置
 *        
 *    @param sht_win      键盘选中图层
 *
 *    @param flg          鼠标响应事件(0:有效图层选中,1:关闭图层,2:无有效图层选中)
 * 
 *    @return void
 */
void mouse_event(unsigned nEvent, unsigned nButtons, int nPosX, int nPosY,SHEET **sht_win,unsigned *flg);

/**
 *    触摸屏事件处理
 *       
 *    @param nEvent       触摸屏时间，参考ETouchScreenEvent
 * *  
 *    @param nPosX，nPosY 点击的绝对位置
 *        
 *    @param sht_win      选中的窗口
 *
 *    @param flg          触屏响应事件(0:有效图层选中,1:关闭图层,2:无有效图层选中)
 * 
 *    @return void
 */
void touchscreen_event(unsigned nEvent, unsigned nPosX, unsigned nPosY,SHEET **sht_win, unsigned *flg);
/**
 *    键盘相应处理
 *    
 *    @param sht_win 窗口层次结构体
 *
 *    @param vx0，vy0 文字相对位置
 *        
 *    @param input 键盘输入字符
 *
 *    @return void
 */
void keyboard_event(SHEET *sht_win,const char *input);

/**
 *    键盘相应处理
 *      
 *    @param sht_win 窗口层次结构体
 *     
 *    @param vx0，vy0 文字相对位置
 *            
 *    @param input 键盘输入字符
 *
 *    @param len 字符长度
 *       
 *    @return void
 */

void keyboard_event2(SHEET *sht_win,const char *input,int len);

/**
 *     屏幕打印输出
 *       
 *     @param sht   响应图层
 *            
 *     @param input 输入字符
 *       
 *     @return void
 */
void write_asc(SHEET *sht,const char *input);

/**
 *     屏幕打印输出
 *         
 *     @param sht   响应图层
 *              
 *     @param input 输入字符
 *
 *     @param len 字符长度
 *            
 *     @return void
 */
void write_asc2(SHEET *sht,const char *input,int len);

/**
 *     屏幕打印输出
 *         
 *     @param sht   响应图层
 *
 *     @param vx0，vy0 文字相对位置
 *                 
 *     @param input 输入字符
 *           
 *     @return void
 */
void write_asc_position(SHEET *sht,int vx0, int vy0,const char *input);

void switch_or_close_win_sheet(int vx0, int vy0);

SHEET * get_current_sheet(int vx0, int vy0);

void sheet_slide(SHEET *sht, int vx0, int vy0);

void sheet_slide_step(SHEET *sht, int dx, int dy);

boolean isWindowCloseArea(SHEET * pSheet, unsigned nPosX, unsigned nPosY);

void sheet_updown(SHEET *sht, int height);

/**
 *     屏幕刷新
 *             
 *     @param sht   响应图层
 *     
 *     @return void
 */
void full_refresh(SHEET *sht);

/**
 *    获得最上层可操作sheet
 *           
 *    @return sht
 */
SHEET * get_max_top_sheet(void);

void sheet_refresh(SHEET *sht, int bx0, int by0, int bx1, int by1);

void sheet_refreshmap(SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);

/**
 *    关闭窗口
 *              
 *    @return (0:成功 1:失败)
 */
int sheet_free(SHEET *sht);
/**
 *   窗口创建
 *   
 *   @param sht_win 窗口层次结构体
 *
 *   @param vx0，vy0 窗口相对位置
 *
 *   @param bxsize，bysize 窗口大小
 *
 *   @param editFlg 是否可编辑 edit_off，edit_on
 *
 *   @param title   屏幕标题
 *     
 *   @return 反回创建成功失败
 */
int create_win(SHEET **sht_win, unsigned vx0, unsigned vy0, unsigned bxsize,
					unsigned bysize, int editFlg, const char * title);

int create_imge_win(SHEET **sht_win, unsigned vx0, unsigned vy0, unsigned bxsize,unsigned bysize,
		            TScreenColor bcolor,int style, const char * title);

/**
 *   窗口移动
 *
 *   @return void
 */
void win_sheet_slide(void);

/**
 *   图片加载  
 *              
 *   @param current_n_pos,switch_n_pos     当前图片以及迁移图片的描画位置
 *   
 *   @param current_width current_height   当前图片大小
 *
 *   @param switch_width switch_height     迁移图片大小
 *
 *   @param current_picBuff switch_picBuff 当前图片以及迁移图片缓存
 *
 *   @param current_type switch_tpye       当前图片以及迁移图片加载方式 -1:全部倒序加载 -2:全部正序加载
 *
 *   @return void
 */
void add_to_pic_by_switch(u32 current_width,u32 current_height,u32 switch_width,u32 switch_height,int current_n_pos,int switch_n_pos,unsigned char *current_picBuff,unsigned char *switch_picBuff,int current_type,int switch_type);

/**
 *   图片加载  
 *              
 *   @param sht        预加载图层
 *   
 *   @param pPicBuff   图片信息
 *
 *   @param nCurY      加载方式 -1:全部倒序加载 -2:全部正序加载 非负:按nCurY值分段加载
 *
 *   @param scaleType  加载样式
 *     
 *   @return void
 */
void add_to_piclayer(SHEET *sht, u32 width, u32 height, unsigned char * pPicBuff,int nCurY,int scaleType);

void add_to_piclayer_by_rgb565(SHEET *sht, u32 width, u32 height, unsigned char *pRgb565, int nCurY);

void hide_mouse_cursor(void);

/**
 *   图片加载  
 *              
 *   @param sht                 预加载图层
 *
 *   @param width height        图片大小
 *   
 *   @param pPicBuff            图片像素
 *
 *   @param nCurY               加载方式 -1:全部倒序加载 -2:全部正序加载 非负:按nCurY值分段加载
 *
 *   @param scaleType           图片加载样式
 *     
 *   @return void
 */
void lcd_draw_pic(SHEET *sht, u32 width, u32 height, unsigned char * pPicBuff,int nCurY,int scaleTyp);

#ifdef __cplusplus
}
#endif

#endif
