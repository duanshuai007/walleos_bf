#ifndef _PHOTOGRAPH_PIC_OPT_PICTURE_OPT_H_
#define _PHOTOGRAPH_PIC_OPT_PICTURE_OPT_H_

#include <graphic/graphic.h>
#include <config.h>

/**
 *  获取图片缩放比例
 *    
 *  @param sht   窗口
 *   
 *  @param width 图片宽度
 *       
 *  @param hight 图片高度
 *         
 *  @param flg   0:按宽缩放，1:按高缩放
 *                 
 *  @return   缩放比例
 */

float pic_opt_get_scale(SHEET *sht, int width, int hight, int *flg);

void pic_opt_zoom_pic_by_twoline(   unsigned char *src,
                                    int srcPitch,
                                    unsigned char *dst,
                                    int dstWidth,
                                    int dstHeight,
                                    float scale);

/**
 *  改变图片大小
 *    
 *  @param src                 源图片
 *   
 *  @param srcPith             源图片深度
 *       
 *  @param srcWidth srcHeight  源图片大小
 *
 *  @param dst                 目标图片
 *
 *  @param dstWidth dstHeight  目标图片大小
 *
 *  @param showAllFlg          是否完全加载显示
 *
 *  @param showAllFlg          显示类型
 *
 *  @return 
 */
void pic_opt_change_size_by_twoline(    unsigned char *src,
                                        int srcPitch, int srcWidth, int srcHeight,
                                        unsigned char *dst,
                                        int* dstWidth, int* dstHeight,
                                        int showAllFlg,
                                        int scaleType);

void pic_opt_change_size_by_scale(float scale);
/**
 *  获取目标图片缩放所需的Y轴行号
 *    
 *  @param dstHeight   目标高度
 *   
 *  @param scale       缩放比例
 *       
 *  @param pix         对应行号
 *         
 *  @return 
 */
void pic_opt_get_dst_ypix(int dstHeight,float scale,int *pix);

/**
 *  根据图片的大小裁剪图图是图片剧中显示
 *    
 *  @param src         原图
 *   
 *  @param dst         目标图
 *       
 *  @param showAllFlg  显示方式
 *         
 *  @return 
 */
void pic_opt_drop(unsigned char *src,unsigned char *dst,int showAllFlg);

/**
 *  根据最大放大倍率计算双击时每次放大的放大系数
 *    
 *  @return 缩放系数
 */
float pic_opt_calculate_scale(void);

/**
 *  双击放大或还原图片
 *    
 *  @return
 */
void pic_opt_change_size_by_double_t(void);

/**
 *  双击放大回调函数
 *
 *  @param msgType 消息类型
 *
 *  @return
 */
void DTouchTimerHandleMessage(int msgType);

/**
 *  双击移动回调函数
 *
 *  @param msgType 消息类型
 *
 *  @return
 */
void PSwitchTimerHandleMessage(int msgType);

/**
 *  图片局部移动
 *    
 *  @param nPosX       X轴偏移
 *   
 *  @param nPosY       Y轴偏移
 *       
 *  @return 
 */
void pic_opt_move_image(int nPosX, int nPosY);

/**
 *  图片切换
 *    
 *  @param nPosX       X轴偏移
 *   
 *  @param nPosY       Y轴偏移
 *       
 *  @param mode        动画模式
 *         
 *  @return 
 */
void pic_opt_switch_pic(int nPosX,int nPosY,int mode);

#endif
