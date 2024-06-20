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


#ifndef _usbd_class_h
#define _usbd_class_h
#include "emf_typedef.h" 
#include "utils/emf_utils.h"
#include "api/usb/device/usbd_core.h"

#if USBD_SOCKET_ENABLE
#include "api/usb/usbd_socket.h"
#endif

#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
#include "api/usb/device/usbd_hid.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
#include "api/usb/device/usbd_audio.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
#include "api/usb/device/usbd_printer.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
#include "api/usb/device/usbd_msd.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
#include "api/usb/device/usbd_cdc.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
#include "api/usb/device/usbd_iap2.h"
#endif
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
#include "api/usb/device/usbd_auto.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
//usbd 一个接口一个usbd_class
extern usbd_class_t m_usbd_class[USBD_NUM][USBD_MAX_ITF_NUM];
/*****************************************************************************************************
**  Function
******************************************************************************************************/
usbd_class_t *usbd_class_find_by_ep(uint8_t id, uint8_t ep);
usbd_class_t *usbd_class_find_by_itf(uint8_t id, uint8_t itf);
usbd_class_t *usbd_class_find_by_type(uint8_t id, dev_type_t type, uint8_t sub);
error_t usbd_assign_configuration_desc(uint8_t id, dev_type_t type,hid_type_t hid_type, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len);
uint16_t usbd_class_get_itf_desc(uint8_t id, uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_class_control_request_process(uint8_t id, usbd_req_t* const preq);
error_t usbd_class_in(uint8_t id, dev_type_t type, uint8_t sub_type, uint8_t* buf,uint16_t len);
error_t usbd_class_notify_evt(uint8_t id, usbd_event_t event,uint32_t val);
bool usbd_class_vendor_process_weak(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);	//weak
void 	usbd_class_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_class_init(uint8_t id);
error_t usbd_class_deinit(uint8_t id);



#ifdef __cplusplus
}
#endif
#endif





