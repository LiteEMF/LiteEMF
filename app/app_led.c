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
#if APP_LED_ENABLE

#include  "app/app_led.h"
#include  "api/api_gpio.h"
#include  "api/api_tick.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
#ifdef HW_LED_MAP				//used io led
	const_t pin_t m_led_map[] = HW_LED_MAP;
	uint8c_t m_led_num = countof(m_led_map);
	uint8c_t m_led_active_map[countof(m_led_map)] = HW_LED_ACTIVE_MAP;
	led_ctb_t led_ctb[countof(m_led_map)];
	uint8_t led_frame[countof(m_led_map)];
#else						//not used io led
	#ifndef HW_LED_NUM		//for default
	#define HW_LED_NUM	3
	#endif
	uint8c_t m_led_num = HW_LED_NUM;
	led_ctb_t led_ctb[HW_LED_NUM];
	uint8_t led_frame[HW_LED_NUM];
#endif


/*****************************************************************************************************
**	static Function
******************************************************************************************************/
static void show_led(void)
{	
	static uint32_t	led_tick=0;		//50ms
	int8_t i = 0;

	led_tick++;
	
	for(i=0; i< m_led_num; i++){
		if(0 != led_ctb[i].period){
			if (0 == (led_tick % led_ctb[i].period)){
				led_ctb[i].turn = !led_ctb[i].turn;	
				if(led_ctb[i].turn && led_ctb[i].times) {
					led_ctb[i].times--;
					if(0 == led_ctb[i].times){
						led_ctb[i].period = LED_OFF;
					}
				}
			}
		}
		
		//show 
		if(LED_ON == led_ctb[i].period){
			led_frame[i] = true;
		}else if(LED_OFF == led_ctb[i].period){
			led_frame[i] = false;
		}else {
			#ifdef HW_LED_MAP
			led_frame[i] = !(led_ctb[i].turn ^ m_led_active_map[i]);
			#else
			led_frame[i] = led_ctb[i].turn;
			#endif
		}
	}	

	app_led_show(led_frame);
}	

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK bool app_led_show(uint8_t* frame)
{
	bool ret = true;
	uint8_t id;

	for (id = 0; id < m_led_num; id++){
		#ifdef HW_LED_MAP
		api_gpio_out(m_led_map[id],  frame[id]);
		#endif
	}

	return ret;
}
#endif


bool app_set_led(uint8_t id, uint8_t period,uint8_t times)
{
	uint8_t i;
	if (id >= m_led_num) return false;

	//sync
	if((led_ctb[id].period != period) && (period != LED_ON) && (period != LED_OFF)){
		for(i=0; i< m_led_num; i++){
			if(i == id) continue;
			if (period == led_ctb[i].period){
				led_ctb[id].turn = led_ctb[i].turn;
			}
		}
	}

	led_ctb[id].times = times? (times+1) : 0;
	led_ctb[id].period = period;

	return true;
}


uint8_t app_get_led(uint8_t id)
{
	if (id < m_led_num){
		return led_ctb[id].period;
	}
	return LED_OFF;
}

bool app_led_turn(uint8_t id)
{
	if (id < m_led_num){
		led_ctb[id].turn = !led_ctb[id].turn;
		return true;
	}
	return false;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_led_init(void)
{
	return app_led_deinit();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_led_deinit(void)
{
	uint8d_t i=0;
	memset(&led_ctb,0,sizeof(led_ctb));
	memset(&led_frame,0,sizeof(led_frame));
	#ifdef HW_LED_MAP
	for(i=0; i< m_led_num; i++){
		api_gpio_out(m_led_map[i], !m_led_active_map[i]);
		api_gpio_dir(m_led_map[i], PIN_OUT, 0);
	}
	#endif

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_led_handler(uint32_t period_10us)
{
	static timer_t	led_timer;
	if ((m_task_tick10us - led_timer) >= period_10us){
		led_timer = m_task_tick10us;
		show_led();
	}
}

#endif






