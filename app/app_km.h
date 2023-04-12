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


#ifndef _app_km_h
#define _app_km_h
#include "emf_typedef.h"
#include "km_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/


typedef struct {
	uint8_t key[8];
	mouse_t mouse;
}km_key_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_km_init(void);
bool app_km_deinit(void);
void app_km_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





