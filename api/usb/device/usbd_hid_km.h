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


#ifndef _usbd_hid_km_h
#define _usbd_hid_km_h
#include "emf_typedef.h" 
#include "api/usb/device/usbd_core.h"
#include "api/hid/hid_dev_desc.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef USBD_HID_KB_INTERVAL
#define USBD_HID_KB_INTERVAL		2
#endif
#ifndef USBD_HID_MOUSE_INTERVAL
#define USBD_HID_MOUSE_INTERVAL		1
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_km_reset(uint8_t id);
error_t usbd_hid_km_suspend(uint8_t id);
uint16_t usbd_hid_km_get_itf_desc(uint8_t id, uint8_t hid_type, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_hid_km_control_request_process(uint8_t id, usbd_class_t *pclass,  usbd_req_t* const preq);
error_t usbd_hid_km_out_process(uint8_t id, usbd_class_t *pclass);
void 	usbd_hid_km_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_hid_km_init(uint8_t id);
error_t usbd_hid_km_deinit(uint8_t id);


#ifdef __cplusplus
}
#endif
#endif





