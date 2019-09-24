//
// uspienv/fs/partition.h
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
#ifndef ENV_INCLUDE_USPIENV_FS_PARTITION_H
#define ENV_INCLUDE_USPIENV_FS_PARTITION_H

#include <types.h>
#include <fs/fs_struct.h>

void CPartition (TPartition *pThis, u32 nFirstSector, u32 nNumberOfSectors, s8 *pPartitionName);
void _CPartition (TPartition *pThis);

s32 PartitionRead (TPartition *pThis, void *pBuffer, size_t start, size_t sectorNum);
s32 PartitionWrite (TPartition *pThis, size_t  start, size_t  blkcnt, const void *src);

#endif
