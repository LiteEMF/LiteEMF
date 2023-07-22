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


#ifndef _api_wdt_h
#define _api_wdt_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hal_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef API_WDT_TIME		//wdt timeout time us
#define API_WDT_TIME	4000	
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_wdt_feed(void);
bool api_wdt_init(uint32_t ms);
bool api_wdt_deinit(void);

//hal
void hal_wdt_feed(void);
bool hal_wdt_init(uint32_t ms);
bool hal_wdt_deinit(void);




#ifdef __cplusplus
}
#endif
#endif





