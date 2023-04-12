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


#ifndef _usbh_h
#define _usbh_h
#include "emf_typedef.h"
#include "utils/emf_utils.h"
#include "utils/list.h"
#include "api/usb/host/usbh_core.h"
#include "api/usb/host/usbh_class.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



extern usbh_dev_t m_usbh_dev[USBH_MAX_PORTS][HUB_MAX_PORTS+1];


extern uint16_t m_usbh_types;
extern uint16_t m_usbh_hid_types;
extern uint16_t usbh_iap_hid_types;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_det_event(uint8_t id, uint8_t attached );
error_t usbh_disconnect(uint8_t id);
error_t usbh_reset(uint8_t id);
error_t usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen);
error_t usbh_endp_unregister(uint8_t id,usb_endp_t *endpp);
error_t usbh_endp_register(uint8_t id,usb_endp_t *endpp);
error_t usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms);
error_t usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len);

usbh_dev_t* get_usbh_dev(uint8_t id);   
uint8_t usbh_find_by_status(uint8_t usb_stas);   
error_t usbh_set_status(uint8_t id, usb_state_t usb_sta, uint8_t addr) ; 
error_t usbh_init( uint8_t id );
error_t usbh_deinit(uint8_t id);
void usbh_handler( uint32_t period_10us );


//hal
error_t hal_usbh_port_disable(uint8_t id);		//用于关闭usb口,防止多个设备在地址状态!!!
error_t hal_usbh_port_reset(uint8_t id);
error_t hal_usbh_set_addr(uint8_t id,uint8_t addr);
error_t hal_usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen);
error_t hal_usbh_endp_unregister(uint8_t id,usb_endp_t *endpp);
error_t hal_usbh_endp_register(uint8_t id,usb_endp_t *endpp);
error_t hal_usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms);
error_t hal_usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len);
void 	hal_usbh_driver_handler(uint32_t period_10us);
error_t hal_usbh_driver_init(uint8_t id);
error_t hal_usbh_driver_deinit(uint8_t id);



#ifdef __cplusplus
}
#endif
#endif





