//
// uspienv/fs/fatdir.h
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
#ifndef ENV_INCLUDE_USPIENV_FS_FATDIR_H
#define ENV_INCLUDE_USPIENV_FS_FATDIR_H

#include <fs/fatcache.h>
#include <fs/fatinfo.h>
#include <fs/fat.h>
#include <types.h>
#include <fs/fat_struct.h>
#include <fs/file_struct.h>

void CFATDirectory (TFATDirectory *pThis, TFatCache *pCache, TFatInfo *pFATInfo, TFat *pFAT);
void _CFATDirectory (TFATDirectory *pThis);

TFATDirectoryEntry *FATDirectoryGetRootEntry (TFATDirectory *pThis);

TFATDirectoryEntry *FATDirectoryGetEntry (TFATDirectory *pThis, s32 nUpperFirstCluster, const s8 *pName);
TFATDirectoryEntry *FATDirectoryCreateEntry (TFATDirectory *pThis, s32 nUpperFirstCluster, s32 nFirstCluster, s32 isDir, const s8 *pName);
void FATDirectoryFreeEntry (TFATDirectory *pThis, boolean bChanged);

boolean FATDirectoryFindFirst (TFATDirectory *pThis, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);
boolean FATDirectoryFindNext (TFATDirectory *pThis, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);

u32 FATDirectoryTime2FAT (u32 nTime);   // returns FAT time (date << 16 | time)
void FATDirectoryFAT2Name (const s8 *pFATName, s8 *pName);

#endif
