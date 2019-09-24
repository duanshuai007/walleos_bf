//
// uspienv/fs/fat.h
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
#ifndef ENV_INCLUDE_USPIENV_FS_FAT_H
#define ENV_INCLUDE_USPIENV_FS_FAT_H

#include <fs/fatcache.h>
#include <fs/fatinfo.h>
#include <types.h>

void CFAT (TFat *pThis, TFatCache *pCache, TFatInfo *pFATInfo);
void _CFAT (TFat *pThis);

u32 FATGetClusterEntry (TFat *pThis, u32 nCluster);
boolean FATIsEOC (TFat *pThis, u32 nClusterEntry);		// end of cluster chain?
void FATSetClusterEntry (TFat *pThis, u32 nCluster, u32 nEntry);

u32 FATAllocateCluster (TFat *pThis);			// returns 0 on failure
void FATFreeClusterChain (TFat *pThis, u32 nFirstCluster);

u32 FATGetFileLastCluster (TFat *pThis, u32 nFirstCluster);

u32 FATGetFileCluster (TFat *pThis, u32 nFirstCluster, u32 nOffset);

#endif
