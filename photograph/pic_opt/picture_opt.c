#include "picture_opt.h"
#include <uspios.h>
#include <string.h>
#include <alloc.h>
#include <stdio.h>
#include <global_timer.h>
#include <config.h>

int switch_flg = 0;
int increment = 0;
int total_v = 0;
int total_l_v = 0;
int total_r_v = 0;

float pic_opt_get_scale(SHEET *sht, int width, int hight, int *flg)
{
    if(sht == NULL)
        return 0;

    int xsize = sht->bxsize - (WIN_LEFT_PADDING + WIN_RIGHT_PADDING);
	int ysize = sht->bysize - (WIN_TOP_PADDING + WIN_BOTTOM_PADDING);
	if(width > xsize || hight > ysize)
	{
	    //缩放比例计算
		float scale,wscale,hscale;
	    wscale = (float)(width-1)/(xsize-1);
		hscale = (float)(hight-1)/(ysize-1);
		if(wscale - hscale > 0)
		{
		    scale = wscale;
			*flg = 0;
	    }
	    else
		{
		    scale = hscale;
			*flg = 1;
		}
		return scale;
	}
     
	return 1.0f;
}

void  pic_opt_zoom_pic_by_twoline(  unsigned char *src, 
                                    int srcPitch, 
                                    unsigned char *dst,
                                    int dstWidth, int dstHeight,
                                    float scale)
{
    if((src == NULL) || (dst == NULL))
    {
        return;
    }
    float x0, y0;
	int y1, y2, x1, x2;
	float fx1, fx2, fy1, fy2;
	int* arr_x1    = (int *)malloc(sizeof(int)*dstWidth);
    if(arr_x1 == NULL)
    { 
        return;
    }
    int* arr_x2    = (int *)malloc(sizeof(int)*dstWidth);
    if(arr_x2 == NULL) 
    {
        free(arr_x1);
        return;
    }
    float* arr_fx1 = (float *)malloc(sizeof(float)*dstWidth);
    if(arr_fx1 == NULL) 
    {
        free(arr_x1);
        free(arr_x2);
        return;
    }

	unsigned char *pDst = dst;
	for(int x=0; x<dstWidth; x++)
	{
	    x0 = x*scale;
	    arr_x1[x] = (int)x0;
		arr_x2[x] = (int)(x0+0.5f);
		arr_fx1[x] = x0 - arr_x1[x];
	}
	for(int y=0; y<dstHeight; y++)
	{
	    y0 = y*scale;
		y1 = (int)y0;
	    y2 = (int)(y0+0.5f);
		fy1 = y0-y1;
		fy2 = 1.0f - fy1; 
		for(int x=0; x<dstWidth; x++)
		{
		    x1 = arr_x1[x];
			x2 = arr_x2[x];
			fx1 = arr_fx1[x];
		    fx2 = 1.0f-fx1; 
			float s1 = fx2*fy2;
			float s2 = fx1*fy2;
			float s3 = fx1*fy1;
			float s4 = fx2*fy1;
			unsigned char* p11 = src + srcPitch*y1 + 3*x1;
		    unsigned char* p12 = src + srcPitch*y1 + 3*x2;
			unsigned char* p21 = src + srcPitch*y2 + 3*x1;
			unsigned char* p22 = src + srcPitch*y2 + 3*x2;
			*pDst = (unsigned char)((*p11)*s1 + (*p12)*s2 + (*p21)*s4 + (*p22)*s3);
			pDst++;
			p11++;
			p12++;
			p21++;
			p22++;
			*pDst = (unsigned char)((*p11)*s1 + (*p12)*s2 + (*p21)*s4 + (*p22)*s3);
		    pDst++;
	        p11++;
			p12++;
			p21++;
			p22++;
			*pDst = (unsigned char)((*p11)*s1 + (*p12)*s2 + (*p21)*s4 + (*p22)*s3);
		    pDst++;
	    }
		pDst = dst + y*dstWidth*3;
	}
	free(arr_x1);
	free(arr_x2);
	free(arr_fx1);
}

void pic_opt_get_dst_ypix(int dstHeight, float scale, int *pix)
{
    if(pix == NULL)
    {
        return;
    }
    float y0;
	int y1, y2;
	int *tmp=pix;
	for(int y=0; y<dstHeight; y++)
	{
	    y0 = (float)y*scale;
		y1 = (int)y0;
		y2 = (int)(y0+0.5f);
		*tmp   = y1;
		tmp++;
		*tmp   = y2;
		if(y < dstHeight-1)
		{
		    tmp++;
		}
	}	 
}

void  pic_opt_crop( unsigned char *src,
                    int srcWidth, int srcHeight,
                    unsigned char *dst,
                    int *dstWidth, int *dstHeight,
                    int scaleType)
{
    if((src == NULL) || (dst == NULL))
    {
        return;
    }

	int xStartP = 0,xEndP = 0,yStartP = 0,yEndP = 0;
	if(*dstWidth >= srcWidth && *dstHeight >= srcHeight)
    {
	    memcpy(dst,src,srcWidth*srcHeight*3);
	    *dstWidth  = srcWidth;
		*dstHeight = srcHeight;
	    return;
	}   
	else if(*dstWidth < srcWidth && *dstHeight < srcHeight)
    {   
		if(scaleType != MATRIX)
		{
	        xStartP = (srcWidth-*dstWidth)/2;
		    xEndP   = (srcWidth+*dstWidth)/2;   
		    yStartP = (srcHeight-*dstHeight)/2; 
		    yEndP   = (srcHeight+*dstHeight)/2;
		}
		else
		{
            xEndP = *dstWidth;
			if(m_tmp_sht_win->picType == -2)
			{
			    yEndP = *dstHeight;
			}
			else
			{
				yStartP = srcHeight - *dstHeight;
				yEndP   = srcHeight;
			}
		}
	}
	else if(*dstWidth < srcWidth && *dstHeight >= srcHeight)
    {
		if(scaleType != MATRIX)
		{
            xStartP    = (srcWidth-*dstWidth)/2; 
	        xEndP      = (srcWidth+*dstWidth)/2;
	        yStartP    = 0;
			yEndP      = srcHeight;
			*dstHeight = srcHeight;
		}
		else
		{
             xEndP = *dstWidth;
			 yEndP = srcHeight;
			 *dstHeight = srcHeight;
		}
	}
    else if(*dstWidth >=srcWidth && *dstHeight < srcHeight)
	{
		if(scaleType != MATRIX)
		{
		    xStartP   = 0;
		    xEndP     = srcWidth;
            yStartP   = (srcHeight-*dstHeight)/2;
		    yEndP     = (srcHeight+*dstHeight)/2;
			*dstWidth = srcWidth;
		}
		else
		{
			 
			 xEndP = srcWidth;
			 if( m_tmp_sht_win->picType == -1)
			 {
				 yStartP = srcHeight - *dstHeight;
				 yEndP = srcHeight;
			 }
			 else
			 {
				 yEndP = *dstHeight;
			 }
			 *dstWidth = srcWidth;
		}
	}
    int dst_count = 0,count = 0;
	for(int hnum = yStartP;hnum < yEndP;hnum++)
    {
	    for(int wnum = xStartP; wnum < xEndP;wnum++)
		{ 
	        int pixel_point  = hnum*srcWidth*3 + wnum*3;
			int dst_pixel_point  = dst_count*(*dstWidth*3) + count*3;
			dst[dst_pixel_point]   = src[pixel_point];
		    dst[dst_pixel_point+1] = src[pixel_point+1];
			dst[dst_pixel_point+2] = src[pixel_point+2];
			count++;
			if(count == *dstWidth)
			{
		        count = 0;
			}
	     }
		 dst_count++;
	}
}

float pic_opt_calculate_scale()
{
	//根据最大放大倍率计算双击时每次放大的放大系数
	float quotiety = 0;
	int maxSize = 0;
	int sWidth  = m_tmp_sht_win->bxsize;
	int sHeight = m_tmp_sht_win->bysize;
	int picWidth  = m_tmp_sht_win->srcWidth;
	int picHeight = m_tmp_sht_win->srcHeight;
	if(sWidth >= sHeight)
	{
		maxSize = sWidth * MAX_ZOOMED;
	}
	else
	{
		maxSize = sHeight * MAX_ZOOMED;
	}
	if(picWidth >= picHeight)
	{
        quotiety = (float)((maxSize - picWidth)/ZOOM_COUNT)/picWidth;
	}
	else
	{
		quotiety = (float)((maxSize - picHeight)/ZOOM_COUNT)/picHeight;
	}
    return quotiety;
}

void pic_opt_change_size_by_double_t(void)
{
    static int times = 0;
	if(m_tmp_sht_win->current_scale == 0)
	{
		return;
	}
	times++;
	if(!m_tmp_sht_win->current_status)
	{
		 pic_opt_change_size_by_scale(1+times*m_tmp_sht_win->quotiety);
//		 m_tmp_sht_win->current_scale = 1+(float)times*m_tmp_sht_win->quotiety;
	}
	else
	{
		 pic_opt_change_size_by_scale(1+(float)(ZOOM_COUNT-times)*m_tmp_sht_win->quotiety);
//		 m_tmp_sht_win->current_scale = 1+(float)(ZOOM_COUNT-times)*m_tmp_sht_win->quotiety;
	}

	if(times == ZOOM_COUNT)
	{
 //        if(m_tmp_sht_win->current_status == 0)
	// 	{
	// printk("pic_opt_change_size_by_double_t current_status = 1\r\n");

	//  	    m_tmp_sht_win->current_status = 1;//当前处于放大状态，下次双击后，应该是缩写的动作
	// 		m_tmp_sht_win->zoom_status = 3;//宽高都超出
	// 	}
	// 	else
	// 	{
	// printk("pic_opt_change_size_by_double_t current_status = 0\r\n");
	// 		 m_tmp_sht_win->current_status = 0;
	// 		 m_tmp_sht_win->zoom_status = 0;
	// 	}
		times = 0;
	}
}

void pic_opt_target_pixel(  unsigned char * target_pixel,
                            unsigned char *src,
                            int srcWidth,
                            int xStartP, int xEndP,
                            int yStartP, int yEndP)
{
    if((target_pixel == NULL) || (src == NULL))
    {
        return;
    }
	int dst_count = 0,count = 0;
	
    for(int hnum = yStartP;hnum < yEndP;hnum++)
	{
	    for(int wnum = xStartP; wnum < xEndP;wnum++)
		{
			 int pixel_point  = hnum*srcWidth*3 + wnum*3;
			 int dst_pixel_point  = dst_count*((xEndP-xStartP)*3) + count*3;
			 target_pixel[dst_pixel_point]   = src[pixel_point];
			 target_pixel[dst_pixel_point+1] = src[pixel_point+1];
			 target_pixel[dst_pixel_point+2] = src[pixel_point+2];
			 count++;
			 if(count == xEndP-xStartP)
		     {
			     count = 0;
			 }
		}
		dst_count++;
	}
}

void resetSheet(void)
{
	TScreenColor COL8_C6C6C6 = COLOR16(198, 198, 198);
	for(int y=0;y<m_tmp_sht_win->bysize;y++)
    {
	    for(int x = 0;x<m_tmp_sht_win->bxsize;x++)
	    {
	        m_tmp_sht_win->buf[y*m_tmp_sht_win->bxsize+x] = COL8_C6C6C6;
		}
	}
}

void pic_opt_switch_pic(int nPosX,int nPosY,int mode)
{
    UNUSED(nPosY);
	// printk("pic_opt_switch_pic m_tmp_sht_win->current_pic_sht_win->current_status = %d\r\n", m_tmp_sht_win->current_pic_sht_win->current_status);
 //    if(m_tmp_sht_win->current_pic_sht_win->current_status)
	// {
	// 	printk("pic_opt_switch_pic return 1111111\r\n");
	// 	return;
	// }
    int cur_vx0 = (m_tmp_sht_win->bxsize - m_tmp_sht_win->current_pic_sht_win->srcWidth)/2;
    if(total_v <= -(m_tmp_sht_win->current_pic_sht_win->srcWidth+cur_vx0) && nPosX < 0)
	{
        nPosX = 0;
	}
	if(total_v >= m_tmp_sht_win->current_pic_sht_win->srcWidth + cur_vx0 && nPosX > 0)
	{
        nPosX = 0;
	}
	if(m_tmp_sht_win->next_pic_sht_win && total_v < 0 && (-total_v >= m_tmp_sht_win->next_pic_sht_win->srcWidth/2 ||  -total_v > 10))
	{
        switch_flg = 1;
	}
	if(m_tmp_sht_win->pre_pic_sht_win && total_v > 0 && (total_v >= m_tmp_sht_win->pre_pic_sht_win->srcHeight/2 || total_v > 10))
	{
		switch_flg = 2;
	}
	total_v += nPosX;
	if(total_v <= -(m_tmp_sht_win->current_pic_sht_win->srcWidth+cur_vx0))
	{
        total_v = -(m_tmp_sht_win->current_pic_sht_win->srcWidth+cur_vx0);
	}
	if(total_v >= m_tmp_sht_win->current_pic_sht_win->srcWidth + cur_vx0)
	{
		total_v = m_tmp_sht_win->current_pic_sht_win->srcWidth + cur_vx0;
	}
	if(!mode)
	{
	    increment = total_v;
	}
	//int xStartP = 0,xEndP = 0,yStartP = 0,yEndP = 0;
	unsigned char *tmpSrc=0,*dst = 0;
	m_tmp_sht_win->current_pic_sht_win->n_pos_x += nPosX;
	int dstWidth = m_tmp_sht_win->current_pic_sht_win->srcWidth;
	int dstHeight= m_tmp_sht_win->current_pic_sht_win->srcHeight;
	dst = m_tmp_sht_win->current_pic_sht_win->picBuf;
	if(m_tmp_sht_win->current_pic_sht_win->n_pos_x < -dstWidth)
	{
		m_tmp_sht_win->current_pic_sht_win->n_pos_x = -dstWidth;
	}
	if(m_tmp_sht_win->current_pic_sht_win->n_pos_x > m_tmp_sht_win->bxsize)
	{
		m_tmp_sht_win->current_pic_sht_win->n_pos_x = m_tmp_sht_win->bxsize;
	}
	resetSheet();
	if((total_v < 0 && !m_tmp_sht_win->next_pic_sht_win) || (total_v > 0 && !m_tmp_sht_win->pre_pic_sht_win))
	{
		if(m_tmp_sht_win->current_pic_sht_win->n_pos_x > 0 && m_tmp_sht_win->current_pic_sht_win->n_pos_x < m_tmp_sht_win->bxsize-dstWidth)
		{
			add_to_piclayer(m_tmp_sht_win,dstWidth,dstHeight,dst,m_tmp_sht_win->current_pic_sht_win->picType,SWITCH);
		}
		else if(m_tmp_sht_win->current_pic_sht_win->n_pos_x <= 0)
		{
			total_l_v += nPosX;
			m_tmp_sht_win->current_pic_sht_win->n_pos_x = 0;
			int pic1Width = dstWidth+total_l_v;
		    if(pic1Width > 0)
		    {
		        tmpSrc = (unsigned char *)malloc(pic1Width*dstHeight*3);
                if(tmpSrc == NULL)
                    return;
                pic_opt_target_pixel(tmpSrc,dst,dstWidth,-total_l_v,dstWidth,0,dstHeight);
	            add_to_piclayer(m_tmp_sht_win,pic1Width,dstHeight,tmpSrc,m_tmp_sht_win->current_pic_sht_win->picType,SWITCH);
		        free(tmpSrc);
		    }
		    else
		    {
			    pic1Width = 0;
		    }
			if(pic1Width == 0)
		    {
			    add_to_piclayer(m_tmp_sht_win,0,0,0,m_tmp_sht_win->current_pic_sht_win->picType,SWITCH);
			}
		}
		else if(m_tmp_sht_win->current_pic_sht_win->n_pos_x >= m_tmp_sht_win->bxsize-dstWidth)
		{
			total_r_v += nPosX;
			m_tmp_sht_win->current_pic_sht_win->n_pos_x = m_tmp_sht_win->bxsize - dstWidth + total_r_v;
			int pic1Width = dstWidth-total_r_v;
            if(pic1Width > 0)
		    {
		        tmpSrc = (unsigned char *)malloc(pic1Width*dstHeight*3);
                if(tmpSrc == NULL)
                    return;
		        pic_opt_target_pixel(tmpSrc,dst,dstWidth,0,pic1Width,0,dstHeight);
		        add_to_piclayer(m_tmp_sht_win,pic1Width,dstHeight,tmpSrc,m_tmp_sht_win->picType,SWITCH);
		        free(tmpSrc);
		    }
		    else
		    {
                 pic1Width = 0;
		    }
			if(pic1Width == 0)
		    {
			    add_to_piclayer(m_tmp_sht_win,0,0,0,m_tmp_sht_win->current_pic_sht_win->picType,SWITCH);
			}	
		}
	}
	else if(total_v < 0 && m_tmp_sht_win->next_pic_sht_win)
	{
		unsigned char *current_picBuff,*switch_picBuff;
		int current_n_pos = 0;
		int switch_n_pos  = m_tmp_sht_win->bxsize + total_v;
		u32 current_width, current_height = m_tmp_sht_win->current_pic_sht_win->srcHeight;
		u32 switch_width  = -total_v;
		int current_type  = m_tmp_sht_win->current_pic_sht_win->picType;
        int switch_type   = m_tmp_sht_win->next_pic_sht_win->picType;
		u32 switch_height = m_tmp_sht_win->next_pic_sht_win->srcHeight;
        if(m_tmp_sht_win->current_pic_sht_win->n_pos_x > 0)
		{
			current_picBuff = dst;
			current_width   = dstWidth;
			current_height  = dstHeight;
			current_n_pos = m_tmp_sht_win->current_pic_sht_win->n_pos_x;
			if(-total_v <= m_tmp_sht_win->next_pic_sht_win->srcWidth)
			{
                switch_picBuff = (unsigned char *)malloc(switch_width*switch_height*3);
                if(switch_picBuff == NULL) 
                    return;
				pic_opt_target_pixel(switch_picBuff,m_tmp_sht_win->next_pic_sht_win->picBuf,m_tmp_sht_win->next_pic_sht_win->srcWidth
		                    ,0,switch_width,0,switch_height);
                add_to_pic_by_switch(current_width,current_height,switch_width,switch_height,current_n_pos,switch_n_pos,current_picBuff
						,switch_picBuff,current_type,switch_type);
		        free(switch_picBuff);
			}
			else
			{
				switch_picBuff = m_tmp_sht_win->next_pic_sht_win->picBuf;
				switch_width   = m_tmp_sht_win->next_pic_sht_win->srcWidth; 
                add_to_pic_by_switch(current_width,current_height,switch_width,switch_height,current_n_pos,switch_n_pos,current_picBuff
						,switch_picBuff,current_type,switch_type);
			}
		}
		else
		{
			//LogWrite ("11111", LOG_NOTICE, "num=%s","sssssss");
			total_l_v += nPosX;
            current_n_pos = 0;
			int pic1Width = dstWidth+total_l_v;
			if(pic1Width > 0)
			{
		        tmpSrc = (unsigned char *)malloc(pic1Width*dstHeight*3);
                if(tmpSrc == NULL)
                    return;
                pic_opt_target_pixel(tmpSrc,dst,dstWidth,-total_l_v,dstWidth,0,dstHeight);
				if(-total_v > m_tmp_sht_win->next_pic_sht_win->srcWidth)
				{
				    add_to_pic_by_switch(pic1Width,current_height,m_tmp_sht_win->next_pic_sht_win->srcWidth,switch_height,current_n_pos,switch_n_pos,tmpSrc
						    ,m_tmp_sht_win->next_pic_sht_win->picBuf,current_type,switch_type);
				}
				else
				{
                    switch_picBuff = (unsigned char *)malloc(switch_width*switch_height*3);
                    if(switch_picBuff == NULL)
                        return;
				    pic_opt_target_pixel(switch_picBuff,m_tmp_sht_win->next_pic_sht_win->picBuf,m_tmp_sht_win->next_pic_sht_win->srcWidth
		                    ,0,switch_width,0,switch_height);
                    add_to_pic_by_switch(pic1Width,current_height,switch_width,switch_height,current_n_pos,switch_n_pos,tmpSrc
						    ,switch_picBuff,current_type,switch_type);
		            free(switch_picBuff);
				}
		        free(tmpSrc);
			}
			else
			{
				 pic1Width = 0;
			}
			if(pic1Width == 0)
		    {
				 add_to_pic_by_switch(pic1Width,0,m_tmp_sht_win->next_pic_sht_win->srcWidth,switch_height,current_n_pos,switch_n_pos,0
						                               ,m_tmp_sht_win->next_pic_sht_win->picBuf,current_type,switch_type);
				//m_tmp_sht_win->current_pic_sht_win->n_pos_x = switch_n_pos;
				//add_to_piclayer(m_tmp_sht_win,m_tmp_sht_win->next_pic_sht_win->srcWidth,m_tmp_sht_win->next_pic_sht_win->srcHeight
				//		,m_tmp_sht_win->next_pic_sht_win->picBuf,m_tmp_sht_win->next_pic_sht_win->picType,SWITCH);
			}

		}
	}
	else if(total_v > 0 && m_tmp_sht_win->pre_pic_sht_win)
	{
		unsigned char *current_picBuff,*switch_picBuff;
		u32 current_width, current_height = m_tmp_sht_win->current_pic_sht_win->srcHeight;
		int current_n_pos = 0;
		int switch_n_pos  = total_v - m_tmp_sht_win->pre_pic_sht_win->srcWidth;
		u32 switch_width  = total_v;
		int current_type  = m_tmp_sht_win->current_pic_sht_win->picType;
        int switch_type   = m_tmp_sht_win->pre_pic_sht_win->picType;
		u32 switch_height = m_tmp_sht_win->pre_pic_sht_win->srcHeight;
        if(m_tmp_sht_win->current_pic_sht_win->n_pos_x < m_tmp_sht_win->bxsize-dstWidth)
		{

			current_picBuff = dst;
			current_width   = dstWidth;
			current_height  = dstHeight;
			current_n_pos   = m_tmp_sht_win->current_pic_sht_win->n_pos_x;
			if(total_v <= m_tmp_sht_win->pre_pic_sht_win->srcWidth)
			{
                switch_picBuff = (unsigned char *)malloc(switch_width*switch_height*3);
                if(switch_picBuff == NULL)
                    return;
				pic_opt_target_pixel(switch_picBuff,m_tmp_sht_win->pre_pic_sht_win->picBuf,m_tmp_sht_win->pre_pic_sht_win->srcWidth
		                    ,m_tmp_sht_win->pre_pic_sht_win->srcWidth-switch_width,m_tmp_sht_win->pre_pic_sht_win->srcWidth,0,switch_height);
                add_to_pic_by_switch(current_width,current_height,switch_width,switch_height,current_n_pos,0,current_picBuff
						,switch_picBuff,current_type,switch_type);
		        free(switch_picBuff);
			}
			else
			{
				switch_picBuff = m_tmp_sht_win->pre_pic_sht_win->picBuf;
				switch_width   = m_tmp_sht_win->pre_pic_sht_win->srcWidth; 
                add_to_pic_by_switch(current_width,current_height,switch_width,switch_height,current_n_pos,switch_n_pos,current_picBuff
						,switch_picBuff,current_type,switch_type);
			}
		}
		else
		{
			total_r_v += nPosX;
			current_n_pos = m_tmp_sht_win->bxsize - dstWidth + total_r_v;
			int pic1Width = dstWidth-total_r_v;
            if(pic1Width > 0)
		    {
		        tmpSrc = (unsigned char *)malloc(pic1Width*dstHeight*3);
                if(tmpSrc == NULL)
                    return;
		        pic_opt_target_pixel(tmpSrc,dst,dstWidth,0,pic1Width,0,dstHeight);
				if(total_v > m_tmp_sht_win->pre_pic_sht_win->srcWidth)
				{
					//LogWrite ("11111", LOG_NOTICE, "num=%d",switch_n_pos);
					add_to_pic_by_switch(pic1Width,current_height,m_tmp_sht_win->pre_pic_sht_win->srcWidth,switch_height,current_n_pos,switch_n_pos,tmpSrc
							,m_tmp_sht_win->pre_pic_sht_win->picBuf,current_type,switch_type);
				}
                else
				{
                    switch_picBuff = (unsigned char *)malloc(switch_width*switch_height*3);
                    if(switch_picBuff == NULL)
                        return;
				    pic_opt_target_pixel(switch_picBuff,m_tmp_sht_win->pre_pic_sht_win->picBuf,m_tmp_sht_win->pre_pic_sht_win->srcWidth
		                     ,m_tmp_sht_win->pre_pic_sht_win->srcWidth-switch_width,m_tmp_sht_win->pre_pic_sht_win->srcWidth,0,switch_height);
                    //LogWrite ("11111", LOG_NOTICE, "num=%d",current_n_pos);
					add_to_pic_by_switch(pic1Width,dstHeight,switch_width,switch_height,current_n_pos,0,tmpSrc
						    ,switch_picBuff,current_type,switch_type);
		            free(switch_picBuff);

				}
				free(tmpSrc);
		    }
		    else
		    {
                 pic1Width = 0;
		    }
			if(pic1Width == 0)
			{
				if(switch_n_pos < 0)
				{
                    switch_n_pos = 0;
				}
				add_to_pic_by_switch(pic1Width,0,m_tmp_sht_win->pre_pic_sht_win->srcWidth,switch_height,current_n_pos,switch_n_pos,0
						                               ,m_tmp_sht_win->pre_pic_sht_win->picBuf,current_type,switch_type);
			}
		}
	}
}

void pic_opt_move_image(int nPosX,int nPosY)
{
    static int tmpX = 0,tmpY = 0;
    //图片放大倍数没有超过sheet尺寸不可移动直接返回
    if(m_tmp_sht_win->zoom_status == 0 || (tmpX == nPosX && tmpY == nPosY))
    {
        return;
    }
    if(m_tmp_sht_win->zoom_status == 1)
    {
        nPosY = 0;
    }
    else if(m_tmp_sht_win->zoom_status == 2)
    {               
        nPosX = 0;
    }
    tmpX = nPosX;
    tmpY = nPosY;	 

    int xStartP = 0,xEndP = 0,yStartP = 0,yEndP = 0;
    unsigned char *tmpSrc=0,*dst = 0;
    int srcPitch   = m_tmp_sht_win->srcPitch;
    int srcWidth   = m_tmp_sht_win->srcWidth;
    int srcHeight  = m_tmp_sht_win->srcHeight;
    int dstWidth = m_tmp_sht_win->bxsize;
    int dstHeight= m_tmp_sht_win->bysize;
    
    m_tmp_sht_win->start_x -= nPosX;
    
    if(m_tmp_sht_win->picType == -1)
    {
        m_tmp_sht_win->start_y += nPosY;
    }
    else
    {
        m_tmp_sht_win->start_y -= nPosY;
    }
    
    m_tmp_sht_win->end_x   -= nPosX;
    
    if(m_tmp_sht_win->picType == -1)
    {
        m_tmp_sht_win->end_y   += nPosY;
    }
    else
    {
        m_tmp_sht_win->end_y   -= nPosY;
    }
    
    xStartP    = m_tmp_sht_win->start_x;
    xEndP      = m_tmp_sht_win->end_x ;
    
    if(xStartP <= 0)
    {
        xStartP = 0;
        xEndP   = dstWidth/m_tmp_sht_win->current_scale + 0.5f;
        m_tmp_sht_win->start_x = xStartP;
        m_tmp_sht_win->end_x = xEndP;
    }
    
    if(xEndP >= srcWidth)
    { 
        xStartP = srcWidth- dstWidth/m_tmp_sht_win->current_scale + 0.5f;
        xEndP   = srcWidth;
        m_tmp_sht_win->start_x = xStartP;
        m_tmp_sht_win->end_x = xEndP;
    }

    yStartP    = m_tmp_sht_win->start_y;
    yEndP      = m_tmp_sht_win->end_y;
    if(yStartP <= 0)
    {
        yStartP = 0;
        yEndP   = dstHeight/m_tmp_sht_win->current_scale + 0.5f;
        m_tmp_sht_win->start_y = yStartP;
        m_tmp_sht_win->end_y = yEndP;
    }
    else if(yEndP >= srcHeight)
    {
        yStartP = srcHeight- dstHeight/m_tmp_sht_win->current_scale + 0.5f;
        yEndP   = srcHeight;
        m_tmp_sht_win->start_y = yStartP;
        m_tmp_sht_win->end_y = yEndP;
    }
    
    dst = (unsigned char *)malloc(dstWidth*dstHeight*3);
    if(dst == NULL)
        return;
    
    tmpSrc = (unsigned char *)malloc((xEndP-xStartP)*(yEndP-yStartP)*3);
    if(tmpSrc == NULL)
        return;

    if(m_tmp_sht_win->zoom_status == 1)
    {
        dstHeight = (int)m_tmp_sht_win->srcHeight*m_tmp_sht_win->current_scale;
        yStartP = 0;
        yEndP = srcHeight;
    }
    else if(m_tmp_sht_win->zoom_status == 2)
    {
        dstWidth = (int)m_tmp_sht_win->srcWidth*m_tmp_sht_win->current_scale;
        xStartP = 0;
        xEndP = srcWidth;
    }
    pic_opt_target_pixel(tmpSrc,m_tmp_sht_win->picBuf,m_tmp_sht_win->srcWidth,xStartP,xEndP,yStartP,yEndP);
    srcHeight = yEndP-yStartP;
    srcWidth = xEndP-xStartP;
    srcPitch = srcWidth*3;
    //LogWrite ("11112", LOG_NOTICE, "num=%d,%d",xStartP,xEndP);
    for(int hnum=0; hnum < dstHeight; hnum++)
    {
        for(int wnum=0; wnum < dstWidth; wnum++)
        {
            float d_original_img_hnum = (float)hnum*srcHeight/dstHeight;
            float d_original_img_wnum = (float)wnum*srcWidth/dstWidth;
            int i_original_img_hnum = d_original_img_hnum;
            int i_original_img_wnum = d_original_img_wnum;
            float distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//在原图像中与a点的水平距离    
            float distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//在原图像中与a点的垂直距离  
            int original_point_a = i_original_img_hnum*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点A      
            int original_point_b = i_original_img_hnum*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点B    
            int original_point_c = (i_original_img_hnum + 1)*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点C     
            int original_point_d = (i_original_img_hnum + 1)*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点D 
            if (i_original_img_hnum +1== dstHeight - 1)
            {
                original_point_c = original_point_a;
                original_point_d = original_point_b;
            }
            if (i_original_img_wnum +1== dstWidth - 1)
            {
                original_point_b = original_point_a;
                original_point_d = original_point_c;
            }
            int pixel_point = hnum*(dstWidth)*3 + wnum*3;//映射尺度变换图像数组位置偏移量 
            dst[pixel_point] =    tmpSrc[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
                tmpSrc[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
                tmpSrc[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
                tmpSrc[original_point_d] * distance_to_a_y*distance_to_a_x;
            dst[pixel_point + 1] =tmpSrc[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
                tmpSrc[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
                tmpSrc[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
                tmpSrc[original_point_d + 1] * distance_to_a_y*distance_to_a_x;
            dst[pixel_point + 2] =tmpSrc[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
                tmpSrc[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
                tmpSrc[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
                tmpSrc[original_point_d + 2] * distance_to_a_y*distance_to_a_x;
        }
    }
    free(tmpSrc);
    TScreenColor COL8_C6C6C6 = COLOR16(198, 198, 198);
    for(int y=0;y<m_tmp_sht_win->bysize;y++)
    {
        for(int x = 0;x<m_tmp_sht_win->bxsize;x++)
        {
            m_tmp_sht_win->buf[y*m_tmp_sht_win->bxsize+x] = COL8_C6C6C6;
        }
    }
    add_to_piclayer(m_tmp_sht_win,dstWidth,dstHeight,dst,m_tmp_sht_win->picType,CENTER);
    free(dst);
}

void DTouchTimerHandleMessage(int msgType)
{
    static int n_count = 0;
	switch(msgType)
	{
		case 0:
            pic_opt_change_size_by_double_t();
			break;
		case 2:
			//switch_flg = 2;
		case 3:
			//switch_flg = 1;
		case 1:
			n_count++;
			if(!switch_flg)
			{
				if(n_count < MOVE_COUNT)
				{
			        pic_opt_switch_pic(-increment/MOVE_COUNT,0,1);
				}
			}
			else if(switch_flg == 1)
			{
				if(n_count < MOVE_COUNT)
				{
                    pic_opt_switch_pic(increment/MOVE_COUNT,0,1);
				}
			}
			else
			{
				if(n_count < MOVE_COUNT)
				{
                    pic_opt_switch_pic(increment/MOVE_COUNT,0,1);
				}
			}
			if(n_count == MOVE_COUNT)
			{
				if(switch_flg == 1)
				{
					m_tmp_sht_win->pre_pic_sht_win     =  m_tmp_sht_win->current_pic_sht_win;
                    m_tmp_sht_win->current_pic_sht_win =  m_tmp_sht_win->next_pic_sht_win;
					m_tmp_sht_win->next_pic_sht_win = 0;
					//m_tmp_sht_win->pre_pic_sht_win->n_pos_x = 0;
					for(int i = m_tmp_sht_win->current_pic_sht_win->height - 1 ; i > 0 ; i--)
                    {
		                m_tmp_sht_win->next_pic_sht_win = m_tmp_sht_win->ctl->sheets[i];
		                if(!m_tmp_sht_win->next_pic_sht_win->zoom_status)
		                {
							 //m_tmp_sht_win->next_pic_sht_win->n_pos_x = 50;
			                 break; 
		                } 
		                else
		                {
			                m_tmp_sht_win->next_pic_sht_win = 0;
		                }
                     }
				}
				else if(switch_flg == 2)
				{
					m_tmp_sht_win->next_pic_sht_win    =  m_tmp_sht_win->current_pic_sht_win;
                    m_tmp_sht_win->current_pic_sht_win =  m_tmp_sht_win->pre_pic_sht_win;
					m_tmp_sht_win->pre_pic_sht_win = 0;
				//	m_tmp_sht_win->current_pic_sht_win->n_pos_x = 0;
				//	m_tmp_sht_win->next_pic_sht_win->n_pos_x = 0;
					for(int i = m_tmp_sht_win->current_pic_sht_win->height + 1 ; i < m_tmp_sht_win->ctl->top - 1 ; i++)
					{
                        m_tmp_sht_win->pre_pic_sht_win = m_tmp_sht_win->ctl->sheets[i];
						if(!m_tmp_sht_win->pre_pic_sht_win->zoom_status)
						{
							//m_tmp_sht_win->pre_pic_sht_win->n_pos_x = 0;
							break;
						}
						else
						{
							 m_tmp_sht_win->pre_pic_sht_win = 0;
						}
					}
				}
				resetSheet();
				add_to_piclayer(m_tmp_sht_win,m_tmp_sht_win->current_pic_sht_win->srcWidth,m_tmp_sht_win->current_pic_sht_win->srcHeight
						                               ,m_tmp_sht_win->current_pic_sht_win->picBuf,m_tmp_sht_win->current_pic_sht_win->picType,CENTER);
				m_tmp_sht_win->current_pic_sht_win->n_pos_x = (m_tmp_sht_win->bxsize-m_tmp_sht_win->current_pic_sht_win->srcWidth)/2;				
		       // m_tmp_sht_win->n_pos_x =  50;	
	            total_v = 0;
	            total_l_v = 0;
	            total_r_v = 0;
				n_count = 0;
				increment = 0;
				switch_flg = 0;
			}
			break;
	    default:
			break;
	}
}

void  pic_opt_change_size_by_scale(float scale)
{
	//当sheet为非图片sheet时或scale等于sheet当前的scale直接返回
	if(m_tmp_sht_win->current_scale == 0 || scale - m_tmp_sht_win->current_scale == 0) 
	{
	    return;
	}


	if (m_tmp_sht_win->current_scale >= MAX_ZOOMED && m_tmp_sht_win->current_status == 0)
	{
		return;
	}

	if (m_tmp_sht_win->current_scale <= MIN_ZOOMED && m_tmp_sht_win->current_status == 1)
	{
		return;
	}
	//最大扩大到sheet的MAX_ZOOMED倍
	//int csize = 0;

	//if(m_tmp_sht_win->bxsize >= m_tmp_sht_win->bysize)
	//{
	//	csize = m_tmp_sht_win->bxsize;//960
	//}
	//else
	//{
	//	csize = m_tmp_sht_win->bysize;
	//}


	if(scale >= MAX_ZOOMED)
	{
		m_tmp_sht_win->current_status = 1;
		m_tmp_sht_win->current_scale = MAX_ZOOMED;
		scale = MAX_ZOOMED;
//        return;
	}

	if (scale <= 1)
	{
		m_tmp_sht_win->current_status = 0;
		//最小缩小到图片的MIN_ZOOMED倍
		if(scale < MIN_ZOOMED)
		{
			m_tmp_sht_win->current_scale = MIN_ZOOMED;
			scale = MIN_ZOOMED;
//			return;
		}
	}

	int xStartP = 0,xEndP = 0,yStartP = 0,yEndP = 0,flg = 0,drop_width = 0 ,drop_height = 0;
	//flg:1放大后的目标宽度大于sheet宽度,flg:2放大后的目标高度大于sheet高度,flg:3放大后的宽高都大于sheet高度
	unsigned char *src = m_tmp_sht_win->picBuf,*tmpSrc=0;
	int srcPitch   = m_tmp_sht_win->srcPitch;
	int srcWidth   = m_tmp_sht_win->srcWidth;
	int srcHeight  = m_tmp_sht_win->srcHeight;
	int dstWidth = m_tmp_sht_win->bxsize;
	int dstHeight= m_tmp_sht_win->bysize;
	unsigned char *dst = (unsigned char *)malloc(dstWidth*dstHeight*3);
    if(dst == NULL)
        return;
	int scaleType = m_tmp_sht_win->scaleType;
	switch(scaleType)
	{
		case CENTER:
		case CENTER_CROP:
		case CENTER_INSIDE:
		case FIT_END:
		case FIT_START:
		case FIT_XY:
		case MATRIX:
		case FIT_CENTER:
			//比较sheet的宽度是否大于放大后目标图的宽度,如果大于目标宽度等于放大后图片的宽度	
			if(dstWidth >= srcWidth*scale)
			{
			    dstWidth = (int)srcWidth*scale;
			}
			else
			{
				if(m_tmp_sht_win->bysize < srcHeight*scale)
				{
				    flg = 3;//图片宽高都小于sheet宽高
				}
				else
				{
				    flg = 1;//图片的宽大于sheet宽,高小于sheet高
				}
			}
			//比较sheet的高度是否大于放大后目标图的高度,如果大于目标高度等于放大后图片的高度
			if(dstHeight >= srcHeight*scale)
			{
			    dstHeight = (int)srcHeight*scale;
			}
			else
			{
				if(m_tmp_sht_win->bxsize < srcWidth*scale)
				{
				    flg = 3;
				}
				else
				{
					flg = 2;//图片的高大于sheet高,图片的宽小于sheet宽
				}
			}
			m_tmp_sht_win->zoom_status = flg;

			if(flg == 0)
			{
				break;
			}
			else if(flg == 1)
			{
                xStartP  = (int)(srcWidth -(int)(dstWidth/scale))/2;
				xEndP    = (int)(srcWidth +(int)(dstWidth/scale))/2;
				drop_width = xEndP - xStartP;

                //点击位置偏左
				if(m_tmp_sht_win->x_position - m_tmp_sht_win->pvx0 <= m_tmp_sht_win->srcWidth/2)
				{
					//左侧不足一半的放大距离，左侧目标全部保留，否则取左侧一半距离
                    if(m_tmp_sht_win->x_position - m_tmp_sht_win->pvx0 < drop_width/2)
					{
                        xStartP = 0;
						xEndP   = drop_width;
					}
					else
					{
                        xStartP = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 - drop_width/2;
						xEndP   = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 + drop_width/2;
					}
				}
				else//点击位置偏右
				{
					//右侧不足一半的放大距离，右侧目标全部保留,否则取右侧一半距离
                    if(m_tmp_sht_win->pvx0+m_tmp_sht_win->srcWidth-m_tmp_sht_win->x_position < drop_width/2)
					{
                        xStartP = m_tmp_sht_win->srcWidth - drop_width;
						xEndP   = m_tmp_sht_win->srcWidth;
					}
					else
					{
                        xStartP = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 - drop_width/2;
						xEndP   = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 + drop_width/2;
					}
				}


				yEndP    = srcHeight;
				m_tmp_sht_win->start_x = xStartP;
				m_tmp_sht_win->end_x   = xEndP;
				m_tmp_sht_win->start_y = 0;
				m_tmp_sht_win->end_y   = yEndP;
			}
			else if(flg ==2)
			{
                 yStartP = (int)(srcHeight-(int)(dstHeight/scale))/2;
				 yEndP   = (int)(srcHeight+(int)(dstHeight/scale))/2;
				 drop_height = yEndP - yStartP;
                 if(m_tmp_sht_win->y_position - m_tmp_sht_win->pvy0 <= m_tmp_sht_win->srcHeight/2)
				 {

                     if(m_tmp_sht_win->y_position - m_tmp_sht_win->pvy0 < drop_height/2)
					 {
						 if(m_tmp_sht_win->picType == -2)
						 {
                             yStartP = 0;
						     yEndP   = drop_height;
						 }
						 else
						 {
							 yStartP = m_tmp_sht_win->srcHeight - drop_height;
							 yEndP = m_tmp_sht_win->srcHeight;
						 }
					  }
					  else
					  {
                          yStartP = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 - drop_height/2;
						   yEndP   = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 + drop_height/2;
				      }
				  }
				  else
				  {
                      if(m_tmp_sht_win->pvy0+m_tmp_sht_win->srcHeight-m_tmp_sht_win->y_position < drop_height/2)
					  {
                          if(m_tmp_sht_win->picType == -2)
						  {
					          yStartP = m_tmp_sht_win->srcHeight - drop_height;
						      yEndP   = m_tmp_sht_win->srcHeight;
						  }
						  else
						  {
						      yStartP = 0;
							  yEndP   = drop_height;
						  }
					 }
					 else
					 {
                         yStartP = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 - drop_height/2;
	                     yEndP   = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 + drop_height/2;
					 }
				 }

				 xEndP   = srcWidth;
				 m_tmp_sht_win->start_x = 0;
				 m_tmp_sht_win->end_x   = xEndP;
				 m_tmp_sht_win->start_y = yStartP;
				 m_tmp_sht_win->end_y   = yEndP;
			}
			else if(flg == 3)
			{

				xStartP    = (int)(srcWidth - (int)(dstWidth/scale))/2;
			    xEndP      = (int)(srcWidth + (int)(dstWidth/scale))/2;
				yStartP    = (int)(srcHeight- (int)(dstHeight/scale))/2;
				yEndP      = (int)(srcHeight+ (int)(dstHeight/scale))/2;
				drop_width = xEndP - xStartP;
				drop_height= yEndP - yStartP;

                //点击位置偏左
				if(m_tmp_sht_win->x_position - m_tmp_sht_win->pvx0 <= m_tmp_sht_win->srcWidth/2)
				{
					//左侧不足一半的放大距离，左侧目标全部保留，否则取左侧一半距离
                    if(m_tmp_sht_win->x_position - m_tmp_sht_win->pvx0 < drop_width/2)
					{
                        xStartP = 0;
						xEndP   = drop_width;
					}
					else
					{
                        xStartP = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 - drop_width/2;
						xEndP   = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 + drop_width/2;
					}
				}
				else//点击位置偏右
				{
					//右侧不足一半的放大距离，右侧目标全部保留,否则取右侧一半距离
                    if(m_tmp_sht_win->pvx0+m_tmp_sht_win->srcWidth-m_tmp_sht_win->x_position < drop_width/2)
					{
                        xStartP = m_tmp_sht_win->srcWidth - drop_width;
						xEndP   = m_tmp_sht_win->srcWidth;
						//LogWrite ("11112", LOG_NOTICE, "num=%d,%d",xStartP,xEndP);
					}
					else
					{
                        xStartP = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 - drop_width/2;
						xEndP   = m_tmp_sht_win->x_position-m_tmp_sht_win->pvx0 + drop_width/2;
						 //LogWrite ("11113", LOG_NOTICE, "num=%d,%d",xStartP,xEndP);
					}
				}
                if(m_tmp_sht_win->y_position - m_tmp_sht_win->pvy0 <= m_tmp_sht_win->srcHeight/2)
				{

                    if(m_tmp_sht_win->y_position - m_tmp_sht_win->pvy0 < drop_height/2)
					{
						if(m_tmp_sht_win->picType == -2)
						{
                            yStartP = 0;
						    yEndP   = drop_height;
						}
						else
						{
							yStartP = m_tmp_sht_win->srcHeight - drop_height;
							yEndP = m_tmp_sht_win->srcHeight;
						}
						//LogWrite ("11113", LOG_NOTICE, "num=%d,%d,%d,%d",xStartP,xEndP,yStartP,yEndP);
					}
					else
					{
                        yStartP = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 - drop_height/2;
						yEndP   = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 + drop_height/2;
					}
				}
				else
				{
                    if(m_tmp_sht_win->pvy0+m_tmp_sht_win->srcHeight-m_tmp_sht_win->y_position < drop_height/2)
					{
                        if(m_tmp_sht_win->picType == -2)
						{
					        yStartP = m_tmp_sht_win->srcHeight - drop_height;
						    yEndP   = m_tmp_sht_win->srcHeight;
						}
						else
						{
							yStartP = 0;
							yEndP   = drop_height;
						}
					}
					else
					{
                         yStartP = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 - drop_height/2;
	                     yEndP   = m_tmp_sht_win->y_position-m_tmp_sht_win->pvy0 + drop_height/2;
					}
				}
				m_tmp_sht_win->start_x = xStartP;
				m_tmp_sht_win->start_y = yStartP;
				m_tmp_sht_win->end_x   = xEndP;
				m_tmp_sht_win->end_y   = yEndP;
			}

			tmpSrc = (unsigned char *)malloc((xEndP-xStartP)*(yEndP-yStartP)*3);
            if(tmpSrc == NULL)
                return;
			pic_opt_target_pixel(tmpSrc,m_tmp_sht_win->picBuf,m_tmp_sht_win->srcWidth,xStartP,xEndP,yStartP,yEndP);
			srcHeight = yEndP-yStartP;
			srcWidth = xEndP-xStartP;
			srcPitch = srcWidth*3;

			break;
	    default:
			break;
	}

	for(int hnum=0; hnum < dstHeight; hnum++)
    {
        for(int wnum=0; wnum < dstWidth; wnum++)
	    {
	        float d_original_img_hnum = (float)hnum*srcHeight/dstHeight;
		    float d_original_img_wnum = (float)wnum*srcWidth/dstWidth;
		    int i_original_img_hnum = d_original_img_hnum;
		    int i_original_img_wnum = d_original_img_wnum;
		    float distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//在原图像中与a点的水平距离    
		    float distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//在原图像中与a点的垂直距离  
		    int original_point_a = i_original_img_hnum*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点A      
			int original_point_b = i_original_img_hnum*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点B    
		    int original_point_c = (i_original_img_hnum + 1)*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点C     
		    int original_point_d = (i_original_img_hnum + 1)*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点D 
		    if (i_original_img_hnum +1== dstHeight - 1)
		    {
			    original_point_c = original_point_a;
				original_point_d = original_point_b;
		    }
		    if (i_original_img_wnum +1== dstWidth - 1)
		    {
		         original_point_b = original_point_a;
			     original_point_d = original_point_c;
			}
		    int pixel_point = hnum*(dstWidth)*3 + wnum*3;//映射尺度变换图像数组位置偏移量 
		    if(!flg)
			{	
		        dst[pixel_point] =    src[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
					              src[original_point_d] * distance_to_a_y*distance_to_a_x;
		        dst[pixel_point + 1] =src[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
					              src[original_point_d + 1] * distance_to_a_y*distance_to_a_x;
		        dst[pixel_point + 2] =src[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
     				              src[original_point_d + 2] * distance_to_a_y*distance_to_a_x;
			}
			else
			{
		        dst[pixel_point] =    tmpSrc[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              tmpSrc[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
					              tmpSrc[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
					              tmpSrc[original_point_d] * distance_to_a_y*distance_to_a_x;
		        dst[pixel_point + 1] =tmpSrc[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              tmpSrc[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
					              tmpSrc[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
					              tmpSrc[original_point_d + 1] * distance_to_a_y*distance_to_a_x;
		        dst[pixel_point + 2] =tmpSrc[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              tmpSrc[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
					              tmpSrc[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
     				              tmpSrc[original_point_d + 2] * distance_to_a_y*distance_to_a_x;

			}
		}
	}
	if(flg)
	{
		free(tmpSrc);
	}
	TScreenColor COL8_C6C6C6 = COLOR16(198, 198, 198);
	for(int y=0;y<m_tmp_sht_win->bysize;y++)
	{
	    for(int x = 0;x<m_tmp_sht_win->bxsize;x++)
	    {
		    m_tmp_sht_win->buf[y*m_tmp_sht_win->bxsize+x] = COL8_C6C6C6;
		}
	}
	add_to_piclayer(m_tmp_sht_win,dstWidth,dstHeight,dst,m_tmp_sht_win->picType,scaleType);
	free(dst);

	m_tmp_sht_win->current_scale = scale;
}

void pic_opt_change_size_by_twoline(    unsigned char *src,
                                        int srcPitch, int srcWidth, int srcHeight,
                                        unsigned char *dst,
                                        int* dstWidth, int* dstHeight, 
                                        int showAllFlg,
                                        int scaleType )
{
	int tsWidth = *dstWidth,tsHeight = *dstHeight;
	float scale,wscale,hscale;
	switch(scaleType)
	{
		case CENTER:
			pic_opt_crop(src,srcWidth,srcHeight,dst,dstWidth,dstHeight,scaleType);
			return;
		case CENTER_CROP:
			wscale = (float)srcWidth/(*dstWidth);
			hscale = (float)srcHeight/(*dstHeight);
			if(wscale > 1 && hscale > 1)
			{
				scale = (wscale - hscale) > 0 ? hscale:wscale;
			}
			else
			{
				scale = (wscale - hscale) > 0 ? wscale:hscale;
			}
			if(wscale == 1 &&  hscale == 1)
			{
                 memcpy(dst,src,srcWidth*srcHeight*3);
				 *dstWidth  = srcWidth;
				 *dstHeight = srcHeight;
				 return;
			}
			else
			{	
				if(wscale < 1 &&  hscale < 1)
				{
					*dstWidth  = *dstHeight = (wscale - hscale)>0 ? tsHeight:tsWidth;
				}
				else if(wscale > 1 &&  hscale > 1)
			    {
                    if(wscale <= hscale)
			 	    {
                        *dstWidth  = srcWidth;
					    *dstHeight = (*dstHeight)*wscale;
				    }
				    else
				    {
                        *dstWidth  = (*dstWidth)*hscale;
					    *dstHeight = srcHeight;
				    }
				
			    }
			    else if(wscale >= 1 &&  hscale < 1)
			    {
				    *dstWidth  = srcWidth*hscale;
                    *dstHeight = srcHeight;
			    }
			    else if(wscale < 1 &&  hscale >= 1)
			    {
				    *dstWidth  = srcWidth;
				    *dstHeight = srcHeight*wscale;
			    }
				unsigned char *tmpBuffer = (unsigned char *)malloc((*dstWidth)*(*dstHeight)*3);
                if(tmpBuffer == NULL)
                    return;
				pic_opt_crop(src,srcWidth,srcHeight,tmpBuffer,dstWidth,dstHeight,scaleType);
				srcWidth = *dstWidth;
				srcHeight = *dstHeight;
				srcPitch = srcWidth*3;
				*dstWidth = tsWidth;
				*dstHeight = tsHeight;
				memcpy(src,tmpBuffer,srcPitch*srcHeight);
				free(tmpBuffer);
			}
		    break;
		case CENTER_INSIDE:
			//LogWrite ("11111", LOG_NOTICE, "num=%d,%d,%d,%d",*dstWidth,srcWidth,*dstHeight,srcHeight);
			if(*dstWidth >= srcWidth && *dstHeight >= srcHeight)
			{
				 memcpy(dst,src,srcWidth*srcHeight*3);
				 *dstWidth  = srcWidth;
				 *dstHeight = srcHeight;
				 return;
			}
			else 
			{
				wscale = (float)srcWidth/(*dstWidth);
				hscale = (float)srcHeight/(*dstHeight);
				if(wscale - hscale > 0)
			    {
				    scale = wscale;
				}
				else
			    {
				    scale = hscale;
				}
				*dstWidth  = srcWidth/scale;
				*dstHeight = srcHeight/scale;
			}
		    break;
		case FIT_CENTER:
		case FIT_END:
		case FIT_START:
			wscale = (float)srcWidth/(*dstWidth);
			hscale = (float)srcHeight/(*dstHeight);
			scale  = wscale-hscale>0 ? wscale:hscale;
			*dstWidth  = srcWidth/scale;
            *dstHeight = srcHeight/scale;
		    break;
		case FIT_XY:
		    break;
		case MATRIX:
             pic_opt_crop(src,srcWidth,srcHeight,dst,dstWidth,dstHeight,scaleType);
		    return;
	    default:
			break;
	}
    int hnumCount = *dstHeight;
	if(!showAllFlg)
	{
	    hnumCount = 1;
	}
	for(int hnum=0; hnum<hnumCount; hnum++)
    {
        for(int wnum=0; wnum<*dstWidth; wnum++)
	    {
	        float d_original_img_hnum = (float)hnum*srcHeight/(*dstHeight);
		    float d_original_img_wnum = (float)wnum*srcWidth/(*dstWidth);
		    int i_original_img_hnum = d_original_img_hnum;
		    int i_original_img_wnum = d_original_img_wnum;
		    float distance_to_a_x = d_original_img_wnum - i_original_img_wnum;//在原图像中与a点的水平距离    
		    float distance_to_a_y = d_original_img_hnum - i_original_img_hnum;//在原图像中与a点的垂直距离  
		    int original_point_a = i_original_img_hnum*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点A      
			int original_point_b = i_original_img_hnum*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点B    
		    int original_point_c = (i_original_img_hnum + 1)*srcPitch + i_original_img_wnum * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点C     
		    int original_point_d = (i_original_img_hnum + 1)*srcPitch + (i_original_img_wnum + 1) * 3;//数组位置偏移量，对应于图像的各像素点RGB的起点,相当于点D 
		    if (i_original_img_hnum +1== *dstHeight - 1)
		    {
			    original_point_c = original_point_a;
				original_point_d = original_point_b;
		    }
		    if (i_original_img_wnum +1== *dstWidth - 1)
		    {
		         original_point_b = original_point_a;
			     original_point_d = original_point_c;
			}
		    int pixel_point = hnum*(*dstWidth)*3 + wnum*3;//映射尺度变换图像数组位置偏移量    
		    dst[pixel_point] =    src[original_point_a] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c] * distance_to_a_y*(1 - distance_to_a_x) +
					              src[original_point_d] * distance_to_a_y*distance_to_a_x;
		    dst[pixel_point + 1] =src[original_point_a + 1] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b + 1] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c + 1] * distance_to_a_y*(1 - distance_to_a_x) +
					              src[original_point_d + 1] * distance_to_a_y*distance_to_a_x;
		    dst[pixel_point + 2] =src[original_point_a + 2] * (1 - distance_to_a_x)*(1 - distance_to_a_y) +
					              src[original_point_b + 2] * distance_to_a_x*(1 - distance_to_a_y) +
					              src[original_point_c + 2] * distance_to_a_y*(1 - distance_to_a_x) +
     				              src[original_point_d + 2] * distance_to_a_y*distance_to_a_x;
		}
	}
}
