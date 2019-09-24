//
// fatfs.cpp
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
#include <fs/fatfs.h>
#include <timer.h>
#include <uspienv.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fs/partitionmanager.h>
#include <fs/fatformal.h>
#include <stddef.h>
#include <s5p4418_serial_stdio.h>
#include <errno.h>
#include <headsmp.h>

#define PARTITION   "emmc1-1"

TFATFileSystem *g_pFileSystem;

s32 CFATFileSystem (void)
{
    printf("CFATFileSystem: malloc size = %d\r\n", sizeof(TFATFileSystem));
    g_pFileSystem = (TFATFileSystem *)malloc(sizeof(TFATFileSystem));
    if(g_pFileSystem == NULL)
    {
        printf("malloc failed\r\n");
        return ENOMEM;
    }
    CPartitionManager(&g_pFileSystem->m_PartitionManager, "emmc1");

    CFATCache (&g_pFileSystem->m_Cache);
    CFATInfo(&g_pFileSystem->m_FATInfo, &g_pFileSystem->m_Cache);
    CFAT(&g_pFileSystem->m_FAT, &g_pFileSystem->m_Cache, &g_pFileSystem->m_FATInfo);
    CFATDirectory(&g_pFileSystem->m_Root, &g_pFileSystem->m_Cache, &g_pFileSystem->m_FATInfo, &g_pFileSystem->m_FAT);

    memset (&g_pFileSystem->m_Files, 0, sizeof g_pFileSystem->m_Files);

    return 1;
}

void _CFATFileSystem (void)
{
    _CFATDirectory(&g_pFileSystem->m_Root);
    free(g_pFileSystem);
    g_pFileSystem = 0;
}

//挂载文件系统
s32 FATFileSystemMount (void)
{
    if(!CFATFileSystem()){
        return 0;
    }

    if (!PartitionManagerInitialize (&g_pFileSystem->m_PartitionManager))
    {
        return FALSE;
    }

    TPartition *pPartition = PartitionManagerGetPartition(&g_pFileSystem->m_PartitionManager, PARTITION);
    if (!FATCacheOpen(&g_pFileSystem->m_Cache, pPartition))
    {
        return 0;
    }

    if (!isFatFormalFin(&g_pFileSystem->m_Cache))
    {
        CFatFormal();
        if(FormatFat32Disk(&g_pFileSystem->m_Cache, pPartition))
        {
            printf("FormatFat32Disk failed \n");
            return 0;
        }
    }

    if (!FATInfoInitialize(&g_pFileSystem->m_FATInfo))
    {
        FATCacheClose(&g_pFileSystem->m_Cache);
        return 0;
    }

    return 1;
}

//卸载文件系统
void FATFileSystemUnMount (void)
{
    FATInfoUpdateFSInfo (&g_pFileSystem->m_FATInfo);

    FATCacheClose(&g_pFileSystem->m_Cache);

    _CFATFileSystem();
}
//同步文件系统
void FATFileSystemSynchronize (void)
{
    FATInfoUpdateFSInfo (&g_pFileSystem->m_FATInfo);
    FATCacheFlush(&g_pFileSystem->m_Cache);
    FATCacheClusterFlush(&g_pFileSystem->m_Cache);
}

//遍历目录－获取第一个目录项
u32 FATFileSystemDirFindFirst (TDirectory *pUpperDir, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    pCurrentEntry->nCluster = pUpperDir->nCurDirFirstCluster;
    pCurrentEntry->nEntry = 0;

    return FATDirectoryFindFirst(&g_pFileSystem->m_Root, pCurrentEntry, pEntry) ? 1 : 0;
}

//遍历目录－获取下一个目录项
u32 FATFileSystemDirFindNext (TDirectory *pUpperDir, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    if (pCurrentEntry->nEntry == 0)
    {
        pCurrentEntry->nCluster = pUpperDir->nCurDirFirstCluster;
    }

    return FATDirectoryFindNext (&g_pFileSystem->m_Root, pCurrentEntry, pEntry) ? 1 : 0;
}

//打开根目录
u32 FATFileSystemRootDirOpen (TDirectory *pTargetDir)
{
    assert (pTargetDir != 0);
    TFATDirectoryEntry *pEntry = FATDirectoryGetRootEntry(&g_pFileSystem->m_Root);
    if (pEntry == 0)
    {
        return FS_DIR_NOT_EXIST;
    }

    pTargetDir->nCurDirFirstCluster = FATInfoGetRootCluster(&g_pFileSystem->m_FATInfo);
    pTargetDir->nUpperDirFirstCluster = FS_ROOT_UPPER_CLUSTER;
    FATDirectoryFAT2Name ((const s8 *) pEntry->Name, pTargetDir->cDirectoryName);
    pTargetDir->nInitFlg = 1;

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, FALSE);

    return FS_DIR_OPEN_SUCCESS;
}


//打开目录
u32 FATFileSystemDirOpen (TDirectory *pUpperDir, TDirectory *pTargetDir)
{
    assert (pUpperDir != 0);
    assert (pTargetDir != 0);
    TFATDirectoryEntry *pEntry = FATDirectoryGetEntry(&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, pTargetDir->cDirectoryName);
    if (pEntry == 0)
    {
        return FS_DIR_NOT_EXIST;
    }

    pTargetDir->nCurDirFirstCluster = (u32) pEntry->nFirstClusterHigh << 16 | pEntry->nFirstClusterLow;
    pTargetDir->nUpperDirFirstCluster = pUpperDir->nCurDirFirstCluster;
    pTargetDir->nInitFlg = 1;

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, FALSE);

    return FS_DIR_OPEN_SUCCESS;
}

//打开文件
FILE *FATFileSystemFileOpen (TDirectory *pUpperDir, const s8 *pTitle, u32 mode)
{
    u32 hFile;

    for (hFile = 1; hFile <= FAT_FILES; hFile++)
    {

        if (!strcmp (g_pFileSystem->m_Files[hFile-1].cFileName, pTitle))
        {
            return NULL;
        }

        if (!g_pFileSystem->m_Files[hFile-1].nUseCount)
        {
            break;
        }
    }

    if (hFile > FAT_FILES)
    {
        return NULL;
    }

    assert (pTitle != 0);
    TFATDirectoryEntry *pEntry = FATDirectoryGetEntry(&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, pTitle);//获取根目录结构
    if (pEntry == 0)
    {
        return NULL;
    }

    FILE *pFile = &(g_pFileSystem->m_Files[hFile-1]);

    pFile->nUseCount = 1;
    strncpy (pFile->cFileName, pTitle, FS_TITLE_LEN);
    pFile->cFileName[FS_TITLE_LEN] = '\0';
    pFile->nSize = pEntry->nFileSize;

    pFile->upperDir = (s8 *)malloc(sizeof(TDirectory));
    if(pFile->upperDir == NULL)
    {
        return NULL;
    }
    memcpy(pFile->upperDir, pUpperDir, sizeof(TDirectory));

    pFile->nFirstCluster = (u32) pEntry->nFirstClusterHigh << 16 | pEntry->nFirstClusterLow;

    if (mode == 0)
    {
        pFile->nOffset = 0;

        pFile->bWrite = FALSE;
 
        pFile->nCluster = pFile->nFirstCluster;

        pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 0);

    }
    else
    {
        pFile->nOffset = pEntry->nFileSize;

        pFile->bWrite = TRUE;

        pFile->nCluster = FATGetFileLastCluster(&g_pFileSystem->m_FAT, pFile->nFirstCluster);

        pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 1);

    }

    assert (pFile->pBuffer != NULL);
 
    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, FALSE);

    return pFile;
}

//创建目录
u32 FATFileSystemDirCreate (TDirectory *pUpperDir, const s8 *pDirName)
{
    assert(pUpperDir != 0);
    assert(pDirName != 0);

    TFATDirectoryEntry *pDirectoryEntry = FATDirectoryGetEntry (&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, pDirName);

    if (pDirectoryEntry != 0)
    {
        FATDirectoryFreeEntry (&g_pFileSystem->m_Root, FALSE);
        return FS_DIR_CREATE_EXISTED;
    }

    u32 nFirstCluster = FATAllocateCluster (&g_pFileSystem->m_FAT);
    if (nFirstCluster == 0)
    {
        return FS_ERROR;
    }

    TFATDirectoryEntry *pEntry = FATDirectoryCreateEntry (&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, nFirstCluster, 0, pDirName);
    if (pEntry == 0)
    {
        return FS_DIR_CREATE_FAILED;
    }

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);

    FATFileSystemSynchronize ();

    TFATDirectoryEntry *pSiglePointEntry = FATDirectoryCreateEntry (&g_pFileSystem->m_Root, nFirstCluster, nFirstCluster, 1, ".");
    if (pSiglePointEntry == 0)
    {
        return FS_DIR_CREATE_FAILED;
    }

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);

    FATFileSystemSynchronize ();


    TFATDirectoryEntry *PDoublePointEntry = FATDirectoryCreateEntry (&g_pFileSystem->m_Root, nFirstCluster, pUpperDir->nCurDirFirstCluster, 1, "..");
    if (PDoublePointEntry == 0)
    {
        return FS_DIR_CREATE_FAILED;
    }

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);    

    FATFileSystemSynchronize ();

    return FS_DIR_CREATE_SUCCESS;
}

//创建文件
FILE *FATFileSystemFileCreate (TDirectory *pUpperDir, const s8 *pFileName)
{
    u32 hFile;
    for (hFile = 1; hFile <= FAT_FILES; hFile++)
    {
        if (!g_pFileSystem->m_Files[hFile-1].nUseCount)
        {
            break;
        }
    }

    if (hFile > FAT_FILES)
    {
        return NULL;
    }

    assert (pFileName != 0);
    if (FATFileSystemFileDelete (pUpperDir, pFileName) < 0)
    {
        //m_FileTableLock.Release ();

        return NULL;
    }

    TFATDirectoryEntry *pEntry = FATDirectoryCreateEntry (&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, 0, 2, pFileName);
    if (pEntry == NULL)
    {
        //m_FileTableLock.Release ();

        return NULL;
    }

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);

    FATFileSystemSynchronize();

    FILE *pFile = &(g_pFileSystem->m_Files[hFile-1]);

    pFile->nUseCount = 1;
    strncpy (pFile->cFileName, pFileName, FS_TITLE_LEN);
    pFile->cFileName[FS_TITLE_LEN] = '\0';
    pFile->nSize = 0;
    pFile->nOffset = 0;
    pFile->nCluster = 0;
    pFile->nFirstCluster = 0;
    pFile->pBuffer = NULL;
    pFile->bWrite = 1;
    pFile->upperDir = (s8 *)malloc(sizeof(TDirectory));    
    if(pFile->upperDir == NULL)
    {
        return NULL;
    }
    memcpy(pFile->upperDir, pUpperDir, sizeof(TDirectory));

    //m_FileTableLock.Release ();

    return pFile;
}

u32 FATFileSystemFileClose (FILE * file)
{   
    assert(file != NULL);

    //m_FileTableLock.Acquire ();

    if (!file->nUseCount)
    {
        //m_FileTableLock.Release ();
        return 0;
    }

    if (file->nUseCount > 1)
    {
        file->nUseCount--;
        //m_FileTableLock.Release ();
        return 1;
    }

    if (file->pBuffer != NULL)
    {
        FATCacheFreeCluster (&g_pFileSystem->m_Cache, (TFATClusterBuffer *)(file->pBuffer), 0);
        file->pBuffer = NULL;
    }

    if (file->bWrite)
    {
        TDirectory *pDirectory = (TDirectory *)(file->upperDir);
        TFATDirectoryEntry *pEntry = FATDirectoryGetEntry (&g_pFileSystem->m_Root, pDirectory->nCurDirFirstCluster, file->cFileName);
        if (pEntry != 0)
        {
            pEntry->nAttributes |= FAT_DIR_ATTR_ARCHIVE;

            pEntry->nFirstClusterHigh = file->nFirstCluster >> 16;
            pEntry->nFirstClusterLow  = file->nFirstCluster & 0xFFFF;

            pEntry->nFileSize = file->nSize;

			u8 cpu = get_cpuid();
            u32 nDateTime = FATDirectoryTime2FAT (TimerGetTime(TimerGet(cpu)));
            pEntry->nWriteDate = nDateTime >> 16;
            pEntry->nWriteTime = nDateTime & 0xFFFF;

            FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);
        }

        FATFileSystemSynchronize ();
    }

    if (NULL != file->upperDir)
    {
        free(file->upperDir);
        file->upperDir = NULL;

    }
    memset(file, 0x0, sizeof(FILE));

    //m_FileTableLock.Release ();

    return 1;
}

u32 FATFileSystemFileRead (FILE *pFile, void *pBuffer, u32 ulBytes)
{
    assert (pFile != 0);

    u32 ulBytesRead = 0;


    //m_FileTableLock.Acquire ();

    if (   !pFile->nUseCount
        || pFile->bWrite
        || ulBytes == 0)
    {
        //m_FileTableLock.Release ();
        return FS_ERROR;
    }

    while (ulBytes > 0)
    {
        u32 ulBlockOffset;
        u32 ulBytesLeft;
        u32 ulCopyBytes;

        assert (pFile->nSize >= pFile->nOffset);
        ulBytesLeft = pFile->nSize - pFile->nOffset;
        if (ulBytesLeft == 0)
        {
            //m_FileTableLock.Release ();
            return ulBytesRead;
        }

        if (pFile->pBuffer == NULL)
        {
            if (pFile->nOffset > 0)
            {
                    //m_FileTableLock.Release ();

                pFile->nCluster = FATGetClusterEntry (&g_pFileSystem->m_FAT, pFile->nCluster);
                if (FATIsEOC (&g_pFileSystem->m_FAT, pFile->nCluster))
                {
                    //m_FileTableLock.Release ();
                    return FS_ERROR;
                }
            }

            pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 0);
            assert (pFile->pBuffer != NULL);
        }

        u32 uClusterSize = FAT_SECTOR_SIZE * FATInfoGetSectorsPerCluster(&g_pFileSystem->m_FATInfo);

        ulCopyBytes = ulBytesLeft;
        if (ulBytesLeft > uClusterSize)
        {
            ulCopyBytes = uClusterSize;
        }


        if (ulCopyBytes > ulBytes)
        {
            ulCopyBytes = ulBytes;
        }

    
        ulBlockOffset = pFile->nOffset % uClusterSize;
        if (ulCopyBytes > uClusterSize - ulBlockOffset)
        {
            ulCopyBytes = uClusterSize - ulBlockOffset;
        }

 

        assert (pBuffer != NULL);
        assert (ulCopyBytes > 0);

        TFATClusterBuffer *pTFATClusterBuffer = (TFATClusterBuffer *)pFile->pBuffer;
        memcpy (pBuffer, pTFATClusterBuffer->pBuf + ulBlockOffset, ulCopyBytes);

        pBuffer = (void *) (((u8 *) pBuffer) + ulCopyBytes);

        pFile->nOffset += ulCopyBytes;

        ulBytes -= ulCopyBytes;
        ulBytesRead += ulCopyBytes;

        if ((pFile->nOffset % uClusterSize) == 0)
        {
            assert (pFile->pBuffer != 0);
            FATCacheFreeCluster (&g_pFileSystem->m_Cache, (TFATClusterBuffer *)(pFile->pBuffer), 0);
            pFile->pBuffer = NULL;
        }
    }

    //m_FileTableLock.Release ();

    return ulBytesRead;
}

u32 FATFileSystemFileWrite (FILE *pFile, const void *pBuffer, u32 ulBytes)
{
    assert (pFile != 0);

    u32 ulBytesWritten = 0;

    if (   !pFile->nUseCount
        || !pFile->bWrite
        || ulBytes == 0)
    {
        //m_FileTableLock.Release ();
        return FS_ERROR;
    }

    while (ulBytes > 0)
    {
        u32 ulBlockOffset;
        u32 ulBytesLeft;
        u32 ulCopyBytes;

        assert (pFile->nSize <= FAT_MAX_FILESIZE);
        ulBytesLeft = FAT_MAX_FILESIZE - pFile->nSize;
        if (ulBytesLeft == 0)
        {
            //m_FileTableLock.Release ();
            return ulBytesWritten;
        }

        if (pFile->pBuffer == NULL)
        {
            u32 nSectorOffset = pFile->nOffset / FAT_SECTOR_SIZE;
            u32 nClusterOffset = nSectorOffset % FATInfoGetSectorsPerCluster (&g_pFileSystem->m_FATInfo);
            if (nClusterOffset == 0)
            {
                u32 nNextCluster = FATAllocateCluster (&g_pFileSystem->m_FAT);
                if (nNextCluster == 0)
                {
                    //m_FileTableLock.Release ();
                    return FS_ERROR;
                }

                if (pFile->nFirstCluster == 0)
                {
                    pFile->nFirstCluster = nNextCluster;
                }
                else
                {
                    FATSetClusterEntry (&g_pFileSystem->m_FAT, pFile->nCluster, nNextCluster);
                }

                pFile->nCluster = nNextCluster;
            }

            pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 1);
            assert (pFile->pBuffer != NULL);
        }

        u32 uClusterSize = FAT_SECTOR_SIZE * FATInfoGetSectorsPerCluster(&g_pFileSystem->m_FATInfo);
        ulCopyBytes = ulBytesLeft;

        if (ulBytesLeft > uClusterSize)
        {
            ulCopyBytes = uClusterSize;
        }

        if (ulCopyBytes > ulBytes)
        {
            ulCopyBytes = ulBytes;
        }

        ulBlockOffset = pFile->nOffset % uClusterSize;
        if (ulCopyBytes > uClusterSize - ulBlockOffset)
        {
            ulCopyBytes = uClusterSize - ulBlockOffset;
        }

        assert (pBuffer != 0);
        assert (ulCopyBytes > 0);

        TFATClusterBuffer *pTFATClusterBuffer = (TFATClusterBuffer *)pFile->pBuffer;
        memcpy (&(pTFATClusterBuffer->pBuf[ulBlockOffset]), pBuffer, ulCopyBytes);

        pBuffer = (void *) (((u8 *) pBuffer) + ulCopyBytes);

        FATCacheMarkClusterDirty (&g_pFileSystem->m_Cache, (TFATClusterBuffer *)pFile->pBuffer);

        pFile->nOffset += ulCopyBytes;
        assert (pFile->nOffset < FAT_MAX_FILESIZE);
        pFile->nSize += ulCopyBytes;
        assert (pFile->nSize == pFile->nOffset);

        ulBytes -= ulCopyBytes;
        ulBytesWritten += ulCopyBytes;

        if ((pFile->nOffset % uClusterSize) == 0)
        {
            assert (pFile->pBuffer != NULL);

            FATCacheFreeCluster (&g_pFileSystem->m_Cache, (TFATClusterBuffer *)(pFile->pBuffer), 0);
            pFile->pBuffer = NULL;
        }
    }

    //m_FileTableLock.Release ();

    return ulBytesWritten;
}

s32 FATFileSystemFileDelete (TDirectory *pUpperDir, const s8 *pTitle)
{
    assert (pTitle != 0);

    TFATDirectoryEntry *pEntry = FATDirectoryGetEntry (&g_pFileSystem->m_Root, pUpperDir->nCurDirFirstCluster, pTitle);
    if (pEntry == 0)
    {
        return 0;
    }

    if (pEntry->nAttributes & FAT_DIR_ATTR_READ_ONLY)
    {
        FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);
        return -1;
    }

    u32 nFirstCluster =   (u32) pEntry->nFirstClusterHigh << 16
                 | pEntry->nFirstClusterLow;
    if (nFirstCluster != 0)
    {
        FATFreeClusterChain (&g_pFileSystem->m_FAT, nFirstCluster);
    }

    pEntry->Name[0] = FAT_DIR_NAME0_FREE;

    FATDirectoryFreeEntry (&g_pFileSystem->m_Root, TRUE);

    return 1;
}

u32 FATFileSystemGetSize (FILE *pFile)
{
    assert (pFile != 0);

    return pFile->nSize;
}

u32 FATFileSystemFileSeek(FILE *pFile, s32 offset, u32 fromwhere)
{
    assert (pFile != 0);
 
    u32 nSeekOffset = 0;
    u32 nSeekCuster = 0;

    if (fromwhere == FAT_SEEK_SET)//文件头
    {
        if (offset < 0 || offset >= (s32)pFile->nSize)
        {
            return 0;
        }

        nSeekOffset = offset;

    }
    else if(fromwhere == FAT_SEEK_END)//文件尾
    {
        if (offset > 0 || (s32)pFile->nSize + offset < 0)
        {
            return 0;
        }

        nSeekOffset = pFile->nSize + offset;
    }
    else//当前位置
    {
        if ((s32)pFile->nOffset + offset >= (s32)pFile->nSize || (s32)pFile->nOffset + offset < 0)
        {
            return 0;
        }

        nSeekOffset = pFile->nOffset + offset;
    }

    nSeekCuster = FATGetFileCluster(&g_pFileSystem->m_FAT, pFile->nFirstCluster, nSeekOffset);
    if (nSeekCuster == 0)
    {
        return 0;
    }
   
    pFile->nOffset = nSeekOffset;

    if (pFile->nCluster != nSeekCuster)
    {
        pFile->nCluster = nSeekCuster;

        if (pFile->pBuffer != NULL)
        {
            FATCacheFreeCluster (&g_pFileSystem->m_Cache, (TFATClusterBuffer *)(pFile->pBuffer), 0);
            pFile->pBuffer = NULL;
        }

        if (pFile->bWrite)
        {       
            pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 1);
        }
        else
        {
            pFile->pBuffer = (void *)FATCacheGetCluster (&g_pFileSystem->m_Cache, pFile->nCluster, 0);        
        }

        assert (pFile->pBuffer != NULL);    
    }

    return 1;
}

