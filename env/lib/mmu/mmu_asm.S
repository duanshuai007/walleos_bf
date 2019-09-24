/*
 *************************************************************************
 * enable_mmu
 **************************************************************************
 */
.equ	PAGE_DOMAIN,	0x55555555

	.globl enable_mmu
enable_mmu:
		mov		r10, r0							@ r0: PTs 将r0(传入的参数，即页表的首地址)保存在r10寄存器中
		mov		r1, #0                          @ r1中保存0
		mcr		p15, 0, r1, c2, c0, 2			@ performs a page table walk on TLB miss and TTB0 has 16KB boundary将r1的值写入c2中

		ldr		r1, =PAGE_DOMAIN
		mcr		p15, 0, r1, c3, c0, 0			@ setup access to domain 0将PAGE_DOMAIN的值写入c3中
		
        orr		r10, r10, #0x4a					@ Enable the Data Cache mmu的页表地址必须4k对齐，否则低12位不为0在这里就会发生错误。
		mcr		p15, 0, r10, c2, c0, 0

		mov  	r0, #0x0
		mcr		p15, 0, r0, c8, c7, 0			@ flush I+D TLBs，使整个数据和指令TLB无效

		mrc		p15, 0, r1, c1, c0, 0           @ 获取c1的值
		orr		r1, r1, #(0x1<< 0)				@ Enable MMU
		orr		r1, r1, #(0x1<< 1)				@ Enable Alignment Fault Checking
		
        orr		r1, r1, #(0x1<< 2)				@ Enable the Data Cache
		orr		r1, r1, #(0x1<<12)				@ Enable I Cache
		
		orr		r1, r1, #(0x1<<11)				@ 使能跳转预测指令
//		orr		r1, r1, #(0x1<<13)				@ Enable V USE 0xffff0000
		bic		r1, r1, #(0x1<<13)				@ 使能异常向量中断入口为0x0-0x1c
		
		orr		r1, r1, #(0x1<<28)				@ Enable TRE Cache//bok
		mcr		p15, 0, r1, c1, c0, 0

		mov		pc, lr							@ return

	.globl disable_mmu
disable_mmu:
		mrc		p15, 0, r0, c1, c0, 0
		bic		r0, r0, #(1<<0) | (1<<2)		@ 0: MMU, 2: Dcache
		mcr		p15, 0, r0, c1, c0, 0
		mov		pc, lr
