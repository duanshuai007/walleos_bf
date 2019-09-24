/*
 * taskswitch.S
 *
 * Circle - A C++ bare metal environment for Raspberry Pi
 * Copyright (C) 2015  R. Stange <rsta2@o2online.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

	.text

	.globl  TaskSwitch
TaskSwitch:
    //push    {r2, r3}
	//mrs     r2, cpsr //将CPSR状态寄存器读取，保存到R2中
    //add     r3, pc, #64
    //str     r2, [r3] //cpsr->&(0x12345678)
    //pop     {r2, r3}

    //CPSID i

    //保存现场
	stmia	r0!, {r0-r12, lr} //r0-r12 and pc
	stmia	r0!, {r13-r14} // r13,r14

	mrs     r2, cpsr //将CPSR状态寄存器读取，保存到R2中
    //bic     r2, r2, #0x80

    //ldr     r2, [pc, #40] //*(&0x12345678)->r2
	stmia   r0, {r2} //设置cpsr位

    //获得新任务的cpsr
	ldr     r2, [r1, #64] // cpsr --> r2
	//cmp     r2, #0
	//msrne   cpsr, r2 //传送R2 的内容到CPSR

    //mrs     r3, cpsr //新任务的cpsr
    //BIC     R3, R3, #0x80

    //进入管理模式
    MRS     R4, CPSR
    BIC     R4, R4, #0x1f
    ORR     R4, R4, #0x13
    MSR     CPSR, R4

    //将新任务的cpsr导入到spsr中
    MSR     SPSR, R2

    //新任务下一条指令是pc
    add     r1, r1, #56
    ldmia   r1, {r13-r14}^
    sub     r1, r1, #56
    ldmia   r1, {r0-r12, pc}^	//将r1起始地址的数据保存到r0-r1寄存器中，有^并且有PC寄存器时，还将spsr拷贝到cpsr中，可用于异常处理返回

    //.word 0x12345678

	.globl io_hlt
io_hlt:
	wfi
	bx	lr	
/* End */


.global taskGetReg
taskGetReg:
     //保存现场
	stmia	r0!, {r0-r12, lr} //r0-r12 and pc
	stmia	r0!, {r13-r14} // r13,r14

	mrs     r2, cpsr //将CPSR状态寄存器读取，保存到R2中
    //bic     r2, r2, #0x80

    //ldr     r2, [pc, #40] //*(&0x12345678)->r2
	stmia   r0, {r2} //设置cpsr位

.global taskSetReg
taskSetReg:
    add     r1, r1, #56
    ldmia   r1, {r13-r14}^
    sub     r1, r1, #56
    ldmia   r1, {r0-r12, pc}^	//将r1起始地址的数据保存到r0-r1寄存器中，有^并且有PC寄存器时，还将spsr拷贝到cpsr中，可用于异常处理返回


