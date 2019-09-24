#include <stdio.h>
#include <s5p4418_led.h>
#include <s5p4418_gpio.h>
#include <io.h>
#include <types.h>

/*
 *  设置led：使能上拉，设置为输出
 */
void led_init(void)
{
    gpio_set_pull(S5P4418_GPIOC(1), GPIO_PULL_UP);
    gpio_set_direction(S5P4418_GPIOC(1), GPIO_DIRECTION_OUTPUT);
}

/*
 *  Led(GPIOC) 状态取反
 */
void led_toggle(void)
{
    int val;
    val = gpio_get_value(S5P4418_GPIOC(1));
    val = ~val;
    gpio_set_value(S5P4418_GPIOC(1), val);
}

/*
 *  Led(GPIOC) 设置状态
 *  status = 1, Pin High
 *  status = 0, Pin Low
 */
void led_on(int status)
{
    gpio_set_value(S5P4418_GPIOC(1), status);
}
