/*
 * platform_device.h - generic, centralized driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 *
 * This file is released under the GPLv2
 *
 * See Documentation/driver-model/ for more information.
 */

#ifndef _DRIVER_BASE_PLATFORM_DEVICE_H_
#define _DRIVER_BASE_PLATFORM_DEVICE_H_

#include <sizes.h>

enum dev_info
{
    MIN_DEV_NUM = 0,
    MAX_DEV_NUM = 10,
};

struct platform_device 
{
	const char	* name;
	int		id;
};

struct platform_driver 
{
    const char  * name;
    int (*probe)(struct platform_device *);
    //int (*remove)(struct platform_device *);
    //void (*shutdown)(struct platform_device *);
    //int (*suspend)(struct platform_device *, pm_message_t state);
	//int (*resume)(struct platform_device *);
    //struct device_driver driver;
    //const struct platform_device_id *id_table;
};


extern int platform_device_register(struct platform_device *);

extern int platform_driver_register(struct platform_driver *);

#endif /* _PLATFORM_DEVICE_H_ */
