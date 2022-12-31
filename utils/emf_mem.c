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
#include  "emf_mem.h"
#include  "api/api_log.h"
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static int32_t s_mempoll_used;

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_mem_init(void)
{
	s_mempoll_used = 0;
	#if defined(__C51__) 				//c51 must init mempoll
	static uint8x_t s_mempoll[EMF_MEMPOLL_SIZE];
	init_mempoll(&s_mempoll,sizeof(mempoll));
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
	p = malloc(size);
	EMF_ASSERT(NULL != p);
	if(NULL != p){
		s_mempoll_used += size;
	}
	return p;
}
void emf_free(void* p,uint32_t size)
{
	if(NULL != p){
		free(p);
		s_mempoll_used -= size;
		p = NULL;
	}
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	查看系统malloc内存使用情况
*******************************************************************/
void emf_mem_stats()	
{
	logd("emf_mempoll(%d),usde=%d\n",EMF_MEMPOLL_SIZE,s_mempoll_used);
}








