/*
* (C) Copyright 2010
* Texas Instruments, <www.ti.com>
* Aneesh V <aneesh@ti.com>
*
* SPDX-License-Identifier:  GPL-2.0+
*/
#ifndef _ENV_INCLUDE_ARMV7_H_
#define _ENV_INCLUDE_ARMV7_H_

/* CCSIDR */
enum{
    CCSIDR_LINE_SIZE_OFFSET = 0,
    CCSIDR_LINE_SIZE_MASK = 0x7,
    CCSIDR_ASSOCIATIVITY_OFFSET = 3,
    CCSIDR_ASSOCIATIVITY_MASK = (0x3ff << 3),
    CCSIDR_NUM_SETS_OFFSET = 13,
    CCSIDR_NUM_SETS_MASK = (0x7fff << 13),
};

/*
 * Values for InD field in CSSELR
 * Selects the type of cache
 */
enum{
    ARMV7_CSSELR_IND_DATA_UNIFIED = 0,
    ARMV7_CSSELR_IND_INSTRUCTION,
};

/* Values for Ctype fields in CLIDR */
enum{
    ARMV7_CLIDR_CTYPE_NO_CACHE = 0,
    ARMV7_CLIDR_CTYPE_INSTRUCTION_ONLY,
    ARMV7_CLIDR_CTYPE_DATA_ONLY,
    ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA,
    ARMV7_CLIDR_CTYPE_UNIFIED,
};
/*
 * CP15 Barrier instructions
 * Please note that we have separate barrier instructions in ARMv7
 * However, we use the CP15 based instructtions because we use
 * -march=armv5 in U-Boot
 */

#define CP15ISB asm volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define CP15DSB asm volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define CP15DMB asm volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))

#endif

