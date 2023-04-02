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
#include "emf_utils.h"
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
#include "api_iic.h"
#endif
#ifdef HW_SPI_MAP
#include "api_spi.h"
#endif

//app

#if APP_KEY_ENABLE
#include "app_key.h"
#endif
#if APP_KM_ENABLE
#include "app_km.h"
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
#if APP_PM_ENABLE
#include "app_pm.h"
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
#if APP_NFC_ENABLE
#include "nfc/app_nfc.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
#include "adb/app_adb.h"
#endif
#if APP_GPS_ENABLE
#include "gps/app_gps.h"
#endif
#if APP_GSM_ENABLE
#include "gsm/app_gsm.h"
#endif
#if APP_WIFI_ENABLE
#include "wifi/app_wifi.h"
#endif
#if APP_SOFT_TIMER_ENABLE
#include "app_soft_timer.h"
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
extern void emf_handler(void);

int  main( int  argc,  char  *argv[])  
{
	static timer_t timer=0;
	uint32_t i;
	logd("main...\n");

	emf_init();
	
	// crc_test();
	// soft_timer_test();
	hid_desc_parser_test();

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
		emf_handler();
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
	api_tick_init();
	api_storage_init();
	
	#ifdef HW_UART_MAP
	api_uarts_init();
	#endif

	#ifdef HW_ADC_MAP
	api_adcs_init();
	#endif
	#if APP_BATTERY_ENABLE
	app_battery_init();
	#endif
	#if APP_PM_ENABLE
	app_pm_init();
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
	#ifdef HW_SPI_MAP
	api_spis_init();
	#endif
	api_trp_init();
	api_command_init();


	#if APP_PM_ENABLE
	app_pm_init();
	#endif

	#if APP_KEY_ENABLE
	app_key_init();
	#endif
	#if APP_KM_ENABLE
	app_km_init();
	#endif
	#if APP_LED_ENABLE
	app_led_init();
	#endif
	#if APP_RUMBLE_ENABLE
	app_rumble_init();
	#endif
	#if APP_SOFT_TIMER_ENABLE
	soft_timer_init();
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
void emf_handler(void)
{
	#ifdef HW_ADC_MAP
	api_adc_handler();
	#endif

	#if APP_SOFT_TIMER_ENABLE
	soft_timer_handler();
	#endif
	#ifdef HW_UART_MAP
	api_uart_handler();
	#endif

	#if APP_BATTERY_ENABLE
	app_battery_handler();
	#endif
	#if APP_KEY_ENABLE
	app_key_handler();
	#endif
	#if APP_KM_ENABLE
	app_km_handler();
	#endif


	#if APP_LED_ENABLE
	app_led_handler();
	#endif
	#if APP_RUMBLE_ENABLE
	app_rumble_handler();
	#endif
	#if APP_SOFT_TIMER_ENABLE
	soft_timer_handler();
	#endif

	api_trp_handler();
	api_command_handler();
	api_storage_handler();
}








