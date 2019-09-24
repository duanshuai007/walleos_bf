#ifndef ENV_INCLUDE_USPIENV_FS_FILE_H
#define ENV_INCLUDE_USPIENV_FS_FILE_H

#include <fs/file_struct.h>
#include <stdio.h>

#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

#ifndef FILE_SUCCESS
#define FILE_SUCCESS	1
#endif

#ifndef FILE_FAILED
#define FILE_FAILED		0
#endif

//无效的文件handler
#define INVALID_FILE	0

//读写文件错误
#define FS_OPT_ERROR	0xFFFFFFFF

//创建一个新文件，只可以写操作，会覆盖已存在的文件
#define MODE_CREATE		0x01
//打开一个已有文件，只可以写操作
#define MODE_WRITE		0x02
//打开一个已有文件，只可以读操作
#define MODE_READ		0x04


/*
功能：mount文件系统
参数：
    无
返回值：
    TRUE:mount成功，FALSE：mount失败
*/
u32 fmount(void);

/*
功能：unmount文件系统
参数：
    无
返回值：
    无
*/
void funmount(void);

/*
功能：在当前路径下创建一个目录，主要用于命令行
参数：
    （I）pPath:当前的路径
    （I）pDirName：创建的目录名称
返回值：
    创建是否成功
*/
u32 fcreateDir(TPath *pPath, s8 *pDirName);

/*
功能：在当前路径下创建目录，同时可以创建多层目录结构
参数：
    （I）pPath:当前的路径
    （I）pDirName：待创建的目录
返回值：
    创建是否成功
*/
u32 fcreateDirs(TPath *pCurPath, s8 *pCreatePath);

/*
功能：打开根目录，主要用于命令行
参数：
    （O）pPath:返回根目录路径
返回值：
    打开是否成功
*/
u32 fopenRootDir(TPath *pPath);

/*
功能：打开一个目录
参数：
    （I） pPathStr:完整路径字符串
    （I） nCreateFlg:为TRUE时，如果子目录不存在，则创建
    （O） pPath:返回打开的路径
返回值：
    打开是否成功
*/
u32 fopenDir(s8 *pPathStr, u32 nCreateFlg, TPath *pPath);


/*
功能：打开一个文件
参数：
    （I） pPath:文件所在路径
    （I） pFileName:文件名称，不含路径
    （I） mode:打开文件的方式 MODE_CREATE，MODE_WRITE，MODE_READ
返回值：
    文件handler，返回值为0时，为打开失败
*/
FILE *fopenCurFile(TPath *pPath, s8 *pFileName, const s8 *mode);


/*
功能：删除文件
参数：
    （I） pPath:文件所在路径
    （I） file:文件名称，不含路径
返回值：
    删除是否成功
*/
u32 fdelete(TPath *pPath, s8 *file);

/*
功能：切换路径，命令行专用
参数：
    （IO） pCurPath:当前路径，同时做为返回值，返回切换后的路径
    （I） pNewPathStr:待切换的路径STR
返回值：
    切换是否成功
*/
u32 fswitchDir(TPath *pCurPath, s8 *pNewPathStr);

/*
功能：判断文件是否时目录
参数：
    （I） pUpperDir:文件所在的上一层目录
    （I） file:待判断的文件名称，不含路径
返回值：
    TRUE:是目录
*/
u32 isDirectory(TDirectory *pUpperDir, s8 *file);

/*
功能：检索当前目录下的第一个目录项
参数：
    （I） pPath:文件所在路径
    （IO） pCurrentEntry:用来保存检索过程中的目录项信息
    （O） pEntry:检索出来的目录项
返回值：
    TRUE:检索成功
*/
s32 findFirstDir(TPath *pPath, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);

/*
功能：检索当前目录下的下一个目录项
参数：
    （I） pPath:文件所在路径
    （IO） pCurrentEntry:用来保存检索过程中的目录项信息
    （O） pEntry:检索出来的目录项
返回值：
    TRUE:检索成功
*/
s32 findNextDir(TPath *pPath, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry);


#endif
