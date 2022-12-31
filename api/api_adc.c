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

#include "hw_config.h"
#ifdef HW_ADC_MAP
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_adc.h"
#include  "api/api_tick.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t pin_map_t m_adc_map[] = HW_ADC_MAP;          
uint8c_t m_adc_num = countof(m_adc_map);
uint16_t m_adc_val[countof(m_adc_map)];

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		单位mv
*******************************************************************/
uint16_t api_adc_to_voltage(uint16_t adc)
{
    return hal_adc_to_voltage(adc);
}

uint16_t api_adc_value(uint8_t id)
{
    if(id < m_adc_num){
    	return m_adc_val[id];
	}
    return 0;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	used for isr scan, do nothing if not support
*******************************************************************/
bool api_adc_start_scan(void)
{
    return hal_adc_start_scan();
}
bool api_adc_init(uint8_t id)
{
	return hal_adc_init(id);
}

bool api_adc_deinit(uint8_t id)
{
	return hal_adc_deinit(id);
}
void api_adcs_init(void)
{
	uint8_t id;
	uint16_t adc_val;
	memset(m_adc_val,0,sizeof(m_adc_val));
	for(id=0; id < m_adc_num; id++){
		api_adc_init(id);
	}   


	//get val
	for(id=0; id < m_adc_num; id++){
		if(hal_adc_value(id,&adc_val)){
			m_adc_val[id] = adc_val;
		}
	}
	api_adc_start_scan();
}
void api_adcs_deinit(void)
{
	uint8_t id;
	for(id=0; id<m_adc_num; id++){
		api_adc_deinit(id);
	}   
}
void api_adc_handler(void)
{
	uint8_t id;
	uint16_t adc_val;
	static timer_t adc_timer=0;
	if((m_task_tick10us - adc_timer) >= (API_ADC_SCAN_TIME*100)){
        adc_timer = m_task_tick10us;

		for(id=0; id < m_adc_num; id++){
			if(hal_adc_value(id,&adc_val)){
				m_adc_val[id] = adc_val;
			}
		}
		api_adc_start_scan();
	}
}

#endif





