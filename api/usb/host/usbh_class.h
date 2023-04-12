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


#ifndef _usbh_class_h
#define _usbh_class_h
#include "emf_typedef.h"
#include "utils/emf_utils.h"
#include "api/usb/host/usbh_core.h"


#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
#include "api/usb/host/usbh_hid.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
#include "api/usb/host/usbh_audio.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
#include "api/usb/host/usbh_printer.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
#include "api/usb/host/usbh_msd.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
#include "api/usb/host/usbh_hub.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
#include "api/usb/host/usbh_cdc.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
#include "api/usb/host/usbh_vendor.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
#include "api/usb/host/usbh_adb.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
#include "api/usb/host/usbh_aoa.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
#include "api/usb/host/usbh_usbmuxd.h"
#endif
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
#include "api/usb/host/usbh_iap2.h"
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


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_class_buf_init(void);
usbh_class_t* malloc_usbh_class(void);
void free_usbh_class(usbh_class_t* pclass);
usbh_class_t* get_usbh_class(uint8_t class_id);
dev_type_t usbh_match_class( uint8_t id, usbh_class_t *pclass);
usbh_class_t *usbh_class_find_by_type(uint8_t id, dev_type_t type,uint8_t sub_type);
uint8_t usbh_class_find_by_type_all(dev_type_t type,uint8_t sub_type, usbh_class_t **ppcalss);
error_t usbh_class_in(dev_type_t type,uint8_t sub_type,uint8_t* buf, uint16_t* plen, uint16_t timeout_ms);
error_t usbh_class_out(dev_type_t type,uint8_t sub_type,uint8_t* buf, uint16_t len);
void usbh_class_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
void usbh_class_handler(uint32_t period_10us);
error_t  usbh_class_init(uint8_t id,usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t  usbh_class_open(uint8_t id);
error_t  usbh_class_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





