//
// uspienv/fs/fatfs.h
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
#ifndef ENV_INCLUDE_USPIENV_FS_FATFS_H
#define ENV_INCLUDE_USPIENV_FS_FATFS_H

#include <fs/fatfsdef.h>
#include <fs/fatcache.h>
#include <fs/fatinfo.h>
#include <fs/fat.h>
#include <fs/fatdir.h>
#include <types.h>
#include <fs/fat_struct.h>
#include <fs/file_struct.h>
#include <stdio.h>
/*
 * Mount file system
 *
 * Params:  pPartition      Partition to be used
 * Returns: Nonzero on success
 */
s32 FATFileSystemMount (void);

/*
 * UnMount file system
 *
 * Params:  none
 * Returns: none
 */
void FATFileSystemUnMount (void);

/*
 * Flush buffer cache
 *
 * Params:  none
 * Returns: none
 */
void FATFileSystemSynchronize (void);

/*
* Find first directory entry
*
* Params:  pBuffer      Buffer to receive information
*          pCurrentEntry    Pointer to current entry variable
* Returns: != 0         Entry found
*       0           Not found
*/
u32 FATFileSystemDirFindFirst (TDirectory *pUpperDir, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);
/*
* Find next directory entry
*
* Params:  pBuffer      Buffer to receive information
*          pCurrentEntry    Pointer to current entry variable
* Returns: != 0         Entry found
*       0           No more entries
*/
u32 FATFileSystemDirFindNext (TDirectory *pUpperDir, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);

u32 FATFileSystemRootDirOpen (TDirectory *pTargetDir);

u32 FATFileSystemDirOpen (TDirectory *pUpperDir, TDirectory *pTargetDir);

FILE *FATFileSystemFileOpen (TDirectory *pUpperDir, const s8 *pTitle, u32 mode);
u32 FATFileSystemDirCreate (TDirectory *pUpperDir, const s8 *pDirName);
FILE *FATFileSystemFileCreate (TDirectory *pUpperDir, const s8 *pFileName);
u32 FATFileSystemFileClose (FILE * file);
u32 FATFileSystemFileRead (FILE *pFile, void *pBuffer, u32 ulBytes);
u32 FATFileSystemFileWrite (FILE *pFile, const void *pBuffer, u32 ulBytes);

s32 FATFileSystemFileDelete (TDirectory *pUpperDir, const s8 *pTitle);
u32 FATFileSystemGetSize (FILE *pFile);

//文件偏移位置
#define   FAT_SEEK_SET  0  //文件头
#define   FAT_SEEK_CUR  1  //当前位置
#define   FAT_SEEK_END  2  //文件位
u32 FATFileSystemFileSeek(FILE *pFile, s32 offset, u32 fromwhere);

#endif
