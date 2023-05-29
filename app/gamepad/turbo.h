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


#ifndef _turbo_h
#define _turbo_h
#include "utils/emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint8_t turbo_en;
	uint8_t turbo_dir;			//当前turbo 方向

	uint32_t turbo_key;
	uint32_t turbo_period;
}turbo_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool turbo_init(void);
bool turbo_deinit(void);
void turbo_task(turbo_t *pturbo, uint32_t *pkey);
void turbo_handler(uint32_t period_10us,turbo_t *pturbo, uint32_t *pkey);

#ifdef __cplusplus
}
#endif
#endif





