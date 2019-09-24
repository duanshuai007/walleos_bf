#include <uspios.h>
#include <stdio.h>
#include <pic_opt/picture_opt.h>
#include <bmp/bmp.h>
#include <string.h>
#include <alloc.h>
#include <errno.h>

#define BMP_HEAD        0x4D42
#define BMP_HEAD_SIZE   54
#define BMP_PITH        0x1c
#define BMP_PITH_24BIT  0x18
#define BMP_WIDTH       0x12
#define BMP_HEIGHT      0x16
#define BMP_WITH_TITLE  1
#define BMP_NO_TITLE    0

static char* load_all_pic(FILE *stream)
{
    unsigned char Buffer[100] = {0};
    int currentPos = 0;
    unsigned nResult;

    if(stream == NULL)
        return NULL;

	int fileSize = fsize(stream);
	char * pOutput = malloc(fileSize);
    if(pOutput == NULL)
        return NULL;

	while ((nResult = fread (Buffer, sizeof(char), sizeof(Buffer), stream)) > 0)
    {
	    memcpy(pOutput + currentPos, Buffer, nResult);
		currentPos += nResult;
	}

	return pOutput;
}

int bmp_parser_display(char *dir,SHEET *sht,int scaleType)
{
	if(dir == NULL)
        return 1;

    if(sht == NULL)
        return 1;

	if(scaleType < 0 || scaleType > 7)
		return 1;

	FILE *stream = fopen (dir, "r");
    if (NULL == stream)
        return 1;

	unsigned char Buffer[54] = {0};
    unsigned nResult;
	if ((nResult = fread (Buffer, sizeof(char), sizeof(Buffer), stream)) != BMP_HEAD_SIZE)
	{
    	fclose(stream);
		return 2;
	}
    //0x4d42 bmp 
	u16 head = *(u16 *)(Buffer);
	if (head != BMP_HEAD)
	{
    	fclose(stream);
		return 2;
	}
	unsigned char pith  = * (Buffer + BMP_PITH);
	if (pith != BMP_PITH_24BIT)
	{
    	fclose(stream);
		return 2;
	}

	u32 width;
	u32 height;
	memcpy(&width, Buffer + BMP_WIDTH, 4);
	memcpy(&height, Buffer + BMP_HEIGHT, 4);
//    u32 width = * (u32 *) (Buffer + BMP_WIDTH);
//    u32 height = * (u32 *) (Buffer + BMP_HEIGHT);
	if (0 == width || 0 == height)
	{
    	fclose(stream);
		return 2;
	}
	// 图片缩放系数
	int referenceFlg = 0;// 0:宽度参照 1:高度参照
	float scale = pic_opt_get_scale(sht,width,height,&referenceFlg);
	int dstWidth,dstHeight;
	if(sht->style == BMP_WITH_TITLE)
	{   //带标题栏
	    dstWidth = sht->bxsize;
	    dstHeight = sht->bysize;
	}
	else
	{   //无标题栏
	    dstWidth = sht->bxsize-(WIN_LEFT_PADDING+WIN_RIGHT_PADDING);
	    dstHeight = sht->bysize-(WIN_TOP_PADDING+WIN_BOTTOM_PADDING);
	}
	// 计算每行后面补齐字节数
	u32 tmp = width * 3 % 4;
	u32 ignore = 0;
	if (tmp != 0)
	{
		 ignore = 4 - tmp;
    }
	unsigned realWidth = width * 3 + ignore;
	unsigned char *pBuffer;
	//如果图片大小于200k一次性取出显示，否则分段取出显示
	if(fsize(stream)/1024 < 2000)
    {
	    pBuffer = (unsigned char *)load_all_pic(stream);
        if(pBuffer == NULL)
        {
            fclose(stream);
            return ENOMEM;
        }

		sht->scaleType  = scaleType;
		sht->current_scale = 1;
		sht->picType = -1;
	    
        sht->picBuf = (unsigned char *)malloc(dstWidth*dstHeight*3);
	    if(sht->picBuf == NULL)
        {
            free(pBuffer);
            fclose(stream);
            return ENOMEM;
        }
        pic_opt_change_size_by_twoline(pBuffer,realWidth,width,height,sht->picBuf,&dstWidth,&dstHeight,1,scaleType);
	    add_to_piclayer(sht,dstWidth,dstHeight,sht->picBuf,-1,scaleType);

		sht->srcWidth   = dstWidth;
		sht->srcHeight  = dstHeight;
		tmp = dstWidth * 3 % 4;
		if (tmp != 0)
	    {
		    ignore = 4 - tmp;
		}
		sht->srcPitch   = dstWidth*3 + ignore;
        sht->quotiety = pic_opt_calculate_scale();
        sht->current_status = 0;

        free(pBuffer);		
//		free(sht->picBuf);
	}
	else
	{
	   int nCount = 0;
	   unsigned char *dstBuffer;

	   if(scale != 1)
	   {
           //获取源图片采样点的最大最小行号，pix每两个值代表一组
		   int *pix= (int *)malloc(sizeof(int)*dstHeight*2);
		   if(pix == NULL)
           {
               fclose(stream);
               return ENOMEM;
           }
           scale = (float)height/dstHeight;
		   pic_opt_get_dst_ypix(dstHeight,scale,pix);
		   dstBuffer = (unsigned char *)malloc(sht->bxsize*3);
		   if(dstBuffer == NULL)
           {
               free(pix);
               fclose(stream);
               return ENOMEM;
           }
           int dvy = 0;
		   int nSheetBysize = sht->bysize-(WIN_TOP_PADDING + WIN_BOTTOM_PADDING);
		   if(nSheetBysize - dstHeight >= 2)
		   {
		       dvy = (nSheetBysize - dstHeight)/2;
		   }
		   int value = 20;
		   for(int x = 0;x < dstHeight*2;x+=2)
		   {
		       if(x > 0)
			   {
			       fseek(stream, pix[x]*realWidth+54, SEEK_SET);
			   }
			   int number = 1+pix[x+1]-pix[x];
			   pBuffer = (unsigned char *)malloc(realWidth*number);
			   if(pBuffer == NULL)
               {
                   free(pix);
                   free(dstBuffer);
                   fclose(stream);
                   return ENOMEM;
               }
               if((nResult =fread (pBuffer, sizeof(char), number*realWidth, stream)) == number*realWidth)
			   {
				   pic_opt_change_size_by_twoline(pBuffer, realWidth, width, height, dstBuffer, &dstWidth, &dstHeight, 0, CENTER_INSIDE);
				   add_to_piclayer(sht,dstWidth,1,dstBuffer,(dstHeight +dvy+value-1- nCount) * sht->bxsize + WIN_LEFT_PADDING,CENTER);
				   nCount++;
		       }
			   free(pBuffer);
		    }
            free(pix);
		    free(dstBuffer);
	   }
	}
	fclose(stream);
	return 0;
}
