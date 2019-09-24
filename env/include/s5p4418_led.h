#ifndef _ENV_INCLUDE_S5P4418_LED_H_
#define _ENV_INCLUDE_S5P4418_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  设置led：使能上拉，设置为输出
 */
void led_init(void);

/*
 *  Led(GPIOC) 状态取反
 */
void led_toggle(void);

/*
 *  Led(GPIOC) 设置状态
 *  status = 1, Pin High
 *  status = 0, Pin Low
 */
void led_on(int status);


#ifdef __cplusplus
}
#endif

#endif
