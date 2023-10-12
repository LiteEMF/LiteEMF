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
#else						//used driver ic or other
	#ifndef HW_LED_NUM		//for default
	#define HW_LED_NUM	3
	#endif
	uint8c_t m_led_num = HW_LED_NUM;
	uint8c_t m_led_active_map[HW_LED_NUM];
	led_ctb_t led_ctb[HW_LED_NUM];
	uint8_t led_frame[HW_LED_NUM];
#endif


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


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
	
/*******************************************************************
** Parameters:	id: led id	
				period: led period as LED_OFF,LED_FAST,LED_SLOW,LED_ON
				times: LED_FOREVER or no zero
** Returns:	
** Description:		
*******************************************************************/
bool app_set_led(uint8_t id, uint8_t period,uint8_t times)
{
	uint8_t i;
	if (id >= m_led_num) return false;

	
	if(times){			//设置第默认灭灯,保证完整的闪烁周期
		led_ctb[id].turn = 0;
	}else if((led_ctb[id].period != period) && (period != LED_ON) && (period != LED_OFF)){		//sync
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


void app_led_show_task(void *pa)
{	
	static uint32_t	led_tick=0;		// APP_LED_SLICE
	int8_t i = 0;

	led_tick++;
	for(i=0; i< m_led_num; i++){
		//show 
		if(LED_ON == led_ctb[i].period){
			led_frame[i] = true;
		}else if(LED_OFF == led_ctb[i].period){
			led_frame[i] = false;
		}else {		//blink
			if (0 == (led_tick % led_ctb[i].period)){
				led_ctb[i].turn = !led_ctb[i].turn;	

				if(led_ctb[i].turn && led_ctb[i].times) {
					led_ctb[i].times--;
					if(0 == led_ctb[i].times){
						led_ctb[i].period = LED_OFF;
					}
				}
			}
		
			#ifdef HW_LED_MAP
			led_frame[i] = !(led_ctb[i].turn ^ m_led_active_map[i]);
			#else
			led_frame[i] = led_ctb[i].turn;
			#endif
		}
	}	

	app_led_show(led_frame);
}	



#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_led_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_led_show_task(NULL);
	}
}
#endif

#endif






