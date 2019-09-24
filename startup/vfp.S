#
#   vfp使能
#   参数: void
#   返回值: void
#
.global vfp_enable
vfp_enable:
    /* Enable NEON & VFP unit */
    mrc p15, #0, r1, c1, c0, #2
    orr r1, r1, #(0xf << 20)
    mcr p15, #0, r1, c1, c0, #2
    mov r1, #0
    mcr p15, #0, r1, c7, c5, #4
    mov r0, #0x40000000
    fmxr fpexc, r0

    mov pc, lr
