#ifndef _ENV_INCLUDE_MMU_S5P4418_MMU_H_
#define _ENV_INCLUDE_MMU_S5P4418_MMU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
/*
 *  使能mmu，mmu页表地址默认是在TEXT_BASE以下，
 *  分别是TEXT_BASE[CONFIG_SYS_TEXT_BASE]->irq_end[1K]->fiq_end[1K]->sys_end[10K]->
 *  mmu_table_end[16K]
 *  mmu的页表地址必须以0x10000对齐，否则在使能时可能会出现问题
 */
void mmu_on(void);

/*
 *  禁止mmu
 */
void mmu_off(void);

#ifdef SMP_SCU_ENABE
void scu_enable(void *scu_base);
#endif

#ifdef __cplusplus
}
#endif

#endif
