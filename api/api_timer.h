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


#ifndef _api_timer_h
#define _api_timer_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "hal/hal_timer.h"

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
	uint8_t id;
	uint32_t period;
	uint32_t att;
} timer_map_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_timer_hook(uint8_t id);		//__WEAK
bool api_timer_init(uint8_t id,uint32_t us);
bool api_timer_deinit(uint8_t id);

//hal
bool hal_timer_init(uint8_t id,uint32_t us);
bool hal_timer_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





