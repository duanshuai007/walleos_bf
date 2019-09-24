/**
管理fat表
*/

#include <fs/fat.h>
#include <assert.h>
#include <stddef.h>

TFATBuffer *FATGetSector (TFat *pThis, u32 nCluster, u32 *pSectorOffset, u32 nFAT);
u32 FATGetEntry (TFat *pThis, TFATBuffer *pBuffer, u32 nSectorOffset);
void FATSetEntry (TFat *pThis, TFATBuffer *pBuffer, u32 nSectorOffset, u32 nEntry);


void CFAT (TFat *pThis, TFatCache *pCache, TFatInfo *pFATInfo)
{
	pThis->m_pCache = pCache;
	pThis->m_pFATInfo = pFATInfo;
}

void _CFAT (TFat *pThis)
{
	if (pThis->m_pCache != 0)
	{
		_CFATCache(pThis->m_pCache);
		pThis->m_pCache = 0;
	}

	if(pThis->m_pFATInfo != 0){
		_CFATInfo(pThis->m_pFATInfo);
		pThis->m_pFATInfo = 0;
	}
}

//根据簇号获取fat表项
u32 FATGetClusterEntry (TFat *pThis, u32 nCluster)
{
	//m_Lock.Acquire ();

	u32 nSectorOffset;//扇区内的相对偏移量

	assert (pThis->m_pFATInfo != 0);
	TFATBuffer *pBuffer = FATGetSector (pThis, nCluster, &nSectorOffset, FATInfoGetReadFAT(pThis->m_pFATInfo));
	assert (pBuffer != 0);

	u32 nEntry = FATGetEntry (pThis, pBuffer, nSectorOffset);

	assert (pThis->m_pCache != 0);
	FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);

	//m_Lock.Release ();
	
	return nEntry;
}

//根据簇号判断文件是否结束（EOC mark(End of clusterchain)）
boolean FATIsEOC (TFat *pThis, u32 nClusterEntry)
{
	assert (pThis->m_pFATInfo != 0);
	if (FATInfoGetFATType(pThis->m_pFATInfo) == FAT16)
	{
		if (nClusterEntry >= 0xFFF8)
		{
			return TRUE;
		}
	}
	else
	{
		assert (FATInfoGetFATType(pThis->m_pFATInfo) == FAT32);

		if (nClusterEntry >= 0x0FFFFFF8)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//设置对应簇号的FAT表项
void FATSetClusterEntry (TFat *pThis, u32 nCluster, u32 nEntry)
{
	//m_Lock.Acquire ();

	assert (pThis->m_pFATInfo != 0);
	u32 nFAT = FATInfoGetFirstWriteFAT (pThis->m_pFATInfo);
	for (; nFAT <= FATInfoGetLastWriteFAT (pThis->m_pFATInfo); nFAT++)
	{
		u32 nSectorOffset;
		TFATBuffer *pBuffer = FATGetSector (pThis, nCluster, &nSectorOffset, nFAT);
		assert (pBuffer != 0);

		FATSetEntry (pThis, pBuffer, nSectorOffset, nEntry);

		assert (pThis->m_pCache != 0);
		FATCacheMarkDirty (pThis->m_pCache, pBuffer);
		FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
	}

	//m_Lock.Release ();
}

//申请使用一个新簇
u32 FATAllocateCluster (TFat *pThis)
{
	//m_Lock.Acquire ();

	assert (pThis->m_pFATInfo != 0);

	//获取下一个空闲的簇
	u32 nCluster = FATInfoGetNextFreeCluster (pThis->m_pFATInfo);

	while (nCluster < FATInfoGetClusterCount (pThis->m_pFATInfo) + 2)
	{
		assert (nCluster >= 2);

		u32 nSectorOffset;
		TFATBuffer *pBuffer = FATGetSector (pThis, nCluster, &nSectorOffset, FATInfoGetReadFAT (pThis->m_pFATInfo));
		assert (pBuffer != 0);

		u32 nClusterEntry = FATGetEntry (pThis, pBuffer, nSectorOffset);
		
		assert (pThis->m_pCache != 0);
		FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
		
		//更新FAT1和FAT2
		if (nClusterEntry == 0)
		{
			u32 nFAT = FATInfoGetFirstWriteFAT(pThis->m_pFATInfo);
			for (; nFAT <= FATInfoGetLastWriteFAT (pThis->m_pFATInfo); nFAT++)
			{
				pBuffer = FATGetSector (pThis, nCluster, &nSectorOffset, nFAT);
				assert (pBuffer != 0);

				FATSetEntry (pThis, pBuffer, nSectorOffset, FATInfoGetFATType(pThis->m_pFATInfo) == FAT16 ? 0xFFFF : 0x0FFFFFFF);
				
				FATCacheMarkDirty (pThis->m_pCache, pBuffer);
				FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
			}

			FATInfoClusterAllocated(pThis->m_pFATInfo, nCluster);

			//m_Lock.Release ();

			return nCluster;
		}

		nCluster++;
	}

	//m_Lock.Release ();

	return 0;
}
//释放一个簇链，即删除占用多簇的文件时，将F对应的FAT表项全部置为0
void FATFreeClusterChain (TFat *pThis, u32 nFirstCluster)
{
	do
	{
		u32 nNextCluster = FATGetClusterEntry (pThis, nFirstCluster);

		FATSetClusterEntry (pThis, nFirstCluster, 0);

		assert (pThis->m_pFATInfo != 0);
		FATInfoClusterFreed (pThis->m_pFATInfo, nFirstCluster);

		nFirstCluster = nNextCluster;
	}
	while (!FATIsEOC (pThis, nFirstCluster));
}

//获取某一个文件占用的最后一簇
u32 FATGetFileLastCluster (TFat *pThis, u32 nFirstCluster)
{
	u32 mCurCluster = nFirstCluster;
	u32 nNextCluster = FATGetClusterEntry (pThis, mCurCluster);

	while(!FATIsEOC (pThis, nNextCluster)){
		mCurCluster = nNextCluster;
		nNextCluster = FATGetClusterEntry (pThis, mCurCluster);
	}

	return mCurCluster;
}

//根据文件的相对偏移offset（相对于文件头）获取offset对应的簇号
u32 FATGetFileCluster (TFat *pThis, u32 nFirstCluster, u32 nOffset)
{
    u32 nSectorsPerCluster = FATInfoGetSectorsPerCluster (pThis->m_pFATInfo);

    u32 nSectorOffset = nOffset / FAT_SECTOR_SIZE; //当前的offset所处的sector偏移
    u32 nClusterForCurFile = nSectorOffset / nSectorsPerCluster;//当前的offset在文件中处于第几簇

	u32 mCurCluster = nFirstCluster;
	u32 nNextCluster = FATGetClusterEntry (pThis, mCurCluster);

	u32 nCount = 0;

	while(!FATIsEOC (pThis, nNextCluster)){

		if (nCount == nClusterForCurFile)
		{
			break;
		}

		mCurCluster = nNextCluster;
		nCount++;

		nNextCluster = FATGetClusterEntry (pThis, mCurCluster);

	}

	if (nCount < nClusterForCurFile)
	{
		return 0;
	}

	return mCurCluster;
}

//获取fat扇区数据,nFAT表示第几个FAT表，FAT1为0，FAT2为1
TFATBuffer *FATGetSector (TFat *pThis, u32 nCluster, u32 *pSectorOffset, u32 nFAT)
{

	assert (nCluster >= 2);
	
	u32 nFATOffset;

	assert (pThis->m_pFATInfo != 0);
	if (FATInfoGetFATType(pThis->m_pFATInfo) == FAT16)
	{
		nFATOffset = nCluster * 2;
	}
	else
	{
		assert (FATInfoGetFATType(pThis->m_pFATInfo) == FAT32);

		nFATOffset = nCluster * 4;//每个fat表项4个字节
	}

	u32 nFATSector =   FATInfoGetReservedSectors (pThis->m_pFATInfo)//保留区扇区数
			      + nFAT * FATInfoGetFATSize (pThis->m_pFATInfo)//当获取FAT1时，nFAT为0，FAT2时，nFAT为1
			      + (nFATOffset / FAT_SECTOR_SIZE);//nCluster对应的表项在扇区中偏移

	assert (pSectorOffset != 0);
	*pSectorOffset = nFATOffset % FAT_SECTOR_SIZE;

	assert (pThis->m_pCache != 0);
	return FATCacheGetSector (pThis->m_pCache, nFATSector, 0);
}

//获取fat表项（4字节32位）
u32 FATGetEntry (TFat *pThis, TFATBuffer *pBuffer, u32 nSectorOffset)
{
	assert (pBuffer != 0);

	u32 nEntry;
	if (FATInfoGetFATType(pThis->m_pFATInfo) == FAT16)
	{
		assert (nSectorOffset <= FAT_SECTOR_SIZE-2);
		nEntry =   (u16) pBuffer->Data[nSectorOffset]
			 | (u16) pBuffer->Data[nSectorOffset+1] << 8;
	}
	else
	{
		assert (nSectorOffset <= FAT_SECTOR_SIZE-4);
		nEntry = (  (u32) pBuffer->Data[nSectorOffset]
			  | (u32) pBuffer->Data[nSectorOffset+1] << 8
			  | (u32) pBuffer->Data[nSectorOffset+2] << 16
			  | (u32) pBuffer->Data[nSectorOffset+3] << 24) & 0x0FFFFFFF;
	}

	return nEntry;
}

//设置fat表（4字节32位）
void FATSetEntry (TFat *pThis, TFATBuffer *pBuffer, u32 nSectorOffset, u32 nEntry)
{
	assert (pBuffer != 0);

	if (FATInfoGetFATType(pThis->m_pFATInfo) == FAT16)
	{
		assert (nSectorOffset <= FAT_SECTOR_SIZE-2);
		assert (nEntry <= 0xFFFF);
		pBuffer->Data[nSectorOffset]   = (u8) (nEntry & 0xFF);
		pBuffer->Data[nSectorOffset+1] = (u8) (nEntry >> 8);
	}
	else
	{
		assert (nSectorOffset <= FAT_SECTOR_SIZE-4);
		assert (nEntry <= 0x0FFFFFFF);
		pBuffer->Data[nSectorOffset]   = (u8) (nEntry & 0xFF);
		pBuffer->Data[nSectorOffset+1] = (u8) (nEntry >> 8  & 0xFF);
		pBuffer->Data[nSectorOffset+2] = (u8) (nEntry >> 16 & 0xFF);
		pBuffer->Data[nSectorOffset+3] &= 0xF0;
		pBuffer->Data[nSectorOffset+3] |= (u8) (nEntry >> 24 & 0x0F);
	}
}
