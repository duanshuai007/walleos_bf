#include <global_timer.h>
#include <types.h>
#include <stdio.h>
#include <io.h>
#include <smp_macros.h>
#include <config.h>

_global_timer global_timer;

void global_timer_set_counter(_global_timer *gt)
{
	u32 val;
	if(gt == NULL)
		return;
	//clear timer enable bit in the global timer control register
	val = readl(GLOBAL_TIMER_CTRL);
	val &= ~ENABLE;
	writel(GLOBAL_TIMER_CTRL, val);
	//write the lower 32 bit timer counter register
	writel(GLOBAL_TIMER_COUNT_L, (gt->count.lower - 1));
	//write the upper 32 bit timer counter register
	writel(GLOBAL_TIMER_COUNT_H, gt->count.upper);
	//set the timer enable bit
	val = readl(GLOBAL_TIMER_CTRL);
	val |= ENABLE;
	writel(GLOBAL_TIMER_CTRL, val);
}

u64 global_timer_get_counter(void)
{
	u32 count_h, count_l;
	u32 again_count_h;
	while(1)
	{
		//read the upper 32 bit timer counter register
		count_h = readl(GLOBAL_TIMER_COUNT_H);
		//read the lower 32 bit timer counter register
		count_l = readl(GLOBAL_TIMER_COUNT_L);
		//read the upper 32 bit timer counter register again.
		again_count_h = readl(GLOBAL_TIMER_COUNT_H);
		//if the value is different to the 32 bit upper value read previously,
		if(count_h == again_count_h)
		{
			break;
		}
		//go to the previous tep.
		//otherwise the 64 bit timer counter value is correct
	}
	return (((u64)count_h << 32) | count_l);
}

void global_timer_set_control(_global_timer *gt)
{
	u32 val;
	if(gt == NULL)
		return;
	val  = ((gt->prescaler - 1) << 8);
	val |= (gt->auto_increment_enable << 3);
	val |= (gt->irq_enable << 2);
	val |= (gt->comp_enable << 1);
	val |= (gt->enable);
	writel(GLOBAL_TIMER_CTRL, val);
}

void global_timer_enable(boolean enable)
{
	u32 val;
	val = readl(GLOBAL_TIMER_CTRL);
	if(enable == ENABLE)
		val |= ENABLE;
	else
		val &= ~ENABLE;

	writel(GLOBAL_TIMER_CTRL, val);
}

void global_timer_comp_enable(boolean enable)
{
	u32 val;
	val = readl(GLOBAL_TIMER_CTRL);
	if(enable == ENABLE)
		val |= ENABLE;
	else
		val &= ~ENABLE;
	writel(GLOBAL_TIMER_CTRL, val);
}

u32 global_timer_get_control(void)
{
	return readl(GLOBAL_TIMER_CTRL);
}

void global_timer_clear_interrupt(_global_timer *gt)
{
	gt->irq_status = 0;
	writel(GLOBAL_TIMER_INT_STATUS, 1);	//写入1清空中断标志
}

boolean global_timer_get_interrupt(_global_timer *gt)
{
	gt->irq_status = !!readl(GLOBAL_TIMER_INT_STATUS);
	return gt->irq_status; 
}

void global_timer_set_comp(_global_timer *gt)
{
	u32 val;
	if(gt == NULL)
		return;
	//clear the comp enable bit in the timer control register
	val = readl(GLOBAL_TIMER_CTRL);
	val &= ~ENABLE;
	writel(GLOBAL_TIMER_CTRL, val);
	//write the lower 32 bit comparator value register
	writel(GLOBAL_TIMER_COMP_L, gt->comp.lower);
	//write the uplow 32 bit comparator value register
	writel(GLOBAL_TIMER_COMP_H, gt->comp.upper);
	//set the comp enable bit abd if necessary,the irq enable bit
	val = readl(GLOBAL_TIMER_CTRL);
	val |= ENABLE;
	writel(GLOBAL_TIMER_CTRL, val);
}

void golbal_timer_set_auto_increment(_global_timer *gt)
{	//这个寄存器的作用是当comp enable 和 auto_inc enable被设置时，当global counter计数器的值
	//达到了comparator的值，comparator会自动的增加一个auto_incrment的值，可以实现一个周期性
	//事件
	if(gt == NULL)
		return;
	writel(GLOBAL_TIMER_AUTO_INC, gt->auto_increment_value);
}

void global_timer_interrupt_handler(_global_timer *gt)
{
	//printf("in global_timer_interrupt_handler: from cpu%d\r\n", (u32)dat);
	gt->irq_counts++;
}

u64 global_timer_get_irq_counts(_global_timer *gt)
{
	return gt->irq_counts;
}

void global_timer_init(_global_timer *gt)
{
	global_timer_set_counter(gt);
	global_timer_set_control(gt);
}

