//
// partition.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#include <fs/partition.h>
#include <assert.h>
#include <string.h>
#include <drivers/mmc_export.h>
#include <config.h>

void CPartition (TPartition *pThis, u32 nFirstSector, u32 nNumberOfSectors, s8 *pPartitionName)
{
	pThis->m_nFirstSector = nFirstSector;
	pThis->m_nNumberOfSectors = nNumberOfSectors;

	//LoggerWrite(LoggerGet (), "CPartition", LogDebug, "pThis->m_nFirstSector = %d, pThis->m_nNumberOfSectors = %d\n", pThis->m_nFirstSector, pThis->m_nNumberOfSectors);

	assert(pPartitionName!=0);
	strcpy(pThis->m_pPartitionName, pPartitionName);
}

void _CPartition (TPartition *pThis)
{
    UNUSED(pThis);

}

s32 PartitionRead (TPartition *pThis, void *pBuffer, size_t start, size_t sectorNum)
{

	size_t ullTransferEnd = start + sectorNum;

	if (ullTransferEnd > pThis->m_nNumberOfSectors)
	{
		return -1;
	}
	
	return mmc_bread(pThis->m_nFirstSector + start, sectorNum, pBuffer);
}

s32 PartitionWrite (TPartition *pThis, size_t  start, size_t  blkcnt, const void *src)
{
	u64 ullTransferEnd = start + blkcnt;
	if (ullTransferEnd > pThis->m_nNumberOfSectors)
	{
		return -1;
	}
	
	return mmc_bwrite(pThis->m_nFirstSector + start, blkcnt, src);
}
