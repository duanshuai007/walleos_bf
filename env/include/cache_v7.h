#ifndef _ENV_INCLUDE_CACHE_V7_H_
#define _ENV_INCLUDE_CACHE_V7_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

enum{
    ARMV7_DCACHE_INVAL_ALL = 1,
    ARMV7_DCACHE_CLEAN_INVAL_ALL,
    ARMV7_DCACHE_INVAL_RANGE,
    ARMV7_DCACHE_CLEAN_INVAL_RANGE,
};

void v7_maint_dcache_all(u32 operation);
void flush_dcache_all(void);
void arm_init_before_mmu(void);
void mmu_page_table_flush(u32 start, u32 stop);
void flush_cache(u32 start, u32 size);

#ifdef __cplusplus
}
#endif

#endif
