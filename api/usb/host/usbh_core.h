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


#ifndef _usbh_core_h
#define _usbh_core_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include  "api/api_transport.h"
#include "api/api_tick.h"
#include "api/usb/usb_typedef.h"
#include "utils/list.h"

#include "usb/hal_usbh.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#ifndef USBH_NUM				//支持多少个usb
#define USBH_NUM				1
#endif
#ifndef HUB_MAX_PORTS			//支持多少个usb
#define HUB_MAX_PORTS			3
#endif
#ifndef USBH_ENDP_NUM
#define USBH_ENDP_NUM			5
#endif
#ifndef USBH_ENUM_RETRY			//枚举重试次数
#define USBH_ENUM_RETRY			3
#endif

#ifndef USBH_LOOP_ENABLE			//采样轮训方式读取USB数据
#define USBH_LOOP_ENABLE		1
#endif

#define USBH0					0x00
#define USBH1					0x10
#define USBH2					0x20
#define USBH3					0x30
#define USBH_NULL				0xff
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct 
{
	uint8_t		id;				//USBH_NULL: free, 非 USBH_NULL: used
	dev_type_t 	dev_type;
	hid_type_t 	hid_type;

	usb_inf_t  	itf;
	usb_endp_t 	endpin;
	usb_endp_t 	endpout;

	void		*pdat;				//注意内存释放
	list_head_t list;
} usbh_class_t;

typedef struct 
{
	uint8_t 	id; 				//bit4~7:root_port,bit0~3:hub_port
    uint8_t   	addr;
	uint8_t 	class_ready;		//usbh class driver init ready
	uint8_t	  	endp0_mtu;
    uint8_t   	cfg;
    usb_speed_t speed;
    usb_state_t state;
	uint16_t  	vid;
	uint16_t 	pid;
	list_head_t class_list;
} usbh_dev_t;

extern usbh_dev_t m_usbh_dev[USBH_NUM][HUB_MAX_PORTS+1];


/*****************************************************************************************************
**  Function
******************************************************************************************************/
 
error_t usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen);
error_t usbh_req_get_device_desc(uint8_t id, uint8_t* buf, uint16_t *plen);  
error_t usbh_req_configuration_desc(uint8_t id, uint8_t index, uint8_t* buf, uint16_t *plen);  
error_t usbh_req_get_string_desc(uint8_t id, uint8_t index, uint16_t language_id, uint8_t* buf, uint16_t *plen);  
error_t usbh_req_set_addr(uint8_t id, uint8_t addr);  
error_t usbh_req_get_status(uint8_t id, uint8_t* pstatus);  
error_t usbh_req_get_configuration(uint8_t id, uint8_t* pcfg);  
error_t usbh_req_set_configuration(uint8_t id, uint8_t cfg);  
error_t usbh_req_get_itf(uint8_t id, uint8_t inf, uint8_t *palt);  
error_t usbh_req_set_itf(uint8_t id, uint8_t inf, uint8_t alt);  
error_t usbh_req_get_endp_status(uint8_t id, uint8_t endp, uint8_t* pstatus);  
error_t usbh_req_clean_endp_feature(uint8_t id, uint8_t endp); 
error_t usbh_req_set_endp_feature(uint8_t id, uint8_t endp, usb_request_feature_selector_t feature);  

usbh_dev_t* get_usbh_dev(uint8_t id);  
error_t usbh_select_hub_port(uint8_t id);
error_t usbh_port_reset(uint8_t id,uint8_t reset_ms);
error_t usbh_set_speed(uint8_t id, usb_speed_t speed);
error_t usbh_set_addr(uint8_t id,uint8_t addr);
error_t usbh_port_en(uint8_t id, uint8_t en, usb_speed_t* pspeed);
error_t usbh_endp_unregister(uint8_t id,usb_endp_t *endpp);
error_t usbh_endp_register(uint8_t id,usb_endp_t *endpp);
error_t usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms);
error_t usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len);

error_t usbh_core_init( uint8_t id );
error_t usbh_core_deinit( uint8_t id );

//hal
error_t hal_usbh_set_status(uint8_t id,usb_state_t usb_sta);
error_t hal_usbh_port_en(uint8_t id, uint8_t en, usb_speed_t* pspeed);		//用于打开/关闭usb口,防止多个设备在地址状态!!!
error_t hal_usbh_port_reset(uint8_t id, uint8_t reset_ms);
error_t hal_usbh_set_speed(uint8_t id, usb_speed_t speed);
error_t hal_usbh_set_addr(uint8_t id,uint8_t addr);
error_t hal_usbh_endp_unregister(uint8_t id,usb_endp_t *endpp);
error_t hal_usbh_endp_register(uint8_t id,usb_endp_t *endpp);
error_t hal_usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen);
error_t hal_usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms);
error_t hal_usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len);
error_t hal_usbh_driver_init(uint8_t id);
error_t hal_usbh_driver_deinit(uint8_t id);
void 	hal_usbh_driver_task(uint32_t dt_ms);


#ifdef __cplusplus
}
#endif
#endif





