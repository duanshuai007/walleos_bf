//
// fatinfo.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2017  R. Stange <rsta2@o2online.de>
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
//
#include <fs/fatinfo.h>
#include <fs/fatfsdef.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>

static const s8 s_FromFATInfo[] = "fatinfo";

void CFATInfo (TFatInfo *pThis, TFatCache *pCache)
{
	pThis->m_pCache = pCache;
	pThis->m_FATType = FATUnknown;
}

void _CFATInfo (TFatInfo *pThis)
{
	pThis->m_FATType = FATUnknown;

	if (pThis->m_pCache != 0)
	{
		_CFATCache(pThis->m_pCache);
		pThis->m_pCache = 0;
	}
}

boolean FATInfoInitialize (TFatInfo *pThis)
{
	assert (pThis->m_pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector (pThis->m_pCache, 0, 0);//获取引导扇区

	assert (pBuffer != 0);

	TFATBootSector *pBoot = (TFATBootSector *) pBuffer->Data;//获取引导扇区
	assert (pBoot != 0);

	if (    pBoot->nBootSignature      != BOOT_SIGNATURE//引导结束标志0x55ff
	    ||	pBoot->BPB.nBytesPerSector != FAT_SECTOR_SIZE//每个扇区子节数
	    || (pBoot->BPB.nMedia & 0xF0)  != 0xF0)//0xf0为移动存储介质，0xF8为固定存储介质
	{
		FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
		return FALSE;
	}

	pThis->m_nSectorsPerCluster = pBoot->BPB.nSectorsPerCluster;//nSectorsPerCluster:每簇扇区数
	pThis->m_nReservedSectors   = pBoot->BPB.nReservedSectors;//保留扇区数
	pThis->m_nNumberOfFATs      = pBoot->BPB.nNumberOfFATs;//FAT表个数,一般为2
	pThis->m_nRootEntries       = pBoot->BPB.nRootEntries;//FAT32必须等于0，FAT12/FAT16为根目录中目录的个数

	pThis->m_nFATSize = pBoot->BPB.nFATSize16;//FAT32必须为0，FAT12/FAT16为一个FAT 表所占的扇区数,根据此字段判断是FAT32，还是FAT16

	if (pThis->m_nFATSize != 0)
	{
		pThis->m_FATType = FAT16;

		pThis->m_bFATMirroring = TRUE;
	}
	else
	{
		pThis->m_FATType = FAT32;

		if (   pBoot->Struct.FAT32.nBootSignature != 0x29//fat32,该字段必须是0x29
		    || pBoot->Struct.FAT32.nFSVersion     != 0//fat32,该字段必须是0x0
		    || pThis->m_nRootEntries              != 0)//fat32,该字段必须是0x0
		{
			FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
			return FALSE;
		}

		pThis->m_nFATSize = pBoot->Struct.FAT32.nFATSize32;//每个FAT表占用扇区数!!!!!注意：fat16中使用的是pBoot->BPB.nFATSize16

		pThis->m_bFATMirroring = !pBoot->Struct.FAT32.nMirroringOff;
		if (pThis->m_bFATMirroring)
		{
			pThis->m_nActiveFAT = pBoot->Struct.FAT32.nActiveFAT;//活动的fat表个数

			if (pThis->m_nActiveFAT >= pThis->m_nNumberOfFATs)
			{
				FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
				return FALSE;
			}
		}

		pThis->m_nRootCluster  = pBoot->Struct.FAT32.nRootCluster;//根目录所在第一个簇的簇号，2
		pThis->m_nFSInfoSector = pBoot->Struct.FAT32.nFSInfoSector;//FSINFO（文件系统信息扇区）扇区号1，该扇区为操作 系统提供关于空簇总数及下一可用簇的信息
	}

	pThis->m_nTotalSectors = pBoot->BPB.nTotalSectors16;//当为FAT32时pBoot->BPB.nTotalSectors16为0，则使用pBoot->BPB.nTotalSectors32
	if (pThis->m_nTotalSectors == 0)
	{
		pThis->m_nTotalSectors = pBoot->BPB.nTotalSectors32;
	}

	pBoot = 0;
	FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
	pBuffer = 0;

	if (   pThis->m_nReservedSectors == 0
	    || pThis->m_nFATSize == 0)
	{
		return FALSE;
	}

	pThis->m_nRootSectors     = ((pThis->m_nRootEntries * 32) + (FAT_SECTOR_SIZE - 1)) / FAT_SECTOR_SIZE;//根目录扇区总数
	pThis->m_nFirstDataSector = pThis->m_nReservedSectors + (pThis->m_nNumberOfFATs * pThis->m_nFATSize) + pThis->m_nRootSectors;//数据区的第一个数据所在扇区
	pThis->m_nDataSectors     = pThis->m_nTotalSectors - pThis->m_nFirstDataSector;//数据可以使用的扇区总数
	pThis->m_nClusters        = pThis->m_nDataSectors / pThis->m_nSectorsPerCluster;//可以使用簇的总数


	FATCacheSetClusterBuff(pThis->m_pCache, pThis->m_nSectorsPerCluster, pThis->m_nFirstDataSector);

	if (pThis->m_nClusters < 4085)//??应该是4095？？
	{
		return FALSE;			// FAT12 is not supported
	}
	else if (pThis->m_nClusters < 65525)//由于pBoot->BPB.nFATSize16最大只能表示65535
	{
		if (pThis->m_FATType != FAT16)
		{
			return FALSE;
		}
	}
	else//FAT32也有小的限制，FAT32卷必须至少有65527个簇，所以对于小的分区，仍然需要使用FAT16或FAT12
	{
		if (pThis->m_FATType != FAT32)
		{
			return FALSE;
		}
	}

	if (pThis->m_FATType == FAT32)
	{
		if (   pThis->m_nFSInfoSector == 0
		    || pThis->m_nFSInfoSector >= pThis->m_nReservedSectors)
		{
			return FALSE;
		}

		assert (pBuffer == 0);
		pBuffer = FATCacheGetSector(pThis->m_pCache, pThis->m_nFSInfoSector, 0);//获取FATINFO扇区数据
		assert (pBuffer != 0);

		TFAT32FSInfoSector *pFSInfo = (TFAT32FSInfoSector *) pBuffer->Data;
		assert (pFSInfo != 0);

		if (   pFSInfo->nLeadingSignature  != LEADING_SIGNATURE
		    || pFSInfo->nStructSignature   != STRUCT_SIGNATURE
		    || pFSInfo->nTrailingSignature != TRAILING_SIGNATURE)
		{
			FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
			return FALSE;
		}

		pThis->m_nFreeCount       = pFSInfo->nFreeCount;//文件系统的空簇数
		pThis->m_nNextFreeCluster = pFSInfo->nNextFreeCluster;//下一个可用的簇号

		pFSInfo = 0;
		FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
		pBuffer = 0;

		if (pThis->m_nNextFreeCluster == NEXT_FREE_CLUSTER_UNKNOWN)
		{
			pThis->m_nNextFreeCluster = 2;
		}
	}
	else//FSINFO是FAT32特有的扇区
	{
		pThis->m_nFreeCount       = FREE_COUNT_UNKNOWN;
		pThis->m_nNextFreeCluster = 2;
	}

	// Try to read last data sector
	assert (pBuffer == 0);
	pBuffer = FATCacheGetSector(pThis->m_pCache, pThis->m_nFirstDataSector + pThis->m_nDataSectors - 1, 0);
	assert (pBuffer != 0);
	FATCacheFreeSector (pThis->m_pCache, pBuffer, 0);
	pBuffer = 0;

	//乘以10，是打印需要，保留小数点后一位
	//u32 nSectorNumPerCluster = 10 * pThis->m_nSectorsPerCluster * FAT_SECTOR_SIZE / 1024;
	//LoggerWrite(LoggerGet (), s_FromFATInfo, LogDebug, "FAT%u: %u clusters of %u.%uK",
	//			pThis->m_FATType == FAT16 ? 16 : 32, pThis->m_nClusters,
	//			nSectorNumPerCluster / 10, nSectorNumPerCluster % 10);

	return TRUE;
}

//获取FAT格式，FAT32或FAT16
TFATType FATInfoGetFATType (TFatInfo *pThis) 
{
	assert (pThis->m_FATType != FATUnknown);
	return pThis->m_FATType;
}

//获取每簇扇区个数
u32 FATInfoGetSectorsPerCluster (TFatInfo *pThis) 
{
	return pThis->m_nSectorsPerCluster;
}

//获取保留扇区的个数
u32 FATInfoGetReservedSectors (TFatInfo *pThis) 
{
	return pThis->m_nReservedSectors;
}

//获取可用簇的总数
u32 FATInfoGetClusterCount (TFatInfo *pThis) 
{
	return pThis->m_nClusters;
}

//
u32 FATInfoGetReadFAT (TFatInfo *pThis) 
{
	if (   pThis->m_FATType == FAT32
	    && !pThis->m_bFATMirroring)
	{
		return pThis->m_nActiveFAT;
	}

	return 0;
}
u32 FATInfoGetFirstWriteFAT (TFatInfo *pThis) 
{
	if (   pThis->m_FATType == FAT32
	    && !pThis->m_bFATMirroring)
	{
		return pThis->m_nActiveFAT;
	}

	return 0;
}
u32 FATInfoGetLastWriteFAT (TFatInfo *pThis) 
{
	if (   pThis->m_FATType == FAT32
	    && !pThis->m_bFATMirroring)
	{
		return pThis->m_nActiveFAT;
	}

	assert (pThis->m_nNumberOfFATs > 0);
	return pThis->m_nNumberOfFATs-1;
}

//获取FAT表的size
u32 FATInfoGetFATSize (TFatInfo *pThis) 
{
	return pThis->m_nFATSize;
}

//获取根目录所在扇区
u32 FATInfoGetFirstRootSector (TFatInfo *pThis) 
{
	assert (pThis->m_FATType == FAT16);
	return pThis->m_nReservedSectors + (pThis->m_nNumberOfFATs * pThis->m_nFATSize);
}

//获取根目录使用扇区的个数
u32 FATInfoGetRootSectorCount (TFatInfo *pThis) 
{
	assert (pThis->m_FATType == FAT16);
	return pThis->m_nRootSectors;
}

//获取根目录目录的个数，FAT32为0
u32 FATInfoGetRootEntries (TFatInfo *pThis) 
{
	assert (pThis->m_FATType == FAT16);
	return pThis->m_nRootEntries;
}

//获取根目录所在簇数
u32 FATInfoGetRootCluster (TFatInfo *pThis) 
{
	assert (pThis->m_FATType == FAT32);
	return pThis->m_nRootCluster;
}

//获取簇的起始扇区
u32 FATInfoGetFirstSector (TFatInfo *pThis, u32 nCluster) 
{
	nCluster -= 2;
	assert (nCluster < pThis->m_nClusters);

	return pThis->m_nFirstDataSector + (nCluster * pThis->m_nSectorsPerCluster);
}

//更新FSFSINFO扇区
void FATInfoUpdateFSInfo (TFatInfo *pThis)
{
	if (pThis->m_FATType != FAT32)
	{
		return;
	}

	//m_Lock.Acquire ();
	
	assert (pThis->m_pCache != 0);
	TFATBuffer *pBuffer = FATCacheGetSector(pThis->m_pCache, pThis->m_nFSInfoSector, 0);
	assert (pBuffer != 0);

	TFAT32FSInfoSector *pFSInfo = (TFAT32FSInfoSector *) pBuffer->Data;
	assert (pFSInfo != 0);

	if (   pFSInfo->nLeadingSignature  != LEADING_SIGNATURE
	    || pFSInfo->nStructSignature   != STRUCT_SIGNATURE
	    || pFSInfo->nTrailingSignature != TRAILING_SIGNATURE)
	{
		FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
		//m_Lock.Release ();
		return;
	}

	pFSInfo->nFreeCount       = pThis->m_nFreeCount;
	pFSInfo->nNextFreeCluster = pThis->m_nNextFreeCluster;

	FATCacheMarkDirty(pThis->m_pCache, pBuffer);

	pFSInfo = 0;
	FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
	pBuffer = 0;

	//m_Lock.Release ();
}


void FATInfoClusterAllocated (TFatInfo *pThis, u32 nCluster)
{
	//m_Lock.Acquire ();

	if (   pThis->m_nFreeCount != FREE_COUNT_UNKNOWN
	    && pThis->m_nFreeCount > 0)
	{
		pThis->m_nFreeCount--;
	}

	if (nCluster <= pThis->m_nClusters)
	{
		pThis->m_nNextFreeCluster = nCluster+1;
	}
	else
	{
		pThis->m_nNextFreeCluster = 2;
	}
	
	//m_Lock.Release ();
}
void FATInfoClusterFreed (TFatInfo *pThis, u32 nCluster)
{
	//m_Lock.Acquire ();

	if (   pThis->m_nFreeCount != FREE_COUNT_UNKNOWN
	    && pThis->m_nFreeCount < pThis->m_nClusters)
	{
		pThis->m_nFreeCount++;
	}

	assert (pThis->m_nNextFreeCluster != NEXT_FREE_CLUSTER_UNKNOWN);
	assert (pThis->m_nNextFreeCluster >= 2);
	assert (pThis->m_nNextFreeCluster <= pThis->m_nClusters+1);
	if (nCluster < pThis->m_nNextFreeCluster)
	{
		pThis->m_nNextFreeCluster = nCluster;
	}

	//m_Lock.Release ();
}

u32 FATInfoGetNextFreeCluster (TFatInfo *pThis)
{
	//m_Lock.Acquire ();

	u32 nCluster = pThis->m_nNextFreeCluster;

	//m_Lock.Release ();

	return nCluster;
}
