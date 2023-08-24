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


#ifndef _emf_h
#define _emf_h
#include "emf_typedef.h" 

#include "hw_config.h"
#include "hw_board.h"

#include "utils/emf_utils.h"
#include "api/api_tick.h"
#include "api/api_transport.h"
#include "api/api_commander.h"
#include "unit-test/unit_test.h"
//api
#ifdef HW_UART_MAP
#include "api/api_uart.h"
#endif
#ifdef HW_ADC_MAP
#include "api/api_adc.h"
#endif
#if	defined HW_TIMER_MAP && API_TIMER_BIT_ENABLE
#include "api/api_timer.h"
#endif
#ifdef HW_PWM_MAP
#include "api/api_pwm.h"
#endif
#ifdef HW_IIC_MAP
#include "api/api_iic_host.h"
#endif
#ifdef HW_SPI_HOST_MAP
#include "api/api_spi_host.h"
#endif

#if API_WDT_ENABLE
#include  "api/api_wdt.h"
#endif
#if API_STORAGE_ENABLE
#include "api/api_storage.h"
#endif

#if API_NFC_ENABLE
#include "api/nfc/api_nfc.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
#include "app/adb/app_adb.h"
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
#if API_SOFT_TIMER_ENABLE
#include "api/api_soft_timer.h"
#endif
// #if API_PM_ENABLE
#include "api/api_pm.h"
// #endif
#if API_USBD_BIT_ENABLE
#include "api/usb/device/usbd.h"
#endif
#if API_USBH_BIT_ENABLE
#include "api/usb/host/usbh.h"
#endif
#if API_OTG_BIT_ENABLE
#include "api/usb/api_otg.h"
#endif
#if API_BT_ENABLE
#include  "api/bt/api_bt.h"
#endif

//app
#if APP_KEY_ENABLE
#include "app/app_key.h"
#endif
#if APP_KM_ENABLE
#include "app/app_km.h"
#endif
#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad_key.h"
#endif
#if APP_IMU_ENABLE
#include "app/imu/app_imu.h"
#endif
#if APP_JOYSTICK_ENABLE
#include "app/app_joystick.h"
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
#include  "app/app_command.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void user_vender_init(void);			//__WEAK
void hw_user_vender_init(void);			//__WEAK			
void user_vender_deinit(void);			//__WEAK
void user_vender_handler(void);			//__WEAK

bool emf_api_init(void);
bool emf_init(void);
bool emf_deinit(void);
void emf_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





