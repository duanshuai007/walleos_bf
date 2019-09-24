/*
 * Copyright (c) 2011 The Chromium OS Authors.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef _ENV_INCLUDE_DRIVERS_INITICAL_H_
#define _ENV_INCLUDE_DRIVERS_INITICAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*init_fnc_t)(void);

int initcall_run_list(const init_fnc_t init_sequence[]);

#ifdef __cplusplus
}
#endif

#endif
