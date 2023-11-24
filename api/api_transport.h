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


#ifndef _api_transporte_h
#define _api_transporte_h
#include "emf_typedef.h" 
#include "hw_config.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#ifndef BLE_CMD_MTU
#define BLE_CMD_MTU			48
#endif
#ifndef EDR_CMD_MTU
#define EDR_CMD_MTU			64
#endif
#ifndef BLEC_CMD_MTU
#define BLEC_CMD_MTU		48
#endif
#ifndef EDRC_CMD_MTU
#define EDRC_CMD_MTU		64
#endif
#ifndef RF_CMD_MTU
#define RF_CMD_MTU			32
#endif
#ifndef RFC_CMD_MTU
#define RFC_CMD_MTU			32
#endif
#ifndef USBD_CMD_MTU
#define USBD_CMD_MTU		64
#endif
#ifndef USBH_CMD_MTU
#define USBH_CMD_MTU		64
#endif
#ifndef UART_CMD_MTU
#define UART_CMD_MTU		64
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	TR_BLE		= DEF_TR_BLE,
	TR_EDR		= DEF_TR_EDR,
	TR_BLEC		= DEF_TR_BLEC,
	TR_EDRC		= DEF_TR_EDRC,
	TR_BLE_RF	= DEF_TR_BLE_RF,				//BLE模拟2.4G	
	TR_BLE_RFC	= DEF_TR_BLE_RFC,
	TR_RF		= DEF_TR_RF,
	TR_RFC		= DEF_TR_RFC,
	TR_USBD		= DEF_TR_USBD,
	TR_USBH		= DEF_TR_USBH,
	TR_UART		= DEF_TR_UART,
	TR_MAX,
	TR_NULL = TR_MAX,
}trp_t;


typedef struct {
	trp_t trp;
	uint8_t id;
	uint16_t index;	//high 8bit:dev_type_t low 8bit:sub_type
}trp_handle_t;


//dev type
typedef enum{
	DEV_TYPE_VENDOR = 	DEF_DEV_TYPE_VENDOR , 
	DEV_TYPE_AUDIO	=	DEF_DEV_TYPE_AUDIO  , 
	DEV_TYPE_CDC	=	DEF_DEV_TYPE_CDC	,	
	DEV_TYPE_HID	=	DEF_DEV_TYPE_HID	,	
	DEV_TYPE_PRINTER=	DEF_DEV_TYPE_PRINTER,
	DEV_TYPE_MSD	=	DEF_DEV_TYPE_MSD   	,   	
	DEV_TYPE_HUB	=	DEF_DEV_TYPE_HUB    ,     
	//
	DEV_TYPE_ADB	=	DEF_DEV_TYPE_ADB    ,     
	DEV_TYPE_AOA	=	DEF_DEV_TYPE_AOA    ,	    
	DEV_TYPE_USBMUXD=	DEF_DEV_TYPE_USBMUXD,	
	DEV_TYPE_IAP2	=	DEF_DEV_TYPE_IAP2	,		

	DEV_TYPE_AUTO	=	DEF_DEV_TYPE_AUTO	,
	DEV_TYPE_NONE 	= 	DEF_DEV_TYPE_NONE	,
}dev_type_t;

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



extern uint16_t m_trps;


//产品模式
extern uint16_t m_dev_mode;
extern uint16_t m_hid_mode;	

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_trp_is_usb(trp_t trp);
bool api_trp_is_bt(trp_t trp);
bool api_trp_is_slave(trp_t trp);
bool api_trp_is_host(trp_t trp);
bool api_transport_set_type(uint8_t id, trp_t trp, uint16_t dev_types, uint16_t hid_types);
bool api_transport_ready(trp_handle_t* phandle);
uint16_t api_transport_get_mtu(trp_handle_t* phandle);
bool api_transport_tx(trp_handle_t* phandle, void* buf,uint16_t len);
bool api_trp_init(void);
bool api_trp_deinit(void);

#ifdef __cplusplus
}
#endif
#endif





