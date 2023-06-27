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
#include "utils/emf_typedef.h" 
#include "utils/emf_utils.h"
// #include "utils/list.h"
#include "api/usb/device/usbd_core.h"
#include "api/usb/device/usbd_class.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern char const* usbd_string_desc[4];

extern uint16_t m_usbd_types[USBD_NUM];
extern uint16_t m_usbd_hid_types[USBD_NUM];
/*****************************************************************************************************
**  Function
******************************************************************************************************/
char* usbd_user_get_string(uint8_t id, uint8_t index);		//__WEAK 
void usbd_user_set_device_desc(uint8_t id, usb_desc_device_t *pdesc); //__WEAK 

void usbd_reset_event(uint8_t id);								              //__WEAK
void usbd_suspend_event(uint8_t id);							              //__WEAK
void usbd_resume_event(uint8_t id);								              //__WEAK
void usbd_sof_event(uint8_t id);								                //__WEAK
void usbd_endp_in_event(uint8_t id ,uint8_t ep);				        //__WEAK
void usbd_endp_out_event(uint8_t id ,uint8_t ep, uint8_t len);	//__WEAK
void usbd_setup_event(uint8_t id,usb_control_request_t *pctrl_req ,uint8_t pctrl_len);	//__WEAK


error_t usbd_init(uint8_t id);
error_t usbd_deinit(uint8_t id);
void usbds_init(void);
void usbds_deinit(void);
void 	usbd_task( void *pa );
void usbd_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





