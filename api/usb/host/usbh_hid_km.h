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


#ifndef _usbh_hid_km_h
#define _usbh_hid_km_h
#include "emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/usb/host/usbh_core.h"
#include "api/hid/hid_desc_parser.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef MAX_KM_ITEMS_NUM
#define MAX_KM_ITEMS_NUM		6
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
/* 2.4G键鼠很多同时存在键盘和鼠标描述符, 所以键盘描述符里面会包含鼠标描述符,鼠标描述里面也会包含键盘描述符*/
typedef struct{
	uint8_t find;
	hid_items_t kb_fun;
	hid_items_t kb_normal;
	hid_items_t kb2_normal;	//部分键盘会有两个report id
	hid_items_t led;		
}kb_items_t;

typedef struct{
	uint8_t find;
	hid_items_t button;
	hid_items_t x;
	hid_items_t y;
	hid_items_t w;
}mouse_items_t;

typedef struct{					
	uint8_t		magic;			//0: free, 非0: used
	kb_items_t kb;
	mouse_items_t mouse;
} km_items_t;	


typedef union{
    struct{
        uint8_t numLock       : 1;
        uint8_t capsLock      : 1;
        uint8_t scrollLock    : 1;
        uint8_t res : 5;
    }bits;
    uint8_t val;
}kb_led_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_hid_km_pa_init(uint8_t id);
void usbh_hid_kb_set_led(kb_led_t *pled, uint8_t* pkb, uint8_t len);
void usbh_hid_km_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_hid_km_open( uint8_t id, usbh_class_t *pclass) ;
error_t usbh_hid_km_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo);
error_t usbh_hid_km_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_hid_km_task(uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





