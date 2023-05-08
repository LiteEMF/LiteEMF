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
#include "utils/emf_utils.h"
#include "api_tick.h"
#include "api_storage.h"
#include "api_transport.h"
#include "api_commander.h"
#include "unit_test.h"
//api
#ifdef HW_UART_MAP
#include "api_uart.h"
#endif
#ifdef HW_ADC_MAP
#include "api_adc.h"
#endif
#ifdef HW_TIMER_MAP
#include "api_timer.h"
#endif
#ifdef HW_PWM_MAP
#include "api_pwm.h"
#endif
#ifdef HW_IIC_MAP
#include "api_iic_host.h"
#endif
#ifdef HW_SPI_HOST_MAP
#include "api_spi_host.h"
#endif

//app

#if APP_KEY_ENABLE
#include "app_key.h"
#endif
#if APP_KM_ENABLE
#include "app_km.h"
#endif
#if APP_GAMEAPD_ENABLE
#include "gamepad/app_gamepad_key.h"
#endif
#if APP_IMU_ENABLE
#include "imu/app_imu.h"
#endif
#if APP_JOYSTICK_ENABLE
#include "app_joystick.h"
#endif
#if APP_BATTERY_ENABLE
#include "app_battery.h"
#endif
#if API_PM_ENABLE
#include "api_pm.h"
#endif
#if APP_RUMBLE_ENABLE
#include "app_rumble.h"
#endif
#if APP_LED_ENABLE
#include "app_led.h"
#endif
#if APP_RGB_ENABLE
#include "rgb/app_rgb.h"
#endif
#if API_NFC_ENABLE
#include "api/nfc/api_nfc.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
#include "adb/app_adb.h"
#endif
#if API_GPS_ENABLE
#include "gps/api_gps.h"
#endif
#if API_GSM_ENABLE
#include "gsm/api_gsm.h"
#endif
#if API_WIFI_ENABLE
#include "wifi/api_wifi.h"
#endif
#if API_SOFT_TIMER_ENABLE
#include "api/api_soft_timer.h"
#endif


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t m_dev_type	= DEV_TYPES_DEFAULT;
uint16_t m_hid_type	= HID_TYPES_DEFAULT;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern void emf_init(void);
extern void emf_handler(uint32_t period_10us);

int  main( int  argc,  char  *argv[])  
{
	static timer_t timer=0;
	uint32_t i;
	logd("main...\n");

	emf_init();
	
	// crc_test();
	// soft_timer_test();
	// hid_desc_parser_test();
	// mem_test();
	// api_commander_test();
	app_rgb_test();

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
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_init(void)
{
	emf_mem_init();

	#ifdef HW_UART_MAP
	api_uart_init(UART_DEBUG_ID);			
	#endif

	api_tick_init();
	api_storage_init();

	#ifdef HW_UART_MAP
	api_uarts_init();				
	#endif
	#ifdef HW_ADC_MAP
	api_adcs_init();
	#endif
	#ifdef HW_TIMER_MAP
	api_timer_init();
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
	api_trp_init();

	#ifndef API_SOFT_TIMER_ENABLE
	soft_timer_init();
	#endif
	#if API_PM_ENABLE
	api_pm_init();
	#endif
	#ifndef API_NFC_ENABLE
	api_nfc_init();
	#endif
	#ifndef API_GPS_ENABLE
	api_gps_init();
	#endif
	#ifndef API_GSM_ENABLE
	api_gsm_init();
	#endif
	#ifndef API_WIFI_ENABLE
	api_wifi_init();
	#endif

	#if APP_BATTERY_ENABLE
	app_battery_init();
	#endif
	#if APP_KEY_ENABLE
	app_key_init();
	#endif
	#if APP_KM_ENABLE
	app_km_init();
	#endif
	#ifndef APP_IMU_ENABLE
	app_imu_init();
	#endif
	#ifndef APP_JOYSTICK_ENABLE
	app_joystick_init();
	#endif
	#if APP_GAMEAPD_ENABLE
	app_gamepad_key_init();
	#endif
	#if APP_RUMBLE_ENABLE
	app_rumble_init();
	#endif
	#if APP_LED_ENABLE
	app_led_init();
	#endif
	#if APP_RGB_ENABLE
	app_rgb_init();
	#endif
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_deinit(void)
{

}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void emf_handler(uint32_t period_10us)
{
	#ifdef HW_ADC_MAP
	api_adc_handler(400);
	#endif
	#ifdef HW_UART_MAP
	api_uart_handler(200);
	#endif

	#ifndef API_SOFT_TIMER_ENABLE
	soft_timer_task(NULL);
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
	app_key_handler(100);
	#endif
	#if APP_KM_ENABLE
	app_km_handler(100);
	#endif
	#ifndef APP_IMU_ENABLE
	app_imu_handler(400);
	#endif
	#ifndef APP_JOYSTICK_ENABLE
	app_joystick_init(400);
	#endif
	#if APP_GAMEAPD_ENABLE
	app_gamepad_key_handler(400);
	#endif

	#if APP_GAMEAPD_ENABLE						//二选一
	app_key_decode_handler(100,m_gamepad_key.key);
	#elif APP_KEY_ENABLE
    app_key_decode_handler(KEY_PERIOD_DEFAULT,m_key_scan);
	#endif

	#if APP_RUMBLE_ENABLE
	app_rumble_handler(64*100);
	#endif
	#if APP_LED_ENABLE
	app_led_handler(APP_LED_SLICE);
	#endif
	#if APP_RGB_ENABLE
	app_rgb_handler(APP_RGB_SLICE*100);
	#endif

	#if API_SOFT_TIMER_ENABLE
	soft_timer_handler(0);
	#endif
	api_storage_handler(API_STORAGE_TIME);

}




