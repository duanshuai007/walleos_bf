#ifndef ENV_INCLUDE_USPIENV_FS_FILE_STRUCT_H
#define ENV_INCLUDE_USPIENV_FS_FILE_STRUCT_H

#include <types.h>

#define FS_DIR_NAME_LEN				8  // maximum of all file systems

#define FS_DIR_CREATE_SUCCESS		0
#define FS_DIR_CREATE_EXISTED		1
#define FS_DIR_CREATE_FAILED		2

#define FS_DIR_OPEN_SUCCESS			0
#define FS_DIR_NOT_EXIST			1

#define FS_FILE_ALREADY_OPENED		0xFFFFFFFF
#define FS_FILE_NOT_EXIST			0xFFFFFFFE
#define FS_FILE_OPEN_FAILED			0

#define FS_ROOT_UPPER_CLUSTER		0xFFFFFFFF

#define FS_PATH_SERIES_MAX_NUM		20

#define FAT_DIR_ATTR_READ_ONLY		0x01
#define FAT_DIR_ATTR_HIDDEN			0x02
#define FAT_DIR_ATTR_SYSTEM			0x04
#define FAT_DIR_ATTR_VOLUME_ID		0x08
#define FAT_DIR_ATTR_DIRECTORY		0x10
#define FAT_DIR_ATTR_ARCHIVE		0x20

#define FS_TITLE_LEN				12  // maximum of all file systems

typedef struct TDirentry                    // for directory listing
{
    s8        cFileName[FS_TITLE_LEN+1];  // 0-terminated
    u32    nSize;
    u32    nAttributes;
    s8        WriteTime[20];
    s8        CreatedTime[20];
}TDirentry;

typedef struct TFindCurrentEntry            // current position for directory listing
{
    u32    nEntry;
    u32    nCluster;
}TFindCurrentEntry;

#define FS_ERROR					0xFFFFFFFF

typedef struct TDirectory
{
    s8         cDirectoryName[FS_DIR_NAME_LEN+1];
    u32     nSeries;   //目录的级数，根目录为0级
    u32     nInitFlg;   //是否已经初始化的标志位
    u32     nCurDirFirstCluster;     //文件所在所在的第一簇
    u32     nUpperDirFirstCluster;     //文件上一层目录所在的第一簇
} TDirectory;

typedef struct TPath
{
    u32        nPathType;                          //全路径还是部分路径
#define FS_PATH_TYPE_ALL			0
#define FS_PATH_TYPE_PART			1
    u32        nSeriesNum;                         //共有多少层目录
    TDirectory      aDirectory[FS_PATH_SERIES_MAX_NUM]; //最多支持20层目录

} TPath;

#endif
