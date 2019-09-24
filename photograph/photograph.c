#include <photograph.h>
#include <stdio.h>
#include <stddef.h>
#include <jpeg/jpeg.h>
#include <bmp/bmp.h>
#include <errno.h>

#include <graphic.h>

const unsigned short  TYPE_BMP = 0x4D42;
const unsigned short  TYPE_JPG = 0xD8FF;
const unsigned short  TYPE_PNG[4] = { 0x5089, 0x474E, 0x0A0D, 0x0A1A };
const unsigned short  TYPE_GIF[3] = { 0x4947, 0x3846, 0x6139 };

int pg_load_pic(char *dir, SHEET *sht, int scaleType)
{
    if(dir == NULL)
    {
        return ENOMEM; 
    }
	FILE *fis;
	int result = 0;

    unsigned short pis[4];

    if( NULL == (fis = fopen(dir, "r")) )
    {
    	printf("pg_load_pic fopen  %s  failed\n", dir);
    	return 1;
    } 

    fread(pis, 2, 4, fis);

	fclose(fis);

	if(pis[0] == TYPE_BMP)
	{	
		result = bmp_parser_display(dir,sht,scaleType);
	}
	else if(pis[0] == TYPE_JPG)
	{
		result = jpeg_parser_display(dir,sht,scaleType);
	}
	// else if( TYPE_PNG[0]==pis[0] && TYPE_PNG[1]==pis[1] && TYPE_PNG[2]==pis[2] && TYPE_PNG[3]==pis[3] )
	// {
	// 	//it is a png
	// }
	// else if( TYPE_GIF[0]==pis[0] && TYPE_GIF[1]==pis[1] && TYPE_GIF[2]==pis[2] )
	// {
	// 	//it is a gif
	// }
	else
	{
    	return 1;		
	}

	return result;
}
