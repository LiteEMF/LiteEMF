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


#ifndef _app_battery_h
#define _app_battery_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef BAT_R		//(R1+R2)/R1
#define	BAT_R     	(2/1)
#endif
#ifndef	BAT_PROTECT_VOL					//mv
#define	BAT_PROTECT_VOL     			3350
#endif
#ifndef	BAT_POWERON_PROTECT_VOL
#define	BAT_POWERON_PROTECT_VOL			3400     
#endif
#ifndef	BAT_LOW_POWER_VOL
#define	BAT_LOW_POWER_VOL            	3450
#endif


typedef enum _tbat_stu_t{
	BAT_NORMAL_STA,
	BAT_LOWPOWER_STA,
	BAT_PROTECT_STA,	
	BAT_CHARGE_STA,
	BAT_CHARGE_DONE_STA,
} bat_state_t;


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern uint8_t m_battery;
extern bat_state_t	m_battery_sta;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bat_state_t app_battery_sta(bool power_on,uint16_t bat_vol);		//__WEAK
uint16_t app_battery_vol(void);						//__WEAK
uint8_t app_battery_percent(uint16_t vol);
void app_battery_scan(bool power_on);
bool app_battery_init(void);
bool app_battery_deinit(void);
void app_battery_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif

#endif





