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


#ifndef _usbd_cdc_h
#define _usbd_cdc_h
#include "emf_typedef.h" 
#include "api/usb/device/usbd_core.h"
#include "api/usb/usb_cdc_typedef.h"

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
error_t usbd_cdc_reset(uint8_t id);
error_t usbd_cdc_suspend(uint8_t id);
uint16_t usbd_cdc_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_cdc_control_request_process(uint8_t id, usbd_class_t *pclass,  usbd_req_t* const preq);
error_t usbd_cdc_out_process(uint8_t id, usbd_class_t *pclass);
void 	usbd_cdc_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_cdc_init(uint8_t id);
error_t usbd_cdc_deinit(uint8_t id);


#ifdef __cplusplus
}
#endif
#endif





