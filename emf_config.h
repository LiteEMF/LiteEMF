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


#ifndef LITEEMF_ENABLED
#define LITEEMF_ENABLED	
#endif

/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define ID_NULL   				(0xff)

#ifndef VID_DEFAULT
#define VID_DEFAULT				0X4353
#endif

#ifndef TASK_HANDLER_ENABLE
#define TASK_HANDLER_ENABLE		1
#endif


//*********************************************************************************//
//                          传输类型                               		//
//*********************************************************************************//
#define DEF_TR_BLE			0
#define DEF_TR_EDR			1
#define DEF_TR_BLEC			2
#define DEF_TR_EDRC			3
#define DEF_TR_BLE_RF		4				
#define DEF_TR_BLE_RFC		5
#define DEF_TR_RF			6
#define DEF_TR_RFC			7

#define DEF_TR_USBD			8
#define DEF_TR_USBH			9
#define DEF_TR_UART			10
typedef enum{
	TR_BLE		= 0,
	TR_EDR		= 1,
	TR_BLEC		= 2,
	TR_EDRC		= 3,
	TR_BLE_RF	= 4,				//BLE模拟2.4G	
	TR_BLE_RFC	= 5,
	TR_RF		= 6,
	TR_RFC		= 7,
	TR_USBD		= 8,
	TR_USBH		= 9,
	TR_UART		= 10,
	TR_MAX,
	TR_NULL = TR_MAX,
}trp_t;

//*********************************************************************************//
//                          支持的设备类型                               		//
//*********************************************************************************//
/*------------------------type defined-----------------------------*/
//dev type
#define DEF_DEV_TYPE_HID				0
#define DEF_DEV_TYPE_AUDIO     			1
#define DEF_DEV_TYPE_PRINTER   			2
#define DEF_DEV_TYPE_MSD   				3		/*mass storage devices*/
#define DEF_DEV_TYPE_HUB       			4
#define DEF_DEV_TYPE_CDC				5		/*Communications Device Class*/
//6,7
#define DEF_DEV_TYPE_VENDOR    			8
#define DEF_DEV_TYPE_ADB          		9
#define DEF_DEV_TYPE_AOA          		10
#define DEF_DEV_TYPE_USBMUXD	   		11
#define DEF_DEV_TYPE_IAP2				12
#define DEF_DEV_TYPE_AUTO				15
#define DEF_DEV_TYPE_NONE				16		/*type none 0 == ((uint16_t)BIT(DEV_TYPE_NONE)*/
//dev type
typedef enum{
	DEV_TYPE_HID	=	DEF_DEV_TYPE_HID	,	
	DEV_TYPE_AUDIO	=	DEF_DEV_TYPE_AUDIO  , 
	DEV_TYPE_PRINTER=	DEF_DEV_TYPE_PRINTER,
	DEV_TYPE_MSD	=	DEF_DEV_TYPE_MSD   	,   	
	DEV_TYPE_HUB	=	DEF_DEV_TYPE_HUB    ,     
	DEV_TYPE_CDC	=	DEF_DEV_TYPE_CDC	,	
	//
	DEV_TYPE_VENDOR = 	DEF_DEV_TYPE_VENDOR , 
	DEV_TYPE_ADB	=	DEF_DEV_TYPE_ADB    ,     
	DEV_TYPE_AOA	=	DEF_DEV_TYPE_AOA    ,	    
	DEV_TYPE_USBMUXD=	DEF_DEV_TYPE_USBMUXD,	
	DEV_TYPE_IAP2	=	DEF_DEV_TYPE_IAP2	,		

	DEV_TYPE_AUTO	=	DEF_DEV_TYPE_AUTO	,
	DEV_TYPE_NONE 	= 	DEF_DEV_TYPE_NONE	,
}dev_type_t;

//hid type
#define DEF_HID_TYPE_VENDOR			0
#define DEF_HID_TYPE_KB 			1
#define DEF_HID_TYPE_MOUSE    		2
#define DEF_HID_TYPE_CONSUMER    	3
#define DEF_HID_TYPE_TOUCH			4
#define DEF_HID_TYPE_MT				5
#define DEF_HID_TYPE_GAMEPADE		6
#define DEF_HID_TYPE_X360			7
#define DEF_HID_TYPE_XBOX			8
#define DEF_HID_TYPE_SWITCH			9
#define DEF_HID_TYPE_PS3			10
#define DEF_HID_TYPE_PS4			11
#define DEF_HID_TYPE_PS5			12
//RES
#define DEF_HID_TYPE_NONE 			16

//hid type
typedef enum{
	HID_TYPE_VENDOR		=	DEF_HID_TYPE_VENDOR		,
	HID_TYPE_KB 		=	DEF_HID_TYPE_KB 		,
	HID_TYPE_MOUSE    	=	DEF_HID_TYPE_MOUSE    	,
	HID_TYPE_CONSUMER   =	DEF_HID_TYPE_CONSUMER   ,
	HID_TYPE_TOUCH		=	DEF_HID_TYPE_TOUCH		,		//单独触控
	HID_TYPE_MT			=	DEF_HID_TYPE_MT			,		//多点触控
	HID_TYPE_GAMEPADE	=	DEF_HID_TYPE_GAMEPADE	,
	HID_TYPE_X360		=	DEF_HID_TYPE_X360		,
	HID_TYPE_XBOX		=	DEF_HID_TYPE_XBOX		,
	HID_TYPE_SWITCH		=	DEF_HID_TYPE_SWITCH		,
	HID_TYPE_PS3		=	DEF_HID_TYPE_PS3		,
	HID_TYPE_PS4		=	DEF_HID_TYPE_PS4		,
	HID_TYPE_PS5		=	DEF_HID_TYPE_PS5		,
	//
	HID_TYPE_NONE    	=	DEF_HID_TYPE_NONE		,
}hid_type_t;

typedef struct{
	dev_type_t dev;
	hid_type_t hid;
}emf_type_t;

#define HID_SWITCH_MASK			BIT_ENUM(HID_TYPE_SWITCH)
#define HID_PS_MASK				(BIT_ENUM(HID_TYPE_PS3) | BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5))
#define HID_XBOX_MASK			(BIT_ENUM(HID_TYPE_X360) | BIT_ENUM(HID_TYPE_XBOX))
#define HID_GAMEPAD_MASK		(HID_SWITCH_MASK | HID_PS_MASK | HID_XBOX_MASK | BIT_ENUM(HID_TYPE_GAMEPADE))


/*------------------------dev type default-----------------------------*/
#ifndef   DEV_TRPS_DEFAULT						//默认传输类型
#define   DEV_TRPS_DEFAULT				0
#endif
#ifndef   DEV_TYPES_DEFAULT						//默认设备类型
#define   DEV_TYPES_DEFAULT				0
#endif
#ifndef   HID_TYPES_DEFAULT						//默认HID设备类型
#define   HID_TYPES_DEFAULT				0
#endif
//dev type support dev_type_t
#ifndef BLE_TYPE_SUPPORT						//不能同时支持内部和外部蓝牙所以定义一个就可以
#define BLE_TYPE_SUPPORT		0				
#endif
#ifndef BLE_RF_TYPE_SUPPORT						//不能同时支持内部和外部蓝牙所以定义一个就可以
#define BLE_RF_TYPE_SUPPORT		0				
#endif
#ifndef EDR_TYPE_SUPPORT		
#define EDR_TYPE_SUPPORT		0
#endif
#ifndef BLEC_TYPE_SUPPORT		
#define BLEC_TYPE_SUPPORT		0
#endif
#ifndef BLEC_RF_TYPE_SUPPORT		
#define BLEC_RF_TYPE_SUPPORT	0
#endif
#ifndef EDRC_TYPE_SUPPORT		
#define EDRC_TYPE_SUPPORT		0
#endif
#ifndef USBD_TYPE_SUPPORT		
#define USBD_TYPE_SUPPORT		0
#endif
#ifndef USBH_TYPE_SUPPORT		
#define USBH_TYPE_SUPPORT		0
#endif
//dev hid type support	hid_type_t
#ifndef BLE_HID_SUPPORT
#define BLE_HID_SUPPORT			0
#endif
#ifndef BLE_RF_HID_SUPPORT
#define BLE_RF_HID_SUPPORT		0
#endif
#ifndef EDR_HID_SUPPORT
#define EDR_HID_SUPPORT			0
#endif
#ifndef BLEC_HID_SUPPORT
#define BLEC_HID_SUPPORT		0
#endif
#ifndef BLEC_RF_HID_SUPPORT
#define BLEC_RF_HID_SUPPORT		0
#endif
#ifndef EDRC_HID_SUPPORT
#define EDRC_HID_SUPPORT		0
#endif
#ifndef USBD_HID_SUPPORT
#define USBD_HID_SUPPORT		0
#endif
#ifndef USBH_HID_SUPPORT				//usbh支持哪些hid设备类型枚举
#define USBH_HID_SUPPORT		0
#endif
#ifndef AOA_HID_SUPPORT					//aoa模式下,支持的hid设备类型
#define AOA_HID_SUPPORT			0
#endif
#ifndef IAP2_HID_SUPPORT				//iap2模式下,支持的hid设备类型
#define IAP2_HID_SUPPORT		0
#endif

#define BT_TYPE_SUPPORT		(BLE_TYPE_SUPPORT | BLE_RF_TYPE_SUPPORT | EDR_TYPE_SUPPORT)
#define BTC_TYPE_SUPPORT	(BLEC_TYPE_SUPPORT | BLEC_RF_TYPE_SUPPORT | EDRC_TYPE_SUPPORT)
#define BT_HID_SUPPORT		(BLE_HID_SUPPORT | BLE_RF_HID_SUPPORT | EDR_HID_SUPPORT)
#define BTC_HID_SUPPORT		(BLEC_HID_SUPPORT | BLEC_RF_HID_SUPPORT | EDRC_HID_SUPPORT)
//hid device support宏定义预编译,必须使用 BIT_ENUM(enum)
#ifndef HIDD_SUPPORT
#define HIDD_SUPPORT		(BT_HID_SUPPORT | USBD_HID_SUPPORT | AOA_HID_SUPPORT | IAP2_HID_SUPPORT)		
#endif
//hid host support宏定义预编译,必须使用 BIT_ENUM(enum)	
#ifndef HIDH_SUPPORT
#define HIDH_SUPPORT		(BTC_HID_SUPPORT | USBH_HID_SUPPORT)		
#endif

//*********************************************************************************//
//                                 蓝牙配置                                   	  //
//*********************************************************************************//
// 默认: id: 0:芯片自带蓝牙, 1: 外部蓝牙模块, ID_NULL: 无 
// 蓝牙上层目前不支持自带蓝牙和外部蓝牙相同模块同时支持,项目可以调整 api_bt_ctb_t 来支持
#ifndef BT0_SUPPORT
#define BT0_SUPPORT 		0
#endif
#ifndef BT1_SUPPORT
#define BT1_SUPPORT 		0
#endif

#define BT_SUPPORT 			(BT0_SUPPORT | BT1_SUPPORT)
#define BT_ENABLE			(BT_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF) | BIT_ENUM(TR_EDR)))
#define BTC_ENABLE			(BT_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC) | BIT_ENUM(TR_EDRC)))
#define API_RF_ENABLE		(BT_SUPPORT & (BIT_ENUM(TR_RF) | BIT_ENUM(TR_RFC)))
#define API_BT_ENABLE		(BT_ENABLE | BTC_ENABLE | API_RF_ENABLE)

//*********************************************************************************//
//                       		USB			                  					//
//*********************************************************************************//
#ifndef API_OTG_BIT_ENABLE				//used BIT(n);
#define API_OTG_BIT_ENABLE			0
#endif
#ifndef API_USBD_BIT_ENABLE				//used BIT(n);
#define API_USBD_BIT_ENABLE			0
#endif
#ifndef API_USBH_BIT_ENABLE
#define API_USBH_BIT_ENABLE			0
#endif
#define APP_USB_ENABLE		(API_USBD_BIT_ENABLE | API_USBH_BIT_ENABLE)

// usb socket 用于手柄引导
#ifndef USBD_SOCKET_ENABLE
#define USBD_SOCKET_ENABLE		0
#endif
#ifndef USBH_SOCKET_ENABLED
#define USBH_SOCKET_ENABLED		0
#endif

#ifndef SOCKET_HID_TYPE_SUPPORT
#define SOCKET_HID_TYPE_SUPPORT		(BIT_DEF(HID_TYPE_XBOX) | BIT_DEF(HID_TYPE_X360) | BIT_DEF(HID_TYPE_PS4))
#endif

//*********************************************************************************//
//                       	other API modules                  						//
//*********************************************************************************//
#ifndef API_TIMER_BIT_ENABLE		// 打开哪些硬件定时器BIT(0) | BIT(2)
#define API_TIMER_BIT_ENABLE		0
#endif
#ifndef API_SOFT_TIMER_ENABLE
#define API_SOFT_TIMER_ENABLE		0
#endif
#ifndef API_NFC_ENABLE
#define API_NFC_ENABLE			0
#endif
#ifndef API_GPS_ENABLE
#define API_GPS_ENABLE			0
#endif
#ifndef API_GSM_ENABLE
#define API_GSM_ENABLE			0
#endif
#ifndef API_WIFI_ENABLE
#define API_WIFI_ENABLE			0
#endif
#ifndef API_AUDIO_ENABLE
#define API_AUDIO_ENABLE		0
#endif
#ifndef API_WDT_ENABLE
#define API_WDT_ENABLE		0
#endif
#ifndef API_STORAGE_ENABLE	
#define API_STORAGE_ENABLE		0
#endif
#ifndef API_PM_ENABLE
#define API_PM_ENABLE			0
#endif

//*********************************************************************************//
//                       	APP modules                  						   //
//*********************************************************************************//
#ifndef APP_BATTERY_ENABLE
#define APP_BATTERY_ENABLE		0
#endif
#ifndef APP_KEY_ENABLE
#define APP_KEY_ENABLE			0
#endif
#ifndef APP_KM_ENABLE
#define APP_KM_ENABLE			0
#endif
#ifndef APP_IMU_ENABLE
#define APP_IMU_ENABLE			0
#endif
#ifndef APP_JOYSTICK_ENABLE
#define APP_JOYSTICK_ENABLE		0
#endif
#ifndef APP_GAMEAPD_ENABLE
#define APP_GAMEAPD_ENABLE 		((HIDD_SUPPORT | HIDH_SUPPORT) & HID_GAMEPAD_MASK)
#endif
#ifndef APP_RUMBLE_ENABLE
#define APP_RUMBLE_ENABLE		0
#endif
#ifndef APP_LED_ENABLE
#define APP_LED_ENABLE			0
#endif
#ifndef APP_RGB_ENABLE
#define APP_RGB_ENABLE			0
#endif
#ifndef APP_WS2812FX_ENABLE
#define APP_WS2812FX_ENABLE		1
#endif

#ifndef APP_CMD_ENABLE
#define APP_CMD_ENABLE			0
#endif


#if APP_RGB_ENABLE
#undef SIN8_ENABLE
#define SIN8_ENABLE				1
#endif

#ifdef __cplusplus
}
#endif
#endif





