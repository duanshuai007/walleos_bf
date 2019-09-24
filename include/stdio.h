#ifndef _INCLUDE_STDIO_H_
#define _INCLUDE_STDIO_H_

#include <types.h>
#include <stdarg.h>

#define     SEEK_SET    0  //文件头
#define     SEEK_CUR    1  //当前位置
#define     SEEK_END    2  //文件位

#define     FS_SHORT_FILE_NAME_LEN  12

typedef struct FILE
{
    u32     nUseCount;
    s8      cFileName[FS_SHORT_FILE_NAME_LEN+1]; // 文件的真实名称，目前支持短名
    u32     nSize;                     //文件大小
    u32     nOffset;                   //文件的偏移
    u32     nCluster;                  //文件偏移所在的簇
    u32     nFirstCluster;             //文件的第一簇
    void	*pBuffer;                   //当前正在操作的扇区buffer
    s32     bWrite;                    //只写标志
    s8 	 	*upperDir;                 //文件的上层路径
}FILE;

/*
功能：打开一个文件
参数：
    （I） filename:文件路径
    （I） mode:打开文件的方式 “w”, "a", "r"
返回值：
    文件指针，返回值为NULL时，为打开失败
*/
FILE *fopen(const s8 *filename, const s8 *mode);

/*
功能：关闭一个文件
参数：
    （I） stream: 文件指针
返回值：
    关闭是否成功，TRUE:成功，FALSE：失败
*/
u32 fclose(FILE *stream);

/*
功能：获取文件大小
参数：
    （I） stream: 文件指针
返回值：
    文件大小
*/
u32 fsize(FILE *stream);

/*
功能：写操作
参数：
    （I） ptr:待写入数据
    （I） size:单个数据size
    （I） nmemb:待写入数据的个数
    （I） stream: 文件指针
返回值：
    成功写入数据的size
*/
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/*
功能：读操作
参数：
    （I） hFile：文件handler
    （O） pBuffer:读出数据的缓存
    （I） ulBytes：缓存size（BYTE）
返回值：
    写入数据的size
*/
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);



/*
功能：设置流 stream 的文件位置为给定的偏移 offset，参数 offset 意味着从给定的 whence 位置查找的字节数。
参数：
    （I） hFile：文件handler
    （O） fromwhere:偏移的相对位置（SEEK_SET，SEEK_CUR， SEEK_END）
    （I） offset：偏移量,正值是向后偏移，负值是向前偏移。

返回值：
    文件大小
*/
s32 fseek(FILE *stream, ssize_t offset, s32 whence);

/*
功能：测试给定流 stream 的文件结束标识符。
参数：
    （I） hFile：文件handler
    （O） fromwhere:偏移的相对位置（SEEK_SET，SEEK_CUR， SEEK_END）
    （I） offset：偏移量,正值是向后偏移，负值是向前偏移。

返回值：
    文件大小
*/
s32 feof(FILE *stream);
s32 ferror(FILE *stream);
s32 fflush(FILE *stream);

s32 vsprintf(s8 *s, const s8 *format, va_list arg);
s32 sprintf(s8 *s, const s8 *format, ...);

s32 printf(const s8* fmt, ...);

int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);

void printk_init(void);
int printk(const char *fmt, ...);

#endif
