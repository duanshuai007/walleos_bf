#ifndef _PHOTOGRAPH_JPEG_JPEG_H_
#define _PHOTOGRAPH_JPEG_JPEG_H_

#include <graphic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *   解析jpeg图片，并显示  
 *   @param dir   图片路径
 *   @param sht   预加载图层
 *   @return 0:成功 1:失败
 */
int jpeg_parser_display(char *dir, SHEET *sht,int scaleType);

#ifdef __cplusplus
}
#endif

#endif
