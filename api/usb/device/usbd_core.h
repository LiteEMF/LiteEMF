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


#ifndef _usbd_core_h
#define _usbd_core_h
#include "utils/emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/api_transport.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef USBD_NUM
#define USBD_NUM				1
#endif
#ifndef USBD_ENDP_NUM
#define USBD_ENDP_NUM			5
#endif
#ifndef USBD_ENDP0_MTU
#define USBD_ENDP0_MTU			64
#endif
#ifndef USBD_SPEED_MODE
#define USBD_SPEED_MODE		USB_SPEED_FULL
#endif

#ifndef USBD_BCD_VERSION
#if (USB_SPEED_LOW == USBD_SPEED_MODE)
	#define USBD_BCD_VERSION		0X110
#else
	#define USBD_BCD_VERSION		0X200
#endif
#endif


#ifndef USBD_REMOTE_WAKEUP			//bit6
#define USBD_REMOTE_WAKEUP		0
#endif
#ifndef USBD_SELF_POWERED			//bit5
#define USBD_SELF_POWERED		0
#endif

#ifndef USBD_LOOP_OUT_ENABLE			//采样轮训方式读取USB数据
#define USBD_LOOP_OUT_ENABLE	1
#endif

#ifndef USB_VID
#define USB_VID				VID_DEFAULT
#endif
#ifndef USB_PID	
#define USB_PID				(USBD_HID_SUPPORT | USBD_TYPE_SUPPORT)
#endif


#ifndef USBD_MAX_ITF_NUM
#define USBD_MAX_ITF_NUM		8
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct 
{
	dev_type_t 	dev_type;
	hid_type_t 	hid_type;

	usb_inf_t  	itf;
	usb_endp_t 	endpin;
	usb_endp_t 	endpout;
} usbd_class_t;

typedef struct {
	uint8_t itf_index;			//m_usbd_class 下标序号, 和接口号不是一一对应关系(itf_alt原因)
	uint8_t itf_num;			//接口号
	uint8_t ep_in_num;			//端点号
	uint8_t ep_out_num;			//端点号
}itf_ep_index_t;

typedef struct
{
	volatile uint8_t busy    : 1;
	volatile uint8_t stalled : 1;
	volatile uint8_t claimed : 1;
}usbd_endp_state_t;

typedef struct{
	struct{
		volatile uint8_t setup			: 1;	// 接收到usb setup 指令
		volatile uint8_t reset			: 1;	// reset msg
		volatile uint8_t suspend		: 1;	// suspend msg

		uint8_t remote_wakeup_en      	: 1; // enable/disable by host
		uint8_t remote_wakeup_support 	: 1; // configuration descriptor's attribute
		uint8_t self_powered          	: 1; // configuration descriptor's attribute
	}dev;

	uint8_t 	ready;						//usbd ready
	uint8_t 	address;
	usb_state_t state;
	uint16_t vid;
	uint16_t pid;
	volatile uint8_t cfg_num; 			// current active configuration (0x00 is not configured)
	uint8_t itf_alt[USBD_MAX_ITF_NUM];	// current bAlternateSetting
	usbd_endp_state_t ep_status[USBD_ENDP_NUM][2];
}usbd_dev_t;


typedef struct _usbd_req_t{
	usb_control_request_t req;
    uint8_t *setup_buf;   		//必须调用usbd_malloc_setup_buffer分配内存, usbd_free_setup_buffer 释放内存
	uint16_t setup_len;			//ep0 setup data stage数据长度
    uint16_t setup_index;     	//ep0 setup 当前传输的位置
}usbd_req_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/



#ifdef __cplusplus
}
#endif
#endif





