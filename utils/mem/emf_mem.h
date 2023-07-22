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


#ifndef _emf_mem_h
#define _emf_mem_h
#include "emf_typedef.h" 
#include "hw_config.h" 
#include "hal_mem.h" 
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef EMF_MEMPOLL_SIZE
#define EMF_MEMPOLL_SIZE		(2*0x400)			/*2k*/
#endif

#define emf_free_and_clear(p)	do {emf_free(p); p = NULL;} while(0)
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
	uint8_t alignment;
	uint8_t alignment_mask;
	uint16_t heap_index;
	uint16_t heap_len;
	uint8_t* pheap_head;
}mem_buf_t;				//只分分配内存,不释放内存
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void emf_mem_init(void);
void* emf_malloc(uint32_t size);
void emf_free(void* p);
void emf_mem_stats(void);
// mem buf
void *mem_buf_alloc(mem_buf_t * pmbuf, uint16_t size);
void mem_buf_init(mem_buf_t * pmbuf, uint8_t *buf, uint16_t buf_size, uint8_t alignment);
uint16_t get_mem_buf_free_size(mem_buf_t * pmbuf );



//hal
void* hal_malloc(uint32_t size);
void hal_free(void* p);
void hal_mem_stats(void);

#ifdef __cplusplus
}
#endif
#endif





