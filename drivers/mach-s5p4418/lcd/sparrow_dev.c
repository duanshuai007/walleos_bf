/*
 * sparrow_dev.c
 *
 * A demonstation driver using Module in Linux kernel
 *
 * Author: wowo<www.wowotech.net>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#include "sparrow.h"
#include <s5p4418_serial_stdio.h>

#define DEVICE_NAME			"sparrow"

static struct platform_device spa_pdev = 
{
	.name 	= DEVICE_NAME,
	.id 	= -1,
};

int  sparrow_dev_init(void) 
{ 
	int 		ret;
	printf("Hello world, this is Sparrow Dev!\r\n"); 
	ret = platform_device_register(&spa_pdev);
	if (ret < 0) {
		printf("Can't register spa_pdev!\r\n"); 
		return ret;
	}
	return 0; 
}



