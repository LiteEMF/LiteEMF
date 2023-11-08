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


#ifndef _usbd_hid_h
#define _usbd_hid_h
#include "emf_typedef.h" 
#include "api/usb/device/usbd_core.h"
#include "api/hid/hid_dev_desc.h"

#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
#include "api/usb/device/usbd_hid_vendor.h"
#endif
#if (USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER)))
#include "api/usb/device/usbd_hid_km.h"
#endif
#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT))
#include "api/usb/device/usbd_hid_mt.h"
#endif

#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE))
#include "api/usb/device/usbd_hid_gamepade.h"
#endif
#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH))
#include "api/usb/device/usbd_hid_switch.h"
#endif
#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360))
#include "api/usb/device/usbd_hid_x360.h"
#endif
#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
#include "api/usb/device/usbd_hid_xbox.h"
#endif
#if (USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5)))
#include "api/usb/device/usbd_hid_ps.h"
#endif
#if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3))
#include "api/usb/device/usbd_hid_ps3.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef BCD_HID_VERSION
#define BCD_HID_VERSION		0x111
#endif

// usb_hid_descriptor_hid_t
#define USBD_HID_DESC	0x09,0x21,BCD_HID_VERSION&0XFF,BCD_HID_VERSION>>8,0x00,0x01,0x22,0x00,0x00
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern uint8c_t usbd_hid_descriptor_tab[9];





/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_reset(uint8_t id);
error_t usbd_hid_suspend(uint8_t id);
uint16_t usbd_hid_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_hid_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq);
void 	usbd_hid_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_hid_init(uint8_t id);
error_t usbd_hid_deinit(uint8_t id);


#ifdef __cplusplus
}
#endif
#endif





