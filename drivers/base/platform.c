#include "platform_device.h"
#include <string.h>

static int s_current_dev_num = -1;

static struct platform_device s_register_device[MAX_DEV_NUM];

int platform_device_register(struct platform_device *pdev)
{
	//判断设备是否已经注册如果已经注册直接返回
	for(int i = 0;i <= s_current_dev_num;i++)
	{
		if (!strcmp(pdev->name,s_register_device[i].name))
		{
			return -1;
		}
	}

	s_current_dev_num ++;


	//判断注册是否达到最大限制如果达到直接返回
	if(s_current_dev_num < MAX_DEV_NUM)
	{
        s_register_device[s_current_dev_num].name = pdev->name;
		s_register_device[s_current_dev_num].id   = pdev->id;
	}
	else
	{
		return -1;
	}

	return 0;
}

int platform_driver_register(struct platform_driver *drv)
{
	for(int i = 0;i < MAX_DEV_NUM;i++)
	{
		if (!strcmp(drv->name,s_register_device[i].name)) 
		{
            drv->probe(&s_register_device[i]);
		}
	}
	return 0;
}
