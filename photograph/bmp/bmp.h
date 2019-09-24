#ifndef _PHOTOGRAPH_BMP_BMP_H_
#define _PHOTOGRAPH_BMP_BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/graphic.h>

/**
 *   解析bmp图片，并显示  
 *   @param dir         图片路径
 *   @param sht         预加载图层
 *   @param scaleType   图片缩放类型
 *   @return 0:成功 1:失败
 */
int bmp_parser_display(char *dir, SHEET *sht, int scaleType);

#ifdef __cplusplus
}
#endif

#endif
