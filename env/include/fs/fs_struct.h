#ifndef ENV_INCLUDE_USPIENV_FS_FS_STRUCT_H
#define ENV_INCLUDE_USPIENV_FS_FS_STRUCT_H
#include <types.h>

#define MAX_PARTITIONS		4

#define FS_BLOCK_SIZE		512
#define FS_BLOCK_MASK		(FS_BLOCK_SIZE-1)
#define FS_BLOCK_SHIFT		9

#define PARTITION_NAME_LEN_MAX	20

typedef struct TPartition{
	s8 m_pPartitionName[PARTITION_NAME_LEN_MAX];
	u32 m_nFirstSector;
	u32 m_nNumberOfSectors;
} TPartition;

typedef struct TPartitionManager{
	const s8  *m_DeviceName;
	TPartition *m_pPartition[MAX_PARTITIONS];	
}TPartitionManager;

#endif
