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


#ifndef _hw_config_h
#define _hw_config_h

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define LOG_ENABLE                      1

#define PROJECT_KM                  0           //keyboard and mouse project
#define PROJECT_DEMO                1           //keyboard and mouse

#if PROJECT_KM
	#define HELLOW_KEYBOARD						1

	#if HELLOW_KEYBOARD
		#define DEV_TRPS_DEFAULT				(BIT(TR_BLE)|BIT(TR_USBD))		/*产品传输层支持*/
		#define SW_VERSION                     	0x0100
        #define DEFAULT_NAME			       	"hellow_keyboard"
        #define DEFAULT_MODEL					"HKB"
	#else

	#endif
#elif PROJECT_DEMO

	#define DEMO1								0
	#define DEMO_WIN32               			1

	#if DEMO1
		#define DEV_TRPS_DEFAULT				(BIT(TR_BLE)|BIT(TR_USBD))		/*产品传输层支持*/
		#define SW_VERSION                     	0x0100
        #define DEFAULT_NAME			       	"demo01"
        #define DEFAULT_MODEL					"DEMO01"
	#elif DEMO_WIN32
		#define DEV_TRPS_DEFAULT				(BIT(TR_BLE)|BIT(TR_USBD))		/*产品传输层支持*/
		#define SW_VERSION                     	0x0100
        #define DEFAULT_NAME			       	"win32"
        #define DEFAULT_MODEL					"win32"


		#define APP_USBD_ENABLE			1
		#define APP_USBH_ENABLE			1

		#define USBH_TYPE_SUPPORT		0XFFFF
		#define USBD_TYPE_SUPPORT		0XFFFF
		#define USBD_HID_SUPPORT		0XFFFF
		#define USBH_HID_SUPPORT        0XFFFF

		//for test 	desc parser
		#define HIDH_SUPPORT			0XFFFF
		#define HIDD_SUPPORT			0XFFFF	
		#define AOA_HID_SUPPORT			0xFFFF
	#else

	#endif
#endif

#ifndef BT0_SUPPORT
#define BT0_SUPPORT 		(BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF) | BIT_ENUM(TR_EDR) | BIT_ENUM(TR_RF))
#endif
#ifndef BT1_SUPPORT
#define BT1_SUPPORT 		(BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC) | BIT_ENUM(TR_EDRC) | BIT_ENUM(TR_RFC))
#endif


#include "emf_config.h"
#include "hw_board.h"
#ifdef __cplusplus
}
#endif
#endif





