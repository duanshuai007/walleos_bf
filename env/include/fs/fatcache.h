//
// uspienv/fs/fatcache.h
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
//
#ifndef ENV_INCLUDE_USPIENV_FS_FATCACHE_H
#define ENV_INCLUDE_USPIENV_FS_FATCACHE_H

#include <fs/fatfsdef.h>
#include <fs/fat_struct.h>

void CFATCache (TFatCache *pThis);
void _CFATCache (TFatCache *pThis);

/*
 * Open buffer cache
 *
 * Params:  pPartition		Partition to be used
 * Returns: Nonzero on success
 */
s32 FATCacheOpen (TFatCache *pThis, TPartition *pPartition);

/*
 * Close buffer cache
 *
 * Params:  none
 * Returns: none
 */
void FATCacheClose (TFatCache *pThis);


/*
 * Flush buffer cache
 *
 * Params:  none
 * Returns: none
 */
void FATCacheFlush (TFatCache *pThis);

/*
 * Get sector from buffer cache
 *
 * Params:  nSector	Sector number
 *	    bWriteOnly	Do not read physical block into buffer
 * Returns: != 0	Pointer to buffer
 *	    0		Failure
 */
TFATBuffer *FATCacheGetSector (TFatCache *pThis, u32 nSector, s32 bWriteOnly);

/*
 * Free sector in buffer cache
 *
 * Params:  pBuffer	Pointer to buffer
 *	    bCritical	Sector is part of critical data
 * Returns: none
 */
void FATCacheFreeSector (TFatCache *pThis, TFATBuffer *pBuffer, s32 bCritical);

/*
 * Mark buffer dirty (has to be written to disk)
 *
 * Params:  pBuffer	Pointer to buffer
 * Returns: none
 */
void FATCacheMarkDirty (TFatCache *pThis, TFATBuffer *pBuffer);

void FATCacheSetClusterBuff(TFatCache *pThis, u32 nSectorPerCluster, u32 nFirstClusterOffset);

void FATCacheClusterFlush (TFatCache *pThis);

TFATClusterBuffer *FATCacheGetCluster (TFatCache *pThis, u32 nCluster, s32 bWriteOnly);

void FATCacheFreeCluster (TFatCache *pThis, TFATClusterBuffer *pBuffer, s32 bCritical);

void FATCacheMarkClusterDirty (TFatCache *pThis, TFATClusterBuffer *pBuffer);

#endif
