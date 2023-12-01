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
#include "hw_board.h"
#ifdef HW_ADC_MAP
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_adc.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"
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
uint16_t m_adc_val[countof(m_adc_map)];		//滤波后adc值

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

void api_adcs_init(void)
{
	memset(m_adc_val,0,sizeof(m_adc_val));
	hal_adc_init();

	//start adc
	api_adc_scan_task(NULL);
}
void api_adcs_deinit(void)
{
	hal_adc_deinit();  
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_adc_scan_task(void* pa)
{
	uint16_t adc_val;
	uint8_t id;
	for(id=0; id < m_adc_num; id++){
		if(hal_adc_value(id,&adc_val)){
			m_adc_val[id] = adc_val;
		}
	}
	api_adc_start_scan();

	UNUSED_PARAMETER(pa);
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_adc_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_adc_scan_task(NULL);
	}
}
#endif


#endif





