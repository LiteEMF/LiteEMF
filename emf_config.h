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
#define ID_NULL   (0xff)

#ifndef INTER_MODULE
#define INTER_MODULE			BIT(0)
#endif
#ifndef EXTERNAL_MODULE
#define EXTERNAL_MODULE			BIT(1)
#endif

//*********************************************************************************//
//                                VID/PID cfg                                      //
//*********************************************************************************//
#ifndef CFG_VID
#define CFG_VID				0X4353
#endif
#ifndef CFG_KM_PID
#define CFG_KM_PID			0X0001
#endif
#ifndef CFG_MT_PID
#define CFG_MT_PID			0X0002
#endif
#ifndef CFG_DINPUT_PID
#define CFG_DINPUT_PID		0X0003
#endif
#ifndef CFG_XINPUT_PID
#define CFG_XINPUT_PID		0X0004
#endif
#ifndef CFG_PRINTER_PID
#define CFG_PRINTER_PID		0X0005
#endif
#ifndef CFG_MSD_PID
#define CFG_MSD_PID			0X0006
#endif
#ifndef CFG_CDC_PID
#define CFG_CDC_PID			0X0007
#endif
#ifndef CFG_AUDIO_PID
#define CFG_AUDIO_PID		(0X0008 | AUDIO_PID_MAK)
#endif
#ifndef CFG_VENDOR_PID
#define CFG_VENDOR_PID		0X000F
#endif
#ifndef AUDIO_PID_MAK
#define AUDIO_PID_MAK		0X0010
#endif


//TODO
#ifndef USB_CFG_VID
#define USB_CFG_VID			CFG_VID
#endif
#ifndef USB_KM_PID
#define USB_KM_PID			CFG_KM_PID
#endif
#ifndef USB_MT_PID
#define USB_MT_PID			CFG_MT_PID
#endif
#ifndef USB_DINPUT_PID
#define USB_DINPUT_PID		CFG_DINPUT_PID
#endif
#ifndef USB_XINPUT_PID
#define USB_XINPUT_PID		CFG_XINPUT_PID
#endif
#ifndef USB_PRINTER_PID
#define USB_PRINTER_PID		CFG_PRINTER_PID
#endif
#ifndef USB_MSD_PID
#define USB_MSD_PID			CFG_MSD_PID
#endif
#ifndef USB_CDC_PID
#define USB_CDC_PID			CFG_CDC_PID
#endif
#ifndef USB_AUDIO_PID
#define USB_AUDIO_PID		(CFG_AUDIO_PID | AUDIO_PID_MAK)
#endif
#ifndef USB_VENDOR_PID
#define USB_VENDOR_PID		CFG_VENDOR_PID
#endif


//*********************************************************************************//
//                                模式配置                                    	   //
//*********************************************************************************//
/*------------------------mode-----------------------------*/
#define	MODE_KM				0
#define	MODE_CKM			1
#define	MODE_GP				2
#define	MODE_MT				3
#define	MODE_CDC			4
#define	MODE_AUDIO			5
#define	MODE_MSD			6
#define	MODE_VENDOR			7
/*------------------------transport-----------------------------*/
#define	TR_BLE				0
#define	TR_EDR				1
#define	TR_BLEC				2
#define	TR_EDRC				3
#define	TR_BLE_RF			4				//BLE模拟2.4G	
#define	TR_BLEC_RF			5
#define	TR_RF				6
#define	TR_RFC				7
#define	TR_USBD				8
#define	TR_USBH				9
#define	TR_UART				10

/*------------------------sub mode-----------------------------*/
typedef enum {
	DINPUT_MODE=0,	
	SWITCH_MODE,	
	PS3_MODE,	
	PS4_MODE,	
	PS5_MODE,
	X360_MODE,
	XBOX_MODE,
}gpad_sub_mode_t;

#ifndef   PRODUCT_MODE						//产品模式
#error	  "must defined PRODUCT_MODE"
#endif
#ifndef   PRODUCT_MODE_SUPPORT				//支持的产品模式
#error	  "must defined PRODUCT_MODE_SUPPORT"
#endif
#ifndef   PRODUCT_SUB_MODE					//产品子模式
#error	  "must defined PRODUCT_SUB_MODE"
#endif
#ifndef   PRODUCT_TRS					//产品传输层支持
#error	  "must defined PRODUCT_TRS"
#endif
#ifndef   PRODUCT_ATT_SUPPORT
#define   PRODUCT_ATT_SUPPORT   			0
#endif

//*********************************************************************************//
//                          传输支持的设备类型                               		//
//*********************************************************************************//
/*------------------------type defined-----------------------------*/
//transport type
#define TYPE_VENDOR    			0
#define TYPE_AUTO				1
#define TYPE_HID				2
#define TYPE_AUDIO     			3
#define TYPE_PRINTER   			4
#define TYPE_MSD   				5		//mass storage devices
#define TYPE_HUB       			6
#define TYPE_CDC				7		// Communications Device Class
#define TYPE_ADB          		8
#define TYPE_USBMUXD	   		9
#define TYPE_AOA          		10
#define TYPE_IAP2				11
#define DEV_TYPE_NONE			16		//type none 0 == ((uint16_t)BIT(DEV_TYPE_NONE)
//hid type
#define HID_TYPE_GAMEPADE		0
#define HID_TYPE_SWITCH			1
#define HID_TYPE_PS3			2
#define HID_TYPE_PS4			3
#define HID_TYPE_PS5			4
#define HID_TYPE_X360			5
#define HID_TYPE_XBOX			6
#define HID_TYPE_KB 			7
#define HID_TYPE_MOUSE    		8
#define HID_TYPE_CONSUMER    	9
#define HID_TYPE_MT				10

#define HID_TYPE_VENDOR			15
#define HID_TYPE_NONE    		16	//hid type none0 == ((uint16_t)BIT(DEV_TYPE_NONE)

#define HID_SWITCH_MASK			BIT(HID_TYPE_SWITCH)
#define HID_PS_MASK				(BIT(HID_TYPE_PS3) | BIT(HID_TYPE_PS4) | BIT(HID_TYPE_PS5))
#define HID_XBOX_MASK			(BIT(HID_TYPE_X360) | BIT(HID_TYPE_XBOX))


/*------------------------transport type support-----------------------------*/
#ifndef BLE_TYPE_SUPPORT		
#define BLE_TYPE_SUPPORT		0				//不能同时支持内部和外部蓝牙所以定义一个就可以
#endif
#ifndef BLE_RF_TYPE_SUPPORT		
#define BLE_RF_TYPE_SUPPORT		0				//不能同时支持内部和外部蓝牙所以定义一个就可以
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
//dev hid type support
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

#define HID_SUPPORT			(BT_HID_SUPPORT | USBD_HID_SUPPORT)


// extern uint16_t usbd_types;
// extern uint16_t usbd_hid_types;

// extern uint16_t usbh_types;
// extern uint16_t usbh_hid_types;
// extern uint16_t aoa_hid_types;
// extern uint16_t iap_hid_types;


//*********************************************************************************//
//                                 蓝牙配置                                   	  //
//*********************************************************************************//
/*
disable: 0
enable: INTER_MODULE,EXTERNAL_MODULE
*/
#ifndef RF_ENABLE
#define RF_ENABLE 			0		//rf peripheral 0,INTER_MODULE,EXTERNAL_MODULE
#endif
#ifndef RFC_ENABLE
#define RFC_ENABLE 			0		//rf central
#endif
#ifndef BLE_ENABLE
#define BLE_ENABLE			0		//ble peripheral
#endif	
#ifndef BLEC_ENABLE	
#define BLEC_ENABLE			0		//ble central
#endif
#ifndef BLE_RF_ENABLE	
#define BLE_RF_ENABLE		0		//ble peripheral 模拟2.4G
#endif	
#ifndef BLEC_RF_ENABLE	
#define BLEC_RF_ENABLE		0		//ble central
#endif	
#ifndef EDR_ENABLE	
#define EDR_ENABLE			0		//edr peripheral
#endif	
#ifndef EDRC_ENABLE	
#define EDRC_ENABLE			0		//edr central
#endif	

#define BT_ENABLE			(BLE_ENABLE  | BLE_RF_ENABLE | EDR_ENABLE)
#define BTC_ENABLE			(BLEC_ENABLE  | BLEC_RF_ENABLE | EDRC_ENABLE)


//*********************************************************************************//
//                       	USB			                  						   //
//*********************************************************************************//



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
#ifndef APP_ADB_ENABLE
#define APP_ADB_ENABLE			1
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
#define APP_RF_ENABLE		(RF_ENABLE | RFC_ENABLE)
#define APP_BT_ENABLE		(BT_ENABLE | BTC_ENABLE | RF_ENABLE | RFC_ENABLE)
#define APP_USB_ENABLE		(USBD_ENABLE | USBH_ENABLE)



/*-------------------------transport link----------------------------*/
#ifndef TR_BLE_ENABLE
#define TR_BLE_ENABLE		0
#endif
#ifndef TR_EDR_ENABLE
#define TR_EDR_ENABLE		0
#endif
#ifndef TR_BLEC_ENABLE
#define TR_BLEC_ENABLE		0
#endif
#ifndef TR_EDRC_ENABLE
#define TR_EDRC_ENABLE		0
#endif
#ifndef TR_RF_ENABLE
#define TR_RF_ENABLE		0
#endif
#ifndef TR_RFC_ENABLE
#define TR_RFC_ENABLE		0
#endif
#ifndef TR_HUART_ENABLE
#define TR_HUART_ENABLE		1
#endif
#ifndef TR_USBD_ENABLE
#define TR_USBD_ENABLE		0
#endif
#ifndef TR_USBH_ENABLE
#define TR_USBH_ENABLE		0
#endif






#ifdef __cplusplus
}
#endif
#endif





