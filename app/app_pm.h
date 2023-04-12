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


#ifndef _app_pm_h
#define _app_pm_h
#include "emf_typedef.h"
#include "api/api_pm.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef DISCONNECTED_SLEEP_TIME
#define DISCONNECTED_SLEEP_TIME    	(3*1000*60UL)		//ms
#endif
#ifndef CONNECTED_SLEEP_TIME
#define CONNECTED_SLEEP_TIME    	(15*1000*60UL)		//ms
#endif
#ifndef KEY_POWERON_TIME
#define KEY_POWERON_TIME    		(0)					//ms
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum _tgpad_sta{
	PM_STA_NORMAL,
	PM_STA_CHARG_NOT_WORK,
	PM_STA_RESET,
	PM_STA_SLEEP,
}pm_sta_t;

extern pm_sta_t	m_pm_sta;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_pm_sleep_hook(void);				//__WEAK 
void app_pm_weakup_check(void);
void app_boot(uint8_t index);
void app_reset(void);
void app_sleep(void);
bool app_pm_init(void);
bool app_pm_deinit(void);
void app_pm_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





