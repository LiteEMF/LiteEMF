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
#include "emf_typedef.h" 
#include "hw_config.h"
#include "api/api_transport.h"
#include "api/usb/usb_typedef.h"
#include "api/api_transport.h"

#include "usb/hal_usbd.h"
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
#define USBD_SPEED_MODE		0		/*0:full, 1 low, 2,high*/
#endif

#ifndef USBD_BCD_VERSION
#if (1 == USBD_SPEED_MODE)
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

#ifndef USBD_LOOP_ENABLE			//1 采样轮训方式处理usb //TODO优化兼容函数回调方式
#define USBD_LOOP_ENABLE		1
#endif


#ifndef USB_VID
#define USB_VID				VID_DEFAULT
#endif
// #ifndef USB_PID	
// #define USB_PID				(USBD_HID_SUPPORT | USBD_TYPE_SUPPORT)
// #endif


#ifndef USBD_MAX_ITF_NUM
#define USBD_MAX_ITF_NUM		8
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum
{
  USBD_EVENT_NULL = 0,
  USBD_EVENT_RESET,
  USBD_EVENT_SOF,
  USBD_EVENT_SUSPEND,
  USBD_EVENT_RESUME,

  USBD_EVENT_EP_OUT,
  USBD_EVENT_EP_IN,
} usbd_event_t;


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
	uint8_t is_alt_itf;		//最新接口号,用于分配端点
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
		volatile uint8_t setup			: 1;		// 接收到usb setup 指令
		volatile uint8_t reset			: 1;		// reset msg
		volatile uint8_t suspend		: 1;		// suspend msg
		volatile uint8_t resume			: 1;		// resume msg

		uint8_t remote_wakeup_en      	: 1; 		// enable/disable by host
		uint8_t remote_wakeup_support 	: 1; 		// configuration descriptor's attribute
		uint8_t self_powered          	: 1; 		// configuration descriptor's attribute
	}dev;

	uint8_t 	ready;								//usbd ready
	uint8_t 	address;
	usb_state_t state;
	uint16_t vid;
	uint16_t pid;
	volatile uint8_t cfg_num; 						// current active configuration (0x00 is not configured)
	uint8_t itf_alt[USBD_MAX_ITF_NUM];				// current bAlternateSetting
	usbd_endp_state_t ep_status[USBD_ENDP_NUM][2];
	volatile uint8_t enpd_in_busy[ USBD_ENDP_NUM ];	//bit0:busy, bit7: 中断标志注意端点0, 和其他端点判断busy方式不一样
	volatile uint8_t enpd_out_len[ USBD_ENDP_NUM ];
	uint8_t endp0_mtu;
}usbd_dev_t;


typedef struct _usbd_req_t{
	volatile usb_control_request_t req;
	uint8_t *setup_buf;   		//必须调用usbd_malloc_setup_buffer分配内存, usbd_free_setup_buffer 释放内存
	volatile uint16_t setup_len;			//ep0 setup data stage数据长度
    volatile uint16_t setup_index;     		//ep0 setup 当前传输的位置
}usbd_req_t;



/*****************************************************************************************************
**  Function
******************************************************************************************************/
usbd_dev_t *usbd_get_dev(uint8_t id);
usbd_req_t *usbd_get_req(uint8_t id);
error_t usbd_malloc_setup_buffer(uint8_t id, usbd_req_t *preq);
error_t usbd_free_setup_buffer(usbd_req_t *preq);

error_t usbd_endp_dma_init(uint8_t id);
error_t usbd_endp_open(uint8_t id, usb_endp_t *pendp);
error_t usbd_endp_close(uint8_t id, uint8_t ep);
error_t usbd_endp_ack(uint8_t id, uint8_t ep, uint16_t len);
error_t usbd_endp_nak(uint8_t id, uint8_t ep);
error_t usbd_clear_endp_stall(uint8_t id, uint8_t ep);
error_t usbd_endp_stall(uint8_t id, uint8_t ep);
bool 	usbd_get_endp_stalled(uint8_t id, uint8_t ep);
void 	*usbd_get_endp_buffer(uint8_t id, uint8_t ep);
error_t usbd_in(uint8_t id, uint8_t ep, uint8_t* buf,uint16_t len);
error_t usbd_out(uint8_t id, uint8_t ep,uint8_t* buf, uint16_t* plen);
error_t usbd_set_address(uint8_t id,uint8_t address);
error_t usbd_set_ready(uint8_t id, uint8_t ready);
error_t usbd_reset(uint8_t id);
error_t usbd_core_init(uint8_t id);
error_t usbd_core_deinit(uint8_t id);




//hal
error_t hal_usbd_endp_dma_init(uint8_t id);
error_t hal_usbd_endp_open(uint8_t id, usb_endp_t *pendp);
error_t hal_usbd_endp_close(uint8_t id, uint8_t ep);
error_t hal_usbd_endp_ack(uint8_t id, uint8_t ep, uint16_t len);
error_t hal_usbd_endp_nak(uint8_t id, uint8_t ep);
error_t hal_usbd_clear_endp_stall(uint8_t id, uint8_t ep);
error_t hal_usbd_endp_stall(uint8_t id, uint8_t ep);
uint8_t 	*hal_usbd_get_endp_buffer(uint8_t id, uint8_t ep);
error_t hal_usbd_in(uint8_t id, uint8_t ep, uint8_t* buf,uint16_t len);
error_t hal_usbd_out(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t* plen);
error_t hal_usbd_reset(uint8_t id);
error_t hal_usbd_set_address(uint8_t id,uint8_t address);
error_t hal_usbd_init(uint8_t id);
error_t hal_usbd_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





