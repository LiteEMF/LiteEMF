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
//@ref: https://www.freertos.org/zh-cn-cmn-s/a00111.html
//@ref: https://cn.bing.com/search?q=FREERTOS+heap%E8%AF%A6%E8%A7%A3&PC=U316&FORM=CHROMN



#ifndef _heap_config_h
#define _heap_config_h
#include "emf_typedef.h"
#include "utils/mem/emf_mem.h"
#include  "api/api_system.h"

#include "api/api_log.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
/*
heap_1 —— 最简单，不允许释放内存。
heap_2—— 允许释放内存，但不会合并相邻的空闲块。
heap_3 —— 简单包装了标准 malloc() 和 free()，以保证线程安全。
heap_4 —— 合并相邻的空闲块以避免碎片化。 包含绝对地址放置选项。
heap_5 —— 如同 heap_4，能够跨越多个不相邻内存区域的堆。
*/
#ifndef HEAP_ID
#define HEAP_ID			4					/* used heap_n file*/
#endif

/* Memory allocation related definitions. */
#define configSUPPORT_DYNAMIC_ALLOCATION 	1				/* must always 1 */
#define configTOTAL_HEAP_SIZE 				EMF_MEMPOLL_SIZE
#ifndef configAPPLICATION_ALLOCATED_HEAP
#define configAPPLICATION_ALLOCATED_HEAP 	0				/* ucHeap defined by project */
#endif

#ifndef traceMALLOC
    #define traceMALLOC( pvAddress, uiSize )	
    // #define traceMALLOC( pvAddress, uiSize )	logd("malloc addr=%x, size=%d\n",pvAddress, uiSize)
#endif
#ifndef traceFREE
    #define traceFREE( pvAddress, uiSize )	   
    // #define traceFREE( pvAddress, uiSize )	    logd("free addr=%x, size=%d\n",pvAddress, uiSize)
#endif


#define taskENTER_CRITICAL()        API_ENTER_CRITICAL()
#define taskEXIT_CRITICAL()         API_EXIT_CRITICAL()

#ifndef configASSERT
	#define configASSERT(expr) 				EMF_ASSERT(expr)	
#endif
#ifndef vTaskSuspendAll
	#define vTaskSuspendAll() 					
#endif
#ifndef xTaskResumeAll
	#define xTaskResumeAll() 					0
#endif
#ifndef configUSE_MALLOC_FAILED_HOOK
    #define configUSE_MALLOC_FAILED_HOOK    0
#endif
#ifndef mtCOVERAGE_TEST_MARKER
    #define mtCOVERAGE_TEST_MARKER()
#endif
/* No test marker by default. */
#ifndef mtCOVERAGE_TEST_MARKER
    #define mtCOVERAGE_TEST_MARKER()
#endif



#define portPOINTER_SIZE_TYPE           uintptr_t
#define portBYTE_ALIGNMENT              sizeof(uintptr_t)
#define portBYTE_ALIGNMENT_MASK         ( portBYTE_ALIGNMENT -1 )

#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA
#define FREERTOS_SYSTEM_CALL


#define BaseType_t					bool
#define pdFALSE						false
#define pdTRUE						true
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
/* Used by heap_5.c to define the start address and size of each memory region
 * that together comprise the total FreeRTOS heap space. */
typedef struct HeapRegion
{
    uint8_t * pucStartAddress;
    size_t xSizeInBytes;
} HeapRegion_t;

/* Used to pass information about the heap out of vPortGetHeapStats(). */
typedef struct xHeapStats
{
    size_t xAvailableHeapSpaceInBytes;      /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    size_t xSizeOfLargestFreeBlockInBytes;  /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xSizeOfSmallestFreeBlockInBytes; /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xNumberOfFreeBlocks;             /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xMinimumEverFreeBytesRemaining;  /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    size_t xNumberOfSuccessfulAllocations;  /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    size_t xNumberOfSuccessfulFrees;        /* The number of calls to vPortFree() that has successfully freed a block of memory. */
} HeapStats_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void * pvPortMalloc( size_t xWantedSize );
void vPortFree( void * pv );
size_t xPortGetMinimumEverFreeHeapSize( void );
void vPortGetHeapStats( HeapStats_t * pxHeapStats );
void vPortInitialiseBlocks( void );
size_t xPortGetFreeHeapSize( void );

#ifdef __cplusplus
}
#endif
#endif





