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
#include "api/api_storage.h"

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
#if APP_RUMBLE_ENABLE
#include "app/app_rumble.h"
#endif
#if APP_LED_ENABLE
#include "app/app_led.h"
#endif
#if APP_RGB_ENABLE
#include "app/rgb/app_rgb.h"
#endif
#if API_NFC_ENABLE
#include "api/nfc/api_nfc.h"
#endif
#if API_GPS_ENABLE
#include "api/gps/api_gps.h"
#endif
#if API_GSM_ENABLE
#include "api/gsm/api_gsm.h"
#endif
#if API_WIFI_ENABLE
#include "api/wifi/api_wifi.h"
#endif
#if APP_USB_ENABLE
#include "api/usb/device/usbd.h"
#endif
#if API_BT_ENABLE
#include "api/bt/api_bt.h"
#endif


#include "emf.h"
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define SOFT_RESET_MASK		('r')

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
pm_sta_t m_pm_sta = PM_STA_NORMAL;
timer_t m_pm_sleep_timer;								//休眠定时器
uint32_t m_pm_sleep_timerout = DISCONNECTED_SLEEP_TIME;	//休眠超时时间
bool app_pm_key_sleep = 0;								//是否是按键进入休眠

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
__WEAK bool api_pm_sleep_hook(void)
{
	return false;
}
#endif


void api_pm_weakup_check(void)
{
	uint16_t i;

	app_battery_init();
	if(BAT_PROTECT_STA == m_battery_sta) hal_sleep();

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

void api_boot(uint8_t index)
{
	logd("boot %d\n",(uint16_t)index);
	hal_boot(index);
}

void api_reset(void)
{
	logd("rest\n");
	m_pm_sta = PM_STA_RESET;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	used in SDK befor stop 
*******************************************************************/
void api_weakup_init(void)
{
   hal_weakup_init();
}

void api_sleep(void)
{
    //充电状态下休眠
	if((BAT_CHARGE_STA == m_battery_sta) || (BAT_CHARGE_DONE_STA == m_battery_sta)){
		logd_r("charge notwork mode...\n");
		m_pm_sta = PM_STA_CHARG_NOT_WORK;
		api_pm_deinit();
	}else{									//无充电，直接休眠
		logd_r("sleep...\n");
		m_pm_sta = PM_STA_SLEEP;
		api_pm_deinit();
	}
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
	if(SOFT_RESET_MASK == m_storage.reset_reson){
		m_reset_reson = PM_RESON_SOFT;
	}
    return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_pm_deinit(void)
{
	s_pm_timer = m_systick;

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
	api_storage_sync();

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

		if(BAT_PROTECT_STA == m_battery_sta) api_sleep();		//低电保护休眠
		break;
	case PM_STA_CHARG_NOT_WORK:
		if((BAT_CHARGE_STA != m_battery_sta) && (BAT_CHARGE_DONE_STA != m_battery_sta)){
			api_sleep();
		}
		break;
	case PM_STA_RESET:
	case PM_STA_SLEEP:
		if(api_pm_sleep_hook()) return;

		#if STORAGE_ENABLED
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
			m_storage.reset_reson = SOFT_RESET_MASK;
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






