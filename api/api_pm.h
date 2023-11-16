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


#ifndef _api_pm_h
#define _api_pm_h
#include "emf_typedef.h" 
#include "api/api_tick.h"
#include "api/api_storage.h"
#include "hal_pm.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef DISCONNECTED_SLEEP_TIME							//ms
#define DISCONNECTED_SLEEP_TIME    	(3*1000*60UL)
#endif
#ifndef CONNECTED_SLEEP_TIME
#define CONNECTED_SLEEP_TIME    	(15*1000*60UL)
#endif
#ifndef KEY_POWERON_TIME
#define KEY_POWERON_TIME    		(0)
#endif


#define SOFT_RESET_MASK		('r')

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	PM_RESON_POR=0,		//power on reset
	PM_RESON_VCM,		//exten isr weakup reset
	PM_RESON_SOFT,		//soft reset
	PM_RESON_RESET_PIN,	//pin reset
	PM_RESON_WDT,
	PM_RESON_SYS,
}pm_reson_t;




typedef enum{
	PM_STA_NORMAL,
	PM_STA_CHARG_NOT_WORK,
	PM_STA_RESET,
	PM_STA_SLEEP,
}pm_sta_t;

extern pm_reson_t m_reset_reson;
extern pm_sta_t	m_pm_sta;
extern timer_t m_pm_sleep_timer;								//休眠定时器
extern uint32_t m_pm_sleep_timerout;	//休眠超时时间
extern bool app_pm_key_sleep;		

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_pm_sleep_hook(void);				//__WEAK 
bool api_pm_sleep_deinit(void);				//__WEAK
void api_weakup_init(void);					//__WEAK

void api_pm_weakup_check(void);
void api_boot(uint8_t index);
void api_reset(void);
void api_sleep(void);
bool api_pm_init(void);
void api_pm_task(void*pa);
void api_pm_handler(uint32_t period_10us);


//hal 
pm_reson_t hal_get_reset_reson(void);
void hal_boot(uint8_t index);
void hal_reset(void);
void hal_sleep(void);

#ifdef __cplusplus
}
#endif
#endif





