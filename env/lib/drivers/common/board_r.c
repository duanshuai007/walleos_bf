/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <board_r.h>
#include <types.h>
#include <drivers/initcall.h>
#include <drivers/mmc.h>

int initr_mmc(void)
{
	mmc_initialize();
	return 0;
}


/*
 * Over time we hope to remove these functions with code fragments and
 * stub funtcions, and instead call the relevant function directly.
 *
 * We also hope to remove most of the driver-related init and do it if/when
 * the driver is later used.
 *
 * TODO: perhaps reset the watchdog in the initcall function after each call?
 */
init_fnc_t init_sequence_r[] = {

	initr_mmc,
	NULL
};

void board_init_r(void)
{
	if (initcall_run_list(init_sequence_r))
	{
	}
}
