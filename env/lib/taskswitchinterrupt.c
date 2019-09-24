#include <taskswitch.h>
#include <uspienv.h>
#include <synchronize.h>
#include <headsmp.h>

extern unsigned long _stack_irq_end;
extern unsigned long _stack_irq_end1;
extern unsigned long _stack_irq_end2;
extern unsigned long _stack_irq_end3;

//在定时器中断发生，产生任务切换，需要保存任务的寄存器
//将旧任务的r0-r15保存到任务中，新任务的r0-r15放入堆栈从而完成任务切换
//r0-r15寄存器保存的顺序需要跟irq_save_user_regs中相同。
void TaskSwitchInterrupt (TTaskRegisters *pOldRegs, TTaskRegisters *pNewRegs)
{
	unsigned long stack_irq_end = 0;

	switch(get_cpuid()){
		case 0:
			stack_irq_end = _stack_irq_end;
			break;

		case 1:
			stack_irq_end = _stack_irq_end1;
			break;

		case 2:
			stack_irq_end = _stack_irq_end2;
			break;

		case 3:
			stack_irq_end = _stack_irq_end3;
			break;

		default:
			stack_irq_end = _stack_irq_end;
			break;
	}

	u32 *pBase = (u32 *)stack_irq_end;
    //将当前的irq堆栈指针内容保存到oldreg中
    pBase--;
	pOldRegs->pc = *pBase--;
	pOldRegs->r12 = *pBase--;
	pOldRegs->r11 = *pBase--;
	pOldRegs->r10 = *pBase--;
	pOldRegs->r9 = *pBase--;
	pOldRegs->r8 = *pBase--;
	pOldRegs->r7 = *pBase--;
	pOldRegs->r6 = *pBase--;
	pOldRegs->r5 = *pBase--;
	pOldRegs->r4 = *pBase--;
	pOldRegs->r3 = *pBase--;
	pOldRegs->r2 = *pBase--;
	pOldRegs->r1 = *pBase--;
	pOldRegs->r0 = *pBase--;
	pOldRegs->lr = *pBase--;
	pOldRegs->sp = *pBase--;
	pOldRegs->spsr = *pBase--;

	pBase = (u32 *)stack_irq_end;
	pBase--;
    //newreg中的内容写入到irq的堆栈内存中去，在中断结束退栈时就会切换到新的任务中去执行了
	*pBase-- = pNewRegs->pc;
	*pBase-- = pNewRegs->r12;
	*pBase-- = pNewRegs->r11;
	*pBase-- = pNewRegs->r10;
	*pBase-- = pNewRegs->r9;
	*pBase-- = pNewRegs->r8;
	*pBase-- = pNewRegs->r7;
	*pBase-- = pNewRegs->r6;
	*pBase-- = pNewRegs->r5;
	*pBase-- = pNewRegs->r4;
	*pBase-- = pNewRegs->r3;
	*pBase-- = pNewRegs->r2;
	*pBase-- = pNewRegs->r1;
	*pBase-- = pNewRegs->r0;
	*pBase-- = pNewRegs->lr;
	*pBase-- = pNewRegs->sp;
	if (0 != pNewRegs->spsr)
    {   
        *pBase-- = pNewRegs->spsr;
    } 
}
