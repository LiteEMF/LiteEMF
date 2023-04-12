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
** Description:		
*******************************************************************/
void emf_mem_init(void)
{
	#if 3 == HEAP_ID
		#if defined(__C51__) 				//c51 must init mempoll
		static uint8x_t s_mempoll[EMF_MEMPOLL_SIZE];
		init_mempoll(&s_mempoll,sizeof(mempoll));
		#endif
	#else
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
	p = pvPortMalloc(size);
	EMF_ASSERT(NULL != p);
	return p;
}
void emf_free(void* p)
{
	vPortFree(p);
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
	#elif (3 != HEAP_ID )
	uint32_t heap_free_size;
	heap_free_size = xPortGetFreeHeapSize();
	logd("emf heap(%d) free size=%d\n",EMF_MEMPOLL_SIZE,heap_free_size);
	#endif
}








