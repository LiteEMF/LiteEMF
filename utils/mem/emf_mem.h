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
#include "utils/emf_typedef.h" 

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

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

#define emf_free_and_clear(p)	do {emf_free(p); p = NULL;} while(0)

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void emf_mem_init(void);
void* emf_malloc(uint32_t size);
void emf_free(void* p);
void emf_mem_stats(void);


//hal
void* hal_malloc(uint32_t size);
void hal_free(void* p);
void hal_mem_stats(void);

#ifdef __cplusplus
}
#endif
#endif





