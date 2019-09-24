#include <config.h>

.globl  _stext

_stext:         @label _stext
    b   reset   @跳转到reset子程序
/****    CONFIG VECTORS  ******/
    ldr pc, _undefined_instruction  @将后面的对应的一个word的值，赋值给pc,即跳转到后面对应的位置执行中断程序
    ldr pc, _software_interrupt
    ldr pc, _prefetch_abort
    ldr pc, _data_abort
    ldr pc, _not_used
    ldr pc, _irq
    ldr pc, _fiq

_undefined_instruction: 
    .word undefined_instruction @定义一个word空间，里面保存的值是undefined_instruction，_undefined_instruction = &undefined_instruction
_software_interrupt:    
    .word software_interrupt
_prefetch_abort:    
    .word prefetch_abort
_data_abort:        
    .word data_abort
_not_used:      
    .word not_used
_irq:           
    .word irq 
_fiq:           
    .word fiq

/*******     定义label,将arm.ld中的地址值取出来*********/
//定义bss段起始地址
.global TEXT_BASE
TEXT_BASE:
    .word CONFIG_SYS_TEXT_BASE

.global _bss_start_offset
_bss_start_offset:
    .word __bss_start - _stext

.global _bss_end_offset
_bss_end_offset:
    .word __bss_end - _stext

.global _text_start
_text_start:
    .word _stext
.global _text_end
_text_end:
    .word _etext
.global _bss_start
_bss_start:
    .word __bss_start
.global _bss_end
_bss_end:
    .word __bss_end
.global _data_start
_data_start:
    .word __data_start
.global _data_end
_data_end:
    .word __data_end

.global _heap_start
_heap_start:
    .word 0x0badbeef

.global _stack_start
_stack_start:
    .word 0x0badbeef
//cpu0 stack
.global _stack_fiq_end
_stack_fiq_end:
    .word 0x0badbeef
.global _stack_irq_end
_stack_irq_end:
    .word 0x0badbeef
.global _stack_und_end
_stack_und_end:
    .word 0x0badbeef
.global _stack_abt_end
_stack_abt_end:
    .word 0x0badbeef
.global _stack_svc_end
_stack_svc_end:
    .word 0x0badbeef
.global _stack_sys_end
_stack_sys_end:
    .word 0x0badbeef
//cpu1 statck
.global _stack_irq_end1
_stack_irq_end1:
    .word 0x0badbeef
.global _stack_sys_end1
_stack_sys_end1:
    .word 0x0badbeef
//cpu2 stack
.global _stack_irq_end2
_stack_irq_end2:
    .word 0x0badbeef
.global _stack_sys_end2
_stack_sys_end2:
    .word 0x0badbeef
//cpu3 stack
.global _stack_irq_end3
_stack_irq_end3:
    .word 0x0badbeef
.global _stack_sys_end3
_stack_sys_end3:
    .word 0x0badbeef

    //处理器运行于特权模式时，这些位也可以由程序修改
#define MODE_SVC 0x13
#define I_BIT    0x80

#define MODE_MASK   0x1f    //系统模式
#define NO_INT      0xc0
#define UDF_MODE    0x1b    //未定义模式
#define ABT_MODE    0x17    //中止模式
#define IRQ_MODE    0x12
#define FIQ_MODE    0x11
#define SVC_MODE    0x13    //管理模式
#define SYS_MODE    0x1f    //系统模式
#define CPSR_IRQ_EN     0x80
#define CPSR_IRQ_MASK   0x40
#define CPSR_FIQ_MASK   0x80

#define STACK_IRQ_SIZE  1024*10
#define STACK_FIQ_SIZE  1024*10
#define STACK_UND_SIZE  1024*10
#define STACK_ABT_SIZE  1024*10
#define STACK_SVC_SIZE  1024*10
#define STACK_SYS_SIZE  50*1024
/****************开始执行程序******************/

.globl reset
reset:
    //进入系统模式
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT 
    orr r0, r0, #SYS_MODE 
    msr cpsr, r0

    bl vfp_enable

    //执行复制代码之前需要关闭mmu，清空然后禁止cache
    //Invalidate L1 I/D  
    mov r0, #0          @ set up for MCR
    mcr p15, 0, r0, c8, c7, 0   @ invalidate TLBs
    mcr p15, 0, r0, c7, c5, 0   @ invalidate icache
    mcr p15, 0, r0, c7, c5, 6   @ invalidate BP array

    dsb 
    isb 

    //disable MMU stuff and caches
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x00002000 @ clear bits 13 (--V-)
    bic r0, r0, #0x00000007 @ clear bits 2:0 (-CAM)
    orr r0, r0, #0x00000002 @ set bit 1 (--A-) Align
    orr r0, r0, #0x00000800 @ set bit 11 (Z---) BTB
    bic r0, r0, #0x00001000 @ clear bit 12 (I) I-cache
    mcr p15, 0, r0, c1, c0, 0
    //relocate  
    adr r0, _stext          //获取实际的text段起始地址
    ldr r1, TEXT_BASE       //获取希望的text段起始地址
    cmp r0, r1              //判断是否相等
    beq in_ram              //如果相等则在ram中，跳转

    ldr r2, _bss_start_offset
    add r2, r0, r2          //r2中保存text段的结束地址

copy_source:
    ldmia r0!, {r3-r10}     //copy from source address [r0]
    stmia r1!, {r3-r10}     //copy to   target address [r1] 
    cmp r0, r2              //比较判断是否到了代码段末尾
    ble copy_source         //如果没到，则继续复制

    ldr r1, TEXT_BASE       //复制完成，将希望的text段起始地址赋给pc，执行跳转
    mov pc, r1

in_ram:
    bl clear_bss
    bl init_stack
    //计算mmu的地址
#ifdef SMP_MMU
    bl mmu_on
#endif
    //设置中断向量
    bl set_vbar
    //计算堆起始地址 
    ldr r0, =_heap_start    //将_heap_start的地址放入r0
    ldr r1, _bss_end        //将_bss_end的值放入r1
    add r1, r1, #0x1000
    str r1, [r0]            //将r1的值赋值给r0地址指向的空间
    //配置堆内存空间
	ldr r0, _heap_start
#ifdef DMA_SIZE
    ldr r1, =(MAX_MEMORY - DMA_SIZE * SIZE_1MB)		//cache空间尾地址
    sub r1, r1, r0			//转换为cache空间大小
    ldr r2, =(MAX_MEMORY - DMA_SIZE * SIZE_1MB)		//dma空间首地址
    ldr r3, =(DMA_SIZE * SIZE_1MB)		//dma空间大小
#else
	ldr r1, =MAX_MEMORY
	sub r1, r1, r0
	mov r2, #0x0
	mov r3, #0x0
#endif
    bl mem_initial
    //跳转到主函数
    mov r0, #0x1
    mov r1, #0x1
    b hello_world
    
#
#   设置栈空间,
#   参数: void
#   返回值: void
#
init_stack:
    /****不能设置用户模式的栈指针，在用户模式下不能够切换到其他模式****/
    /*系统模式：用于支持os的特权任务，与用户模式类似，但可以直接切换到其他任务
     除了用户模式外，其他模式都是特权模式，某些寄存器只能在特权模式下访问
     CPSR寄存器可以在任何模式下访问。用户模式与系统模式共有一套r13和r14寄存器
     r0-r12所有模式下公用一套*/
    //cpu0 堆栈空间
    ldr r1, =TEXT_BASE
    ldr r1, [r1]   
    //IRQ 堆栈设置 
    ldr r0, =_stack_irq_end
    str r1, [r0]
    sub r1, r1, #STACK_IRQ_SIZE
    //FIQ
    ldr r0, =_stack_fiq_end
    str r1, [r0]
    sub r1, r1, #STACK_FIQ_SIZE
    //SYS
    ldr r0, =_stack_sys_end
    str r1, [r0]
    sub r1, r1, #STACK_SYS_SIZE
    //ABT
    ldr r0, =_stack_abt_end
    str r1, [r0]
    sub r1, r1, #STACK_ABT_SIZE
    //UND
    ldr r0, =_stack_und_end
    str r1, [r0]
    sub r1, r1, #STACK_UND_SIZE
    //SVC
    ldr r0, =_stack_svc_end
    str r1, [r0]
    sub r1, r1, #STACK_SVC_SIZE

#ifdef SMP
    //cpu1 IRQ 堆栈空间
    ldr r0, =_stack_irq_end1
    str r1, [r0]
    sub r1, r1, #STACK_IRQ_SIZE
    //cpu1 SYS
    ldr r0, =_stack_sys_end1
    str r1, [r0]
    sub r1, r1, #STACK_SYS_SIZE
    //cpu2 堆栈空间
    ldr r0, =_stack_irq_end2
    str r1, [r0]
    sub r1, r1, #STACK_IRQ_SIZE
    //cpu1 SYS
    ldr r0, =_stack_sys_end2
    str r1, [r0]
    sub r1, r1, #STACK_SYS_SIZE
    //cpu3 堆栈空间
    ldr r0, =_stack_irq_end3
    str r1, [r0]
    sub r1, r1, #STACK_IRQ_SIZE
    //cpu1 SYS
    ldr r0, =_stack_sys_end3
    str r1, [r0]
    sub r1, r1, #STACK_SYS_SIZE
#endif

    //保存栈底位置
    ldr r0, =_stack_start
    str r1, [r0]
    
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #IRQ_MODE
    msr cpsr, r0
    ldr sp, _stack_irq_end

    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #FIQ_MODE
    msr cpsr, r0
    ldr sp, _stack_fiq_end
    
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #SVC_MODE
    msr cpsr, r0
    ldr sp, _stack_svc_end

    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #SYS_MODE
    msr cpsr, r0
    ldr sp, _stack_sys_end

    mov pc, lr
#
#   init_stack end
#

#
#   设置多核心栈空间
#   参数: u32 cpu_id
#   返回值: void
#
#ifdef SMP
//用于开启1,2,3核心时设置堆栈地址
.global cpu_x_init_stacks
cpu_x_init_stacks:
    cmp r0, #0x1
    beq cpu1_sp_label
    cmp r0, #0x2
    beq cpu2_sp_label
    cmp r0, #0x3
    beq cpu3_sp_label
cpu1_sp_label:
    ldr r4, _stack_irq_end1
    ldr r5, _stack_sys_end1
    b set_stack_end
cpu2_sp_label:
    ldr r4, _stack_irq_end2
    ldr r5, _stack_sys_end2
    b set_stack_end
cpu3_sp_label:
    ldr r4, _stack_irq_end3
    ldr r5, _stack_sys_end3
set_stack_end:
    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #IRQ_MODE
    msr cpsr, r0
    mov sp, r4

    mrs r0, cpsr
    bic r0, r0, #MODE_MASK | NO_INT
    orr r0, r0, #SYS_MODE
    msr cpsr, r0
    mov sp, r5
    mov pc, lr
#endif
#
#   多核心栈空间设置 end
#

#
#   清空bss
#   参数: void
#   返回值: void
#
clear_bss:
    ldr r0, _bss_start_offset
    ldr r1, _bss_end_offset
    ldr r4, TEXT_BASE
    add r0, r0, r4  //获取实际bss起始地址
    add r1, r1, r4  //获取实际bss结束地址
    mov r2, #0x00000000
clrbss:
    str r2, [r0]    //将r2的值保存到r0值对应的地址
    add r0, r0, #4
    cmp r0, r1      //比较r0和r1，用r0-r1，如果产生借位CF=1，如果溢出OF=1，如果是负数SF=1，如果结果不为0，ZF=1
    bne clrbss      //Z标识位不等于0，则跳转到clrbss
    //清空bss结束
    mov pc, lr
#
#   清空bss end
#

#
#   设置中断向量表地址
#
set_vbar:
    //将_stext的值载入r0,例如_stext = 0x48000000, 则执行完后r0 = 0x48000000.如果执行ldr sp, _stext 则将0x4800000地址处的值加载入r0
    ldr r0, =_stext
    mcr p15, 0, r0, c12, c0, 0  @Set VBAR
    mov pc, lr
#
#   设置中断向量表地址 end
#

#define S_FRAME_SIZE    72
#define S_OLD_R0    68
#define S_PSR       64
#define S_PC        60
#define S_LR        56
#define S_SP        52
#define S_IP        48
#define S_FP        44
#define S_R10       40
#define S_R9        36
#define S_R8        32
#define S_R7        28
#define S_R6        24
#define S_R5        20
#define S_R4        16
#define S_R3        12
#define S_R2        8
#define S_R1        4
#define S_R0        0

    @这个宏的功能是先将r0-r12(13)保存到堆栈中，然后紧接着将sp_usr，lr_usr(2)放到堆栈中
    @然后再将lr_irq(1)(对应中断前模式的pc)，spsr_irq(1)(对应中断前模式的cpsr)紧接着放入
    @堆栈中，最后将r0(1)放入堆栈，一共保存18个字。
    @[0                 S_FRAME                 72] 
    @0  4  8  12 16 20 24 28 32 36 40  44  48  52     56      60     64       68  
    @r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 sp_usr lr_usr  lr_irq spsr_irq r0_old

    .macro  irq_save_user_regs
    sub sp, sp, #S_FRAME_SIZE       @
    stmia   sp, {r0 - r12}  @ Calling r0-r12,sp后没有!，执行完本指令sp仍等于上一条指令后的结果
    @ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
    add r8, sp, #S_PC
    @地址先减4然后完成操作，r8-=4, 
    stmdb   r8, {sp, lr}^       @ Calling SP, LRi 后面加^表示保存用户模式的专用寄存器
    str lr, [r8, #0]        @ Save calling PC
    mrs r6, spsr
    str r6, [r8, #4]        @ Save CPSR(spsr_irq)
    str r0, [r8, #8]        @ Save OLD_R0
    mov r0, sp              @将保存r0-OLD_R0内存单元首地址赋给r0，供C程序函数调用
    .endm

    @这个宏功能是当执行完中断服务程序返回到中断前的状态
    .macro  irq_restore_user_regs
    @寄存器弹栈解释为do{sp-=4, sp = lr, sp -= 4, sp = ..., sp -= 4, sp = r0}
    @弹栈是将栈内的值赋给sp指针。
    ldmia   sp, {r0 - lr}^          @ Calling r0 - lr将r0-r12,sp_usr,lr_usr，恢复到用户寄存器
    mov r0, r0                  @nop
    ldr lr, [sp, #S_PC]         @ Get PC 获取中断返回的指令地址,在sp+S_PC的位置保存的是lr_irq
    add sp, sp, #S_FRAME_SIZE   @将sp_irq恢复
    subs    pc, lr, #4          @ return & move spsr_svc into cpsr，从中断返回，伴随着处理器模式切换
    @默认应切换到用户模式
    .endm

    .macro get_bad_stack
    ldr r13, _stack_und_end @ setup our mode stack

    str lr, [r13]   @ save caller lr in position 0 of saved stack
    mrs lr, spsr    @ get the spsr
    str lr, [r13, #4]   @ save spsr in position 1 of saved stack
    mov r13, #MODE_SVC  @ prepare SVC-Mode
    @ msr   spsr_c, r13
    msr spsr, r13   @ switch modes, make sure moves will execute
    mov lr, pc      @ capture return pc
    movs    pc, lr      @ jump to next instruction & switch modes.
    .endm

    .macro  bad_save_user_regs
    @ carve out a frame on current user stack
    sub sp, sp, #S_FRAME_SIZE
    stmia   sp, {r0 - r12}  @ Save user registers (now in svc mode) r0-r12
    ldr r2, _stack_und_end
    @ get values for "aborted" pc and cpsr (into parm regs)
    ldmia   r2, {r2 - r3}
    add r0, sp, #S_FRAME_SIZE       @ grab pointer to old stack
    add r5, sp, #S_SP
    mov r1, lr
    stmia   r5, {r0 - r3}   @ save sp_SVC, lr_SVC, pc, cpsr
mov r0, sp      @ save current stack into r0 (param register)
    .endm

    .macro irq_save_sys
    sub lr, lr, #4          /* lr: return address */
    stmfd   sp!, {r0-r12, lr}       /* save r0-r12 and return address */
    stmfd   sp, {r13-r14}^
    sub sp, sp, #8
    mrs r0, spsr //传送SPSR 的内容到R0
    stmfd   sp, {r0}
    sub sp, sp, #4
    .endm

    .macro irq_restore_sys
    add sp, sp, #4
    ldmdb   sp, {r0}
    msr spsr, r0 //传送R0 的内容到SPSR
    add sp, sp, #8
    ldmdb   sp, {r13-r14}^
    ldmfd   sp!, {r0-r12, pc}^      /* restore registers and return */
    .endm

/*** execption handler ***/
undefined_instruction:
    get_bad_stack
    bad_save_user_regs
    bl do_undefined_instruction

software_interrupt:
    get_bad_stack
    bad_save_user_regs
    bl do_software_interrupt

prefetch_abort:
    get_bad_stack
    bad_save_user_regs
    bl do_prefetch_abort


data_abort:
    get_bad_stack
    bad_save_user_regs
    bl do_data_abort

not_used:
    get_bad_stack
    bad_save_user_regs
    bl do_not_used

irq:
    irq_save_sys
    bl  do_irq
    irq_restore_sys

fiq:
    /* someone ought to write a more effiction fiq_save_user_regs */
    irq_save_user_regs
    bl  do_fiq
    irq_restore_user_regs
