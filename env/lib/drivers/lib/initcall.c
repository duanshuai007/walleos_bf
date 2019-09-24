/*
 * Copyright (c) 2013 The Chromium OS Authors.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <drivers/initcall.h>

#include <s5p4418_serial_stdio.h>

int initcall_run_list(const init_fnc_t init_sequence[])
{
	const init_fnc_t *init_fnc_ptr;
	int i =0;
	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		i++;
		unsigned long reloc_ofs = 0;

		printf("initcall: %p\r\n", (char *)*init_fnc_ptr - reloc_ofs);

		if ((*init_fnc_ptr)()) {
			printf("initcall sequence %p failed at call %p\r\n",
			       init_sequence,
			       (char *)*init_fnc_ptr - reloc_ofs);
			return -1;
		}
	}
	return 0;
}
