#ifndef _ENV_INCLUDE_HEADSMP_H_
#define _ENV_INCLUDE_HEADSMP_H_

#include <types.h>

/*
 *	获取cpsr寄存器内容
 */
u32 getcpsr(void);

/*
 *	设置cpsr寄存器，使能中断开关
 */
void setcpsr(void);

/*
 *	获取cpuid，返回的是0-3
 */
u32 get_cpuid(void);

/*
 *	获取c5 数据寄存器内容
 */
u32 get_cp15_c5_data(void);

/*
 *	获取c5 指令寄存器内容
 */
u32 get_cp15_c5_inst(void);

/*
 *	获取c6 数据寄存器内容
 */
u32 get_cp15_c6_data(void);

/*
 *	获取c6 指令寄存器内容
 */
u32 get_cp15_c6_inst(void);

/*
 *	获取c1 寄存器内容 参数0
 */
u32 get_cp15_c1(void);

/*
 *	获取c2 寄存器内容 参数0
 */
u32 get_cp15_c2(void);

/*
 *	获取当前cpu的sp指针
 */
u32 get_sp(void);

#endif
