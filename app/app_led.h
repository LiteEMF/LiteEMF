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


#ifndef _app_led_h
#define _app_led_h
#include "hw_config.h"
#include "emf_typedef.h" 
#include "api/api_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef		APP_LED_SLICE
#define 	APP_LED_SLICE	(50*100)	/*10us 单位*/
#endif

#define LED_OFF		0
#define LED_ON		0XFF		
#ifndef	LED_FAST
#define LED_FAST	2			/* 单位 APP_LED_SLICE*2 */
#endif
#ifndef	LED_SLOW
#define LED_SLOW	12			/* 单位 APP_LED_SLICE*2 */
#endif

#define LED_FOREVER	0			/* times */



typedef struct{
	uint8_t	period;
    uint8_t	times;
	uint8_t	times_tick;
	uint8_t	turn;
}led_ctb_t;


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
#if APP_LED_ENABLE
extern const_t pin_t m_led_map[];
extern uint8c_t m_led_num;
#endif

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_led_show(uint8_t* frame);						//__WEAK
void app_led_finished_cb(uint8_t id);					//__WEAK
bool app_set_led(uint8_t id, uint8_t period,uint8_t times);
uint8_t app_get_led(uint8_t id);
uint8_t app_get_led_times(uint8_t id);
bool app_led_turn(uint8_t id);
bool app_led_init(void);
bool app_led_deinit(void);
void app_led_show_task(void *pa);
void app_led_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





