#ifndef _PHOTOGRAPH_PHOTOGRAPH_H_
#define _PHOTOGRAPH_PHOTOGRAPH_H_


#include <graphic.h>

/**
 *   图片加载到内存  
 *               
 *   @param dir   图片路径
 *     
 *   @param sht   预加载图层
 *       
 *   @return 0:成功 1:失败
 */
int pg_load_pic(char *dir, SHEET *sht, int scaleType);


#endif
