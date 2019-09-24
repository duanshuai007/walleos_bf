//
// fatcache.cpp
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
#include <alloc.h>
#include <fs/fatcache.h>
#include <fs/partition.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <config.h>

#define BUFFER_MAGIC		0x4641544D
#define BUFFER_NOSECTOR		0xFFFFFFFF
#define BUFFER_NOCLUSTER		0xFFFFFFFF

#define FAULT_NO_BUFFER		0x1501
#define FAULT_READ_ERROR	0x1502
#define FAULT_WRITE_ERROR	0x1503


void FATCacheMoveBufferFirst (TFatCache *pThis, TFATBuffer *pBuffer);
void FATCacheMoveBufferLast (TFatCache *pThis, TFATBuffer *pBuffer);
void FATCacheMoveClusterBufferFirst (TFatCache *pThis, TFATClusterBuffer *pBuffer);
void FATCacheMoveClusterBufferLast (TFatCache *pThis, TFATClusterBuffer *pBuffer);
void FATCacheFault (TFatCache *pThis, u32 nCode);

void CFATCache (TFatCache *pThis)
{
	pThis->m_pPartition = 0;
	pThis->m_pBufferMem = 0;
	pThis->m_pClusterBufferMem = 0;

}

void _CFATCache (TFatCache *pThis)
{
    UNUSED(pThis);
}

s32 FATCacheOpen (TFatCache *pThis, TPartition *pPartition)
{
	TFATBuffer *pBuffer;
	s32 i;
	
	assert (pThis->m_pPartition == 0);
	pThis->m_pPartition = pPartition;
	assert (pThis->m_pPartition != 0);

	assert (pThis->m_pBufferMem == 0);
	pThis->m_pBufferMem = malloc (sizeof (TFATBuffer)*FAT_BUFFERS);//FAT_BUFFERS==100
	if (!pThis->m_pBufferMem)
	{
		return ENOMEM;
	}

	for (pBuffer = (TFATBuffer *) (pThis->m_pBufferMem), i = 1; i <= FAT_BUFFERS; pBuffer++, i++)
	{
		pBuffer->nMagic    = BUFFER_MAGIC;
		pBuffer->pNext     = (i == FAT_BUFFERS ? 0 : pBuffer + 1);
		pBuffer->pPrev     = (i == 1 ? 0 : pBuffer - 1);
		pBuffer->nSector    = BUFFER_NOSECTOR;
		pBuffer->nUseCount = 0;
		
		if (i == 1)
		{
			pThis->m_BufferList.pFirst = pBuffer;//第一项
		}
		else if (i == FAT_BUFFERS)
		{
			pThis->m_BufferList.pLast = pBuffer;//最后一项
		}
	}


	assert (pThis->m_pClusterBufferMem == 0);
	pThis->m_pClusterBufferMem = malloc (sizeof (TFATClusterBuffer)*FAT_CLUSER_BUFFERS);//FAT_CLUSER_BUFFERS==10
	if (!pThis->m_pClusterBufferMem)
	{
		return ENOMEM;
	}

	TFATClusterBuffer *pCluserBuffer;
	for (pCluserBuffer = (TFATClusterBuffer *) (pThis->m_pClusterBufferMem), i = 1; i <= FAT_CLUSER_BUFFERS; pCluserBuffer++, i++)
	{
		pCluserBuffer->nMagic    = BUFFER_MAGIC;
		pCluserBuffer->pNext     = (i == FAT_CLUSER_BUFFERS ? 0 : pCluserBuffer + 1);
		pCluserBuffer->pPrev     = (i == 1 ? 0 : pCluserBuffer - 1);
		pCluserBuffer->nCluster    = BUFFER_NOCLUSTER;
		pCluserBuffer->nUseCount = 0;
		pCluserBuffer->pBuf = 0;
		pCluserBuffer->nSectorNumPerCluster = 0;
		
		if (i == 1)
		{
			pThis->m_ClusterBufList.pFirst = pCluserBuffer;//第一项
		}
		else if (i == FAT_CLUSER_BUFFERS)
		{
			pThis->m_ClusterBufList.pLast = pCluserBuffer;//最后一项
		}
	}

	return 1;
}

//初始化每簇buff，只有在解析完引导扇区后，才知道一簇的大小。
void FATCacheSetClusterBuff(TFatCache *pThis, u32 nSectorPerCluster, u32 nFirstDataSector)
{
	TFATClusterBuffer *pCluserBuffer = pThis->m_ClusterBufList.pFirst;

	u32 size =  nSectorPerCluster * FAT_SECTOR_SIZE;
	while (0 != pCluserBuffer)
	{
		assert(pCluserBuffer->pBuf == 0);

		pCluserBuffer->pBuf = (u8 *)malloc(size);
        if(pCluserBuffer->pBuf == NULL)
        {
            return;
        }
		pCluserBuffer->nSectorNumPerCluster = nSectorPerCluster;
		
		pCluserBuffer->m_nFirstDataSector = nFirstDataSector;

		pCluserBuffer = pCluserBuffer->pNext;
	}
}

void FATCacheClose (TFatCache *pThis)
{
	FATCacheFlush (pThis);

	pThis->m_BufferList.pFirst = 0;
	pThis->m_BufferList.pLast = 0;

	assert (pThis->m_pBufferMem != 0);
	free (pThis->m_pBufferMem);
	pThis->m_pBufferMem = 0;

	TFATClusterBuffer *pCluserBuffer = pThis->m_ClusterBufList.pFirst;
	while (0 != pCluserBuffer)
	{
		if (0 != pCluserBuffer->pBuf)
		{
			free(pCluserBuffer->pBuf);
			pCluserBuffer->pBuf = 0;
		}
		
		pCluserBuffer = pCluserBuffer->pNext;
	}

	assert (pThis->m_pClusterBufferMem != 0);
	free(pThis->m_pClusterBufferMem);
	pThis->m_pClusterBufferMem = 0;

}

void FATCacheFlush (TFatCache *pThis)
{
	TFATBuffer *pBuffer;

	//	m_BufferListLock.Acquire ();

	for (pBuffer = pThis->m_BufferList.pFirst; pBuffer != 0; pBuffer = pBuffer->pNext)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nSector != BUFFER_NOSECTOR)
		{
			if (pBuffer->bDirty)
			{
				//	m_DiskLock.Acquire ();
				if (PartitionWrite (pThis->m_pPartition, pBuffer->nSector, 1, pBuffer->Data) != 1)
				{
					FATCacheFault (pThis, FAULT_WRITE_ERROR);
				}

				//	m_DiskLock.Release ();

				pBuffer->bDirty = 0;
			}
		}
	}
	//	m_BufferListLock.Release ();
}


TFATBuffer *FATCacheGetSector (TFatCache *pThis, u32 nSector, s32 bWriteOnly)
{
	TFATBuffer *pBuffer;

	//	m_BufferListLock.Acquire ();

	for (pBuffer = pThis->m_BufferList.pFirst; pBuffer != 0; pBuffer = pBuffer->pNext)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nSector == nSector)
		{
			break;
		}
	}

	if (pBuffer != 0)
	{
		FATCacheMoveBufferFirst (pThis, pBuffer);

		pBuffer->nUseCount++;

		//	m_BufferListLock.Release ();

		return pBuffer;
	}


	for (pBuffer = pThis->m_BufferList.pLast; pBuffer != 0; pBuffer = pBuffer->pPrev)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nSector == BUFFER_NOSECTOR)
		{
			break;
		}
	}

	if (pBuffer == 0)
	{
		for (pBuffer = pThis->m_BufferList.pLast; pBuffer != 0; pBuffer = pBuffer->pPrev)
		{
			assert (pBuffer->nMagic == BUFFER_MAGIC);

			if (pBuffer->nUseCount == 0)
			{
				break;
			}
		}

		if (pBuffer == 0)
		{
			FATCacheFault (pThis, FAULT_NO_BUFFER);

			//	m_BufferListLock.Release ();
			return 0;
		}

		if (pBuffer->bDirty)
		{
			//	m_DiskLock.Acquire ();

			if (PartitionWrite (pThis->m_pPartition, pBuffer->nSector, 1, pBuffer->Data) != 1) //写一个扇区数据
			{
				FATCacheFault (pThis, FAULT_WRITE_ERROR);
				//	m_DiskLock.Release ();
				//	m_BufferListLock.Release ();

				return 0;
			}

			//	m_DiskLock.Release ();
		}

		pBuffer->nSector = BUFFER_NOSECTOR;
		assert (pBuffer->nUseCount == 0);
	}

	pBuffer->nUseCount = 1;
	assert (pBuffer->nSector == BUFFER_NOSECTOR);
	pBuffer->nSector = nSector;
	pBuffer->bDirty = 0;

	if (!bWriteOnly)
	{
		//	m_DiskLock.Acquire ();

		if (PartitionRead (pThis->m_pPartition, pBuffer->Data, nSector, 1) != 1)//读取一个扇区数据
		{
			pBuffer->nUseCount--;
			pBuffer->nSector = BUFFER_NOSECTOR;

			FATCacheFault (pThis, FAULT_READ_ERROR);
			//	m_DiskLock.Release ();
			//	m_BufferListLock.Release ();

			return 0;
		}

		//	m_DiskLock.Release ();
	}

	FATCacheMoveBufferFirst (pThis, pBuffer);

	//	m_BufferListLock.Release ();

	return pBuffer;
}


void FATCacheFreeSector (TFatCache *pThis, TFATBuffer *pBuffer, s32 bCritical)
{
	assert (pBuffer->nMagic == BUFFER_MAGIC);
	assert (pBuffer->nUseCount > 0);
	pBuffer->nUseCount--;
	if (pBuffer->nUseCount > 0)
	{
		return;
	}

	if (bCritical)
	{
#if 0
		if (pBuffer->bDirty)
		{
			m_DiskLock.Acquire ();

			m_pPartition->Seek (pBuffer->nSector * FAT_SECTOR_SIZE);
			if (m_pPartition->Write (pBuffer->Data, FAT_SECTOR_SIZE) == FAT_SECTOR_SIZE)
			{
				pBuffer->bDirty = 0;
			}

			m_DiskLock.Release ();
		}
#endif
	}
	else
	{
		//	m_BufferListLock.Acquire ();

		FATCacheMoveBufferLast(pThis, pBuffer);

		//	m_BufferListLock.Release ();
	}
}


void FATCacheMarkDirty (TFatCache *pThis, TFATBuffer *pBuffer)
{
    UNUSED(pThis);

	assert (pBuffer->nMagic == BUFFER_MAGIC);
	assert (pBuffer->nUseCount > 0);
	pBuffer->bDirty = 1;
}


//移到首位
void FATCacheMoveBufferFirst (TFatCache *pThis, TFATBuffer *pBuffer)
{
	if (pThis->m_BufferList.pFirst != pBuffer)
	{
		TFATBuffer *pNext = pBuffer->pNext;
		TFATBuffer *pPrev = pBuffer->pPrev;

		pPrev->pNext = pNext;

		if (pNext)
		{
			pNext->pPrev = pPrev;
		}
		else
		{
			pThis->m_BufferList.pLast = pPrev;
		}

		pThis->m_BufferList.pFirst->pPrev = pBuffer;
		pBuffer->pNext = pThis->m_BufferList.pFirst;
		pThis->m_BufferList.pFirst = pBuffer;
		pBuffer->pPrev = 0;
	}
}

//移动到末尾
void FATCacheMoveBufferLast (TFatCache *pThis, TFATBuffer *pBuffer)
{
	if (pThis->m_BufferList.pLast != pBuffer)
	{
		TFATBuffer *pNext = pBuffer->pNext;
		TFATBuffer *pPrev = pBuffer->pPrev;

		pNext->pPrev = pPrev;

		if (pPrev)
		{
			pPrev->pNext = pNext;
		}
		else
		{
			pThis->m_BufferList.pFirst = pNext;
		}

		pThis->m_BufferList.pLast->pNext = pBuffer;
		pBuffer->pPrev = pThis->m_BufferList.pLast;
		pThis->m_BufferList.pLast = pBuffer;
		pBuffer->pNext = 0;
	}
}


void FATCacheClusterFlush (TFatCache *pThis)
{
	TFATClusterBuffer *pBuffer;

	//	m_BufferListLock.Acquire ();

	for (pBuffer = pThis->m_ClusterBufList.pFirst; pBuffer != 0; pBuffer = pBuffer->pNext)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nCluster != BUFFER_NOCLUSTER)
		{
			if (pBuffer->bDirty)
			{
				//	m_DiskLock.Acquire ();
				size_t start = pBuffer->m_nFirstDataSector + ( pBuffer->nCluster - 2 ) * pBuffer->nSectorNumPerCluster;

				if ((u32)PartitionWrite (pThis->m_pPartition, start, pBuffer->nSectorNumPerCluster, pBuffer->pBuf) != pBuffer->nSectorNumPerCluster)
				{
					FATCacheFault (pThis, FAULT_WRITE_ERROR);
				}
				//	m_DiskLock.Release ();
				pBuffer->bDirty = 0;
			}
		}
	}
	//	m_BufferListLock.Release ();
}


TFATClusterBuffer *FATCacheGetCluster (TFatCache *pThis, u32 nCluster, s32 bWriteOnly)
{
	TFATClusterBuffer *pBuffer;

	//	m_BufferListLock.Acquire ();

	for (pBuffer = pThis->m_ClusterBufList.pFirst; pBuffer != 0; pBuffer = pBuffer->pNext)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nCluster == nCluster)
		{
			break;
		}
	}

	if (pBuffer != 0)
	{
		FATCacheMoveClusterBufferFirst (pThis, pBuffer);

		pBuffer->nUseCount++;

		//	m_BufferListLock.Release ();

		return pBuffer;
	}


	for (pBuffer = pThis->m_ClusterBufList.pLast; pBuffer != 0; pBuffer = pBuffer->pPrev)
	{
		assert (pBuffer->nMagic == BUFFER_MAGIC);

		if (pBuffer->nCluster == BUFFER_NOCLUSTER)
		{
			break;
		}
	}

	if (pBuffer == 0)
	{
		for (pBuffer = pThis->m_ClusterBufList.pLast; pBuffer != 0; pBuffer = pBuffer->pPrev)
		{
			assert (pBuffer->nMagic == BUFFER_MAGIC);

			if (pBuffer->nUseCount == 0)
			{
				break;
			}
		}

		if (pBuffer == 0)
		{
			FATCacheFault (pThis, FAULT_NO_BUFFER);

			//	m_BufferListLock.Release ();
			return 0;
		}

		if (pBuffer->bDirty)
		{
			//	m_DiskLock.Acquire ();
			size_t start = pBuffer->m_nFirstDataSector + ( pBuffer->nCluster - 2 ) * pBuffer->nSectorNumPerCluster;
			//PartitionSeek (pThis->m_pPartition, pBuffer->nCluster * pBuffer->nSectorNumPerCluster);
			if ((u32)PartitionWrite (pThis->m_pPartition, start, pBuffer->nSectorNumPerCluster, pBuffer->pBuf) != pBuffer->nSectorNumPerCluster)
			{
				FATCacheFault (pThis, FAULT_WRITE_ERROR);
				//	m_DiskLock.Release ();
				//	m_BufferListLock.Release ();

				return 0;
			}

			//	m_DiskLock.Release ();
		}

		pBuffer->nCluster = BUFFER_NOCLUSTER;
		assert (pBuffer->nUseCount == 0);
	}

	pBuffer->nUseCount = 1;
	assert (pBuffer->nCluster == BUFFER_NOCLUSTER);
	pBuffer->nCluster = nCluster;
	pBuffer->bDirty = 0;

	if (!bWriteOnly)
	{
		//	m_DiskLock.Acquire ();
		size_t start = pBuffer->m_nFirstDataSector + ( pBuffer->nCluster - 2 ) * pBuffer->nSectorNumPerCluster;

		//PartitionSeek (pThis->m_pPartition, pBuffer->nCluster * pBuffer->nSectorNumPerCluster);
		if ((u32)PartitionRead (pThis->m_pPartition, pBuffer->pBuf, start, pBuffer->nSectorNumPerCluster ) != pBuffer->nSectorNumPerCluster)
		{
			pBuffer->nUseCount--;
			pBuffer->nCluster = BUFFER_NOCLUSTER;

			FATCacheFault (pThis, FAULT_READ_ERROR);
			//	m_DiskLock.Release ();
			//	m_BufferListLock.Release ();

			return 0;
		}

		//	m_DiskLock.Release ();
	}

	FATCacheMoveClusterBufferFirst (pThis, pBuffer);

	//	m_BufferListLock.Release ();

	return pBuffer;
}


void FATCacheFreeCluster (TFatCache *pThis, TFATClusterBuffer *pBuffer, s32 bCritical)
{
	assert (pBuffer->nMagic == BUFFER_MAGIC);
	assert (pBuffer->nUseCount > 0);
	pBuffer->nUseCount--;
	if (pBuffer->nUseCount > 0)
	{
		return;
	}

	if (bCritical)
	{
#if 0
		if (pBuffer->bDirty)
		{
			m_DiskLock.Acquire ();

			m_pPartition->Seek (pBuffer->nSector * FAT_SECTOR_SIZE);
			if (m_pPartition->Write (pBuffer->Data, FAT_SECTOR_SIZE) == FAT_SECTOR_SIZE)
			{
				pBuffer->bDirty = 0;
			}

			m_DiskLock.Release ();
		}
#endif
	}
	else
	{
		//	m_BufferListLock.Acquire ();

		FATCacheMoveClusterBufferLast(pThis, pBuffer);

		//	m_BufferListLock.Release ();
	}
}

void FATCacheMarkClusterDirty (TFatCache *pThis, TFATClusterBuffer *pBuffer)
{
    UNUSED(pThis);
	assert (pBuffer->nMagic == BUFFER_MAGIC);
	assert (pBuffer->nUseCount > 0);
	pBuffer->bDirty = 1;
}

//移到首位
void FATCacheMoveClusterBufferFirst (TFatCache *pThis, TFATClusterBuffer *pBuffer)
{
	if (pThis->m_ClusterBufList.pFirst != pBuffer)
	{
		TFATClusterBuffer *pNext = pBuffer->pNext;
		TFATClusterBuffer *pPrev = pBuffer->pPrev;

		pPrev->pNext = pNext;

		if (pNext)
		{
			pNext->pPrev = pPrev;
		}
		else
		{
			pThis->m_ClusterBufList.pLast = pPrev;
		}

		pThis->m_ClusterBufList.pFirst->pPrev = pBuffer;
		pBuffer->pNext = pThis->m_ClusterBufList.pFirst;
		pThis->m_ClusterBufList.pFirst = pBuffer;
		pBuffer->pPrev = 0;
	}
}


//移动到末尾
void FATCacheMoveClusterBufferLast (TFatCache *pThis, TFATClusterBuffer *pBuffer)
{
	if (pThis->m_ClusterBufList.pLast != pBuffer)
	{
		TFATClusterBuffer *pNext = pBuffer->pNext;
		TFATClusterBuffer *pPrev = pBuffer->pPrev;

		pNext->pPrev = pPrev;

		if (pPrev)
		{
			pPrev->pNext = pNext;
		}
		else
		{
			pThis->m_ClusterBufList.pFirst = pNext;
		}

		pThis->m_ClusterBufList.pLast->pNext = pBuffer;
		pBuffer->pPrev = pThis->m_ClusterBufList.pLast;
		pThis->m_ClusterBufList.pLast = pBuffer;
		pBuffer->pNext = 0;
	}
}

void FATCacheFault (TFatCache *pThis, u32 nCode)
{
    UNUSED(pThis);
	const s8 *pMsg = "General error";

	switch (nCode)
	{
	case FAULT_NO_BUFFER:
		pMsg = "No buffer available";
		break;

	case FAULT_READ_ERROR:
		pMsg = "Read error";
		break;

	case FAULT_WRITE_ERROR:
		pMsg = "Write error";
		break;

	default:
		assert (0);
		break;
	}

	//LoggerWrite(LoggerGet (), "fatcache", LogPanic, pMsg);
	printf("fatcache: %s \r\n", pMsg);
}
