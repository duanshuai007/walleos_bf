#include <fs/file.h>
#include <fs/file_struct.h>
#include <fs/path.h>
#include <fs/fatfs.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <alloc.h>
#include <errno.h>

void tool_separate_dir_and_file(const s8 *pPathStr, s8 **ppcCurPath, s8 **ppFileName){

    assert(pPathStr != 0);
    assert(ppcCurPath != 0);
    assert(ppFileName != 0);

    s32 i;
    for (i = strlen(pPathStr)-1; i >= 0; i--)
    {
        if ((pPathStr[i] == '/') && (i == (s32)strlen(pPathStr)-1))
        {            
            return;               
        }

        if (pPathStr[i] == '/')
        {
            *ppcCurPath = (s8 *) malloc (i+1);
            if(*ppcCurPath == NULL)
                return;
            memcpy(*ppcCurPath, pPathStr, i);
            (*ppcCurPath)[i] = '\0';

            *ppFileName = (s8 *) malloc (strlen(pPathStr) - i);           
            if(*ppFileName == NULL)
                return;
            memcpy(*ppFileName, &(pPathStr[i+1]), strlen(pPathStr) - i - 1);           
            (*ppFileName)[strlen(pPathStr) - i - 1] = '\0';

            break;
        }
    }

    if (i < 0)
    {
        *ppcCurPath = (s8 *) malloc (2);
        if(*ppcCurPath == NULL)
            return;
        strcpy(*ppcCurPath, "/");

        *ppFileName = (s8 *) malloc (strlen(pPathStr) + 1);     
        if(*ppFileName == NULL)
            return;      
        memcpy(*ppFileName, pPathStr, strlen(pPathStr));
        (*ppFileName)[strlen(pPathStr)] = '\0';
    }

}


u32 fmount(void){
    return FATFileSystemMount() == 0 ? FALSE : TRUE;
}

void funmount(void){
    FATFileSystemUnMount();
}

u32 fcreateDir(TPath *pPath, s8 *pDirName)
{
    assert(pPath != 0);
    assert(pDirName != 0);

    TDirectory *pUpperDir = &(pPath->aDirectory[pPath->nSeriesNum - 1]);

    return FATFileSystemDirCreate(pUpperDir, pDirName);
}


u32 fcreateDirs(TPath *pCurPath, s8 *pCreatePath)
{
    assert(pCurPath != 0);
    assert(pCreatePath != 0);

    TPath targetPath;
    memset(&targetPath, 0, sizeof(TPath));

    if (!parserPath(pCreatePath, &targetPath))
    {
        return FALSE;
    }

    TPath curPath;
    memset(&curPath, 0, sizeof(TPath));

    memcpy(&curPath, pCurPath, sizeof(TPath));

    if(!mergePath(&curPath, &targetPath)){
        return FALSE;
    }

    u32 i = 1;
    for (; i < curPath.nSeriesNum; ++i)
    {
        if (curPath.aDirectory[i].nInitFlg == 1)
        {
            continue;
        }

        if(FATFileSystemDirOpen(&(curPath.aDirectory[i-1]), &(curPath.aDirectory[i])) == FS_DIR_NOT_EXIST)
        {
            if (FS_DIR_CREATE_FAILED == FATFileSystemDirCreate(&(curPath.aDirectory[i-1]), curPath.aDirectory[i].cDirectoryName))
            {
                return FALSE;
            }
            else
            {
                if(FATFileSystemDirOpen(&(curPath.aDirectory[i-1]), &(curPath.aDirectory[i])) == FS_DIR_NOT_EXIST)
                {
                    return FALSE;
                }                
            }

        }
    }

    return TRUE;
}

u32 fopenRootDir(TPath *pPath)
{

    assert(pPath != 0);
    if(FS_DIR_NOT_EXIST == FATFileSystemRootDirOpen(&pPath->aDirectory[0]))
    {
        return FALSE;
    }

    pPath->nSeriesNum = 1;
    
    return TRUE;
}

u32 fopenDir(s8 *pPathStr, u32 nCreateFlg, TPath *pPath)
{
    assert(pPath != 0);

    if(!fopenRootDir(pPath)){
        return FALSE;
    }

    if (pPathStr != 0)
    {
        TPath targetPath;
        memset(&targetPath, 0, sizeof(TPath));

        if (!parserPath(pPathStr, &targetPath))
        {
            return FALSE;
        }

        TPath curPath;
        memset(&curPath, 0, sizeof(TPath));

        memcpy(&curPath, pPath, sizeof(TPath));

        if(!mergePath(&curPath, &targetPath)){
            return FALSE;
        }

        u32 i = 1;
        for (; i < curPath.nSeriesNum; ++i)
        {
            if (curPath.aDirectory[i].nInitFlg == 1)
            {
                continue;
            }

            if (!isDirectory(&(curPath.aDirectory[i-1]), curPath.aDirectory[i].cDirectoryName))
            {
                return FALSE;
            }

            if(FATFileSystemDirOpen(&(curPath.aDirectory[i-1]), &(curPath.aDirectory[i])) == FS_DIR_NOT_EXIST)
            {
                if (nCreateFlg == TRUE)
                {
                    if(FS_DIR_CREATE_FAILED == FATFileSystemDirCreate(&(curPath.aDirectory[i-1]), curPath.aDirectory[i].cDirectoryName))
                    {
                        return FALSE;
                    }

                    if(FATFileSystemDirOpen(&(curPath.aDirectory[i-1]), &(curPath.aDirectory[i])) == FS_DIR_NOT_EXIST)
                    {
                        return FALSE;
                    }
                }
                else
                {
                    return FALSE;
                }
            }
        }

        memcpy(pPath, &curPath, sizeof(TPath));

        return TRUE;

    }

    return TRUE;
}


FILE *fopenCurFile(TPath *pPath, s8 *pFileName, const s8 *mode)
{
    assert(pFileName != 0);
    assert(pPath != 0);

    TDirectory *pUpperDir = &(pPath->aDirectory[pPath->nSeriesNum  - 1]);

    if (isDirectory(pUpperDir, pFileName))
    {
        return NULL;
    }

    u32 nMode = 0;
    if (strlen(mode) > 2)
    {
        return NULL;
    }

    if (mode[0] == 'a')
    {
        nMode = MODE_WRITE;
    }
    else if (mode[0] == 'r')
    {   
        nMode = MODE_READ;
    }
    else if (mode[0] == 'w')
    {
        nMode = MODE_CREATE;
    }
    else
    {
        return NULL;
    }

    FILE * stream = 0;
    if (nMode & MODE_CREATE)
    {
        stream = FATFileSystemFileCreate(pUpperDir, pFileName);

    } else if (nMode & MODE_WRITE){

        stream = FATFileSystemFileOpen(pUpperDir, pFileName, 1);

    } else if (nMode & MODE_READ)
    {
        stream = FATFileSystemFileOpen(pUpperDir, pFileName, 0);
    }

    return stream;
}



u32 fdelete(TPath *pPath, s8 *file){
    assert(pPath!=0);

    assert(file!=0);

    TDirectory *pUpperDir = &(pPath->aDirectory[pPath->nSeriesNum  - 1]);

    s32 result = FATFileSystemFileDelete(pUpperDir, file);

    if (-1 != result)
    {
        return FILE_SUCCESS;
    }else{
        return FILE_FAILED;
    }
}


u32 fswitchDir(TPath *pCurPath, s8 *pNewPathStr){
    assert(pCurPath != 0);
    assert(pNewPathStr != 0);

    TPath targetPath;
    memset(&targetPath, 0, sizeof(TPath));

    if (!parserPath(pNewPathStr, &targetPath))
    {
        return FALSE;
    }

    TPath curPath;
    memset(&curPath, 0, sizeof(TPath));

    memcpy(&curPath, pCurPath, sizeof(TPath));

    if(!mergePath(&curPath, &targetPath)){
        return FALSE;
    }

    u32 i = 1;
    for (; i < curPath.nSeriesNum; ++i)
    {
        if (curPath.aDirectory[i].nInitFlg == 1)
        {
            continue;
        }

        if (!isDirectory(&(curPath.aDirectory[i-1]), curPath.aDirectory[i].cDirectoryName))
        {
            return FALSE;
        }

        if(FATFileSystemDirOpen(&(curPath.aDirectory[i-1]), &(curPath.aDirectory[i])) == FS_DIR_NOT_EXIST)
        {
            return FALSE;
        }
    }

    memcpy(pCurPath, &curPath, sizeof(TPath));

    return TRUE;

}

u32 isDirectory(TDirectory *pUpperDir, s8 *file){
    assert(pUpperDir!=0);

    assert(file!=0);

    TDirentry Direntry;
    memset(&Direntry, 0, sizeof(TDirentry));

    TFindCurrentEntry CurrentEntry;

    memset(&CurrentEntry, 0, sizeof(TFindCurrentEntry));

    u32 nEntry = FATFileSystemDirFindFirst (pUpperDir, &CurrentEntry, &Direntry);

    while (nEntry != 0)
    {
        if (strcmp(Direntry.cFileName, file) == 0)
        {
            if (Direntry.nAttributes & FAT_DIR_ATTR_DIRECTORY)
            {
                return TRUE;
            }
        }

        nEntry = FATFileSystemDirFindNext (pUpperDir, &CurrentEntry, &Direntry);
    }

    return FALSE;
}


s32 findFirstDir(TPath *pPath, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    assert(pPath!=0);
    assert(pCurrentEntry!=0);
    assert(pEntry!=0);

    TDirectory *pUpperDir = &(pPath->aDirectory[pPath->nSeriesNum  - 1]);

    return FATFileSystemDirFindFirst(pUpperDir, pCurrentEntry, pEntry);
}

s32 findNextDir(TPath *pPath, TFindCurrentEntry *pCurrentEntry, TDirentry *pEntry)
{
    assert(pPath!=0);
    assert(pCurrentEntry!=0);
    assert(pEntry!=0);

    TDirectory *pUpperDir = &(pPath->aDirectory[pPath->nSeriesNum  - 1]);

    return FATFileSystemDirFindNext(pUpperDir, pCurrentEntry, pEntry);
}


FILE *fopen(const s8 *filename, const s8 *mode)
{
    assert(filename != 0);

    const s8 *pFilePath = filename;

    if (pFilePath == 0)
    {
        return NULL;
    }

    s8 *pcCurPath = 0;
    s8 *pFileName = 0;

    tool_separate_dir_and_file(pFilePath, &pcCurPath, &pFileName);    

   if (pcCurPath == 0 || pFileName == 0 )
    {
        return NULL;
    } 

    TPath wholePath;
    memset(&wholePath, 0, sizeof(TPath));

    if(!fopenDir(pcCurPath, FALSE, &wholePath))
    {
        return NULL;
    }

    FILE *stream = fopenCurFile(&wholePath, pFileName, mode);

    if (pcCurPath != 0)
    {
        free(pcCurPath);
        pcCurPath = 0;
    }

    if (pFileName != 0)
    {
        free(pFileName);
        pFileName = 0;
    }

    return stream;
}

u32 fclose(FILE *stream)
{
    assert(stream!=0);

    return FATFileSystemFileClose(stream) == 0 ? FALSE : TRUE;
}

u32 fsize(FILE *stream)
{
    assert(NULL!=stream);

    return FATFileSystemGetSize(stream);
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    assert(ptr!=NULL);
    assert(stream!=NULL);
    assert(size!=0);
    assert(nmemb!=0);

    u32 ulBytes = size * nmemb;
    u32 result = FATFileSystemFileWrite(stream, ptr, ulBytes);
 
    if(result == FS_ERROR){
        return FS_OPT_ERROR;
    }else{
        return result / size;
    }

}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    assert(ptr!=NULL);
    assert(stream!=NULL);
    assert(size!=0);
    assert(nmemb!=0);

    u32 ulBytes = size * nmemb;
    u32 result = FATFileSystemFileRead(stream, ptr, ulBytes);
    if(result == FS_ERROR){
        return FS_OPT_ERROR;
    }else{
        return result / size;
    }

}

s32 fseek(FILE *stream, ssize_t offset, s32 whence)
{
    return FATFileSystemFileSeek(stream, offset, whence);
}

s32 feof(FILE *stream)
{
    return (stream->nSize == stream->nOffset);
}

s32 ferror(FILE *stream)
{
    UNUSED(stream);
    return 1;
}

s32 fflush(FILE *stream)
{
    UNUSED(stream);
    return 0;
}
