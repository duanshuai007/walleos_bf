#ifndef _ENV_INCLUDE_TASKSWITCH_H_ 
#define _ENV_INCLUDE_TASKSWITCH_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TTaskRegisters
{
	u32	r0;
	u32	r1;
	u32	r2;
	u32	r3;
	u32	r4;
	u32	r5;
	u32	r6;
	u32	r7;
	u32	r8;
	u32	r9;
	u32	r10;
	u32	r11;
	u32	r12;
	u32 pc; //下一条指令
	u32	sp; //56 offset
	u32	lr;
	u32 spsr; //中断模式是spsr，系统模式是cpsr
}TTaskRegisters;

//当前的r0-r14存入pOldRegs。然后用pNewRegs覆盖r0-r14
void TaskSwitch (TTaskRegisters *pOldRegs, TTaskRegisters *pNewRegs);
void TaskSwitchInterrupt (TTaskRegisters *pOldRegs, TTaskRegisters *pNewRegs);
void io_hlt(void);

#ifdef __cplusplus
}
#endif

#endif
