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


#ifndef _api_pwm_h
#define _api_pwm_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hal_pwm.h"
#include "api/api_gpio.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define PWM_FREQ_POS		(0)
#define PWM_FREQ_MASK		0X000FFFFF		/* HZ  max 1M*/
#define PWM_CH_POS			(20)
#define PWM_CH_MASK			0X0FF00000
#define PWM_HL_POS			(28)
#define PWM_HL_MASK			0X10000000
#define PWM_ACTIVE_POS		(29)
#define PWM_ACTIVE_MASK		0X20000000
#define PWM_RES_POS			(30)
#define PWM_RES_MASK		0XC0000000

#ifndef PWM_FREQ_ATT
#define PWM_FREQ_ATT(id)  		(FLD2VAL(PWM_FREQ, m_pwm_map[id].att))
#endif
#ifndef PWM_CH_ATT
#define PWM_CH_ATT(id)  		(FLD2VAL(PWM_CH, m_pwm_map[id].att))
#endif
#ifndef PWM_HL_ATT				//高通道/低通道
#define PWM_HL_ATT(id)  		(FLD2VAL(PWM_HL, m_pwm_map[id].att))
#endif
#ifndef PWM_ACTIVE_ATT			//api layout fix do not fix in hal_pwm
#define PWM_ACTIVE_ATT(id)  	(FLD2VAL(PWM_ACTIVE, m_pwm_map[id].att))
#endif

#ifndef PWM_FREQ_DEFAULT
#define PWM_FREQ_DEFAULT 1000
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern const_t pin_map_t m_pwm_map[];          
extern uint8c_t m_pwm_num;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t api_pwm_get_duty(uint16_t id);
bool api_pwm_set_duty(uint16_t id, uint8_t duty);
bool api_pwm_start(uint16_t id);
bool api_pwm_stop(uint16_t id);
bool api_pwm_init(uint16_t id);
bool api_pwm_deinit(uint16_t id);
void api_pwms_init(void);
void api_pwms_deinit(void);

//hal
bool hal_pwm_set_duty(uint16_t id, uint8_t duty);
bool hal_pwm_init(uint16_t id, uint8_t duty);
bool hal_pwm_deinit(uint16_t id);


#ifdef __cplusplus
}
#endif
#endif





