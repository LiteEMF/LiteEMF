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


#ifndef _api_adc_h
#define _api_adc_h
#include "hw_config.h"
#include "utils/emf_typedef.h" 
#include "hal_adc.h"
#include "api/api_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef ADC_REF_MV
#define ADC_REF_MV		3200UL
#endif
#ifndef ADC_RES_MAX
#define ADC_RES_MAX		4095	
#endif

#ifndef ADC_BATTERY_ID
#define ADC_BATTERY_ID		ID_NULL
#endif
#ifndef ADC_LX_ID			
#define ADC_LX_ID			ID_NULL
#endif
#ifndef ADC_LY_ID	
#define ADC_LY_ID			ID_NULL
#endif
#ifndef ADC_RX_ID	
#define ADC_RX_ID			ID_NULL
#endif
#ifndef ADC_RY_ID	
#define ADC_RY_ID			ID_NULL
#endif
#ifndef ADC_L2_ID	
#define ADC_L2_ID			ID_NULL
#endif
#ifndef ADC_R2_ID	
#define ADC_R2_ID			ID_NULL
#endif
#ifndef ADC_EAR_DET_ID	
#define ADC_EAR_DET_ID		ID_NULL
#endif

#define ADC_CHANNEL_POS		(0)
#define ADC_CHANNEL_MASK	0X0000FFFF
#define ADC_PULL_POS		(16)
#define ADC_PULL_MASK		0X00FF0000

#ifndef ADC_CHANNEL_ATT			
#define ADC_CHANNEL_ATT(id)  	(FLD2VAL(ADC_CHANNEL, m_adc_map[id].att))
#endif
#ifndef ADC_PULL_ATT
#define ADC_PULL_ATT(id)  		(FLD2VAL(ADC_PULL, m_adc_map[id].att))
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

extern const_t pin_map_t m_adc_map[];          
extern uint8c_t m_adc_num;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t api_adc_to_voltage(uint16_t adc);
uint16_t api_adc_value(uint8_t id);
bool api_adc_start_scan(void);
void api_adcs_init(void);
void api_adcs_deinit(void);
void api_adc_scan_task(void* pa);
void api_adc_handler(uint32_t period_10us);

//hal
uint16_t hal_adc_to_voltage(uint16_t adc);
bool hal_adc_value(uint8_t id, uint16_t* valp);
bool hal_adc_start_scan(void);
bool hal_adc_init(void);
bool hal_adc_deinit(void);
#ifdef __cplusplus
}
#endif
#endif





