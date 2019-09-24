#ifndef _FS_FATFORMAL_H_
#define _FS_FATFORMAL_H_

#include <types.h>
#include <fs/fs_struct.h>
#include <fs/fat_struct.h>

void CFatFormal(void);

boolean isFatFormalFin(TFatCache *pCache);

s32 FormatFat32Disk(TFatCache *pCache, TPartition *pPartition);

#endif
