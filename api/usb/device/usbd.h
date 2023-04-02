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


#ifndef _usbd_h
#define _usbd_h
#include "emf_typedef.h"
#include "utils/emf_utils.h"
// #include "utils/list.h"
#include "api/usb/device/usbd_core.h"
#include "api/usb/device/usbd_class.h"

#include "hal/usb/hal_usbd.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern uint16_t m_usbd_types[USBD_NUM];
extern uint16_t m_usbd_hid_types[USBD_NUM];

/*****************************************************************************************************
**  Function
******************************************************************************************************/
usbd_dev_t *usbd_get_dev(uint8_t id);
usbd_req_t *usbd_get_req_buf(uint8_t id);

char* usbd_user_get_string(uint8_t id, uint8_t index);		//__WEAK 
void usbd_user_set_device_desc(uint8_t id, usb_desc_device_t *pdesc); //__WEAK 

error_t usbd_endp_open(uint8_t id, uint8_t ep);
error_t usbd_endp_close(uint8_t id, uint8_t ep);
error_t usbd_endp_nak(uint8_t id, uint8_t ep);
error_t usbd_clear_endp_stall(uint8_t id, uint8_t ep);
error_t usbd_endp_stall(uint8_t id, uint8_t ep);
bool 	usbd_get_endp_stalled(uint8_t id, uint8_t ep);
void 	*usbd_get_endp_buffer(uint8_t id, uint8_t ep);
error_t usbd_cfg_endp_all(uint8_t id);
error_t usbd_in(uint8_t id, uint8_t ep, uint8_t* buf,uint16_t len);
error_t usbd_out(uint8_t id, uint8_t ep,uint8_t* buf, uint16_t* plen);
error_t usbd_reset(uint8_t id);
error_t usbd_set_address(uint8_t id,uint8_t address);

void 	usbd_handler( void );
error_t usbd_init(uint8_t id);
error_t usbd_deinit(uint8_t id);


//hal
error_t hal_usbd_endp_open(uint8_t id, uint8_t ep);
error_t hal_usbd_endp_close(uint8_t id, uint8_t ep);
error_t hal_usbd_endp_nak(uint8_t id, uint8_t ep);
error_t hal_usbd_clear_endp_stall(uint8_t id, uint8_t ep);
error_t hal_usbd_endp_stall(uint8_t id, uint8_t ep);
void 	*hal_usbd_get_endp_buffer(uint8_t id, uint8_t ep);
error_t hal_usbd_cfg_endp(uint8_t id,usb_endp_t *pendp);
error_t hal_usbd_in(uint8_t id, usb_endp_t *pendp, uint8_t* buf,uint16_t len);
error_t hal_usbd_out(uint8_t id, usb_endp_t *pendp,uint8_t* buf, uint16_t* plen);
error_t hal_usbd_reset(uint8_t id);
error_t hal_usbd_set_address(uint8_t id,uint8_t address);
error_t hal_usbd_init(uint8_t id);
error_t hal_usbd_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





