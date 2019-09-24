#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <alloc.h>
#include <graphic/graphic.h>
#include <armv7.h>
#include <cache_v7.h>
#include <s5p4418_serial_stdio.h>
#include <errno.h>

SHEET *m_tmp_sht_win = NULL;
SHEET *m_next_sht_win = NULL;

#if DEPTH == 16
static const TScreenColor s_COL8_000000 = COLOR16(0, 0, 0);
static const TScreenColor s_COL8_FFFFFF = COLOR16(255, 255, 255);
static const TScreenColor s_COL8_C6C6C6 = COLOR16(198, 198, 198);
static const TScreenColor s_COL8_000084 = COLOR16(0, 0, 132);
static const TScreenColor s_COL8_008484 = COLOR16(0, 132, 132);
static const TScreenColor s_COL8_848484 = COLOR16(132, 132, 132);
#elif DEPTH == 32
static const TScreenColor s_COL8_000000 = COLOR32(0, 0, 0,255);
static const TScreenColor s_COL8_FFFFFF = COLOR32(255, 255, 255,255);
static const TScreenColor s_COL8_C6C6C6 = COLOR32(198, 198, 198,255);
static const TScreenColor s_COL8_000084 = COLOR32(132, 0, 0,255);
static const TScreenColor s_COL8_008484 = COLOR32(132, 132, 0,255);
static const TScreenColor s_COL8_848484 = COLOR32(132, 132, 132,255);
#endif

//static boolean s_cursor_color_flg = TRUE; //纪录光标闪烁对应颜色
static unsigned s_total_line_num = 1; //窗口字符输入超长自动换行，纪录行数，需要修改为每个窗口一个
static boolean s_win_slide_flg = FALSE; //窗口移动标记
static boolean s_win_close_flg = FALSE; //窗口关闭标志
static boolean s_timer_flg = FALSE; //控制光标闪烁定时器是否已开启，防止重复
unsigned screen_width,screen_high,color_deep;

unsigned g_nMousePosX = 0; //鼠标左键按下选中窗口时鼠标位置
unsigned g_nMousePosY = 0; ////鼠标左键按下选中窗口时鼠标位置 
unsigned g_nWinPosX = 0; //鼠标左键按下选中窗口时窗口位置
unsigned g_nWinPosY = 0; //鼠标左键按下选中窗口时窗口位置
int g_nMouseMovTotalX = 0; //鼠标移动窗口过程中累计位移
int g_nMouseMovTotalY = 0; //鼠标移动窗口过程中累计位移
int g_nMouseMovMinX = 0; //鼠标移动窗口过程中鼠标位移范围
int g_nMouseMovMaxX = 0; //鼠标移动窗口过程中鼠标位移范围
int g_nMouseMovMinY = 0; //鼠标移动窗口过程中鼠标位移范围
int g_nMouseMovMaxY = 0; //鼠标移动窗口过程中鼠标位移范围

SHTCTL *shtctl;
SHEET *sht_back, *sht_mouse;
//static unsigned int _lcd_bpp = CFG_DISP_PRI_SCREEN_PIXEL_BYTE * 8;

TScreenColor *buf_back, buf_mouse[512],*buf_win;

static int s_win_ysize = 0;

SHTCTL *shtctl_init(TScreenColor *vram, int xsize/*屏幕的宽*/, int ysize/*屏幕的高*/)
{
   	SHTCTL *ctl;
    int i;
    ctl = (SHTCTL *) malloc(sizeof(SHTCTL));
    if (ctl == NULL)
	{
        return NULL;
    }
    ctl->map = (TScreenColor*) malloc(xsize * ysize * sizeof(TScreenColor));
 	if (ctl->map == NULL)
   	{
		free(ctl);
	   	return NULL;
   	}
    ctl->vram = vram;
    ctl->xsize = xsize;
    ctl->ysize = ysize;
	ctl->top = -1;
	for (i = 0; i < MAX_SHEETS; i++) 
	{
		ctl->sheets0[i].flags = 0;
		ctl->sheets0[i].ctl = ctl;
	}

	return ctl;
}

SHEET *sheet_alloc(SHTCTL *ctl)
{
    SHEET *sht;
    for (int i = 0; i < MAX_SHEETS; i++) 
	{
    	if (ctl->sheets0[i].flags == 0) 
		{
        	sht = &ctl->sheets0[i];
            sht->flags = 1; 
            sht->height = -1; 
            return sht;
        }
	}
    return 0;       
}

void draw_cursor(SHEET *sht,TScreenColor c, int x0, int y0, int x1, int y1)
{
    for (int y = y0; y < y1; y++) 
    {
 		for (int x = x0; x < x1; x++)
	   	{
	   		sht->buf[y*sht->bxsize + x] = c;
	   	}
    }
    sheet_refresh(sht,x0,y0,x1,y1);
    return;
}

void sheet_setbuf(SHEET *sht, TScreenColor *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
    sht->bxsize = xsize;
    sht->bysize = ysize;
    sht->col_inv = col_inv;
    return;
}

void boxfill(TScreenColor *vram, int xsize, TScreenColor c, int x0, int y0, int x1, int y1)
{
	//int tmpx0 = 0,tmpy0 = 0,tmpx1 = 0,tmpy1 = 0,width = 0,height = 0;
    for (int y = y0; y <= y1; y++)
    {
        for (int x = x0; x <= x1; x++)
        {
            vram[y * xsize + x] = c;
        }
    }
}

static void init_mouse_cursor(TScreenColor *mouse)
{
    static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
    for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++) 
		{
			if (cursor[y][x] == '*')
			{
            	mouse[y * 16 + x] = s_COL8_000000;
            }
            if (cursor[y][x] == 'O')
			{
            	mouse[y * 16 + x] = s_COL8_FFFFFF;
            }
            if (cursor[y][x] == '.')
			{
            	mouse[y * 16 + x] = 99;
            }
        }
    }
    return;
}

void hide_mouse_cursor()
{
    //int x,y;
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++) 
		{
            buf_mouse[y * 16 + x] = 99;
        }
    }
}

void make_win_for_pic(TScreenColor *buf,int xsize,int ysize,TScreenColor backgroundColor,int style,const char *title)
{
    if(!style)
	{
	    boxfill(buf, xsize, s_COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	    boxfill(buf, xsize, s_COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	    boxfill(buf, xsize, s_COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	    boxfill(buf, xsize, s_COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	    boxfill(buf, xsize, s_COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	    boxfill(buf, xsize, s_COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
		boxfill(buf, xsize, backgroundColor, 2, 2,xsize - 3, ysize - 3);
	    boxfill(buf, xsize, s_COL8_000084, 3,         3,         xsize - 4, 20       );
	    boxfill(buf, xsize, s_COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	    boxfill(buf, xsize, s_COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
		putfonts_asc(buf,xsize,3,3,s_COL8_000084,title);
	}
	else
	{
        boxfill(buf,xsize,backgroundColor,0,0,xsize-1,ysize-1);
	}
}

void make_window(TScreenColor *buf, int xsize, int ysize, int editFlag, const char *title)
{
	s_win_ysize = ysize;
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	char c;
	boxfill(buf, xsize, s_COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	boxfill(buf, xsize, s_COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	boxfill(buf, xsize, s_COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	boxfill(buf, xsize, s_COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	boxfill(buf, xsize, s_COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	boxfill(buf, xsize, s_COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);

    if (edit_on == editFlag)
    {
	    boxfill(buf, xsize, s_COL8_000000, 2, 2,xsize - 3, ysize - 3);
	}
    else
    {
        boxfill(buf, xsize, s_COL8_C6C6C6, 2, 2,xsize - 3, ysize - 3);
    }

	boxfill(buf, xsize, s_COL8_000084, 3,         3,         xsize - 4, 20       );
	boxfill(buf, xsize, s_COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	boxfill(buf, xsize, s_COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);

	putfonts_asc(buf,xsize,3,3,s_COL8_000084,title);
	//return;
	for (int y = 0; y < 14; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			c = closebtn[y][x];
			if (c == '@')
			{
        	     buf[(5 + y) * xsize + (xsize - 21 + x)]=s_COL8_000000;
			}
			else if (c == '$')
			{
                buf[(5 + y) * xsize + (xsize - 21 + x)]=s_COL8_848484;
			}
			else if (c == 'Q')
			{

                buf[(5 + y) * xsize + (xsize - 21 + x)]=s_COL8_C6C6C6;
			}
			else
			{

                buf[(5 + y) * xsize + (xsize - 21 + x)]=s_COL8_FFFFFF;
			}
		}
	}
	s_win_ysize = 0;
	return;
}

void init_screen(TScreenColor *vram, int x, int y)
{
    boxfill(vram, x, s_COL8_008484,  0,     0,      x -  1, y - 29);
    boxfill(vram, x, s_COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
    boxfill(vram, x, s_COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill(vram, x, s_COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

    boxfill(vram, x, s_COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
    boxfill(vram, x, s_COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
    boxfill(vram, x, s_COL8_848484,  3,     y -  4, 59,     y -  4);
    boxfill(vram, x, s_COL8_848484, 59,     y - 23, 59,     y -  5);
    boxfill(vram, x, s_COL8_000000,  2,     y -  3, 59,     y -  3);
    boxfill(vram, x, s_COL8_000000, 60,     y - 24, 60,     y -  3);
	
    boxfill(vram, x, s_COL8_848484, x - 47, y - 24, x -  4, y - 24);
    boxfill(vram, x, s_COL8_848484, x - 47, y - 23, x - 47, y -  4);
    boxfill(vram, x, s_COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
    boxfill(vram, x, s_COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);

    return;
}


void sheet_refreshsub(SHTCTL *ctl, int vx0/*左上角x*/, int vy0/*左上角y*/, int vx1/*右下角x*/, int vy1/*右下角y*/, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
    TScreenColor *buf, *vram = ctl->vram, *map = ctl->map, sid;
    SHEET *sht;

    if (vx0 < 0) { vx0 = 0; }
    if (vy0 < 0) { vy0 = 0; }
    if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
    if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
    for (h = h0; h <= h1; h++) 
	{
        sht = ctl->sheets[h];
        buf = sht->buf;
        sid = sht - ctl->sheets0;
        bx0 = vx0 - sht->vx0;
        by0 = vy0 - sht->vy0;
        bx1 = vx1 - sht->vx0;
        by1 = vy1 - sht->vy0;
        if (bx0 < 0) { bx0 = 0; }
        if (by0 < 0) { by0 = 0; }
        if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
        if (by1 > sht->bysize) { by1 = sht->bysize; }

        for (by = by0; by < by1; by++) 
		{
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++) 
			{
                vx = sht->vx0 + bx;
                if (map[vy * ctl->xsize + vx] == sid) 
				{
#ifdef SCREEN_HORIZONTAL
					vram[vx * ctl->ysize + ctl->ysize - vy] = buf[by * sht->bxsize + bx];
#else
                	vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];					
#endif	
                }
            }
        }

    }
	flush_dcache_all();
	//flush_cache((void *)&vram[(sht->vy0 + by0)*ctl->xsize + sht->vx0 + bx0], ((sht->vy0 + by1 - 1)*(ctl->xsize) + sht->vx0 + bx1 -1));
    return;
}

static void sheet_slide_refresh(SHEET *sht, int old_win_x, int old_win_y)
{
	SHTCTL *ctl = sht->ctl;

	int new_win_x = sht->vx0;
	int new_win_y = sht->vy0;

	int width = sht->bxsize;
	int height = sht->bysize;

	boolean old_refresh_flag[2] = {0};

	int old_need_area1[4] = {0};//0,左上角x，1，左上角y，2，右下角x，3，右下角y
	int old_need_area2[4] = {0};//0,左上角x，1，左上角y，2，右下角x，3，右下角y
	int new_need_area[4] = {0};//0,左上角x，1，左上角y，2，右下角x，3，右下角y

	new_need_area[0] = new_win_x;
	new_need_area[1] = new_win_y;
	new_need_area[2] = new_win_x + width;
	new_need_area[3] = new_win_y + height;		

	if (new_win_x - old_win_x >= width
		|| old_win_x - new_win_x >= width
		|| new_win_y - old_win_y >= height
		|| old_win_y - new_win_y >= height)
	{
		old_need_area1[0] = old_win_x;
		old_need_area1[1] = old_win_y;
		old_need_area1[2] = old_win_x + width;
		old_need_area1[3] = old_win_y + height;

		old_refresh_flag[0] = TRUE;
	}
	else
	{
		if (new_win_x > old_win_x)
		{
			old_need_area1[0] = old_win_x;
			old_need_area1[1] = old_win_y;
			old_need_area1[2] = new_win_x;
			old_need_area1[3] = old_win_y + height;

			old_refresh_flag[0] = TRUE;	

		}
		else if (new_win_x == old_win_x)
		{			/* code */
		}
		else
		{
			old_need_area1[0] = new_win_x + width;
			old_need_area1[1] = old_win_y;
			old_need_area1[2] = old_win_x + width;
			old_need_area1[3] = old_win_y + height;	

			old_refresh_flag[0] = TRUE;			
		}

		if (new_win_y > old_win_y)
		{

			old_need_area2[0] = old_win_x;
			old_need_area2[1] = old_win_y;
			old_need_area2[2] = old_win_x + width;
			old_need_area2[3] = new_win_y;

			old_refresh_flag[1] = TRUE;
		}
		else if (new_win_y == old_win_y)
		{
		}
		else
		{
			old_need_area2[0] = old_win_x;
			old_need_area2[1] = new_win_y + height;
			old_need_area2[2] = old_win_x + width;
			old_need_area2[3] = old_win_y + height;

			old_refresh_flag[1] = TRUE;
		}		
	}

	if (old_refresh_flag[0])
	{		
		sheet_refreshmap(ctl, old_need_area1[0], old_need_area1[1], old_need_area1[2], old_need_area1[3], 0);
		sheet_refreshsub(ctl, old_need_area1[0], old_need_area1[1], old_need_area1[2], old_need_area1[3], 0, sht->height - 1);
	}

	if (old_refresh_flag[1])
	{		
		sheet_refreshmap(ctl, old_need_area2[0], old_need_area2[1], old_need_area2[2], old_need_area2[3], 0);
		sheet_refreshsub(ctl, old_need_area2[0], old_need_area2[1], old_need_area2[2], old_need_area2[3], 0, sht->height - 1);
	}



	sheet_refreshmap(ctl, new_need_area[0], new_need_area[1], new_need_area[2], new_need_area[3], sht->height);
	sheet_refreshsub(ctl, new_need_area[0], new_need_area[1], new_need_area[2], new_need_area[3], sht->height, sht->height);	
}

void mouse_sheet_slide(SHEET *sht, int nDisplacementX, int nDisplacementY)
{
    SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0;
    int old_vy0 = sht->vy0;
    sht->vx0 += nDisplacementX;
    sht->vy0 += nDisplacementY;
    if(sht->vx0 < 0)
    {
        sht->vx0 = 0;
    }
    if(sht->vy0 < 0)
    {
        sht->vy0 = 0;
    }
    if (sht->vx0 > sht_back->bxsize - 1)
    {
        sht->vx0 = sht_back->bxsize - 1;
    }
    if (sht->vy0 > sht_back->bysize - 1)
    {
        sht->vy0 = sht_back->bysize - 1;
    }
	if (sht->height >= 0) 
	{
        sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize,
                        old_vy0 + sht->bysize, 0);
        sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, 
                        sht->vy0 + sht->bysize, sht->height);
        sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, 
                        old_vy0 + sht->bysize, 0, sht->height - 1);
        sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, 
                        sht->vy0 + sht->bysize, sht->height, sht->height);
	}
}

void sheet_updown(SHEET *sht, int height)
{
	SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; 

	if (height > ctl->top + 1) 
	{
		height = ctl->top + 1;
	}
	if (height < -1) 
	{
		height = -1;
	}
	sht->height = height; 

	if (old > height) 
	{	
		if (height >= 0) 
		{
			for (h = old; h > height; h--) 
			{
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
					sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, 
					sht->vy0 + sht->bysize, height + 1, old);
		}
		else 
		{	
			if (ctl->top > old) 
			{
				for (h = old; h < ctl->top; h++) 
				{
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; 
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
					sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
					sht->vy0 + sht->bysize, 0, old - 1);
		}
	} 
	else if (old < height) 
	{
		if (old >= 0) 
		{	
			for (h = old; h < height; h++) 
			{
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} 
		else 
		{	
			for (h = ctl->top; h >= height; h--) 
			{
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; 
		}

		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
						sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize,
				sht->vy0 + sht->bysize, height, height);
	}
}

void sheet_refresh(SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) 
	{
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0,
						sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
}

int sheet_free(SHEET *sht)
{
	if (sht->height >= 0)
	{
	    sheet_updown(sht, -1);
	}
	sht->flags = 0;

	if(sht->picBuf != NULL)
	{
	    free(sht->picBuf);
	}
    free(sht->buf);
	m_tmp_sht_win = get_max_top_sheet();
	return 0;
}

int GetPixel (char chAscii, unsigned nPosX, unsigned nPosY)
{
    unsigned nAscii = (u8) chAscii;
    unsigned nIndex = nAscii - 0x21;
    return font_data[nIndex][nPosY] & (0x80 >> nPosX) ? 1 : 0;
}

void putfont(TScreenColor *vram, int xsize, int x0, int y0, TScreenColor c,char chChar)
{
	int font;

	TScreenColor color;
	if (' ' == chChar)
	{
		for (int y = 0; y < WORD_HEIGHT; y++) 
		{
			for (int x = 0; x < WORD_WIDTH; x++)
			{
			    vram[(y0+y)*xsize + x0+x] = c;
			}
		}

		return;
	}

	for (int y = 0; y < WORD_HEIGHT; y++) 
	{
        for (int x = 0; x < WORD_WIDTH; x++)
		{
			font = GetPixel(chChar,x,y);
			if (font)
			{
				color =  s_COL8_FFFFFF;
			}
			else
			{
				color =  c;
			}

		    vram[(y0+y)*xsize + x0+x] = color;
		}
	}
}

void putfonts_asc(TScreenColor *vram, int xsize, int x, int y, TScreenColor c, const char *s)
{
	for (; *s != 0x00; s++) 
	{ 
		putfont(vram, xsize, x, y, c,*s);
		x += WORD_WIDTH;
	}
}


void sheet_slide(SHEET *sht, int vx0, int vy0)
{
	int old_vx0 = sht->vx0;
    int old_vy0 = sht->vy0;
    sht->vx0 = vx0;
    sht->vy0 = vy0;

    if(sht->vx0 < 0)
    {
        sht->vx0 = 0;
    }
    if(sht->vy0 < 0)
    {
        sht->vy0 = 0;
    }
    if (sht->vx0 > sht_back->bxsize -2)
    {
        sht->vx0 = sht_back->bxsize - 2;
    }
    if (sht->vy0 > sht_back->bysize - 2)
    {
        sht->vy0 = sht_back->bysize - 2;
    }

    if (old_vx0 == sht->vx0 && old_vy0 == sht->vy0)
    {
        return;
    }

	if (sht->height >= 0) 
	{
		sheet_slide_refresh(sht, old_vx0, old_vy0);
	}
}

// int convertPointToFrameBuf(int vmx0, int vmy0, int *outX, int *outY)
// {
// #ifdef SCREEN_HORIZONTAL
// 	*outX = CFG_DISP_PRI_RESOL_HEIGHT - vmy0;
// 	*outY = vmx0;
// #else
// 	*outX = vmx0;
// 	*outY = vmy0;	
// #endif		
// }

// int convertPointToSheet(int vmx0, int vmy0, int *outX, int *outY)
// {
// #ifdef SCREEN_HORIZONTAL
// 	*outX = vmy0;
// 	*outY = CFG_DISP_PRI_RESOL_HEIGHT - vmx0;
// #else
// 	*outX = vmx0;
// 	*outY = vmy0;
// #endif	
// }


void sheet_slide_step(SHEET *sht, int dx, int dy)
{

	if(m_tmp_sht_win->current_status == 1)
	{
	    return;
	}
    int old_sheet_vx0 = sht->vx0;
    int old_sheet_vy0 = sht->vy0;

    sht->vx0 = old_sheet_vx0 + dx;
    sht->vy0 = old_sheet_vy0 + dy;

    if(sht->vx0 < 0) 
    {
        sht->vx0 = 0;
    }
    if(sht->vy0 < 0)
    {
        sht->vy0 = 0;
    }
    if (sht->vx0 > sht_back->bxsize -2)
    {
        sht->vx0 = sht_back->bxsize - 2;
    }
    if (sht->vy0 > sht_back->bysize - 2) 
    {
        sht->vy0 = sht_back->bysize - 2;
    }

    if (old_sheet_vx0 == sht->vx0 && old_sheet_vy0 == sht->vy0)
    {
        return;
    }

    if (sht->height >= 0) 
    {
        sheet_slide_refresh(sht, old_sheet_vx0, old_sheet_vy0);
    }
}

boolean isWindowCloseArea(SHEET * pSheet, unsigned nPosX, unsigned nPosY)
{
    unsigned nMinX = pSheet->vx0 + pSheet->bxsize - 21;
    unsigned nMaxX = pSheet->vx0 + pSheet->bxsize - 6;
    unsigned nMinY = pSheet->vy0 + 5;
    unsigned nMaxY = pSheet->vy0 + 18;

    // unsigned curPressSheetX;
    // unsigned curPressSheetY;

//    convertPointToSheet(nPosX, nPosY, &curPressSheetX, &curPressSheetY);

    if (nPosX >= nMinX
         && nPosX <= nMaxX
         &&  nPosY >= nMinY
         && nPosY <= nMaxY)
    {
        return TRUE;
    }

    return FALSE;
}


//根据坐标返回对应窗口，如果是背景或者鼠标，返回0
SHEET * get_current_sheet(int vmx0, int vmy0)
{							
	// int curPosX;
	// int curposY;

	// convertPointToSheet(vmx0, vmy0, &curPosX, &curposY);

    int layer = 0;
    for (int i = shtctl->top - 1; i > 0; i--)
    {
   		int xsize = shtctl->sheets[i]->bxsize;
        int ysize = shtctl->sheets[i]->bysize;
        int vx0 = shtctl->sheets[i]->vx0;
        int vy0 = shtctl->sheets[i]->vy0;
        if (vmx0 >= vx0 && vmx0 < vx0 + xsize && vmy0 >= vy0 && vmy0 < vy0 + ysize)
        {
   			layer = i;
            break;
        }
    }

    if (layer != 0)
    {
        SHEET * pSheet = shtctl->sheets[layer];
        // if (!isWindowCloseArea(pSheet, curPosX, curposY))
        // {
        sheet_updown(pSheet, shtctl->top - 1);
      	//  }
        m_tmp_sht_win = pSheet;
    
        return pSheet;
    }

    return 0;
}

void keyboard(SHEET *sht,const char *input,int len)
{
 	if(edit_on != sht->editFlag)
  	{
  		return;
   	}

	int cursorx = sht->cursorx;
	int cursory = sht->cursory;

    switch(*input)
    {
        case 0x7F:
            if(cursorx == WIN_LEFT_PADDING && s_total_line_num == 1)
            {
				//在第一行行首位置，退格键无效
                return;
            }

            if (cursorx == WIN_LEFT_PADDING)
            {
				//已经换行，在新行行首位置，清除光标
                draw_cursor(sht, s_COL8_000000, cursorx, cursory,
							cursorx + WORD_WIDTH, cursory + WORD_HEIGHT);
                sht->cursorx  = sht->bxsize - WIN_RIGHT_PADDING - WORD_WIDTH;
				sht->cursory -= WORD_HEIGHT;
				s_total_line_num--;
            }
            else
            {
				//无论是否已经换行，不在行首位置，清除光标以及前一个字符
                draw_cursor(sht, s_COL8_000000, cursorx - WORD_WIDTH, cursory, 
							cursorx + WORD_WIDTH, cursory + WORD_HEIGHT);
            	sht->cursorx -= WORD_WIDTH;
            }
            break;

        default:
            if(len > 0)
            {
            	write_asc2(sht,input,len);
            }
            else
            {
           		write_asc(sht,input);
            }
			break;
    }
}

void sheet_refreshmap(SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	TScreenColor *buf, sid, *map = ctl->map;
	SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) 
	{
	    sht = ctl->sheets[h];
	    sid = sht - ctl->sheets0; 
	    buf = sht->buf;
	    bx0 = vx0 - sht->vx0;
	    by0 = vy0 - sht->vy0;
	    bx1 = vx1 - sht->vx0;
	    by1 = vy1 - sht->vy0;
	    if (bx0 < 0) { bx0 = 0; }
	    if (by0 < 0) { by0 = 0; }
	    if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
	    if (by1 > sht->bysize) { by1 = sht->bysize; }
        if (sht->col_inv == -1) {
			for (by = by0; by < by1; by++) 
			{
		    	vy = sht->vy0 + by;
		    	for (bx = bx0; bx < bx1; bx++) 
				{
		        	vx = sht->vx0 + bx;
					map[vy * ctl->xsize + vx] = sid;
		     	}
			}
		}
		else
		{
			for (by = by0; by < by1; by++) 
			{
		    	vy = sht->vy0 + by;
		    	for (bx = bx0; bx < bx1; bx++) 
				{
					vx = sht->vx0 + bx;
			    	if (buf[by * sht->bxsize + bx] != (u32)sht->col_inv) 
					{
			        	map[vy * ctl->xsize + vx] = sid;
			   		}
		    	}
			}
		}
	}
	return;
}

void keyboard_event2(SHEET *sht,const char *input,int len)
{
    keyboard(sht,input,len);
}

void keyboard_event(SHEET *sht,const char *input)
{
    keyboard(sht,input,0);
}

void win_sheet_slide()
{
    if (s_win_slide_flg)
    {
        sheet_slide(sht_mouse, g_nMousePosX + g_nMouseMovTotalX, g_nMousePosY + g_nMouseMovTotalY);
        sheet_slide(m_tmp_sht_win, g_nWinPosX + g_nMouseMovTotalX, g_nWinPosY + g_nMouseMovTotalY);
    }
}

//不包括右上角的"X"范围
static boolean isWindowTitleArea(SHEET * pSheet, unsigned int nPosX, int nPosY)
{
    unsigned int nMinX = pSheet->vx0;
    unsigned int nMaxX = pSheet->bxsize + nMinX - 22;
    int nMinY = pSheet->vy0;
    int nMaxY = 20 + nMinY;

    if (nPosX >= nMinX && nPosX <= nMaxX
         && nPosY >= nMinY && nPosY <= nMaxY)
    {
        return TRUE;
    }

    return FALSE;
}


void mouse_event(unsigned nEvent, unsigned nButtons, int nDisplacementX, int nDisplacementY,
				SHEET **ppSheet, unsigned *pFlg)
{
    switch(nEvent)
    {
        case MouseEventMouseMove:
            if (s_win_slide_flg)
            {
                g_nMouseMovTotalX += nDisplacementX;
                if (g_nMouseMovTotalX < g_nMouseMovMinX)
                {
                    g_nMouseMovTotalX = g_nMouseMovMinX;
                }
                else if (g_nMouseMovTotalX > g_nMouseMovMaxX)
                {
                    g_nMouseMovTotalX = g_nMouseMovMaxX;
                }

                g_nMouseMovTotalY += nDisplacementY;
                if (g_nMouseMovTotalY < g_nMouseMovMinY)
                {
                    g_nMouseMovTotalY = g_nMouseMovMinY;
                }
                else if (g_nMouseMovTotalY > g_nMouseMovMaxY)
                {
                    g_nMouseMovTotalY = g_nMouseMovMaxY;
                }
                *pFlg = SheetSelected;
            }
            else
            {
                mouse_sheet_slide(sht_mouse, nDisplacementX, nDisplacementY);
                *pFlg = SheetInvalid;
                *ppSheet = 0;
            }
            break; 
        case MouseEventMouseDown:
            if (nButtons != MOUSE_BUTTON_LEFT)
            {
                *pFlg = SheetInvalid;
                *ppSheet = 0;
                break;
            }
            //write_uart("MouseEventMouseDown\n");
            SHEET * pTmpSheet = get_current_sheet(sht_mouse->vx0 ,sht_mouse->vy0);
            if (pTmpSheet != 0)
            {
                *ppSheet = pTmpSheet;
                //m_tmp_sht_win = pTmpSheet;
                *pFlg = SheetSelected;

                if (isWindowTitleArea(m_tmp_sht_win, sht_mouse->vx0, sht_mouse->vy0))
                {
                    //非窗口右上角"X"范围
                    s_win_slide_flg = TRUE;

                    g_nMousePosX = sht_mouse->vx0;
                    g_nMousePosY = sht_mouse->vy0;
                    g_nWinPosX = (*ppSheet)->vx0;
                    g_nWinPosY = (*ppSheet)->vy0;
                    //窗口可移动范围
                    g_nMouseMovMinX = -g_nWinPosX;
                    g_nMouseMovMaxX = sht_back->bxsize - 2 - g_nMousePosX;
                    g_nMouseMovMinY = -g_nWinPosY;
                    g_nMouseMovMaxY =sht_back->bysize - 2 - g_nMousePosY;

                    g_nMouseMovTotalX = 0;
                    g_nMouseMovTotalY = 0;
                }
                else if (isWindowCloseArea(m_tmp_sht_win, sht_mouse->vx0, sht_mouse->vy0))
                {
                    s_win_close_flg = TRUE;
                }
            }
            else
            {
                *ppSheet = 0;
                *pFlg = SheetInvalid;
            }
            break;
        case MouseEventMouseUp:
            if (nButtons != MOUSE_BUTTON_LEFT)
            {
                *ppSheet = 0;
                *pFlg = SheetInvalid;
                break;
            }
            //write_uart("MouseEventMouseUp\n");
            win_sheet_slide();
            s_win_slide_flg = FALSE;
            pTmpSheet = get_current_sheet(sht_mouse->vx0 ,sht_mouse->vy0);

            //之前按下时点中了指定窗口的"X"，并且抬起时仍然是同一个窗口的"X"
            if (pTmpSheet == m_tmp_sht_win && TRUE == s_win_close_flg
                    && isWindowCloseArea(m_tmp_sht_win, sht_mouse->vx0, sht_mouse->vy0))
            {
                *ppSheet = pTmpSheet;
                *pFlg = SheetClosed;
            }
            else
            {
                *ppSheet = 0;
                *pFlg = SheetInvalid;
            }
            s_win_close_flg = FALSE;
            break;
        default:
            break;
    }
}

unsigned nTouchOffsetXForWin = 0;
unsigned nTouchOffsetYForWin = 0;

void touchscreen_event(unsigned nEvent, unsigned nPosX, unsigned nPosY, SHEET **ppSheet, unsigned *pFlg)
{
    SHEET * pTmpSheet = 0;
    switch(nEvent)
    {
        case FingerMove:
			if(m_tmp_sht_win->current_status == 1)
			{
				return;
			}
            if (s_win_slide_flg)
            {
 				sheet_slide((*ppSheet), nPosX - nTouchOffsetXForWin, nPosY - nTouchOffsetYForWin);
            }
            break; 

        case FingerDown:
            pTmpSheet = get_current_sheet(nPosX, nPosY);
            if (0 != pTmpSheet)
            {
                *ppSheet = pTmpSheet;
                //m_tmp_sht_win = pTmpSheet;
                *pFlg = SheetSelected;
                
                if (isWindowTitleArea(m_tmp_sht_win, nPosX, nPosY))
                {
                    //非窗口右上角"X"范围
                    s_win_slide_flg = TRUE;
                    nTouchOffsetXForWin = nPosX - m_tmp_sht_win->vx0;
                    nTouchOffsetYForWin = nPosY - m_tmp_sht_win->vy0;
                }
                else if (isWindowCloseArea(m_tmp_sht_win, nPosX, nPosY))
                {
                    s_win_close_flg = TRUE;
                }
            }
            else
            {
                *pFlg = SheetInvalid;
                *ppSheet = 0;
            }
            break;

        case FingerUp:
            s_win_slide_flg = FALSE;

            nTouchOffsetXForWin = 0;
            nTouchOffsetYForWin = 0;

            pTmpSheet = get_current_sheet(nPosX ,nPosY);
            //之前按下时点中了指定窗口的"X"，并且抬起时仍然是同一个窗口的"X"
            if (pTmpSheet == m_tmp_sht_win && TRUE == s_win_close_flg
                    && isWindowCloseArea(m_tmp_sht_win, nPosX, nPosY))
            {
                *ppSheet = pTmpSheet;
                *pFlg = SheetClosed;
            }
            else
            {
                *ppSheet = 0;
                *pFlg = SheetInvalid;
            }
            s_win_close_flg = FALSE;
            break;

        default:
            break;
	}
}

//void timerHandler (unsigned hTimer, void *pParam, void *pContext)
//{  
// /*   StartKernelTimer (CURSOR_C_TIME * HZ, timerHandler,0,0);
//    if (edit_on != m_tmp_sht_win->editFlag)
//    {
//        return;
//    }
//    int cursorx  = m_tmp_sht_win->cursorx;
//    int cursory  = m_tmp_sht_win->cursory;
//    TScreenColor cursor_color = s_cursor_color_flg ? s_COL8_FFFFFF : s_COL8_000000;
//    draw_cursor(m_tmp_sht_win, cursor_color, cursorx, cursory, 
//				cursorx + WORD_WIDTH, cursory + WORD_HEIGHT);
//    s_cursor_color_flg = !s_cursor_color_flg;*/
//}

int create_imge_win(    SHEET **sht_win, 
                        unsigned vx0, unsigned vy0, 
                        unsigned bxsize,unsigned bysize,
		                TScreenColor bcolor,
                        int style, 
                        const char * title)
{
	int tmp = bxsize % 4;
	if(tmp != 0)
	{
		bxsize += 4-tmp;
	}

	TScreenColor *buf_win;
	*sht_win = sheet_alloc(shtctl);
	
    buf_win = (TScreenColor *) malloc(bxsize*bysize*sizeof(TScreenColor));
    if(buf_win == NULL)
        return ENOMEM;
	
    sheet_setbuf(*sht_win, buf_win, bxsize, bysize, -1);
	(*sht_win)->editFlag        = 0;
	(*sht_win)->cursorx         = 0;
	(*sht_win)->cursory         = 0;
	(*sht_win)->style           = style;
	(*sht_win)->zoom_status     = 0;
	(*sht_win)->current_status  = 0;
	(*sht_win)->current_scale   = 1;
	(*sht_win)->create_flg      = 1;
	make_win_for_pic(buf_win,bxsize,bysize,bcolor,style,title);
	//make_window(buf_win,bxsize,bysize,0,title);
	m_tmp_sht_win = *sht_win;
	sheet_slide(*sht_win, vx0 & ~1, vy0);
	sheet_updown(*sht_win,shtctl->top);

	(*sht_win)->pre_pic_sht_win = 0;
	(*sht_win)->next_pic_sht_win = 0;
	(*sht_win)->current_pic_sht_win = *sht_win;
	//LogWrite ("11111", LOG_NOTICE, "num=%d,%d",(*sht_win)->height,shtctl->top);
	for(int i = shtctl->top - 2 ; i > 0 ; i--)
    {
         m_next_sht_win = shtctl->sheets[i];
		 (*sht_win)->next_pic_sht_win = shtctl->sheets[i];
		 if(!m_next_sht_win->zoom_status)
		 {
			 break; 
		 } 
		 else
		 {
			 m_next_sht_win = 0;
			 (*sht_win)->next_pic_sht_win = 0;
		 }
    }

	return 0;
}

int create_win(SHEET **sht_win, unsigned vx0, unsigned vy0, unsigned bxsize,
					unsigned bysize, int editFlg, const char * title)
{
	TScreenColor *buf_win;
    *sht_win = sheet_alloc(shtctl);
	if (edit_on == editFlg || edit_off == editFlg)
	{
   		if (bxsize < WORD_WIDTH + WIN_LEFT_PADDING + WIN_RIGHT_PADDING)
   		{
   			bxsize = WORD_WIDTH + WIN_LEFT_PADDING + WIN_RIGHT_PADDING;
		}
		else
		{
			unsigned left = (bxsize - (WIN_LEFT_PADDING + WIN_RIGHT_PADDING)) % WORD_WIDTH;
			if (0 != left)
			{
				bxsize = bxsize + WORD_WIDTH - left;
			}
		}
		if (bysize < WORD_HEIGHT + WIN_TOP_PADDING + WIN_BOTTOM_PADDING)
		{
			bysize = WORD_HEIGHT + WIN_TOP_PADDING + WIN_BOTTOM_PADDING;
		}
		else
		{
			unsigned left = (bysize - (WIN_TOP_PADDING + WIN_BOTTOM_PADDING)) % WORD_HEIGHT;
			if (0 != left)
			{
				bysize = bysize + WORD_HEIGHT - left;
			}
		}
	}
	buf_win = (TScreenColor *) malloc(bxsize*bysize*sizeof(TScreenColor));
    if(buf_win == NULL)
        return ENOMEM;
	sheet_setbuf(*sht_win, buf_win, bxsize, bysize, -1);
    (*sht_win)->editFlag = editFlg;
    (*sht_win)->cursorx = 0;
    (*sht_win)->cursory = 0;
    make_window(buf_win,bxsize,bysize,editFlg,title);
    (*sht_win)->cursorx = WIN_LEFT_PADDING;
    (*sht_win)->cursory = WIN_TOP_PADDING;
	(*sht_win)->picBuf = 0;
	(*sht_win)->current_scale = 0;
	(*sht_win)->zoom_status = -1;
    m_tmp_sht_win = *sht_win;
	// if(CHANGE_COORDINATE)
	// {
 //        sheet_slide(*sht_win, (CFG_DISP_PRI_RESOL_WIDTH - vy0 - bxsize) & ~1 , vx0);
	// }
	// else
//	{
    	sheet_slide(*sht_win, vx0, vy0);
//	}

    sheet_updown(*sht_win,shtctl->top);
    if (edit_on == editFlg && (*sht_win)->height == shtctl->top-1 && !s_timer_flg)
    {
		s_timer_flg = TRUE;
		//StartKernelTimer (CURSOR_C_TIME * HZ, timerHandler,0,0);
    }
    return 0;
}

void rollScreen(SHEET *sht)
{
	for (int y = WIN_TOP_PADDING; y < sht->bysize - WIN_BOTTOM_PADDING - WORD_HEIGHT; y++) 
	{
		for (int x = WIN_LEFT_PADDING; x < sht->bxsize - WIN_RIGHT_PADDING; x++) 
		{
			sht->buf[x + y * sht->bxsize] = sht->buf[x + (y + WORD_HEIGHT) * sht->bxsize];
		} 
	}

	//最后一行清空
	for (int y = sht->bysize - WIN_BOTTOM_PADDING - WORD_HEIGHT; y < sht->bysize - WIN_BOTTOM_PADDING; y++)
	{
		for (int x = WIN_LEFT_PADDING; x < sht->bxsize - WIN_RIGHT_PADDING; x++)
		{
			sht->buf[x + y * sht->bxsize] = s_COL8_000000;
		}
	}
	sheet_refresh(sht, WIN_LEFT_PADDING, WIN_TOP_PADDING,
				sht->bxsize - WIN_RIGHT_PADDING, sht->bysize- WIN_BOTTOM_PADDING);

	sht->cursorx = WIN_LEFT_PADDING;
	sht->cursory = sht->bysize - WIN_BOTTOM_PADDING - WORD_HEIGHT;
}

void write(SHEET *sht,const char *input,int len)
{
	TScreenColor backColor;
	if (edit_off == sht->editFlag)
	{
		backColor = s_COL8_C6C6C6;
	}
	else if (edit_on == sht->editFlag)
	{
		backColor = s_COL8_000000;
	}
	else
	{
		return ;
	}
    char tmp[2] = {0};
    int count = 0;
    while (count < len)
    {
        tmp[0] = input[count];
		if ('\n' == tmp[0])
		{
			s_total_line_num = 1;
			int cursorx = sht->cursorx;
			int cursory = sht->cursory;
			//换行，清除当前光标
            draw_cursor(sht, s_COL8_000000, cursorx, cursory, cursorx + WORD_WIDTH, cursory + WORD_HEIGHT);
            sheet_refresh(sht, cursorx, cursory, cursorx + WORD_WIDTH, cursory + WORD_HEIGHT);

			//输入回车之前已经显示到当前窗口最后一行，需要向上滚动窗口
            if(sht->cursory == sht->bysize - (WIN_BOTTOM_PADDING + WORD_HEIGHT))
            {
				rollScreen(sht);
            }
			else
			{
            	sht->cursorx = WIN_LEFT_PADDING;
            	sht->cursory += WORD_HEIGHT;
			}
			count ++;
			continue;
		}

		//普通字符输出到窗口，包括空格
        putfonts_asc(sht->buf, sht->bxsize, sht->cursorx, sht->cursory, backColor, tmp);

        sheet_refresh(sht,sht->cursorx, sht->cursory, sht->cursorx + WORD_WIDTH, sht->cursory + WORD_HEIGHT);
        sht->cursorx += WORD_WIDTH;

		//输入超长需要换行
        if(sht->cursorx == sht->bxsize - WIN_RIGHT_PADDING)
        {
             sht->cursorx  = WIN_LEFT_PADDING;
             sht->cursory += WORD_HEIGHT;
			 
			 //需要向上滚屏
			 if (sht->cursory == sht->bysize - WIN_BOTTOM_PADDING)
             {
				 rollScreen(sht);
             }
             s_total_line_num++;
		}
        count ++;
    }
}

void write_asc2(SHEET *sht,const char *input, int len)
{
    write(sht,input,len);
}

void write_asc(SHEET *sht, const char *input)
{   
    int len  = strlen(input);
    write(sht,input,len);
}
void write_asc_position(SHEET *sht,int vx0, int vy0,const char *input)
{
	sht->cursorx = WIN_LEFT_PADDING + vx0;
    sht->cursory = WIN_TOP_PADDING + vy0;
    write_asc(sht,input);
}

void full_refresh(SHEET *sht)
{   
    boxfill(sht->buf, sht->bxsize, s_COL8_C6C6C6, WIN_LEFT_PADDING,WIN_TOP_PADDING,
			sht->bxsize - WIN_LEFT_PADDING, sht->bysize - WIN_TOP_PADDING);
    sheet_refresh(sht, WIN_LEFT_PADDING, WIN_TOP_PADDING,
				sht->bxsize - WIN_LEFT_PADDING, sht->bysize - WIN_TOP_PADDING);
}

SHEET * get_max_top_sheet()
{
    return shtctl->sheets[shtctl->top-1];    
}

TScreenColor rgb_24_2_565(int r, int g, int b)  
{  
    return (TScreenColor)((((unsigned)r << 8) & 0xF800) |   
            (((unsigned)g << 3) & 0x7E0)  |  
            (((unsigned)b >> 3)));  
} 

//static void putPixel888to888(u32 base,int xpos,int ypos,int width,int height,u32 color)
//{
//	 base = base + (ypos * width + xpos) * 3;
//	 *(u8*)((unsigned long)(base++)) = ((color>> 0)&0xFF);   // B
//	 *(u8*)((unsigned long)(base++)) = ((color>> 8)&0xFF);   // G
//	 *(u8*)((unsigned long)(base))   = ((color>>16)&0xFF);   // R
//}

//static void putPixel888To8888(u32  base,int  xpos,int  ypos,int  width,int  height,u32  color)
//{
//    *(u32*)((unsigned long)(base + (ypos * width + xpos) * 4)) = (0xFF000000) | (color & 0xFFFFFF);
//}

void add_to_piclayer_by_rgb565(SHEET *sht, u32 width, u32 height, unsigned char *pRgb565, int nCurY)
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
	TScreenColor * pScreenColorBuf = sht->buf;	
	int nSheetBxsize = sht->bxsize;

 	
 	u32 dvy = 0;
 	u32 dvx = 0;
    nCurY = (WIN_TOP_PADDING + dvy - 1) * nSheetBxsize  + WIN_LEFT_PADDING;
    for (u32 y = dvy; y < height+dvy; y++) 
    {    
        for (u32 x = dvx; x < width+dvx; x++) 
        {    
            b = *pRgb565++;
            g = *pRgb565++;
            r = *pRgb565++;
 
           pScreenColorBuf[nCurY + x] = rgb_24_2_565(r,g,b);
        }    
        nCurY += nSheetBxsize;
    }  

    sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);
  
}

void add_to_pic_by_switch(  u32 current_width,  u32 current_height,
                            u32 switch_width,   u32 switch_height,
                            int current_n_pos,  int switch_n_pos,
                            unsigned char *current_picBuff ,
                            unsigned char *switch_picBuff,
                            int current_type,
                            int switch_type)
{
	unsigned char r,g,b;
	TScreenColor * pScreenColorBuf = m_tmp_sht_win->buf;
	int nSheetBxsize = m_tmp_sht_win->bxsize,vSheetBxsize = m_tmp_sht_win->bxsize,vSheetBysize = m_tmp_sht_win->bysize;
	int current_nCurY=0,switch_nCurY=0;
	u32 dvy0 = 0,dvy1 =0;
	dvy0 = (vSheetBysize - current_height)/2;
	dvy1 = (vSheetBysize - switch_height)/2;
	//当前图片移动处理
	if(current_type == -1)
	{
		current_nCurY = (current_height + dvy0 - 1) * nSheetBxsize;
	}
	else if(current_type == -2)
	{
		current_nCurY = dvy0 * nSheetBxsize;
	}
    for (u32 y = dvy0; y < current_height + dvy0; y++) 
    {    
        for (int x = current_n_pos; x < (int)current_width + current_n_pos; x++) 
        {    
            b = *current_picBuff++;
            g = *current_picBuff++;
            r = *current_picBuff++;     
            pScreenColorBuf[current_nCurY + x] = rgb_24_2_565(r,g,b);
        }    
		if(current_type == -1)
		{
            current_nCurY -= nSheetBxsize;
		}
		else
		{
		    current_nCurY += nSheetBxsize;
		}
    }    
    //迁移图片移动处理
	if(switch_type == -1)
	{
		switch_nCurY = (switch_height + dvy1-1) * nSheetBxsize;
	}
	else if(switch_type == -2)
	{
		switch_nCurY = dvy1 * nSheetBxsize;
	}
    for (u32 y = dvy1;y < switch_height + dvy1; y++) 
    {    
        for (int x = switch_n_pos; x < (int)switch_width + switch_n_pos; x++) 
        {    
            b = *switch_picBuff++;
            g = *switch_picBuff++;
            r = *switch_picBuff++;
            pScreenColorBuf[switch_nCurY + x] = rgb_24_2_565(r,g,b);
        } 
	    if(switch_type == -1)
		{	
            switch_nCurY -= nSheetBxsize;
		}
		else
		{
		    switch_nCurY += nSheetBxsize;
		}
    }    
	//刷新整个sheet
	sheet_refresh(m_tmp_sht_win, 0, 0,vSheetBxsize,vSheetBysize);
}

int total_pic = 0;
void add_to_piclayer(SHEET *sht, u32 width, u32 height, unsigned char * pPicBuff,int nCurY,int scaleTpye)
{
	unsigned char r,g,b;
    TScreenColor * pScreenColorBuf = sht->buf;
	int nSheetBxsize = sht->bxsize, vSheetBxsize, vSheetBysize;
	int dvx = 0,dvy = 0,x = 0,y = 0;
	if(sht->style == 1)
	{
        vSheetBxsize = sht->bxsize;
		vSheetBysize = sht->bysize;
	}
	else
	{
	    vSheetBxsize = sht->bxsize-(WIN_LEFT_PADDING+WIN_RIGHT_PADDING);
	    vSheetBysize = sht->bysize-(WIN_TOP_PADDING + WIN_BOTTOM_PADDING);
	}

	if(scaleTpye == CENTER || scaleTpye == CENTER_INSIDE || scaleTpye ==FIT_CENTER || scaleTpye ==FIT_XY || scaleTpye == CENTER_CROP || scaleTpye == SWITCH)
	{	
	    if(vSheetBxsize - (int)width >= 2)
 	    {
	        dvx = (vSheetBxsize - width)/2; 
	    }
	    if(vSheetBysize - (int)height >= 2)
     	{
	        dvy = (vSheetBysize - (int)height)/2;
        }
		if(scaleTpye == SWITCH)
		{
			dvx = sht->current_pic_sht_win->n_pos_x;
		}
	}


	if(scaleTpye == FIT_END)
	{
		if(vSheetBysize > (int)height)
		{
            dvy =  vSheetBysize - (int)height;
		}
		if(vSheetBxsize > (int)width)
		{
			dvx = vSheetBxsize - width;
		}
	}
	if(sht->create_flg == 1)
	{
        sht->pvx0 = sht->vx0+dvx;
	    sht->pvy0 = sht->vy0+dvy;
		sht->create_flg = 0;
		sht->n_pos_x = dvx;
	}

    if(nCurY > 0)
    {    
        for(y=dvy ; y<2+dvy; y++)
        {    
            for (x = dvx; x < (int)width+dvx; x++) 
            {    
                b = *pPicBuff++;
                g = *pPicBuff++;
                r = *pPicBuff++;

				#if DEPTH == 32
				pScreenColorBuf[nCurY + x] = COLOR32(b,g,r,255);
	            #elif DEPTH == 16
			    pScreenColorBuf[nCurY + x] = rgb_24_2_565(r,g,b);
	            #endif                
            }    
            nCurY -= nSheetBxsize;
        }    
    } 
    else if(nCurY == -1)
    {   
		if(sht->style == 1)
		{
			 nCurY = (height + dvy - 1) * nSheetBxsize;
		}
		else
		{
            nCurY = (WIN_TOP_PADDING + height + dvy - 1) * nSheetBxsize  + WIN_LEFT_PADDING;
		}
        for (y = dvy; y < (int)height + dvy; y++) 
        {    
            for (x = dvx; x < (int)width+dvx; x++) 
            {    
                b = *pPicBuff++;
                g = *pPicBuff++;
                r = *pPicBuff++;

				#if DEPTH == 32
				pScreenColorBuf[nCurY + x] = COLOR32(b,g,r,255);
	            #elif DEPTH == 16
			    pScreenColorBuf[nCurY + x] = rgb_24_2_565(r,g,b);
	            #endif     
            }    
            nCurY -= nSheetBxsize;
        }    

    }
    else if(nCurY == -2)
	{
		if(sht->style == 1)
		{
			nCurY = dvy * nSheetBxsize; 
		}
		else
		{
	        nCurY = (WIN_TOP_PADDING + dvy - 1) * nSheetBxsize  + WIN_LEFT_PADDING;
		}
		for (int y = dvy; y < (int)height+dvy; y++)
	    {
		    for (int x = dvx; x < (int)width+dvx; x++)
			{
	            b = *pPicBuff++;
				g = *pPicBuff++;
			    r = *pPicBuff++; 

				#if DEPTH == 32
				pScreenColorBuf[nCurY + x] = COLOR32(b,g,r,255);
	            #elif DEPTH == 16
			    pScreenColorBuf[nCurY + x] = rgb_24_2_565(r,g,b);
	            #endif     
 			}
			nCurY += nSheetBxsize;
		}
	}	

	sheet_refresh(sht, 0, 0, sht->bxsize, sht->bysize);

	flush_dcache_all();

	return; 
}


int graphic_init(boolean bWithMouse)
{ 
	{
        screen_width     =  CFG_DISP_PRI_RESOL_WIDTH;
        screen_high      =  CFG_DISP_PRI_RESOL_HEIGHT;
	}
	TScreenColor* m_pBuffer =  (TScreenColor *)CONFIG_FB_ADDR;
    
    shtctl = shtctl_init(m_pBuffer, screen_width, screen_high);
    sht_back  = sheet_alloc(shtctl);
    sht_mouse = sheet_alloc(shtctl);
    
    buf_back  = (TScreenColor *) malloc(screen_width * screen_high * sizeof(TScreenColor));
    if(buf_back == NULL)
        return ENOMEM;
    
    sheet_setbuf(sht_back, buf_back, screen_width, screen_high, -1);
    sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
    init_screen(buf_back, screen_width, screen_high);
	

    init_mouse_cursor(buf_mouse);
    if (!bWithMouse)
    {
        hide_mouse_cursor();
    }
    
    sht_back ->editFlag = edit_off;
    sheet_slide(sht_back, 0, 0);
    
    int initx, inity;
    
    initx = (screen_width - 16) / 2;
    inity = (screen_high - 28-16) / 2;
    sheet_slide(sht_mouse, initx, inity);
    sheet_updown(sht_back,  0);
    sheet_updown(sht_mouse, 1); 
	m_next_sht_win = 0;
    //图片加载测试 start
	//SHEET *sht_win,*sht_win2,*sht_win3;
	//TScreenColor s_COL8_C6C6C6 = COLOR32(198,198,198,255);
	//create_imge_win(&sht_win,100,10,500,400,s_COL8_C6C6C6,1,"pic");
    //create_win(&sht_win2, 100,420, 400,400,1,"test");
	//sheet_refresh(m_tmp_sht_win, 0, 0, m_tmp_sht_win->bxsize-1,m_tmp_sht_win->bysize-1);

	//invalidate_dcache_all();
	//v7_inval_tlb();

   	//create_win(&sht_win3, 100,830, 400,400,1,"test");
	 //create_win(&sht_win3, 100,830, 400,400,1,"test");
	//create_imge_win(&sht_win2,100,420,400,400,s_COL8_C6C6C6,1,"pic");
	//图片加载测试 end
    return 0;
}
