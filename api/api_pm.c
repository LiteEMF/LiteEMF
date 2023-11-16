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
#include "hw_config.h"
#if API_PM_ENABLE
#include "api/api_pm.h"


#ifdef HW_ADC_MAP
#include "api/api_adc.h"
#endif
#if APP_KEY_ENABLE
#include "app/app_key.h"
#endif
#if APP_KM_ENABLE
#include "app/app_km.h"
#endif
#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad_key.h"
#endif
#if APP_BATTERY_ENABLE
#include "app/app_battery.h"
#endif
#if API_BT_ENABLE
#include "api/bt/api_bt.h"
#endif


#include "app/emf.h"
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
pm_sta_t m_pm_sta = PM_STA_NORMAL;
timer_t m_pm_sleep_timer;								//休眠定时器
uint32_t m_pm_sleep_timerout = DISCONNECTED_SLEEP_TIME;	//休眠超时时间
bool app_pm_key_sleep = 0;								//是否是按键进入休眠, 用于波动开关开关机

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static timer_t s_pm_timer;	//修改模式同步时间,保证每reset 和关机都在100ms后处理,防止user_vender_deinit时间不够
/*****************************************************************************************************
**	static Function
******************************************************************************************************/
pm_reson_t m_reset_reson;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE

/*******************************************************************
** Parameters:
** Returns:	true: 继续等待,  false: 操作完成可以用休眠
** Description: 在系统休眠之前, 用于等待特殊用户操作
*******************************************************************/
__WEAK bool api_pm_sleep_hook(void)
{
	return false;
}

__WEAK bool api_pm_sleep_deinit(void)			//api_sleep call
{
	if(PM_STA_SLEEP == m_pm_sta){
		#ifdef HW_ADC_MAP
		api_adcs_deinit();
		#endif
		#if APP_LED_ENABLE
		app_led_deinit();
		#endif
		#if APP_RGB_ENABLE
		app_rgb_deinit();
		#endif
	}

	#if APP_RUMBLE_ENABLE
	app_rumble_deinit();
	#endif
	#if API_BT_ENABLE
	api_bt_enable_all(0);
	#endif
	#if API_STORAGE_ENABLE
	api_storage_sync();
	#endif
	
	user_vender_deinit();

	return true;
}


__WEAK void api_weakup_init(void)			//系统休眠前初始化唤醒IO
{
	#if PIN_NULL != KEY_POWER_GPIO	
	api_gpio_dir(KEY_POWER_GPIO, PIN_IN, POWER_KEY_PULL);
	#endif

	#if PIN_NULL != KEY_USB_DET_GPIO
	api_gpio_dir(KEY_USB_DET_GPIO, PIN_IN, PIN_PULLNONE);
	#endif

	#if !POWER_SWITCH_KEY	//只在非拨动开关情况下做判断
	while(KEY_POWER);
	#endif
}
#endif


void api_pm_weakup_check(void)
{
	uint16_t i;

	#if APP_BATTERY_ENABLE
	app_battery_init();
	if(BAT_PROTECT_STA == m_battery_sta) hal_sleep();
	#endif

	// 首次上电池不开机
	#if (APP_BATTERY_ENABLE) && (KEY_POWER_GPIO)
	if((PM_RESON_POR == m_reset_reson) || (PM_RESON_VCM == m_reset_reson)){
		if(KEY_POWER){				//按键开机	
			#if KEY_POWERON_TIME
			for(i=0; i<KEY_POWERON_TIME; i++){
				delay_ms(1);

				if(!KEY_POWER){
					hal_sleep();
				}
			}
			#endif
		}else{
			if((BAT_CHARGE_STA == m_battery_sta) || (BAT_CHARGE_DONE_STA == m_battery_sta)){
				api_sleep();
			}else{
				hal_sleep();
			}
		}
	}
	#endif

	logi_r("m_pm_sta=%d\n",m_pm_sta);   
}

/*******************************************************************
** Parameters:index : 1: first boot, 2 : second boot		
** Returns:	
** Description:
*******************************************************************/
void api_boot(uint8_t index)
{
	logd("api boot %d\n",(uint16_t)index);
	hal_boot(index);
}

void api_reset(void)
{
	logd("api rest\n");
	m_pm_sta = PM_STA_RESET;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	used in SDK befor stop 
*******************************************************************/


void api_sleep(void)
{
    //充电状态下休眠
	s_pm_timer = m_systick;

	#if APP_BATTERY_ENABLE
	if((BAT_CHARGE_STA == m_battery_sta) || (BAT_CHARGE_DONE_STA == m_battery_sta)){
		logd_r("charge notwork mode...\n");
		m_pm_sta = PM_STA_CHARG_NOT_WORK;
	}
	else
	#endif
	{									//无充电，直接休眠
		logd_r("api sleep...\n");
		m_pm_sta = PM_STA_SLEEP;
	}

	api_pm_sleep_deinit();
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_pm_init(void)
{
	m_pm_sleep_timer = m_systick;
	m_pm_sleep_timerout = DISCONNECTED_SLEEP_TIME;
	app_pm_key_sleep = 0;		
	
	m_reset_reson = hal_get_reset_reson();
	
	#if API_STORAGE_ENABLE
	if(SOFT_RESET_MASK == m_storage.reset_reson){
		m_reset_reson = PM_RESON_SOFT;
	}
	#endif

    return true;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_pm_task(void*pa)
{
	switch (m_pm_sta){
	case PM_STA_NORMAL:
		if(m_systick - m_pm_sleep_timer > m_pm_sleep_timerout){		//超时休眠
			m_pm_sleep_timer = m_systick;
			logd("m_pm_sleep_timer. %d\n", m_pm_sleep_timerout);
			api_sleep();	
		}

		#if APP_BATTERY_ENABLE
		if(BAT_PROTECT_STA == m_battery_sta) api_sleep();		//低电保护休眠
		#endif

		break;
	case PM_STA_CHARG_NOT_WORK:
		#if APP_BATTERY_ENABLE
		if((BAT_CHARGE_STA != m_battery_sta) && (BAT_CHARGE_DONE_STA != m_battery_sta)){
			api_sleep();
		}
		#endif

		break;
	case PM_STA_RESET:
	case PM_STA_SLEEP:
		if(api_pm_sleep_hook()) return;

		#if API_STORAGE_ENABLE
		api_storage_sync();
		if (!api_storage_sync_complete()) return;
		#endif


		#if API_BT_ENABLE
		if(1){		//蓝牙需要正常断开连接
			uint8_t id;
			api_bt_ctb_t* bt_ctbp;

			for(id = 0; id < BT_MAX; id++){
				if(api_bt_is_connected(id)){
					return;				
				}
			}
		}
		#endif

		if(PM_STA_RESET == m_pm_sta)
		{
			logd_r("reset...\n\n");
			#if API_STORAGE_ENABLE
			m_storage.reset_reson = SOFT_RESET_MASK;
			#endif

			hal_reset();
		}else{
			#if POWER_SWITCH_KEY
			if(KEY_POWER && app_pm_key_sleep){
				api_reset();
			}
			#elif defined KEY_POWER_GPIO
			if(KEY_POWER){
				return;
			}
			#endif 
			logd_r("sleep...\n\n");
			hal_sleep();
		}
		break;
	}
	UNUSED_PARAMETER(pa);
}



#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_pm_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_pm_task(NULL);
	}
}
#endif

#endif






