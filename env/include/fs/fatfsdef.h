//
// uspienv/fs/fatfsdef.h
//
#ifndef ENV_INCLUDE_USPIENV_FS_FATFSDEF_H
#define ENV_INCLUDE_USPIENV_FS_FATFSDEF_H

#include <macros.h>
#include <types.h>


typedef struct TFATBPBStruct
{
	u8	Jump[3];		// 0x00~0x02：3字节，跳转指令。
	u8	OEMName[8];		// 0x03~0x0A：8字节，文件系统标志和版本号，这里为MSDOC5.0。
	u16	nBytesPerSector;	// 0x0B~0x0C：2字节，每扇区字节数，512（0X02 00）
	u8	nSectorsPerCluster;	//0x0D~0x0D：1字节，每簇扇区数，8（0x08）
	u16	nReservedSectors;	// 0x0E~0x0F：2字节，保留扇区数，38（0x00 26），符合FAT1起始地址为38扇区
	u8	nNumberOfFATs;		// 0x10~0x10：1字节，FAT表个数，2。 
	u16	nRootEntries;		// 2字节，FAT32必须等于0，FAT12/FAT16为根目录中目录的个数；
	u16	nTotalSectors16;	// 0x13~0x14：2字节，FAT32必须等于0，FAT12/FAT16为扇区总数
	u8	nMedia;			// 0x15~0x15：1字节，哪种存储介质，0xF8标准值，可移动存储介质，常用的 0xF0
	u16	nFATSize16;		// 0x16~0x17:2字节，FAT32必须为0，FAT12/FAT16为一个FAT 表所占的扇区数。
	u16	nSectorsPerTrack;	// 0x18~0x19：2字节，每磁道扇区数，只对于有“特殊形状”（由磁头和柱面每 分割为若干磁道）的存储介质有效，63（0x00 3F）。 
	u16	nNumberOfHeads;		// 0x1A~0x1B：2字节，磁头数，只对特殊的介质才有效，255（0x00 FF）。
	u32	nHiddenSectors;		// 0x1C~0x1F：4字节，EBR分区之前所隐藏的扇区数，8192（0x00 00 20 00），与MBR中地址0x1C6开始的4个字节数值相等
	u32	nTotalSectors32;	// 0x20~0x23：4字节，文件系统总扇区数，7736320（0x 00 76 0C 00），7736320 *  512 = 3960995840  ≈ 3.67GB
}
PACKED TFATBPBStruct;

typedef struct TFAT1xStruct
{
	u8	nDriveNumber;		// ignored
	u8	nReserved;		// ignored
	u8	nBootSignature;		// must be 0x29
	u32	nVolumeSerial;		// ignored
	u8	VolumeLabel[11];	// ignored
	u8	FileSystemType[8];	// ignored
}
PACKED TFAT1xStruct;//28

typedef struct TFAT32Struct
{
	u32	nFATSize32;//0x24~0x27：4字节，每个FAT表占用扇区数，7541（0x 00 00 1D 75）。
	u16	nActiveFAT	: 4,	// 0x28~0x29：2字节，活动 FAT数(从0开始计数，而不是1).只有在不使用镜像时才有效
		nReserved1	: 3,	// ignored
		nMirroringOff	: 1,	// 0值意味着在运行时FAT被映射到所有的FAT, 1值表示只有一个FAT是活动的
		nReserved2	: 8;	// ignored
	u16	nFSVersion;		//0x2A~0x2B：2字节，FAT32版本号0.0，FAT32特有
	u32	nRootCluster;		// 0x2C~0x2F：4字节，根目录所在第一个簇的簇号，2。（虽然在FAT32文件系统 下，根目录可以存放在数据区的任何位置，但是通常情况下还是起始于2号簇）
	u16	nFSInfoSector;		// 0x30~0x31：2字节，FSINFO（文件系统信息扇区）扇区号1，该扇区为操作 系统提供关于空簇总数及下一可用簇的信息。
	u16	nBackupBootSector;	//0x32~0x33：2字节，备份引导扇区的位置。备份引导扇区总是位于文件系统 的6号扇区。
	u8	nReserved3[12];		//0x34~0x3F：12字节，用于以后FAT 扩展使用。 
	u8	nDriveNumber;		// 0x40~0x40：1字节，与FAT12/16 的定义相同，只不过两者位于启动扇区不同的位置而已。
	u8	nReserved4;		// ignored
	u8	nBootSignature;		// 1字节，扩展引导标志，0x29。与FAT12/16 的定义相同，只不过 两者位于启动扇区不同的位置而已
	u32	nVolumeSerial;		// 0x43~0x46：4字节，卷序列号。通常为一个随机值。
	u8	VolumeLabel[11];	//0x47~0x51：11字节，卷标（ASCII码），如果建立文件系统的时候指定了卷 标，会保存在此。
	u8	FileSystemType[8];	// 0x52~0x59：8字节，文件系统格式的ASCII码，FAT32。
}
PACKED TFAT32Struct;//54

typedef struct TFATBootSector//磁盘的引导扇区
{
	TFATBPBStruct	BPB; //36

	union
	{
		TFAT1xStruct	FAT1x;
		TFAT32Struct	FAT32;
	}
	Struct;//54

	u8	BootCode[420]; //0x5A~0x1FD：90~509共420字节，未使用。该部分没有明确的用途。
	u16	nBootSignature;//2
	#define BOOT_SIGNATURE		0xAA55
}
PACKED TFATBootSector;

typedef struct TFAT32FSInfoSector
{
	u32	nLeadingSignature;//扩展引导标志“0x52526141”
	#define LEADING_SIGNATURE	0x41615252
	u8	Reserved1[480];		// ignored
	u32	nStructSignature;//FSINFO签名“0x72724161”
	#define STRUCT_SIGNATURE	0x61417272
	u32	nFreeCount;	//文件系统的空簇数，964466（0x00 0E B7 72	// <= volume cluster count or unknown
	#define FREE_COUNT_UNKNOWN	0xFFFFFFFF
	u32	nNextFreeCluster;//下一可用簇号	// hint to look for free cluster or unknown
	#define NEXT_FREE_CLUSTER_UNKNOWN 0xFFFFFFFF
	u8	Reserved2[12];		// ignored
	u32	nTrailingSignature;
	#define TRAILING_SIGNATURE	0xAA550000
}
PACKED TFAT32FSInfoSector;

typedef struct TFATDirectoryEntry
{
	#define FAT_DIR_NAME_LENGTH	11
	u8	Name[FAT_DIR_NAME_LENGTH];	// name and extension padded by ' '
	#define FAT_DIR_NAME0_LAST	0x00
	#define FAT_DIR_NAME0_FREE	0xE5
	#define FAT_DIR_NAME0_KANJI_E5	0x05
	u8	nAttributes;
	#define FAT_DIR_ATTR_READ_ONLY	0x01
	#define FAT_DIR_ATTR_HIDDEN	0x02
	#define FAT_DIR_ATTR_SYSTEM	0x04
	#define FAT_DIR_ATTR_VOLUME_ID	0x08
	#define FAT_DIR_ATTR_DIRECTORY	0x10
	#define FAT_DIR_ATTR_ARCHIVE	0x20
	#define FAT_DIR_ATTR_LONG_NAME	0x0F
	#define FAT_DIR_ATTR_LONG_NAME_MASK 0x3F
	u8	nNTReserved;		// set to 0 on file create
	u8	nCreatedTimeTenth;	// 0..199 = 0..1.99
	u16	nCreatedTime;		// HHHHHMMMMMMSSSSS (0..23, 0..59, 0..29 = 0..58)
	u16	nCreatedDate;		// YYYYYYYMMMMDDDDD (0..127 = 1980..2107, 1..12, 1..31)
	u16	nLastAccessDate;	// YYYYYYYMMMMDDDDD (0..127 = 1980..2107, 1..12, 1..31)
	u16	nFirstClusterHigh; //文件起始簇的高16位
	u16	nWriteTime;		// HHHHHMMMMMMSSSSS (0..23, 0..59, 0..29 = 0..58)
	u16	nWriteDate;		// YYYYYYYMMMMDDDDD (0..127 = 1980..2107, 1..12, 1..31)
	u16	nFirstClusterLow; //文件起始簇的低16位
	u32	nFileSize;//文件大小
}
PACKED TFATDirectoryEntry;

#endif
