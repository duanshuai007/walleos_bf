#ifndef ENV_INCLUDE_USPIENV_FS_FAT_STRUCT_H
#define ENV_INCLUDE_USPIENV_FS_FAT_STRUCT_H

#include <fs/fs_struct.h>
#include <types.h>
#include <fs/fatfsdef.h>
#include <fs/file_struct.h>
#include <stdio.h>

#define FAT_SECTOR_SIZE			512

#define FAT_BUFFERS				100
#define FAT_CLUSER_BUFFERS		100
#define FAT_FILES				40

#define FAT_MAX_FILESIZE		0xFFFFFFFF

#define FAT_DIR_ENTRY_SIZE		32 //每个目录项的长度，即sizeof（TFATDirectoryEntry）
#define FAT_DIR_ENTRIES_PER_SECTOR	(FAT_SECTOR_SIZE / FAT_DIR_ENTRY_SIZE)//每个扇区目录的个数

/*fatcache*/
typedef struct TFATBuffer
{
	u32	 nMagic;
	struct TFATBuffer	*pNext;
	struct TFATBuffer	*pPrev;
	u32	 nSector;
	u32	 nUseCount;
	s32		 bDirty;
	//TFATBootSector
	//磁盘的引导扇区，通常指设备的第一个扇区，用于加载并转让处理器控制权给操作系统
	u8	 Data[FAT_SECTOR_SIZE];//FAT_SECTOR_SIZE==512 
}TFATBuffer;

typedef struct TFATBufferList
{
	TFATBuffer *pFirst;
	TFATBuffer *pLast;
}TFATBufferList;

/*fatcache*/
typedef struct TFATClusterBuffer
{
	u32	 nMagic;
	struct TFATClusterBuffer	*pNext;
	struct TFATClusterBuffer	*pPrev;
	u32	 nCluster;	
	u32	 nSectorNumPerCluster;
	u32     m_nFirstDataSector;
	u32	 nUseCount;
	s32		 bDirty;
	//TFATBootSector
	//磁盘的引导扇区，通常指设备的第一个扇区，用于加载并转让处理器控制权给操作系统
	u8	 *pBuf;//FAT_SECTOR_SIZE==512 
}TFATClusterBuffer;

typedef struct TFATCluserBufferList
{
	TFATClusterBuffer *pFirst;
	TFATClusterBuffer *pLast;
}TFATCluserBufferList;

typedef struct TFatCache{
	TPartition *m_pPartition;
	void		*m_pBufferMem;
	TFATBufferList	 m_BufferList;

	void		*m_pClusterBufferMem;
	TFATCluserBufferList m_ClusterBufList;

//	CSpinLock m_BufferListLock;
//	CSpinLock m_DiskLock;	
}TFatCache;

typedef enum TFATType
{
	//FAT12,	// not supported
	FAT16,
	FAT32,
	FATUnknown
}TFATType;

typedef struct TFatInfo{
	TFatCache *m_pCache;

	TFATType m_FATType;

	// all FAT types
	u32 m_nSectorsPerCluster;//每簇扇区数
	u32 m_nReservedSectors;//保留扇区数
	u32 m_nNumberOfFATs;//FAT表个数,一般为2
	u32 m_nRootEntries;//FAT32必须等于0，FAT12/FAT16为根目录中目录的个数
	u32 m_nFATSize;//每个FAT表占用扇区数
	u32 m_nTotalSectors;//总的扇区数
	boolean  m_bFATMirroring;//0值意味着在运行时FAT被映射到所有的FAT, 1值表示只有一个FAT是活动的

	// FAT32 only
	u32 m_nActiveFAT;//活动的fat表个数
	u32 m_nRootCluster;///根目录所在第一个簇的簇号，2
	u32 m_nFSInfoSector;//FSINFO（文件系统信息扇区）扇区号1，该扇区为操作 系统提供关于空簇总数及下一可用簇的信息

	// calculated
	u32 m_nRootSectors;//根目录扇区总数
	u32 m_nFirstDataSector;//数据区的第一个数据所在扇区
	u32 m_nDataSectors;//数据可以使用的扇区总数
	u32 m_nClusters;//可以使用簇的总数

	// from FS Info sector (saved on FAT32 only)
	u32 m_nFreeCount;//文件系统的空簇数
	u32 m_nNextFreeCluster;//下一个可用的簇号

//	CSpinLock m_Lock;
}TFatInfo;

typedef struct TFat{	
	TFatCache *m_pCache;
	TFatInfo  *m_pFATInfo;

//	CSpinLock m_Lock;
}TFat;

typedef struct TFATDirectory{
	TFatCache *m_pCache;
	TFatInfo  *m_pFATInfo;
	TFat	  *m_pFAT;

	TFATBuffer *m_pBuffer;

//	CSpinLock m_Lock;
}TFATDirectory;

typedef struct TFATFileSystem{
	TPartitionManager m_PartitionManager;	
	TFatCache	m_Cache;
	TFatInfo	m_FATInfo;
	TFat		m_FAT;
	TFATDirectory	m_Root;
	FILE		m_Files[FAT_FILES];

//	CSpinLock m_FileTableLock;
}TFATFileSystem;


#endif
