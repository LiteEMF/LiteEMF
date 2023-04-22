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
**	Description:	
************************************************************************************************************/
#include  "hw_config.h"
#if APP_BATTERY_ENABLE

#include  "app/app_battery.h"
#include  "key_typedef.h"
#include  "api/api_tick.h"
#if (ID_NULL != ADC_BATTERY_ID)
#include  "api/api_adc.h"
#endif
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint8_t m_battery = 100;
bat_state_t	m_battery_sta = BAT_NORMAL_STA;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
__WEAK bat_state_t app_battery_sta(bool power_on,uint16_t bat_vol)
{
	bat_state_t	state;
	uint16_t protect_vol;

	if(power_on){				//上电保护电源和工作保护电源不一样
		protect_vol = BAT_POWERON_PROTECT_VOL;
	}else{
		protect_vol = BAT_PROTECT_VOL;
	}

	if (KEY_USB_DET || KEY_CHARGER){
		if ((bat_vol >= 4200) || !KEY_CHARGER){
			state = BAT_CHARGE_DONE_STA;
		}else{
			state = BAT_CHARGE_STA;
		}
	}else{
		if(bat_vol <= protect_vol){
			state = BAT_PROTECT_STA;
		}else if(bat_vol <= BAT_LOW_POWER_VOL){
			state = BAT_LOWPOWER_STA;
		}else{
			state = BAT_NORMAL_STA;
		}
	}

	return state;
}

__WEAK uint16_t app_battery_vol(void)
{
	uint16_t adc,vol=4200;

	#if (ID_NULL != ADC_BATTERY_ID)
	adc = api_adc_value(ADC_BATTERY_ID);
	vol = api_adc_to_voltage(adc)*BAT_R;
	#endif

	return vol;
}

uint8_t app_battery_percent(uint16_t vol)
{
	uint8_t battery;

	if((BAT_CHARGE_DONE_STA == m_battery_sta) || (vol >= 4200)){
		battery = 100;
	}else if(vol <= BAT_PROTECT_VOL){
		battery = 0;
	}else{
		battery = ((vol - BAT_PROTECT_VOL) * 100 / (4200 - BAT_PROTECT_VOL));
	}
	return battery;
}

void app_battery_scan(bool power_on)
{
	uint16_t vol;
	bat_state_t	state;

	static bat_state_t	s_state;

	vol = app_battery_vol();
	state = app_battery_sta(power_on,vol);

	if(s_state != state){
		s_state = state;
	}else{
		m_battery_sta = state;
	}
	m_battery = app_battery_percent(vol);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_battery_init(void)
{
	uint8_t i;
	for(i = 0; i < 6; i++){
		app_battery_scan(false);
		delay_ms(1);
	}
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_battery_deinit(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/

void app_battery_scan_task(void *pa)
{
	app_battery_scan(false);
	UNUSED_PARAMETER(pa);
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_battery_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_battery_scan_task(NULL);
	}
}
#endif



#endif







