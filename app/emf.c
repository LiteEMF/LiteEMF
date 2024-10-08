/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "hw_config.h"
#include "app/emf.h"
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK void hw_user_vender_init(void)
{

}
__WEAK void user_vender_init(void)	
{

}
__WEAK void user_vender_deinit(void)			//关机前deinit
{
}

__WEAK void user_vender_handler(uint32_t period_10us)
{
}
#endif

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool emf_api_init(void)
{
	logi_g("emf_api_init,mode:%s\n",DEFAULT_MODEL);
	// emf_mem_init();
	// #ifdef HW_UART_MAP
	// api_uart_init(UART_DEBUG_ID);			
	// #endif

	#if !API_PM_ENABLE
	hal_get_reset_reson();
	#endif


	api_tick_init();
	io_key_init();		//按键上电立即初始化
	#ifdef HW_ADC_MAP
	api_adcs_init();	//ADC上电初始化,检测按键和电池
	#endif
	
	#if API_STORAGE_ENABLE
	api_storage_init();	//存储上电初始化
	#endif

	api_trp_init();		//在storage init 之后初始化 设备模式
	
	#if API_BT_ENABLE	//为了方便项目初始化蓝牙参数,将蓝牙参数初始化放到最前面
	api_bt_ctb_init();
    #endif

	#if API_PM_ENABLE
	api_pm_init();
	#endif
	
	hw_user_vender_init();	

	#if API_WDT_ENABLE
	api_wdt_init(API_WDT_TIME);
	#endif

	#if	defined HW_TIMER_MAP && API_TIMER_BIT_ENABLE
	api_timers_init();
	#endif
	#ifndef API_OS_TIMER_ENABLE
	api_os_timer_init();
	#endif
	#ifdef HW_PWM_MAP
	api_pwms_init();
	#endif
	#ifdef HW_IIC_MAP
	api_iics_init();
	#endif
	#ifdef HW_SPI_HOST_MAP
	api_spis_init();
	#endif
	

	#if APP_BATTERY_ENABLE		//special
	app_battery_init();
	#endif

	#ifdef HW_UART_MAP
	api_uarts_init();				
	#endif

	#if API_NFC_ENABLE
	api_nfc_init();
	#endif
	#if API_GPS_ENABLE
	api_gps_init();
	#endif
	#if API_GSM_ENABLE
	api_gsm_init();
	#endif
	#if API_WIFI_ENABLE
	api_wifi_init();
	#endif
	#if API_BT_ENABLE
    api_bt_init();
    #endif

	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool emf_init(void)
{
	logd("emf_init\n");

	#if APP_KEY_ENABLE
	app_key_init();
	#endif
	#if APP_KM_ENABLE
	app_km_init();
	#endif
	#if APP_IMU_ENABLE
	app_imu_init();
	#endif
	#if APP_JOYSTICK_ENABLE
	app_joystick_init();
	#endif
	#if APP_GAMEPAD_ENABLE
	app_gamepad_key_init();
	#endif
	#if APP_RUMBLE_ENABLE
	app_rumble_init();
	#endif
	#if APP_INFR_ENABLE
	app_infr_init(INFR_PWM_ID1,INFR_PWM_ID2);
	#endif
	#if APP_LRA_ENABLE
	app_lra_init();
	#endif
	#if APP_LED_ENABLE
	app_led_init();
	#endif
	#if APP_RGB_ENABLE
	app_rgb_init();
	#endif
	#if APP_WS2812FX_ENABLE
//	app_rgb_init();
	#endif
	
	#if API_OTG_BIT_ENABLE
		api_otgs_init();
	#else
		#if API_USBD_BIT_ENABLE
		usbds_init();
		#endif
		#if API_USBH_BIT_ENABLE
		usbhs_init();
		#endif
	#endif

	user_vender_init();

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool emf_deinit(void)
{

	return true;
}





/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_handler(uint32_t period_10us)
{
	static timer_t live_timer = 0;
	
	UNUSED_PARAMETER(period_10us);
	
	if( m_systick - live_timer >= 30000){
		live_timer = m_systick;
		logd("t=%ld %ld\n", m_systick,m_task_tick10us);
	}
	#ifdef HW_ADC_MAP
	api_adc_handler(period_10us);				//13us at JL240M
	#endif
	
	#if defined HW_UART_MAP && UART_TX_FIFO_ENABLED
	api_uart_tx_handler(200);
	#endif

	#if API_PM_ENABLE
	api_pm_handler(100*100);
	#endif
	#ifndef API_NFC_ENABLE
	api_nfc_handler(10*100);
	#endif
	#ifndef API_GPS_ENABLE
	api_gps_handler(100);
	#endif
	#ifndef API_GSM_ENABLE
	api_gsm_handler(100);
	#endif
	#ifndef API_WIFI_ENABLE
	api_wifi_handler(100);
	#endif

	#if APP_BATTERY_ENABLE
	app_battery_handler(200*100);
	#endif
	#if APP_KEY_ENABLE
	app_key_handler(period_10us);			//57us at JL240M
	#endif
	#if APP_KM_ENABLE
	app_km_handler(100);
	#endif
	#if APP_JOYSTICK_ENABLE
	app_joystick_handler(period_10us);		//30us at JL240M
	#endif
	#if APP_GAMEPAD_ENABLE
	app_gamepad_key_handler(period_10us);	//12us at JL240M
	#endif
	#if APP_IMU_ENABLE
	app_imu_handler(400);
	#endif
	#if APP_IMU_ENABLE
	imu_driver_handler(800);					//内部有延时,建议在低优先级任务中运行
	#endif
	#if APP_GAMEPAD_ENABLE && APP_KEY_ENABLE	 //二选一 //11us at JL240M
	app_key_decode_handler(KEY_PERIOD_DEFAULT*100,m_gamepad_key.key);
	#elif APP_KEY_ENABLE
    app_key_decode_handler(KEY_PERIOD_DEFAULT*100,m_key_scan);
	#endif

	#if APP_RUMBLE_ENABLE
	app_rumble_handler(64*100);
	#endif
	#if APP_INFR_ENABLE
	app_infr_handler(1*100);
	#endif
	#if APP_LRA_ENABLE
	app_lra_handler(5000*100);
	#endif
	#if APP_LED_ENABLE
	app_led_handler(APP_LED_SLICE);
	#endif
	#if APP_RGB_ENABLE
	app_rgb_handler(APP_RGB_SLICE*100);
	#endif
	#if API_OS_TIMER_ENABLE
		#if API_OS_SOFT_TIMER_ENABLE
		api_os_timer_handler(0);
		#endif
	#endif
	#if API_STORAGE_ENABLE
	api_storage_handler(API_STORAGE_TIME);
	#endif

	#if API_USBD_BIT_ENABLE
	usbd_handler(0);		//23us at JL240M
	#endif
	#if API_USBH_BIT_ENABLE
	usbh_handler(period_10us);
	#endif

	#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO))
	usbd_auto_handler(0, 100);
	#endif

	user_vender_handler(period_10us);
}

#if 0
int  main( int  argc,  char  *argv[])  
{
	static timer_t timer=0;
	uint32_t i;
	logd("main...\n");

	emf_mem_init();
	#ifdef HW_UART_MAP
	api_uart_init(UART_DEBUG_ID);			
	#endif

	emf_api_init();
	emf_init();
	
	// crc_test();
	// soft_timer_test();
	// hid_desc_parser_test();
	// mem_test();
	// api_commander_test();
	// app_rgb_test();

	while(0){
		i=0x400000;
		while(i--);
		m_systick++;
		if(m_systick - timer >= 1000){
			timer = m_systick;
			logd("t=%d\n", m_systick);
		}
		if(m_systick > 10 * 1000){
			break;
		}
		emf_handler(0);
	}

	return 0;
}
#endif

