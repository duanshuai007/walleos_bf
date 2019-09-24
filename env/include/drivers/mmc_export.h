#ifndef _ENV_INCLUDE_DRIVERS_MMC_EXPORTS_H_
#define _ENV_INCLUDE_DRIVERS_MMC_EXPORTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

ulong mmc_bwrite(lbaint_t start, lbaint_t blkcnt, const void *src);
ulong mmc_bread(lbaint_t start, lbaint_t blkcnt, void *dst);

#ifdef __cplusplus
}
#endif

#endif
