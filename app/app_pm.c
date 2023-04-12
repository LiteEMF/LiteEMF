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
#if APP_PM_ENABLE
#include  "app/app_pm.h"
#include "api/api_tick.h"
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

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
pm_sta_t m_pm_sta = PM_STA_NORMAL;
timer_t m_pm_sleep_timer;
bool app_pm_key_sleep = 0;			//按键休眠
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static timer_t s_pm_timer;	//修改模式同步时间,保证每reset 和关机都在100ms后处理,防止user_vender_deinit时间不够
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK bool app_pm_sleep_hook(void)
{
	return false;
}
#endif


void app_pm_weakup_check(void)
{
	uint16_t i;

	app_battery_init();
	if(BAT_PROTECT_STA == m_battery_sta) api_sleep();

	// 首次上电池不开机
	#if (APP_BATTERY_ENABLE) && (KEY_POWER_GPIO)
	if((PM_RESON_POR == m_reset_reson) || (PM_RESON_VCM == m_reset_reson)){
		if(KEY_POWER){				//按键开机	
			#if KEY_POWERON_TIME
			for(i=0; i<KEY_POWERON_TIME; i++){
				delay_ms(1);

				if(!KEY_POWER){
					api_sleep();
				}
			}
			#endif
		}else{
			if((BAT_CHARGE_STA == m_battery_sta) || (BAT_CHARGE_DONE_STA == m_battery_sta)){
				app_sleep();
			}else{
				api_sleep();
			}
		}
	}
	#endif

	logi_r("m_pm_sta=%d\n",m_pm_sta);   
}

void app_boot(uint8_t index)
{
	logd("boot %d\n",(uint16_t)index);
	api_boot(index);
}

void app_reset(void)
{
	logd("rest\n");
	m_pm_sta = PM_STA_RESET;
}

void app_sleep(void)
{
    //充电状态下休眠
	if((BAT_CHARGE_STA == m_battery_sta) || (BAT_CHARGE_DONE_STA == m_battery_sta)){
		logd_r("charge notwork mode...\n");
		m_pm_sta = PM_STA_CHARG_NOT_WORK;
		app_pm_deinit();
	}else{									//无充电，直接休眠
		logd_r("sleep...\n");
		m_pm_sta = PM_STA_SLEEP;
		app_pm_deinit();
	}
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_pm_init(void)
{
	return api_pm_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_pm_deinit(void)
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
void app_pm_handler(uint32_t period_10us)
{
	uint8_t i;
	bool dev_connected = false;
	timer_t sleep_timeout;
	#if API_BT_ENABLE
	api_bt_ctb_t* bt_ctbp;
	#endif

	static bool s_dev_connected = false;

	if((m_task_tick10us - s_pm_timer) >= period_10us){
		s_pm_timer = m_task_tick10us;

		#if APP_KEY_ENABLE
		if( m_app_key.key ){
			m_pm_sleep_timer = m_systick;
		}
		if(m_app_stick_key){
			m_pm_sleep_timer = m_systick;
        }
		#endif

		#if API_BT_ENABLE
		api_bt_ctb_t* bt_ctbp;
		for(i = 0; i < BT_MAX; i++){
			bt_ctbp = api_bt_get_ctb(BIT(i));
			if(NULL != bt_ctbp){
				if(BT_STA_CONN <= bt_ctbp->sta){
					dev_connected = true;
					break;
				}
			}
		}
		#endif


		// #if APP_USBD_ENABLE
		// if(BT_STA_READY == m_ble_sta) dev_connected = true;
		// #endif
		// #if APP_USBH_ENABLE
		// if(BT_STA_READY == m_ble_sta) dev_connected = true;
		// #endif

		switch (m_pm_sta){
			case PM_STA_NORMAL:
				if (dev_connected){
					sleep_timeout = CONNECTED_SLEEP_TIME;
				}else{
					sleep_timeout = DISCONNECTED_SLEEP_TIME;
				}

				if(s_dev_connected != dev_connected){
					s_dev_connected = dev_connected;
					m_pm_sleep_timer = m_systick;
				}
				
				if(m_systick - m_pm_sleep_timer > sleep_timeout){
					m_pm_sleep_timer = m_systick;
					logd("m_pm_sleep_timer. %d\n", sleep_timeout);
					app_sleep();	
				}
				if(BAT_PROTECT_STA == m_battery_sta) app_sleep();
				break;
			case PM_STA_CHARG_NOT_WORK:
				if((BAT_CHARGE_STA != m_battery_sta) && (BAT_CHARGE_DONE_STA != m_battery_sta)){
					app_sleep();
				}
				break;
			case PM_STA_RESET:
			case PM_STA_SLEEP:
				#if STORAGE_ENABLED
				api_storage_sync();
				if (!api_storage_sync_complete()) return;
				#endif
				if(app_pm_sleep_hook()) return;
				if(dev_connected) return;				//wait dev disconnect
				
				if(PM_STA_RESET == m_pm_sta)
				{
					logd_r("reset...\n\n");
					api_reset();
				}else{
					#if POWER_SWITCH_KEY
						if(KEY_POWER && app_pm_key_sleep){
							app_reset();
						}
					#elif defined KEY_POWER_GPIO
						if(KEY_POWER){
							return;
						}
					#endif 
					logd_r("sleep...\n\n");
					hal_sleep();
				}
		}

	}
}

#endif






