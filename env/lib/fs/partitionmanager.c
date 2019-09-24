//
// partitionmanager.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <stdio.h>
#include <types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <alloc.h>
#include <fs/partitionmanager.h>
#include <assert.h>
#include <fs/partition.h>
#include <macros.h>
#include <drivers/mmc_export.h>
#include <s5p4418_serial_stdio.h>
#include <errno.h>

typedef struct TCHSAddress
{
	u8 Head; //开始磁头
	u8 Sector	   : 6,//开始扇区，只用0～5位
		      CylinderHigh : 2;//开始柱面的最高两位
	u8 CylinderLow;//开始煮面的低8位
}
PACKED TCHSAddress;

typedef struct TPartitionEntry
{
	u8	Status;//可引导标志，0x00不可引导，0x80可引导
	TCHSAddress	FirstSector;//分区起始CHS地址(CHS＝磁头、柱面、扇区)，起始地址
	u8	Type;//分区类型
	TCHSAddress	LastSector;//分区结束CHS地址
	u32	LBAFirstSector;//从磁盘开始到该分区开始的偏移量（分区起始LBA地址Little-endian顺序）
	u32	NumberOfSectors;//总扇区数（Little-endian顺序）
}
PACKED TPartitionEntry;

//主引导记录
/*MBR只占用了其中的446个字节，另外64个字节交给
了 DPT（Disk Partition Table硬盘分区表），最
后两个字节“55，AA”是分区的结束标志。DPT由4个分区表组成，每个16字节*/
typedef struct TMasterBootRecord
{
	u8	BootCode[0x1BE];
	TPartitionEntry	Partition[4];
	u16	BootSignature;
	#define BOOT_SIGNATURE		0xAA55
}
PACKED TMasterBootRecord;


//static const s8 s_FromPartitionManager[] = "partm";

void CPartitionManager (TPartitionManager *pThis, const s8 *pDeviceName){

	assert(pThis!=0);

	pThis->m_DeviceName = pDeviceName;

	u32 i = 0;
	for (; i < MAX_PARTITIONS; i++)
	{
		pThis->m_pPartition[i] = 0;
	}


}

void _CPartitionManager (TPartitionManager *pThis)
{
	u32 i = 0;
	for (; i < MAX_PARTITIONS; i++)
	{
		if (pThis->m_pPartition[i] != 0)
		{
			_CPartition(pThis->m_pPartition[i]);			
			free(pThis->m_pPartition[i]);
			pThis->m_pPartition[i] = 0;
		}
	}
}

boolean PartitionManagerInitialize (TPartitionManager *pThis)
{
	TMasterBootRecord MBR;
	assert (sizeof (MBR) == FS_BLOCK_SIZE);

	printf("PartitionManagerInitialize read MDR start\r\n");
	if (mmc_bread(0, 1, &MBR) != 1)
	{
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogError, "Cannot read MBR");
		return FALSE;
	}

	if (MBR.BootSignature != BOOT_SIGNATURE)
	{
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "Drive has no MBR");

		return TRUE;
	}

	u32 nPartition = 0;
	u32 i = 0;
	for (; i < MAX_PARTITIONS; i++)
	{
		// if (   MBR.Partition[i].Type == 0
		//     || MBR.Partition[i].Type == 0x05		// Extended partitions are not supported
		//     || MBR.Partition[i].Type == 0x0F		// Extended partitions are not supported
		//     || MBR.Partition[i].Type == 0xEF		// EFI is not supported
		//     || MBR.Partition[i].LBAFirstSector == 0
		//     || MBR.Partition[i].NumberOfSectors == 0)
		// {
		// 	continue;
		// }
		if (	MBR.Partition[i].Type != 0x01
			|| 	MBR.Partition[i].LBAFirstSector == 0
			|| 	MBR.Partition[i].NumberOfSectors == 0)

		{
			continue;
		}	
			
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "find partitions (%d) is  FAT32\r\n", i);
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "MBR.Partition[%d].Type = 0x%x\r\n", i, MBR.Partition[i].Type);
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "MBR.Partition[%d].LBAFirstSector = 0x%x\r\n", i, MBR.Partition[i].LBAFirstSector);
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "MBR.Partition[%d].NumberOfSectors = 0x%x\r\n", i, MBR.Partition[i].NumberOfSectors);

		assert (pThis->m_pPartition[i] == 0);

		pThis->m_pPartition[i] = (TPartition *)malloc(sizeof(TPartition));
		if(pThis->m_pPartition[i] == NULL)
            return ENOMEM;

		s8 partitionName[PARTITION_NAME_LEN_MAX] = { 0 };
		sprintf(partitionName, "%s-%u", (const s8 *) (pThis->m_DeviceName), ++nPartition);

		CPartition(pThis->m_pPartition[i], MBR.Partition[i].LBAFirstSector, MBR.Partition[i].NumberOfSectors, partitionName);
	}

	if (nPartition == 0)
	{
		//LoggerWrite(LoggerGet (), s_FromPartitionManager, LogWarning, "Drive has no supported partition");

		return TRUE;
	}

	return TRUE;
}

TPartition *PartitionManagerGetPartition(TPartitionManager *pThis, const s8 *pName){

	assert(pThis != 0);

	s32 i = 0;
	for (; i < MAX_PARTITIONS; ++i)
	{

		if (strcmp (pName, pThis->m_pPartition[i]->m_pPartitionName) == 0)
		{
			return pThis->m_pPartition[i];
		}
	}

	return 0;
}

