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


#ifndef _usbh_hid_h
#define _usbh_hid_h
#include "emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/hid/hid_typedef.h"
#include "api/usb/host/usbh_core.h"
#include "api/hid/hid_desc_parser.h"

#include  "api/usb/host/usbh_hid_km.h"
#include  "api/usb/host/usbh_hid_gamepad.h"
#include  "api/usb/host/usbh_hid_vendor.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct 
{
	uint8_t mask;			//0: free, 非0: used
	hid_type_t 	hid_type;
    hid_items_t item;		//占位用
} usbh_items_head_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbh_hid_set_idle(uint8_t id, uint8_t itf);  
error_t usbh_hid_set_report(uint8_t id, uint8_t itf, hid_report_type_t type, uint8_t rep_id, uint8_t* buf, uint16_t len);  
error_t usbh_hid_get_report(uint8_t id, uint8_t itf, hid_report_type_t type, uint8_t rep_id, uint8_t* buf, uint16_t *plen);  
error_t usbh_hid_get_report_desc( uint8_t id, uint8_t index, uint16_t itf, uint8_t* buf, uint16_t *plen);  
uint16_t usbh_get_hid_desc_len(uint8_t* buf ,uint16_t len);


bool usbh_hid_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_hid( uint8_t id, usbh_class_t *pclass);
error_t usbh_hid_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_hid_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_hid_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_hid_task(uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





