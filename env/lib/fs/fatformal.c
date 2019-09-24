#include <fs/fatformal.h>
#include <fs/bpb.h>
#include <fs/fatfsdef.h>
#include <fs/fatcache.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

TFATBootSector tBootSector;
TFAT32FSInfoSector tFSInfoSector;
TFATDirectoryEntry	fatDirEntry;


boolean isFatFormalFin(TFatCache *pCache)
{
	assert (pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector (pCache, 0, 0);//获取引导扇区

	assert (pBuffer != 0);

	TFATBootSector *pBoot = (TFATBootSector *) pBuffer->Data;//获取引导扇区
	assert (pBoot != 0);

	if (    pBoot->nBootSignature      != BOOT_SIGNATURE//引导结束标志0x55ff
	    ||	pBoot->BPB.nBytesPerSector != FAT_SECTOR_SIZE//每个扇区子节数
	    || (pBoot->BPB.nMedia & 0xF0)  != 0xF0)//0xf0为移动存储介质，0xF8为固定存储介质
	{

		printf("fat32 not formal\r\n");
		FATCacheFreeSector (pCache, pBuffer, 0);
		return FALSE;
	}

	FATCacheFreeSector (pCache, pBuffer, 0);
	printf("fat32 finish formal\r\n");
	return TRUE;	
}

void CFatFormal(void)
{
	memset(&tBootSector, 0, sizeof(TFATBootSector));
	memset(&tFSInfoSector, 0, sizeof(TFAT32FSInfoSector));
	memset(&fatDirEntry, 0, sizeof(TFATDirectoryEntry));

}
//将小端字符串的内容转换成十进制
//buf[in]         要转换的字符串
//type[in]        要转成的类型，1 为 BYTE，2 为 WORD，4 为 DWORD
//返回值为转换的结果
s32 StoD(s8 *buf, s32 type)
{
	switch(type)
	{
	case 1: 
		return 0x000000FF & buf[0] ;

	case 2: 
		return (0x0000FFFF&(buf[1]<<8)) | (0x000000FF&buf[0]) ;

	case 4: 
		return (  (0x0000FFFF & ( buf[3]<<8 ) )
				| (0x000000FF & ( buf[2]<<16) )
				| (0x0000FFFF & ( buf[1]<<8 ) )
				| (0x000000FF & buf[0] )    );

	default: 
		return -1 ;
	}
}

//将数转换成小端字符串
//val[in]         要转换的数
//buf[out]        要存入的字符串
//type[in]        要转换的类型，1 为 BYTE，2 为 WORD，4 为 DWORD
//返回值为转换的结果
void DtoS(s32 val, u8 *buf, s32 type)
{
	switch(type)
	{
	case 1: 
		buf[0] = (u8)(val) ; 
		return ;

	case 2: 
		buf[0] = (u8)(val&0x000000FF);
		buf[1] = (u8)((val>>8)&0x000000FF) ; 
		return ;

	case 4: 
		buf[0] = (u8)(val&0x000000FF);
		buf[1] = (u8)((val>>8)&0x000000FF) ;
		buf[2] = (u8)((val>>16)&0x000000FF) ;
		buf[3] = (u8)((val>>24)&0x000000FF) ; 
		return ;
	}
}

//写入启动扇区
//成功返回 0 ，失败返回错误代码
static s32 SetBPB(TFatCache *pCache, u32 firstSector, u32 numberOfSectors)
{
    UNUSED(firstSector);
	assert (pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector (pCache, 0, 1);//获取引导扇区
	assert(pBuffer != NULL);

	assert(sizeof(TFATBootSector) == 512);
	assert(sizeof(bpb_data) == 512);

	memcpy(&tBootSector, bpb_data, sizeof(TFATBootSector));

	//修改文件系统的总扇区数字断
	tBootSector.BPB.nTotalSectors32 = numberOfSectors;

	u32 SectorsNumOfData = numberOfSectors - tBootSector.BPB.nReservedSectors; 
	//每个fat表占用的扇区数字断
	tBootSector.Struct.FAT32.nFATSize32 = SectorsNumOfData / tBootSector.BPB.nSectorsPerCluster * 4 / 512;

	tBootSector.Struct.FAT32.nFATSize32 = (SectorsNumOfData - 2 * tBootSector.Struct.FAT32.nFATSize32) / tBootSector.BPB.nSectorsPerCluster * 4 / 512;

	memcpy(pBuffer->Data, (u8*)&tBootSector, sizeof(TFATBootSector));

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);

	//写入备份引导扇区
	pBuffer = FATCacheGetSector (pCache, tBootSector.Struct.FAT32.nBackupBootSector, 1);//获取引导扇区
	assert(pBuffer != NULL);
	memcpy(pBuffer->Data, (u8*)&tBootSector, sizeof(TFATBootSector));

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);

	return 0;
} 

//写入 FSInfo
//成功返回 0，失败返回错误代码
static s32 SetFSInfo(TFatCache *pCache)
{

	assert (pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector(pCache, tBootSector.Struct.FAT32.nFSInfoSector, 1);//获取FATINFO扇区数据		
	assert(pBuffer != NULL);

	tFSInfoSector.nLeadingSignature = LEADING_SIGNATURE;
	tFSInfoSector.nStructSignature = STRUCT_SIGNATURE;
	tFSInfoSector.nFreeCount = tBootSector.Struct.FAT32.nFATSize32 * 512 / 4;
	tFSInfoSector.nNextFreeCluster = 3;
	tFSInfoSector.nTrailingSignature = TRAILING_SIGNATURE;

	memcpy(pBuffer->Data, (u8 *)&tFSInfoSector, sizeof(tFSInfoSector));

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);

	return 0;		

}

//写入根目录
//hdisk[in]           文件句柄
//lpBuffer[in]        缓冲区，大小至少为 BUFSIZE
//msg[in]             U盘相关信息
static s32 SetRootDir(TFatCache *pCache)
{

	u32 rootDirEntrySector = tBootSector.BPB.nReservedSectors + 2 * tBootSector.Struct.FAT32.nFATSize32;

	assert (pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector(pCache, rootDirEntrySector, 1);//获取FATINFO扇区数据		
	assert(pBuffer != NULL);

	strcpy((s8 *)fatDirEntry.Name, "root");
	fatDirEntry.nAttributes = FAT_DIR_ATTR_VOLUME_ID;

	memset(pBuffer->Data, 0x0, 512);
	memcpy(pBuffer->Data, (u8 *)&fatDirEntry, sizeof(TFATDirectoryEntry));

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);

	s32 i = 1;
	for (; i < tBootSector.BPB.nSectorsPerCluster; ++i)
	{
		pBuffer = FATCacheGetSector(pCache, rootDirEntrySector+i, 1);	
		assert(pBuffer != NULL);
		memset(pBuffer->Data, 0, 512);
		FATCacheMarkDirty (pCache, pBuffer);
		FATCacheFreeSector (pCache, pBuffer, 1);		
	}	

	return 0;		
}

//写入 FAT表
//hdisk[in]           文件句柄
//lpBuffer[in]        写入数据缓冲区，大小至少为 BUFSIZE
//msg[in]             U盘有关信息
static s32 SetFatTab(TFatCache *pCache)
{
	u32 fat1firstSector = tBootSector.BPB.nReservedSectors;
	u32 fat2firstSector = tBootSector.BPB.nReservedSectors;

	assert (pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector(pCache, fat1firstSector, 1);		
	assert(pBuffer != NULL);

	//写 FAT 表的第一个扇区
	DtoS(0x0FFFFFF8, pBuffer->Data, 4) ;
	DtoS(0xFFFFFFFF, pBuffer->Data+4, 4) ;
	DtoS(0x0FFFFFFF, pBuffer->Data+8, 4) ;	
	memset(pBuffer->Data+12, 0, 512-12);

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);	

	pBuffer = FATCacheGetSector(pCache, fat2firstSector, 1);	
	assert(pBuffer != NULL);

	//写 FAT 表的第一个扇区
	DtoS(0x0FFFFFF8, pBuffer->Data, 4) ;
	DtoS(0xFFFFFFFF, pBuffer->Data+4, 4) ;
	DtoS(0x0FFFFFFF, pBuffer->Data+8, 4) ;	
	memset(pBuffer->Data+12, 0, 512-12);

	FATCacheMarkDirty (pCache, pBuffer);
	FATCacheFreeSector (pCache, pBuffer, 1);

	u32 i = 1;
	for (; i < tBootSector.Struct.FAT32.nFATSize32; ++i)
	{
		pBuffer = FATCacheGetSector(pCache, fat1firstSector+i, 1);	
		assert(pBuffer != NULL);
		memset(pBuffer->Data, 0, 512);

		FATCacheMarkDirty (pCache, pBuffer);
		FATCacheFreeSector (pCache, pBuffer, 1);

		pBuffer = FATCacheGetSector(pCache, fat2firstSector+i, 1);	
		assert(pBuffer != NULL);
		memset(pBuffer->Data, 0, 512);
		FATCacheMarkDirty (pCache, pBuffer);
		FATCacheFreeSector (pCache, pBuffer, 1);		
	}

	return 0;	
}

//格式化 U盘
//letter[in]      要格式化的盘符
//volLabel[in]    卷标名
//secPerClus[in]  每簇的扇区数
//errMsg[out]     函数失败时返回失败位置，成功时未定义
//返回值           成功返回 0,失败返回 错误代码
s32 FormatFat32Disk(TFatCache *pCache, TPartition *pPartition)
{

	SetBPB(pCache, pPartition->m_nFirstSector, pPartition->m_nNumberOfSectors);

	SetFSInfo(pCache);

	SetFatTab(pCache);

	SetRootDir(pCache);

	FATCacheFlush(pCache);

	return 0 ;
} 
