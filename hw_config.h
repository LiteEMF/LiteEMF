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
	/**********************************************************************************/
	#define DEV_TRPS_DEFAULT			(BIT(TR_BLE)|BIT(TR_USBD))				//产品传输层支持
	#define DEV_TYPES_DEFAULT			BIT(DEV_TYPE_HID)
	#define HID_TYPES_DEFAULT			BIT(HID_TYPE_KB)

	/**********************************************************************************/

	#define HELLOW_KEYBOARD						1
	#if HELLOW_KEYBOARD
		#define SW_VERSION                     	0x01
        #define DEFAULT_NAME			       	"hellow_keyboard"
        #define DEFAULT_MODEL					"HKB"
	#else

	#endif
#elif PROJECT_DEMO
	/**********************************************************************************/
	#define DEV_TRPS_DEFAULT			(BIT(TR_BLE)|BIT(TR_USBD))			//产品传输层支持4
	#define DEV_TYPES_DEFAULT			(BIT(DEV_TYPE_HID) | BIT(DEV_TYPE_CDC))
	#define HID_TYPES_DEFAULT			BIT(HID_TYPE_VENDOR)
	/**********************************************************************************/

	#define DEMO1								0
	#define DEMO_WIN32               			1

	#if DEMO1
		#define SW_VERSION                     	0x01
        #define DEFAULT_NAME			       	"demo01"
        #define DEFAULT_MODEL					"DEMO01"
	#elif DEMO_WIN32
		#define SW_VERSION                     	0x01
        #define DEFAULT_NAME			       	"win32"
        #define DEFAULT_MODEL					"win32"

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


#ifndef RF_ENABLE				//rf peripheral 0,INTER_MODULE,EXTERNAL_MODULE
#define RF_ENABLE 			EXTERNAL_MODULE					
#endif
#ifndef RFC_ENABLE
#define RFC_ENABLE 			INTER_MODULE
#endif
#ifndef BLE_ENABLE
#define BLE_ENABLE			INTER_MODULE		
#endif	
#ifndef EDR_ENABLE	
#define EDR_ENABLE			INTER_MODULE
#endif				
#ifndef BLEC_ENABLE				
#define BLEC_ENABLE			INTER_MODULE
#endif			
#ifndef EDRC_ENABLE				
#define EDRC_ENABLE			INTER_MODULE
#endif	

#include "emf_config.h"
#include "hw_board.h"
#ifdef __cplusplus
}
#endif
#endif





