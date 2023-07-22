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


#ifndef _usbd_iap2_h
#define _usbd_iap2_h
#include "emf_typedef.h" 
#include "api/usb/device/usbd_core.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern uint16_t usbd_iap_hid_types[USBD_NUM];





/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_iap2_reset(uint8_t id);
error_t usbd_iap2_suspend(uint8_t id);
uint16_t usbd_iap2_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_iap2_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq);
error_t usbd_iap2_out_process(uint8_t id, usbd_class_t *pclass);
void 	usbd_iap2_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_iap2_init(uint8_t id);
error_t usbd_iap2_deinit(uint8_t id);


#ifdef __cplusplus
}
#endif
#endif





