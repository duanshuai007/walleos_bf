#ifndef ENV_INCLUDE_USPIENV_FS_PATH_H
#define ENV_INCLUDE_USPIENV_FS_PATH_H

s32 parserPath(s8 *pathName, TPath* pStPath);

s32 mergePath(TPath *pCurPath, TPath *pTargetPath);

#endif
