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
#include  "app/app_key.h"
#include  "api/api_tick.h"
#if (ID_NC != ADC_BATTERY_ID)
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
__WEAK bat_state_t app_battery_sta(uint16_t bat_vol)
{
	bat_state_t	state;
	if (KEY_USB_DET || KEY_CHARGER){
		if ((bat_vol >= 4200) || !KEY_CHARGER){
			state = BAT_CHARGE_DONE_STA;
		}else{
			state = BAT_CHARGE_STA;
		}
	}else{
		if(bat_vol <= BAT_PROTECT_VOL){
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

	#if (ID_NC != ADC_BATTERY_ID)
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



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_battery_init(void)
{
	int16_t vol;
	vol = app_battery_vol();
	m_battery_sta = app_battery_sta(vol);
	m_battery = app_battery_percent(vol);

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
void app_battery_handler(void)
{
	uint16_t vol;
	bat_state_t	state;

	static bat_state_t	s_state;
	static timer_t battery_timer;

	if ((m_tick - battery_timer) > 200){
    	battery_timer = m_tick;

		vol = app_battery_vol();
		state = app_battery_sta(vol);

		if(s_state != state){
			s_state = state;
		}else{
			m_battery_sta = state;
		}

		m_battery = app_battery_percent(vol);

	}
}



#endif







