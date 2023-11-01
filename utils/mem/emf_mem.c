/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
* 
*/

/************************************************************************************************************
**	Description:Memory Management
************************************************************************************************************/
#include "utils/mem/emf_mem.h"
#include "utils/mem/heap_config.h"

#include  "api/api_log.h"

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	mem_buf_alloc 只分分配内存,不释放内存
	使用的时候需要单独创建 mem_buf_t
*******************************************************************/
void *mem_buf_alloc(mem_buf_t * pmbuf, uint16_t size)
{
    void *p = NULL;

	if(NULL == pmbuf) return NULL;
	if(NULL == pmbuf->pheap_head) return NULL;

    /* Ensure that blocks are always aligned. */
	if( size & pmbuf->alignment_mask ){
		/* Byte alignment required. Check for overflow. */
		if((size + (pmbuf->alignment - (size & pmbuf->alignment_mask))) > size ){
			size += (pmbuf->alignment - (size & pmbuf->alignment_mask) );
		}else{
			size = 0;
		}
	}

	/* Check there is enough room left for the allocation and. */
	if( ( size > 0 ) &&                                /* valid size */
		( ( pmbuf->heap_index + size ) < pmbuf->heap_len ) &&
		( ( pmbuf->heap_index + size ) > pmbuf->heap_index ) ) {/* Check for overflow. */
		/* Return the next free byte then increment the index past this block. */
		p = pmbuf->pheap_head + pmbuf->heap_index;
		pmbuf->heap_index += size;
	}

    return p;
}

void mem_buf_init(mem_buf_t * pmbuf, uint8_t *buf, uint16_t buf_size, uint8_t alignment)
{
	pmbuf->alignment = alignment;
	pmbuf->alignment_mask = alignment -1;
	pmbuf->heap_index = 0;
	pmbuf->heap_len = buf_size - alignment;
	pmbuf->pheap_head = ( uint8_t * ) (((uint32_t)&buf[pmbuf->alignment_mask]) & ~((uint32_t)(pmbuf->alignment_mask)));
}

uint16_t get_mem_buf_free_size( mem_buf_t * pmbuf )
{
    return( pmbuf->heap_len - pmbuf->heap_index );
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_mem_init(void)
{
	#if 3 == HEAP_ID
		#if defined(__C51__) 				//c51 must init mempoll
		static uint8x_t s_mempoll[EMF_MEMPOLL_SIZE];
		init_mempoll(&s_mempoll,sizeof(mempoll));
		#endif
	#elif HEAP_ID
		vPortInitialiseBlocks();
	#endif
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void* emf_malloc(uint32_t size)
{
	void* p = NULL;
	if(0 == size) return p;

	#if HEAP_ID
	p = pvPortMalloc(size);
	#else
	p = hal_malloc(size);
	#endif
	EMF_ASSERT(NULL != p);
	return p;
}
void emf_free(void* p)
{
	if(NULL == p) return;
	
	#if HEAP_ID
	vPortFree(p);
	#else
	hal_free(p);
	#endif
}






/*******************************************************************
** Parameters:		
** Returns:	
** Description:	查看系统malloc内存使用情况
*******************************************************************/
void emf_mem_stats(void)	
{
	#if (4 == HEAP_ID ) || (5 == HEAP_ID )
	HeapStats_t xHeapStats;
	vPortGetHeapStats(&xHeapStats);

	logd("emf mem stats heap(%d) free size=%d\n",EMF_MEMPOLL_SIZE,xHeapStats.xAvailableHeapSpaceInBytes);
	logd("	free size: largest=%d,smallest=%d, num=%d\n",xHeapStats.xSizeOfLargestFreeBlockInBytes,xHeapStats.xSizeOfSmallestFreeBlockInBytes,xHeapStats.xNumberOfFreeBlocks);
	logd("	malloc times=%d, free times=%d\n",xHeapStats.xNumberOfSuccessfulAllocations,xHeapStats.xNumberOfSuccessfulFrees);
	#elif (1 == HEAP_ID ) || (2 == HEAP_ID )
	uint32_t heap_free_size;
	heap_free_size = xPortGetFreeHeapSize();
	logd("emf heap(%d) free size=%d\n",EMF_MEMPOLL_SIZE,heap_free_size);
	#else
	hal_mem_stats();
	#endif
}








