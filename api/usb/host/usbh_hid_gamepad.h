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


#ifndef _usbh_hid_gameapd_h
#define _usbh_hid_gameapd_h
#include "emf_typedef.h"
#include "api/usb/usb_typedef.h"
#include "api/usb/host/usbh_core.h"
#include  "api/hid/hid_desc_parser.h"

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
void usbh_hid_gamepad_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_hid_gamepad_open( uint8_t id, usbh_class_t *pclass) ;
error_t usbh_hid_gamepad_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo);
error_t usbh_hid_gamepad_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_hid_gamepad_handler(uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





