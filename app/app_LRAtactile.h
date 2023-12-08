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


#ifndef _app_lratactile_h
#define _app_lratactile_h
#include "hw_config.h"
#include "emf_typedef.h" 
#include "app/drv2605.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_lra_show(void);						//__WEAK
bool app_lra_init(void);
bool app_lra_deinit(void);
void app_lra_show_task(void *pa);
void app_lra_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





