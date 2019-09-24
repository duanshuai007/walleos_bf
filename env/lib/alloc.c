#include <types.h>
#include <config.h>
#include <alloc.h>
#include <s5p4418_serial_stdio.h>
#include <synchronize.h>
#include <macros.h>
#include <stdio.h>

//定义块对齐
#ifndef BLOCK_ALIGN
#define BLOCK_ALIGN	16
#define ALIGN_MASK	(BLOCK_ALIGN-1)
#endif
//定义页对齐
#define PAGE_MASK	(PAGE_SIZE-1)

//定义堆空间最低起始地址
#ifndef MEM_HEAP_START
#define MEM_HEAP_START      0x42C00000
#endif

//定义临界区函数
#ifdef  ALLOC_ENABLE_CRITICAL
#define ENTER_CRITICAL()    EnterCritical()
#define EXIT_CRITICAL()     LeaveCritical()
#else
#define ENTER_CRITICAL()
#define EXIT_CRITICAL()
#endif

#ifdef DMA_SIZE
#define MEM_DMA_ALLOC
#endif

typedef struct TBlockHeader
{
	u32	nMagic		PACKED;
#define BLOCK_MAGIC	0x424C4D43
	u32	nSize		PACKED;
	struct TBlockHeader *pNext	PACKED;
	u32	nPadding	PACKED;
	u8	Data[0];        //不占地址空间
}TBlockHeader;

typedef struct TBlockBucket
{
	u32	nSize;
	TBlockHeader	*pFreeList;
}TBlockBucket;

//DMA
#ifdef MEM_DMA_ALLOC
typedef struct TDMABlockHeader
{
	u32 		nMagic		PACKED;
#define BLOCK_DMA_MAGIC	0x12ab78de
	u32 		nSize		PACKED;
	struct TDMABlockHeader *pNext 	PACKED;
	u32 		nPadding 	PACKED;
	u8 		Data[0];
}TDMABlockHeader;

typedef struct TDMABlockBucket
{
	u32 		nSize;
	TDMABlockHeader *pFreeList;
}TDMABlockBucket;
#endif

//PAGE
#ifdef MEM_PAGE_ALLOC
typedef struct TFreePage
{
	u32	nMagic;
#define FREEPAGE_MAGIC	0x50474D43
	struct TFreePage *pNext;
}
TFreePage;

typedef struct TPageBucket
{
	TFreePage	*pFreeList;
}
TPageBucket;
#endif

static u8 *s_pNextBlock;
static u8 *s_pBlockLimit;

#ifdef MEM_DMA_ALLOC
static u8 *s_pNextDMABlock;
static u8 *s_pDMABlockLimit;
#endif

#ifdef MEM_PAGE_ALLOC
static u8 *s_pNextPage;
static u8 *s_pPageLimit;
#endif

static TBlockBucket s_BlockBucket[] = { 
    {0x40,          NULL}, 
    {0x400,         NULL}, 
    {0x1000,        NULL}, 
    {0x4000,        NULL}, 
	{0x8000,        NULL},   
    {0x40000,       NULL}, 
    {0x80000,       NULL},  
    {0x100000,      NULL},  //1MB
	{0x200000,      NULL}, 
    {0x400000,      NULL}, 
    {0x800000,      NULL}, 
    {0x1000000,     NULL},
	{0x2000000,     NULL}, 
    {0x4000000,     NULL}, 
    {0,             NULL}
};

#ifdef DMA_SIZE
static TDMABlockBucket s_DMABlockBucket[] ={ 
	{0x100000,      NULL}, 
    {0x200000,      NULL}, 
    {0x400000,      NULL}, 
    {0x800000,      NULL}, 
    {0x1000000,     NULL},
    {0x2000000,     NULL},
    {0x4000000,     NULL},
    {0x8000000,     NULL},
    {0,             NULL}
};
#endif

#ifdef MEM_PAGE_ALLOC
static TPageBucket s_PageBucket;
#endif

u32 heap_start;
u32 heap_size;

void mem_initial (u32 ulBase, u32 ulSize, u32 ulDMABase, u32 ulDMASize)
{
	if (ulBase < MEM_HEAP_START)
	{
		ulBase = MEM_HEAP_START;
	}

	heap_start = ulBase;

	if(ulBase + ulSize > MAX_MEMORY)
	{
		ulSize -= ulBase + ulSize - MAX_MEMORY;
	}
	heap_size = ulSize;

	u32 ulLimit = ulBase + ulSize;

	//获取实际上能够申请的size	
	ulSize = ulLimit - ulBase;
#ifdef MEM_PAGE_ALLOC
	//乘以3/4，前3/4地址空间用于块分配
	u32 ulQuarterSize = (ulSize * 3)/ 4;
	//设置下一块的地址
	s_pNextBlock = (u8 *) ulBase;
	//设置块的尾部地址
	s_pBlockLimit = (u8 *) (ulBase + ulQuarterSize);
	//后1/4空间用于页分配
	//设置页地址
	s_pNextPage = (u8 *) ((ulBase + ulQuarterSize + PAGE_SIZE) & ~PAGE_MASK);
	//设置页空间的尾部地址
	s_pPageLimit = (u8 *) ulLimit;
#else
	s_pNextBlock = (u8 *) ulBase;
	s_pBlockLimit = (u8 *) (ulBase + ulSize);
#endif

#ifdef MEM_DMA_ALLOC
	//DMA
        s_pNextDMABlock = (u8 *)ulDMABase;
        s_pDMABlockLimit = (u8 *)(ulDMABase + ulDMASize);
#endif

	DEBUG(ALLOC_DEBUG, "ulBase:%08x, ulSize:%08x, ulLimit:%08x, ulQuarterSize:%08x\r\n",ulBase, ulSize, ulLimit, ulQuarterSize);
#ifdef DMA_SIZE
	DEBUG(ALLOC_DEBUG, "DMA_BASE:%08x, DMA_END:%08x\r\n", s_pNextDMABlock, s_pDMABlockLimit);
#endif
	DEBUG(ALLOC_DEBUG, "s_pNextBlock:%08x, s_pBlockLimit:%08x\r\ns_pNextPage:%08x, s_pPageLimit:%08x\r\n", s_pNextBlock, s_pBlockLimit, s_pNextPage, s_pPageLimit);
}

void *malloc (u32 ulSize)
{
	if(s_pNextBlock == 0)
	{
		DEBUG(ALLOC_DEBUG, "s_pNextBlock is NULL\r\n");   
		return NULL;
	}

	ENTER_CRITICAL();

	TBlockBucket *pBucket;
	//判断所申请的内存size是否支持，如果所申请的内存空间大于所能支持的最大的内存块，则申请失败
	for (pBucket = s_BlockBucket; pBucket->nSize > 0; pBucket++)
	{
		if (ulSize <= pBucket->nSize)
		{   //如果找到了大于所申请的内存size的内存块
			ulSize = pBucket->nSize;

			break;
		}
	}

	if(pBucket->nSize == 0)
	{   //找不到支持的内存size
		DEBUG(ALLOC_DEBUG, "can't support big memory alloc\r\n");
		EXIT_CRITICAL();

		return NULL;
	}

	//创建临时内存地址指针
	TBlockHeader *pBlockHeader;
	//如果当前的块size大于0（即寻找到合适的内存块）并且该块不是尾节点
	//if ( pBucket->nSize > 0)
	//{
	if((pBucket->nSize > 0)&&((pBlockHeader = pBucket->pFreeList) != 0))
	{   //如果在链表中所要申请的块大小有对应的空闲块，则直接从链表中取出该块
		//		assert (pBlockHeader->nMagic == BLOCK_MAGIC);
		pBucket->pFreeList = pBlockHeader->pNext;//将该块从链表中移出
		pBlockHeader->nMagic = BLOCK_MAGIC;     //设置该块魔数，表示该块处于使用状态
		DEBUG(ALLOC_DEBUG, "pBucket->pFreeList != 0\r\n");
	}
	else
	{
		//pBlockHeader指向s_pNextBlock，s_pNextBlock在每次申请块内存后地址递增，该地址就是空闲地址的首地址
		pBlockHeader = (TBlockHeader *) s_pNextBlock;
		//从s_pNextBlock中减去所申请的空间大小（内存块的头+内存块大小），并且地址对齐
		s_pNextBlock += (sizeof (TBlockHeader) + ulSize + BLOCK_ALIGN-1) & ~ALIGN_MASK;
		//如果申请后的地址在CONFIG_FB_ADDR~CONFIG_FB_ADDR+4MB的范围内
		if(((u32)s_pNextBlock > CONFIG_FB_ADDR) && ((u32)s_pNextBlock <= (CONFIG_FB_ADDR + 4 * 1024 * 1024)))
		{
			s_pNextBlock = (u8 *)((CONFIG_FB_ADDR + (4 * 1024 * 1024) + BLOCK_ALIGN) & ~ALIGN_MASK);
			pBlockHeader = (TBlockHeader *) s_pNextBlock;
			s_pNextBlock += (sizeof (TBlockHeader) + ulSize + BLOCK_ALIGN-1) & ~ALIGN_MASK;
		}

		if (s_pNextBlock > s_pBlockLimit)
		{   //如果超出内存限制，申请失败
			EXIT_CRITICAL();
			return NULL;		// TODO: system should panic here
		}

		pBlockHeader->nMagic = BLOCK_MAGIC;         //设置块为已使用
		pBlockHeader->nSize = (unsigned) ulSize;    //记录块的大小
		DEBUG(ALLOC_DEBUG, "pBucket->pFreeList == 0\r\n");
	}
	//}
	EXIT_CRITICAL();
	pBlockHeader->pNext = 0;

	void *pResult = pBlockHeader->Data;
	//	assert (((u32) pResult & ALIGN_MASK) == 0);
	DEBUG(ALLOC_DEBUG, "in malloc:pBlockHeader:size=%08x, addr=%08x\r\n",ulSize, pBlockHeader->Data);
	return pResult;
}

void free (void *p)
{
	if(p == NULL)
	{
		return;
	}
	//为了寻找到本内存块的实际头地址 [[head][memory]]
	TBlockHeader *pBlockHeader = (TBlockHeader *) ((u32) p - sizeof (TBlockHeader));
	DEBUG(ALLOC_DEBUG,  "free:pBlockHeader=%08x\r\n", pBlockHeader);

	for (TBlockBucket *pBucket = s_BlockBucket; pBucket->nSize > 0; pBucket++)
	{
		//如果找到相同大小的块
		if (pBlockHeader->nSize == pBucket->nSize)
		{
			DEBUG(ALLOC_DEBUG,  "find same size block:%08x\r\n", pBlockHeader->nSize);
			ENTER_CRITICAL();
			//将释放掉的块加入链表
			pBlockHeader->nMagic = 0;
			pBlockHeader->pNext = pBucket->pFreeList; //将头链表指针给pNext
			pBucket->pFreeList = pBlockHeader;

		    EXIT_CRITICAL();

			break;
		}
	}
}

#ifdef MEM_DMA_ALLOC
/*
*	DMA memory 
*/

void *dma_malloc(u32 ulSize)
{
	if(s_pNextDMABlock == 0)
	{
		DEBUG(ALLOC_DEBUG, "s_pNextBlock is NULL\r\n");
		return NULL;
	}

	ENTER_CRITICAL();
	
	TDMABlockBucket *pDMABucket;
	for (pDMABucket = s_DMABlockBucket; pDMABucket->nSize > 0; pDMABucket++)
	{
		if (ulSize <= pDMABucket->nSize)
		{   //如果找到了大于所申请的内存size的内存块
			ulSize = pDMABucket->nSize;

			break;
		}
	}

	if(pDMABucket->nSize == 0)
	{   //找不到支持的内存size
		DEBUG(ALLOC_DEBUG, "can't support big memory alloc\r\n");
		EXIT_CRITICAL();

		return NULL;
	}

	TDMABlockHeader *pDMABlockHeader;
	if((pDMABucket->nSize > 0)&&((pDMABlockHeader = pDMABucket->pFreeList) != 0))
	{   //如果在链表中所要申请的块大小有对应的空闲块，则直接从链表中取出该块
		pDMABucket->pFreeList = pDMABlockHeader->pNext;//将该块从链表中移出
		pDMABlockHeader->nMagic = BLOCK_DMA_MAGIC;     //设置该块魔数，表示该块处于使用状态
			DEBUG(ALLOC_DEBUG, "pBucket->pFreeList != 0\r\n");
	}
	else
	{
		pDMABlockHeader = (TDMABlockHeader *) s_pNextDMABlock;
		s_pNextDMABlock += (sizeof (TDMABlockHeader) + ulSize + BLOCK_ALIGN-1) & ~ALIGN_MASK;
		if (s_pNextDMABlock > s_pDMABlockLimit)
		{   //如果超出内存限制，申请失败
			EXIT_CRITICAL();
			return NULL;            // TODO: system should panic here
		}

		pDMABlockHeader->nMagic = BLOCK_DMA_MAGIC;         //设置块为已使用
		pDMABlockHeader->nSize = (unsigned) ulSize;    //记录块的大小
		DEBUG(ALLOC_DEBUG, "pBucket->pFreeList == 0\r\n");
	}
	
	EXIT_CRITICAL();
	pDMABlockHeader->pNext = 0;

	void *pResult = pDMABlockHeader->Data;
	DEBUG(ALLOC_DEBUG, "in DMA malloc:pDMABlockHeader:size=%08x, addr=%08x\r\n",ulSize, pDMABlockHeader->Data);
	return pResult;
}

void dma_free(void *p)
{
	if(p == NULL)
	{
		return;
	}
	//为了寻找到本内存块的实际头地址 [[head][memory]]
	TDMABlockHeader *pDMABlockHeader = (TDMABlockHeader *) ((u32) p - sizeof (TDMABlockHeader));
	DEBUG(ALLOC_DEBUG,  "free:pBlockHeader=%08x\r\n", pDMABlockHeader);

	for (TDMABlockBucket *pDMABucket = s_DMABlockBucket; pDMABucket->nSize > 0; pDMABucket++)
	{
		//如果找到相同大小的块
		if (pDMABlockHeader->nSize == pDMABucket->nSize)
		{
			DEBUG(ALLOC_DEBUG,  "find same size block:%08x\r\n", pDMABlockHeader->nSize);
			ENTER_CRITICAL();
			//将释放掉的块加入链表
			pDMABlockHeader->nMagic = 0;
			pDMABlockHeader->pNext = pDMABucket->pFreeList; //将头链表指针给pNext
			pDMABucket->pFreeList = pDMABlockHeader;

			EXIT_CRITICAL();

			break;
		}
	}
}
#endif

#ifdef MEM_PAGE_ALLOC

void *palloc (void)
{
	ENTER_CRITICAL();

	TFreePage *pFreePage;
	//如果s_PageBucket.pFreeList不是指向末尾
	if ((pFreePage = s_PageBucket.pFreeList) != 0)
	{
		//将pFreePage页从链表中移出`
		//	assert (pFreePage->nMagic == FREEPAGE_MAGIC);
		s_PageBucket.pFreeList = pFreePage->pNext;
		//将该页设置为使用
		pFreePage->nMagic = 0;
	}
	else
	{//如果指向的是最后一页
		pFreePage = (TFreePage *) s_pNextPage;

		s_pNextPage += PAGE_SIZE;

		if (s_pNextPage > s_pPageLimit)
		{
			EXIT_CRITICAL();

			return NULL;		// TODO: system should panic here
		}
	}

	EXIT_CRITICAL();

	return pFreePage;
}

void pfree (void *pPage)
{
	if(pPage == NULL)
	{
		return;
	}

	TFreePage *pFreePage = (TFreePage *) pPage;

	ENTER_CRITICAL();
	//将页状态设置为空闲
	pFreePage->nMagic = FREEPAGE_MAGIC;
	//将空闲页重新加入链表
	pFreePage->pNext = s_PageBucket.pFreeList;
	s_PageBucket.pFreeList = pFreePage;

	EXIT_CRITICAL();
}

#endif

