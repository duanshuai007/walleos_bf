/*
 *  linux/arch/arm/plat-versatile/headsmp.S
 *
 *  Copyright (c) 2003 ARM Limited
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <config.h>

/*
 * Realview/Versatile Express specific entry point for secondary CPUs.
 * This provides a "holding pen" into which all secondary cores are held
 * until we're ready for them to initialise.
 */
    .globl __secondary_startup
    .align 4,0x90
__secondary_startup:
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r0, r0, #0x1f
    msr cpsr, r0

    ldr r0, =_stext
    mcr p15, 0, r0, c12, c0, 0

#ifdef SMP
    /* initialize stacks */
	mrc	p15, 0, r0, c0, c0, 5
	and	r0, r0, #15// r0 cpu id
    bl cpu_x_init_stacks
#endif

	mrc	p15, 0, r0, c0, c0, 5
	and	r0, r0, #15     // r0 cpu id
	adr	r4, 1f          //add r4, pc, #24
	ldmia	r4, {r5, r6}
	sub	r4, r4, r5
	add	r6, r6, r4
pen:
    ldr	r7, [r6]
	mov r1, r7
	cmp	r7, r0
	bne	pen

	/*
	 * we've been released from the holding pen: secondary_stack
	 * should now contain the SVC stack for this core
	 */
	b	secondary_startup

    .align
1:	.long	.
	.long	pen_release
    
    .type __secondary_startup, %function
    .size __secondary_startup, .-name

/*
 * initialize stacks
 */
.global getcpsr
getcpsr:
    mrs r0, cpsr
    mov pc, lr

.global setcpsr
setcpsr:
    mrs r0, cpsr
    and r0, r0, #~(1<<7)
    msr cpsr, r0
    mov pc, lr  

.global get_cpuid
get_cpuid:
    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #0xf
    mov pc, lr

.global get_cp15_c5_data
get_cp15_c5_data:
    mrc p15, 0, r0, c5, c0, 0
    mov pc, lr

.global get_cp15_c5_inst
get_cp15_c5_inst:
    mrc p15, 0, r0, c5, c0, 1
    mov pc, lr

.global get_cp15_c6_data
get_cp15_c6_data:
    mrc p15, 0, r0, c6, c0, 0
    mov pc, lr

.global get_cp15_c6_inst
get_cp15_c6_inst:
    mrc p15, 0, r0, c6, c0, 2
    mov pc, lr

.global get_cp15_c1
get_cp15_c1:
	mrc p15, 0, r0, c1, c0, 0
	mov pc, lr
.global get_cp15_c2
get_cp15_c2:
	mrc p15, 0, r0, c2, c0, 0
	mov pc, lr

.global get_sp
get_sp:
    mov r0, sp
    mov pc, lr

.global get_r0
get_r0:
    mov pc, lr

.global get_r1
get_r1:
    mov r0, r1
    mov pc, lr

.global get_r2
get_r2:
    mov r0, r2
    mov pc, lr

.global get_r3
get_r3:
    mov r0, r3
    mov pc, lr
