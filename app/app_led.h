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
#include "emf_typedef.h" 

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
	uint8_t	turn;
}led_ctb_t;


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_led_show(uint8_t* frame);						//__WEAK
bool app_set_led(uint8_t id, uint8_t period,uint8_t times);
uint8_t app_get_led(uint8_t id);
bool app_led_turn(uint8_t id);
bool app_led_init(void);
bool app_led_deinit(void);
void app_led_show_task(void *pa);
void app_led_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





