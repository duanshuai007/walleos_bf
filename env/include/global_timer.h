#ifndef _ENV_INCLUDE_GLOBAL_TIMER_H_
#define _ENV_INCLUDE_GLOBAL_TIMER_H_

#include <types.h>
#include <config.h>

#define AUTO_INCREMENT_ENABLE		TRUE
#define IRQ_ENABLE					TRUE
#define COMP_ENABLE					TRUE
#define TIMER_ENABLE				TRUE

typedef struct __bit64__{
	u32 upper;
	u32 lower;
}_bit64;

typedef struct Global_Timer
{
	_bit64			count;
	_bit64			comp;
	u8				prescaler;
	u8				auto_increment_enable;
	u8				irq_enable;
	u8				comp_enable;
	u8				enable;
	u8				irq_status;		//1 = 中断触发， 0 = 无中断触发
	u32				auto_increment_value;
	u64				irq_counts;
}_global_timer;

/*
 *	global timer 初始化，在初始化之前，需要填充globaltimer结构体的内容
 */
void global_timer_init(_global_timer *gt);

/*
 *	设置global timer counter
 */
void global_timer_set_counter(_global_timer *gt);

/*
 *	获取global timer counter
 */
u64 global_timer_get_counter(void);

/*
 *	设置global timer control register
 */
void global_timer_set_control(_global_timer *gt);

/*
 *	使能/禁止global timer
 */
void global_timer_enable(boolean enable);

/*
 *	使能/禁止global timer 的comparator功能
 */
void global_timer_comp_enable(boolean enable);

/*
 *	获取控制寄存器的内容
 */
u32 global_timer_get_control(void);

/*
 *	清空global timer 中断标志位
 */
void global_timer_clear_interrupt(_global_timer *gt);

/*
 *	获取global timer 中断状态
 */
boolean global_timer_get_interrupt(_global_timer *gt);
/*
 *	设置global timer comparator的数值
 */
void global_timer_set_comp(_global_timer *gt);
/*
 *	设置auto increament的值
 */
void golbal_timer_set_auto_increment(_global_timer *gt);

/*
 *	global timer 中断函数
 */
void global_timer_interrupt_handler(_global_timer *gt);

u64 global_timer_get_irq_counts(_global_timer *gt);

static inline u64 cpu_clock(void)
{
	return (u64)(global_timer_get_counter() - INITIAL_JIFFIES) * ((NSEC_PER_SEC / HZ));
}

//声明变量
extern _global_timer global_timer;

#endif
