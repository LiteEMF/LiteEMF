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
#ifdef HW_PWM_MAP

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_pwm.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/






/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t pin_map_t m_pwm_map[] = HW_PWM_MAP;          
uint8c_t m_pwm_num = countof(m_pwm_map);

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t pwm_duty[countof(m_pwm_map)];          //0~255

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_pwm_set_duty(uint16_t id, uint8_t duty)
{
	bool ret = false;

    if(id >= m_pwm_num) return ret;
    if((pin_t)PIN_NULL == m_pwm_map[id].pin) return false;

	pwm_duty[id] = duty;
	if(0 == PWM_ACTIVE_ATT(id)) {
		duty = 0xff - pwm_duty[id];
	}

    hal_pwm_set_duty(id, duty);

    return true;
}


uint8_t api_pwm_get_duty(uint16_t id)
{
    if(id >= m_pwm_num) return 0;
    return pwm_duty[id];
}

bool api_pwm_start(uint16_t id)
{
	bool ret = false;
    if(id >= m_pwm_num) return ret;
    if((pin_t)PIN_NULL == m_pwm_map[id].pin) return false;

    api_pwm_set_duty(id, pwm_duty[id]);
    return true;
}

bool api_pwm_stop(uint16_t id)
{
	bool ret = false;

    if(id >= m_pwm_num) return ret;
    if((pin_t)PIN_NULL == m_pwm_map[id].pin) return false;

    api_pwm_set_duty(id, 0);
    return true;
}

bool api_pwm_init(uint16_t id)
{
	bool ret = false;
    uint8_t duty = 0;
	
    if(id >= m_pwm_num) return ret;
	pwm_duty[id] = 0;

	if((pin_t)PIN_NULL == m_pwm_map[id].pin) return false;

    if(0 == PWM_ACTIVE_ATT(id)) {
		duty = 0xff - pwm_duty[id];
	}
    hal_pwm_init(id,duty);
    return true;
}
bool api_pwm_deinit(uint16_t id)
{
	bool ret = false;
	if(id >= m_pwm_num) return ret;
    if((pin_t)PIN_NULL == m_pwm_map[id].pin) return false;

    hal_pwm_deinit(id);
    return true;
}
void api_pwms_init(void)
{
	uint8_t id;
	for(id=0; id<m_pwm_num; id++){
		api_pwm_init(id);
	}   
}
void api_pwms_deinit(void)
{
	uint8_t id;
	for(id=0; id<m_pwm_num; id++){
		api_pwm_deinit(id);
	}   
}


#endif






