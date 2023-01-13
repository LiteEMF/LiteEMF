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
	#define   PRODUCT_MODE				MODE_KM								//产品模式
	#define   PRODUCT_MODE_SUPPORT		BIT(MODE_KM)						//支持的产品模式
	#define   PRODUCT_SUB_MODE			0									//产品子模式
	#define   PRODUCT_TRS				(BIT(TR_BLE)|BIT(TR_USBD))			//产品传输层支持
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
	#define   PRODUCT_MODE				MODE_KM								//产品模式
	#define   PRODUCT_MODE_SUPPORT		BIT(MODE_KM)						//支持的产品模式
	#define   PRODUCT_SUB_MODE			0									//产品子模式
	#define   PRODUCT_TRS				(BIT(TR_BLE)|BIT(TR_USBD))			//产品传输层支持
	/**********************************************************************************/

	#define DEMO1								1
	#if DEMO1
		#define SW_VERSION                     	0x01
        #define DEFAULT_NAME			       	"demo01"
        #define DEFAULT_MODEL					"DEMO01"
	#else

	#endif
#endif


#ifndef RF_ENABLE
#define RF_ENABLE 			EXTERNAL_MODULE					//rf peripheral 0,INTER_MODULE,EXTERNAL_MODULE
#endif
#ifndef RFC_ENABLE
#define RFC_ENABLE 			INTER_MODULE					//rf central
#endif
#ifndef BLE_ENABLE
#define BLE_ENABLE			INTER_MODULE		//ble peripheral
#endif	
#ifndef EDR_ENABLE	
#define EDR_ENABLE			INTER_MODULE		//edr peripheral
#endif				
#ifndef BLEC_ENABLE				
#define BLEC_ENABLE			INTER_MODULE					//ble central
#endif			
#ifndef EDRC_ENABLE				
#define EDRC_ENABLE			INTER_MODULE					//edr central
#endif	

#include "emf_config.h"
#include "hw_board.h"
#ifdef __cplusplus
}
#endif
#endif





