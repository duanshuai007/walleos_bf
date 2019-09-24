/*
 * Copyright 2008,2010 Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based (loosely) on the Linux code
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ENV_INCLUDE_DRIVERS_MMC_PRIVATE_H_
#define _ENV_INCLUDE_DRIVERS_MMC_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drivers/mmc.h>

extern int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data);
extern int mmc_send_status(struct mmc *mmc, int timeout);
extern int mmc_set_blocklen(struct mmc *mmc, int len);

extern unsigned long mmc_berase(int dev_num, lbaint_t start, lbaint_t blkcnt);

// extern ulong mmc_bwrite(int dev_num, lbaint_t start, lbaint_t blkcnt,
// 		const void *src);
#ifdef __cplusplus
}
#endif

#endif /* _MMC_PRIVATE_H_ */
