//
// alloc.h
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _ENV_INCLUDE_ALLOC_H_
#define _ENV_INCLUDE_ALLOC_H_

#include <config.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 *  该函数由start.S中调用，为用户分配所有bss段结束后的地址空间
 *  不允许出现在用户程序中。
 */
void mem_initial(u32 ulBase, u32 ulSize, u32 ulDMABase, u32 ulDMASize);

/*
 *  查询剩余的空间大小
 */
u32 mem_get_size (void);

/*
 *  从已经分配的内存空间中申请一块内存，内存最小为64Byte
 *  
 */
void *malloc(u32 ulSize);	// resulting block is always 16 bytes aligned

/*
 *  释放由malloc申请到的内存
 *
 */
void free(void *p);

/*
*	dma 申请内存,只分配no cache，no buffer的内存块
*	最小返回1MB内存空间，最大返回4MB内存空间
*/

void *dma_malloc(u32 ulSize);
void dma_free(void *p);

/*
*	PAGE 内存页分配
*
*/
#ifdef MEM_PAGE_ALLOC
void *palloc (void);		// returns 4K page (aligned)
void pfree (void *pPage);
#endif

#ifdef __cplusplus
}
#endif

#endif
