#include <fs/file_struct.h>
#include <fs/path.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

s32 parserPath(s8 *pathName, TPath* pStPath){

    assert(pathName != 0);
    assert(pStPath != 0);

    TPath stPath;

    memset(&stPath, 0, sizeof(TPath));

    s8 *pBuff = pathName;
    if (*pBuff == '/')
    {
        stPath.nPathType = FS_PATH_TYPE_ALL;
    }
    else
    {
        stPath.nPathType = FS_PATH_TYPE_PART;
    }

    s32 start = 0;
    s32 end = 0;

    u32 i = 0;
    for (; i < strlen(pBuff); ++i)
    {      
        if (pBuff[i] == '/')
        {
            if (i != 0)
            {
                if (start != end)
                {
                    if (end > start + 8)
                    {

                        return FALSE;//to do  需要对长目录项进行实现，以后再说
                    }

                    memcpy(stPath.aDirectory[stPath.nSeriesNum].cDirectoryName, pBuff + start, end - start);
                    stPath.aDirectory[stPath.nSeriesNum].cDirectoryName[end - start] = '\0';
                    stPath.nSeriesNum++;

                    if (stPath.nSeriesNum >= FS_PATH_SERIES_MAX_NUM)
                    {
                        return FALSE;
                    }
                }else
                {
                    return FALSE;
                }
            }
            start = i+1;
            end = start;

            continue;
        }
        else
        {
            end++;
        }
    }

    if (start != end)
    {
        memcpy(stPath.aDirectory[stPath.nSeriesNum].cDirectoryName, pBuff + start, end - start);
        stPath.aDirectory[stPath.nSeriesNum].cDirectoryName[end - start] = '\0';
        stPath.nSeriesNum++;
    }

    memcpy(pStPath, &stPath, sizeof(TPath));

    return TRUE;
}

u32 cleanUpPath(TPath* pStPath, TPath* result){
    assert(pStPath != 0);
    assert(result != 0);

    result->nPathType = FS_PATH_TYPE_ALL;

    u32 i = 0;
    for (; i < pStPath->nSeriesNum; ++i)
    {
        if (!strcmp(pStPath->aDirectory[i].cDirectoryName, ".."))
        {
            if (result->nSeriesNum <= 1)//当前已经是根目录，没有上一级目录
            {
                return FALSE;
            }

            memset(&(result->aDirectory[result->nSeriesNum - 1]), 0, sizeof(TDirectory));

            result->nSeriesNum--;
        }
        else if (!strcmp(pStPath->aDirectory[i].cDirectoryName, "."))
        {

        }
        else
        {            
            if (result->nSeriesNum >= FS_PATH_SERIES_MAX_NUM)
            {
                return FALSE;
            }

            memcpy(&(result->aDirectory[result->nSeriesNum]), &(pStPath->aDirectory[i]), sizeof(TDirectory));

            result->nSeriesNum++;

        }
    }

    return TRUE;
}

s32 mergePath(TPath *pCurPath, TPath *pTargetPath){
    assert(pCurPath != 0);
    assert(pTargetPath != 0);
    assert(pCurPath->nSeriesNum <FS_PATH_SERIES_MAX_NUM);
    assert(pTargetPath->nSeriesNum + 1 <FS_PATH_SERIES_MAX_NUM);

    TPath stPath;
    memset(&stPath, 0, sizeof(TPath));

    memcpy(&stPath, pCurPath, sizeof(TPath));

    if (pTargetPath->nPathType == FS_PATH_TYPE_ALL)
    {
        u32 i = 0;
        //先清空pCurPath，但是不清空根目录
        for(i = 1; i < stPath.nSeriesNum; i++)
        {
            memset(&stPath.aDirectory[i], 0x0, sizeof(TDirectory));
        }

        //目标的其他目录项拷贝到pCurPath
        for(i = 0; i < pTargetPath->nSeriesNum; i++)
        {
            memcpy(&(stPath.aDirectory[i+1]), &pTargetPath->aDirectory[i], sizeof(TDirectory));
        }

        stPath.nPathType = pTargetPath->nPathType;
        stPath.nSeriesNum = pTargetPath->nSeriesNum + 1;//根目录也算一层
    }
    else
    {
        u32 i = 0;
        for(; i < pTargetPath->nSeriesNum; i++){
            if (!strcmp(pTargetPath->aDirectory[i].cDirectoryName, ".."))
            {
                if (stPath.nSeriesNum <= 1)//当前已经是根目录，没有上一级目录
                {
                    return FALSE;
                }

                memset(&(stPath.aDirectory[stPath.nSeriesNum - 1]), 0, sizeof(TDirectory));

                stPath.nSeriesNum--;
            }
            else if (!strcmp(pTargetPath->aDirectory[i].cDirectoryName, "."))
            {

            }
            else
            {

                if (stPath.nSeriesNum >= FS_PATH_SERIES_MAX_NUM)
                {
                    return FALSE;
                }

                memcpy(&(stPath.aDirectory[stPath.nSeriesNum]), &pTargetPath->aDirectory[i], sizeof(TDirectory));

                stPath.nSeriesNum++;
            }
        }
    }

    TPath stOutputPath;
    memset(&stOutputPath, 0, sizeof(TPath));

    if (!cleanUpPath(&stPath, &stOutputPath))
    {
        return FALSE;
    }

    memset(pCurPath, 0, sizeof(TPath));
    memcpy(pCurPath, &stOutputPath, sizeof(TPath));

    return TRUE;
}

