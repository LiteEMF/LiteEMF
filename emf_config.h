/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/


#ifndef _emf_config_h
#define _emf_config_h

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define ID_NC   (0xff)

//*********************************************************************************//
//                       	APP modules                  						   //
//*********************************************************************************//
#ifndef APP_KEY_ENABLE
#define APP_KEY_ENABLE			1
#endif
#ifndef APP_KM_ENABLE
#define APP_KM_ENABLE			1
#endif
#ifndef APP_IMU_ENABLE
#define APP_IMU_ENABLE			1
#endif
#ifndef APP_JOYSTICK_ENABLE
#define APP_JOYSTICK_ENABLE		1
#endif
#ifndef APP_BATTERY_ENABLE
#define APP_BATTERY_ENABLE		1
#endif
#ifndef APP_RUMBLE_ENABLE
#define APP_RUMBLE_ENABLE		1
#endif
#ifndef APP_LED_ENABLE
#define APP_LED_ENABLE			1
#endif
#ifndef APP_RGB_ENABLE
#define APP_RGB_ENABLE			1
#endif
#ifndef APP_NFC_ENABLE
#define APP_NFC_ENABLE			1
#endif
#ifndef APP_GPS_ENABLE
#define APP_GPS_ENABLE			1
#endif
#ifndef APP_GSM_ENABLE
#define APP_GSM_ENABLE			1
#endif
#ifndef APP_WIFI_ENABLE
#define APP_WIFI_ENABLE			1
#endif



#ifdef __cplusplus
}
#endif
#endif





