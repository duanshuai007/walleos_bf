//
// fatdir.cpp
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
#include <fs/fatdir.h>
#include <timer.h>
#include <string.h>
#include <assert.h>
#include <types.h>
#include <headsmp.h>
#include <stddef.h>

static boolean FATDirectoryName2FAT (const s8 *pName, s8 *pFATName);
static void FATDirectoryFAT2Time (u16 nCreatedDate, u16 nCreatedTime, s8 *pTime);

void CFATDirectory (TFATDirectory *pThis, TFatCache *pCache, TFatInfo *pFATInfo, TFat *pFAT)
{
    pThis->m_pCache = pCache;
    pThis->m_pFATInfo = pFATInfo;
    pThis->m_pFAT = pFAT;
    pThis->m_pBuffer = 0;
}

void _CFATDirectory (TFATDirectory *pThis)
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

    if(pThis->m_pFAT != 0){
        _CFAT(pThis->m_pFAT);
        pThis->m_pFAT = 0;
    }
}

//获取根目录的目录项
TFATDirectoryEntry *FATDirectoryGetRootEntry (TFATDirectory *pThis)
{
    u32 nCluster = FATInfoGetRootCluster(pThis->m_pFATInfo);
    u32 nSector = FATInfoGetFirstSector(pThis->m_pFATInfo, nCluster);

    assert (pThis->m_pBuffer == 0);
    assert (pThis->m_pCache != 0);
    pThis->m_pBuffer = FATCacheGetSector (pThis->m_pCache, nSector, 0);
    assert (pThis->m_pBuffer != 0);

    TFATDirectoryEntry *pFATEntry = (TFATDirectoryEntry *) &pThis->m_pBuffer->Data[0];

    return pFATEntry;
}

//根据名称获取对应的目录项 nUpperFirstCluster：上层目录的第一簇
TFATDirectoryEntry *FATDirectoryGetEntry (TFATDirectory *pThis, s32 nUpperFirstCluster, const s8 *pName)//获取目录项
{

    assert (pName != 0);
    assert (nUpperFirstCluster>=2);

    s8 FATName[FAT_DIR_NAME_LENGTH];
    if (!FATDirectoryName2FAT (pName, FATName))//
    {
        return 0;
    }

    assert (pThis->m_pFATInfo != 0);
    TFATType FATType = FATInfoGetFATType (pThis->m_pFATInfo);

    u32 nEntry = 0;

    u32 nEntriesPerCluster = 0;
    u32 nCurDirCluster = 0;
    if (FATType == FAT32)
    {
        nCurDirCluster = nUpperFirstCluster;

        nEntriesPerCluster =FATInfoGetSectorsPerCluster (pThis->m_pFATInfo) * FAT_DIR_ENTRIES_PER_SECTOR;//计算每簇目录的个数
    }

    //m_Lock.Acquire ();

    while (1)
    {
        if (FATType == FAT16)
        {
            if (nEntry >= FATInfoGetRootEntries (pThis->m_pFATInfo))
            {
                break;
            }
        }
        else
        {
            assert (FATType == FAT32);
            if (FATIsEOC (pThis->m_pFAT, nCurDirCluster))
            {
                break;
            }
        }

        u32 nSector;
        if (FATType == FAT16)
        {
            nSector =   FATInfoGetFirstRootSector (pThis->m_pFATInfo) + nEntry / FAT_DIR_ENTRIES_PER_SECTOR;
        }
        else
        {
            assert (FATType == FAT32);
            //FATInfoGetFirstSector  获取当前簇所在的第一个扇区
            //nEntriesPerCluster:计算每簇目录的个数
            //nEntry：当前目录
            //FAT_DIR_ENTRIES_PER_SECTOR:每个扇区目录的个数
            //计算当前目录所处的扇区
            //(nEntry % nEntriesPerCluster)/FAT_DIR_ENTRIES_PER_SECTOR计算当前目录在簇中所处的扇区
            nSector = FATInfoGetFirstSector (pThis->m_pFATInfo, nCurDirCluster)
                      + (nEntry % nEntriesPerCluster)
                      / FAT_DIR_ENTRIES_PER_SECTOR;
        }

        assert (pThis->m_pBuffer == 0);
        assert (pThis->m_pCache != 0);
        pThis->m_pBuffer = FATCacheGetSector (pThis->m_pCache, nSector, 0);
                  
        assert (pThis->m_pBuffer != 0);

        u32 nOffset = (nEntry * FAT_DIR_ENTRY_SIZE) % FAT_SECTOR_SIZE;
        TFATDirectoryEntry *pFATEntry = (TFATDirectoryEntry *) &pThis->m_pBuffer->Data[nOffset];
        assert (pFATEntry != 0);

        if (pFATEntry->Name[0] == FAT_DIR_NAME0_LAST)
        {
            FATCacheFreeSector (pThis->m_pCache, pThis->m_pBuffer, 1);
            pThis->m_pBuffer = 0;

            break;
        }

        if (   pFATEntry->Name[0] != FAT_DIR_NAME0_FREE//E5
            && (pFATEntry->nAttributes & (FAT_DIR_ATTR_ARCHIVE | FAT_DIR_ATTR_DIRECTORY))
            && memcmp (pFATEntry->Name, FATName, FAT_DIR_NAME_LENGTH) == 0)
        {
            return pFATEntry;//找到文件
        }

        FATCacheFreeSector (pThis->m_pCache, pThis->m_pBuffer, 1);
        pThis->m_pBuffer = 0;

        nEntry++;

        //如果目录表超出一簇，则获取下一簇数据
        if (   FATType == FAT32
            && nEntry % nEntriesPerCluster == 0)
        {
            assert (pThis->m_pFAT != 0);
            nCurDirCluster = FATGetClusterEntry (pThis->m_pFAT, nCurDirCluster);
        }
    }

    //m_Lock.Release ();

    return 0;
}

//创建目录项
TFATDirectoryEntry *FATDirectoryCreateEntry (TFATDirectory *pThis, s32 nUpperFirstCluster, s32 nFirstCluster, s32 createType, const s8 *pName)
{

    assert (pName != 0);
    assert (nUpperFirstCluster >= 2);

    s8 FATName[FAT_DIR_NAME_LENGTH];
    if (!FATDirectoryName2FAT (pName, FATName))
    {       
        return 0;
    }

    assert (pThis->m_pFATInfo != 0);
    TFATType FATType = FATInfoGetFATType (pThis->m_pFATInfo);

    u32 nEntry = 0;

    u32 nEntriesPerCluster = 0;
    u32 nCurDirCluster = 0;
    if (FATType == FAT32)
    {
        nCurDirCluster = nUpperFirstCluster;

        nEntriesPerCluster =   FATInfoGetSectorsPerCluster (pThis->m_pFATInfo)
                     * FAT_DIR_ENTRIES_PER_SECTOR;//计算每簇目录的个数
    }

    //m_Lock.Acquire ();

    u32 nPrevCluster = 0;

    while (1)
    {
        if (FATType == FAT16)
        {
            if (nEntry >= FATInfoGetRootEntries (pThis->m_pFATInfo))
            {
                break;
            }
        }
        else
        {
            assert (FATType == FAT32);

            assert (pThis->m_pFAT != 0);

            //当前目录项已经占满一簇
            if (FATIsEOC (pThis->m_pFAT, nCurDirCluster))
            {
                nCurDirCluster = FATAllocateCluster (pThis->m_pFAT);//申请使用一个新簇
                if (nCurDirCluster == 0)
                {
                    break;
                }

                u32 nSector = FATInfoGetFirstSector (pThis->m_pFATInfo, nCurDirCluster);
                u32 i = 0;
                for (; i < FATInfoGetSectorsPerCluster (pThis->m_pFATInfo); i++)
                {
                    assert (pThis->m_pCache != 0);
                    TFATBuffer *pBuffer = FATCacheGetSector (pThis->m_pCache, nSector++, 1);
                    assert (pBuffer != 0);

                    memset (pBuffer->Data, 0, FAT_SECTOR_SIZE);

                    FATCacheMarkDirty (pThis->m_pCache, pBuffer);
                    FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
                }

                assert (nPrevCluster >= 2);
                FATSetClusterEntry (pThis->m_pFAT, nPrevCluster, nCurDirCluster);
                nPrevCluster = 0;
            }
        }

        u32 nSector;
        if (FATType == FAT16)
        {
            nSector =   FATInfoGetFirstRootSector (pThis->m_pFATInfo)
                  + nEntry / FAT_DIR_ENTRIES_PER_SECTOR;
        }
        else
        {
            assert (FATType == FAT32);
            nSector = FATInfoGetFirstSector (pThis->m_pFATInfo, nCurDirCluster)
                      + (nEntry % nEntriesPerCluster)
                      / FAT_DIR_ENTRIES_PER_SECTOR;
        }

        assert (pThis->m_pBuffer == 0);
        assert (pThis->m_pCache != 0);
        pThis->m_pBuffer = FATCacheGetSector (pThis->m_pCache, nSector, 0);
        assert (pThis->m_pBuffer != 0);

        u32 nOffset = (nEntry * FAT_DIR_ENTRY_SIZE) % FAT_SECTOR_SIZE;
        TFATDirectoryEntry *pFATEntry = (TFATDirectoryEntry *) &pThis->m_pBuffer->Data[nOffset];
        assert (pFATEntry != 0);

        if (   pFATEntry->Name[0] == FAT_DIR_NAME0_LAST
            || pFATEntry->Name[0] == FAT_DIR_NAME0_FREE)
        {
            memset (pFATEntry, 0, FAT_DIR_ENTRY_SIZE);
            memcpy (pFATEntry->Name, FATName, FAT_DIR_NAME_LENGTH);

            u32 nDateTime = FATDirectoryTime2FAT (TimerGetTime(TimerGet(DO_TICK_CPU)));
            pFATEntry->nCreatedDate = nDateTime >> 16;
            pFATEntry->nCreatedTime = nDateTime & 0xFFFF;

            pFATEntry->nWriteTime = nDateTime >> 16;
            pFATEntry->nWriteDate = nDateTime & 0xFFFF;

            pFATEntry->nLastAccessDate = nDateTime & 0xFFFF;

            if (createType == 0)
            {
                pFATEntry->nAttributes = FAT_DIR_ATTR_DIRECTORY;

                u32 nDirFirstSector = FATInfoGetFirstSector (pThis->m_pFATInfo, nFirstCluster);
                u32 i = 0;
                for (; i < FATInfoGetSectorsPerCluster (pThis->m_pFATInfo); i++)
                {
                    assert (pThis->m_pCache != 0);
                    TFATBuffer *pBuffer = FATCacheGetSector (pThis->m_pCache, nDirFirstSector++, 1);
                    assert (pBuffer != 0);

                    memset (pBuffer->Data, 0, FAT_SECTOR_SIZE);

                    FATCacheMarkDirty (pThis->m_pCache, pBuffer);
                    FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);
                }

                pFATEntry->nFirstClusterHigh = nFirstCluster >> 16;
                pFATEntry->nFirstClusterLow  = nFirstCluster & 0xFFFF;                

            }
            else if (createType == 1)
            {
                pFATEntry->nAttributes = FAT_DIR_ATTR_DIRECTORY;

                pFATEntry->nFirstClusterHigh = nFirstCluster >> 16;
                pFATEntry->nFirstClusterLow  = nFirstCluster & 0xFFFF; 
            }
            else
            {
                pFATEntry->nAttributes = FAT_DIR_ATTR_ARCHIVE;
            }

            return pFATEntry;
        }


        FATCacheFreeSector (pThis->m_pCache, pThis->m_pBuffer, 1);
        pThis->m_pBuffer = 0;

        nEntry++;

        if ( FATType == FAT32
            && nEntry % nEntriesPerCluster == 0)
        {
            nPrevCluster = nCurDirCluster;

            assert (pThis->m_pFAT != 0);
            nCurDirCluster = FATGetClusterEntry (pThis->m_pFAT, nCurDirCluster);
        }
    }


    return 0;
}

//释放buffer
void FATDirectoryFreeEntry (TFATDirectory *pThis, boolean bChanged)
{
    assert (pThis->m_pBuffer != 0);
    assert (pThis->m_pCache != 0);

    if (bChanged)
    {
        FATCacheMarkDirty (pThis->m_pCache, pThis->m_pBuffer);
    }

    FATCacheFreeSector (pThis->m_pCache, pThis->m_pBuffer, 1);
    pThis->m_pBuffer = 0;

    //m_Lock.Release ();
}

//获取第一个目录项
boolean FATDirectoryFindFirst (TFATDirectory *pThis, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    assert (pCurrentEntry != 0);
    assert (pEntry != 0);
    pCurrentEntry->nEntry = 0;

    return FATDirectoryFindNext (pThis, pCurrentEntry, pEntry);
}

//遍历目录项
boolean FATDirectoryFindNext (TFATDirectory *pThis, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    assert (pEntry != 0);
    assert (pCurrentEntry != 0);
    assert (pCurrentEntry->nCluster>=2);

    if (pCurrentEntry->nEntry == 0xFFFFFFFF)
    {
        return FALSE;
    }

    assert (pThis->m_pFATInfo != 0);
    TFATType FATType = FATInfoGetFATType (pThis->m_pFATInfo);//获取文件系统的格式类型

    u32 nEntriesPerCluster = 0;
    if (FATType == FAT32)
    {
        #if 0
        if (pCurrentEntry->nEntry == 0)
        {
            pCurrentEntry->nCluster = FATInfoGetRootCluster (pThis->m_pFATInfo);
        }
        #endif
        nEntriesPerCluster =   FATInfoGetSectorsPerCluster (pThis->m_pFATInfo)
                     * FAT_DIR_ENTRIES_PER_SECTOR;
    }

    //m_Lock.Acquire ();

    while (1)
    {
        if (FATType == FAT16)
        {
            if (pCurrentEntry->nEntry >= FATInfoGetRootEntries (pThis->m_pFATInfo))
            {
                break;
            }
        }
        else
        {
            assert (FATType == FAT32);
            if (FATIsEOC (pThis->m_pFAT, pCurrentEntry->nCluster))
            {
                break;
            }
        }

        u32 nSector;
        if (FATType == FAT16)
        {
            nSector =   FATInfoGetFirstRootSector (pThis->m_pFATInfo)
                  + pCurrentEntry->nEntry / FAT_DIR_ENTRIES_PER_SECTOR;
        }
        else
        {
            assert (FATType == FAT32);
            nSector =   FATInfoGetFirstSector (pThis->m_pFATInfo, pCurrentEntry->nCluster)
                  +   (pCurrentEntry->nEntry % nEntriesPerCluster)
                    / FAT_DIR_ENTRIES_PER_SECTOR;
        }

        assert (pThis->m_pCache != 0);
        TFATBuffer *pBuffer = FATCacheGetSector (pThis->m_pCache, nSector, 0);
        assert (pBuffer != 0);

        u32 nOffset = (pCurrentEntry->nEntry * FAT_DIR_ENTRY_SIZE) % FAT_SECTOR_SIZE;
        TFATDirectoryEntry *pFATEntry = (TFATDirectoryEntry *) &pBuffer->Data[nOffset];
        assert (pFATEntry != 0);

        if (pFATEntry->Name[0] == FAT_DIR_NAME0_LAST)
        {
            FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);

            break;
        }

        boolean bFound = FALSE;

        if (pFATEntry->Name[0] != FAT_DIR_NAME0_FREE)
        {
            FATDirectoryFAT2Name ((const s8 *) pFATEntry->Name, pEntry->cFileName);

            pEntry->nSize = pFATEntry->nFileSize;

            pEntry->nAttributes = pFATEntry->nAttributes;

            FATDirectoryFAT2Time(pFATEntry->nCreatedDate, pFATEntry->nCreatedTime, pEntry->WriteTime);
            FATDirectoryFAT2Time(pFATEntry->nWriteDate, pFATEntry->nWriteTime, pEntry->CreatedTime);


            bFound = TRUE;
        }

        FATCacheFreeSector (pThis->m_pCache, pBuffer, 1);

        pCurrentEntry->nEntry++;

        if (   FATType == FAT32
            && pCurrentEntry->nEntry % nEntriesPerCluster == 0)
        {
            assert (pThis->m_pFAT != 0);
            pCurrentEntry->nCluster = FATGetClusterEntry (pThis->m_pFAT, pCurrentEntry->nCluster);
        }

        if (bFound)
        {
            //m_Lock.Release ();

            return TRUE;
        }
    }

    pCurrentEntry->nEntry = 0xFFFFFFFF;

    //m_Lock.Release ();

    return FALSE;
}

/*
 *	nTime等于系统开机到现在的秒数
 */
u32 FATDirectoryTime2FAT (u32 nTime)
{
    if (nTime == 0)
    {
        return 0;
    }

    u32 nSecond = nTime % 60;
    nTime /= 60;
    u32 nMinute = nTime % 60;
    nTime /= 60;
    u32 nHour = nTime % 24;
    nTime /= 24;

    u32 nYear = 1970;
    while (1)
    {
        u32 nDaysOfYear = IsLeapYear (nYear) ? 366 : 365;
        if (nTime < nDaysOfYear)
        {
            break;
        }

        nTime -= nDaysOfYear;
        nYear++;
    }

    if (nYear < 1980)
    {
        return 0;
    }

    u32 nMonth = 0;
    while (1)
    {
        u32 nDaysOfMonth = GetDaysOfMonth (nMonth, nYear);
        if (nTime < nDaysOfMonth)
        {
            break;
        }

        nTime -= nDaysOfMonth;
        nMonth++;
    }

    u32 nMonthDay = nTime + 1;

    u32 nFATDate = (nYear-1980) << 9;
    nFATDate |= (nMonth+1) << 5;
    nFATDate |= nMonthDay;

    u32 nFATTime = nHour << 11;
    nFATTime |= nMinute << 5;
    nFATTime |= nSecond / 2;

    return nFATDate << 16 | nFATTime;
}

// TODO: standard FAT name conversion
//将文件名称转成u8 Name[FAT_DIR_NAME_LENGTH];  // name and extension padded by ' '
//貌似名称都会转成大写字母？
boolean FATDirectoryName2FAT (const s8 *pName, s8 *pFATName)
{
    assert (pName != 0);
    assert (pFATName != 0);

    if (strcmp(pName, ".") == 0 || strcmp(pName, "..") == 0)
    {
        memset (pFATName, ' ', FAT_DIR_NAME_LENGTH);
        memcpy(pFATName, pName, strlen(pName));
        return TRUE;
    }

    memset (pFATName, ' ', FAT_DIR_NAME_LENGTH);

    const s8 *pFrom;
    s8 *pTo;
    u32 nLength;
    for (pFrom = pName, pTo = pFATName, nLength = 8; *pFrom != '\0'; pFrom++)
    {
        s8 c = *pFrom;

        if (c <= ' ')
        {
            return FALSE;
        }

        static const s8 *s_pBadChars = "\"*+,/:;<=>?[\\]|";
        const s8 *pBad = s_pBadChars;
        for (; *pBad; pBad++)
        {
            if (c == *pBad)
            {
                return FALSE;
            }
        }

        if ('a' <= c && c <= 'z')
        {
            c -= 'a'-'A';
        }

        if (c == '.')
        {
            pTo = pFATName+8;
            nLength = 3;
            continue;
        }

        if (nLength > 0)
        {
            *pTo++ = c;
            nLength--;
        }
    }

    if (pFATName[0] == ' ')
    {
        return FALSE;
    }

    return TRUE;
}

void FATDirectoryFAT2Name (const s8 *pFATName, s8 *pName)
{
    assert (pFATName != 0);
    assert (pName != 0);

    s8 FATName[FAT_DIR_NAME_LENGTH+1];
    strncpy (FATName, pFATName, FAT_DIR_NAME_LENGTH);
    FATName[FAT_DIR_NAME_LENGTH] = '\0';

    if (FATName[0] == FAT_DIR_NAME0_KANJI_E5)
    {
        FATName[0] = '_';
    }

    s8 *p = FATName;
    for (; *p != '\0'; p++)
    {
        if (*p == ' ')
        {
            *p = '\0';
        }
        else if ('A' <= *p && *p <= 'Z')
        {
            *p += 'a'-'A';
        }
    }

    strncpy (pName, FATName, 8);
    pName[8] = '\0';

    if (FATName[8] != '\0')
    {
        strcat (pName, ".");
        strcat (pName, FATName+8);
    }
}

//格式化文件的时间
//  u8  nCreatedTimeTenth;  // 0..199 = 0..1.99
//  u16 nCreatedTime;       // HHHHHMMMMMMSSSSS (0..23, 0..59, 0..29 = 0..58)
//  u16 nCreatedDate;       // YYYYYYYMMMMDDDDD (0..127 = 1980..2107, 1..12, 1..31)
//  u16 nLastAccessDate;    // YYYYYYYMMMMDDDDD (0..127 = 1980..2107, 1..12, 1..31)
void FATDirectoryFAT2Time (u16 nCreatedDate, u16 nCreatedTime, s8 *pTime){
    assert (pTime != 0);

    s32 year = 1980 + ((nCreatedDate>>9) & 0x7F);
    s32 month = (nCreatedDate>>5) & 0xF;;
    s32 day = (nCreatedDate) & 0x1F;

    s32 hour = (nCreatedTime>>11) & 0x1F;
    s32 minute = (nCreatedTime>>5) & 0x3F;
    s32 second = (nCreatedTime) & 0x1F;

    s8 Source[32] = { 0 };
    sprintf (Source, "%4d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);

    strcpy(pTime, Source);
}
