/*
 * sparrow_drv.c
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
#include <lcd/lcd.h>
#include <s5p4418_serial_stdio.h>
#include <config.h>

#define DEVICE_NAME			"sparrow"
static int spa_probe(struct platform_device *pdev) 
{
    UNUSED(pdev);

    bd_display_run();
	return 0;
}

static struct platform_driver spa_pdrv = 
{
	.name   = DEVICE_NAME,
	.probe 	= spa_probe,
};

int sparrow_drv_init(void) 
{ 
	int ret;
	printf("Hello world, this is Sparrow Drv!\n"); 
	ret = platform_driver_register(&spa_pdrv);
	if (ret < 0) 
	{
		printf("Can't register spa_pdev!\n"); 
		return ret;
	}
	return 0;
}

