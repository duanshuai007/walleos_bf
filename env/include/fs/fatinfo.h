//
// uspienv/fs/fatinfo.h
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
#ifndef _ENV_INCLUDE_USPIENV_FS_FATINFO_H_
#define _ENV_INCLUDE_USPIENV_FS_FATINFO_H_

#include <fs/fatcache.h>
#include <types.h>
#include <fs/fat_struct.h>

void CFATInfo (TFatInfo *pThis, TFatCache *pCache);
void _CFATInfo (TFatInfo *pThis);

boolean FATInfoInitialize (TFatInfo *pThis);

// Boot sector
TFATType FATInfoGetFATType (TFatInfo *pThis);

u32 FATInfoGetSectorsPerCluster (TFatInfo *pThis);
u32 FATInfoGetReservedSectors (TFatInfo *pThis);
u32 FATInfoGetClusterCount (TFatInfo *pThis);

u32 FATInfoGetReadFAT (TFatInfo *pThis);
u32 FATInfoGetFirstWriteFAT (TFatInfo *pThis);
u32 FATInfoGetLastWriteFAT (TFatInfo *pThis);
u32 FATInfoGetFATSize (TFatInfo *pThis);

u32 FATInfoGetFirstRootSector (TFatInfo *pThis);	// FAT16 only
u32 FATInfoGetRootSectorCount (TFatInfo *pThis);	// FAT16 only
u32 FATInfoGetRootEntries (TFatInfo *pThis);		// FAT16 only
u32 FATInfoGetRootCluster (TFatInfo *pThis);		// FAT32 only

u32 FATInfoGetFirstSector (TFatInfo *pThis, u32 nCluster);

// FS Info sector
void FATInfoUpdateFSInfo (TFatInfo *pThis);

void FATInfoClusterAllocated (TFatInfo *pThis, u32 nCluster);
void FATInfoClusterFreed (TFatInfo *pThis, u32 nCluster);

u32 FATInfoGetNextFreeCluster (TFatInfo *pThis);

#endif
