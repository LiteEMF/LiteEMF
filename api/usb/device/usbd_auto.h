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


#ifndef _usbd_auto_h
#define _usbd_auto_h
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
typedef enum _usbd_det_t{
	USBD_DET_PC=0,
	USBD_DET_ANDROID,
	USBD_DET_SWITCH,				
	USBD_DET_X360,
	USBD_DET_XBOX,
	USBD_DET_XBOXX,
	USBD_DET_PS3,
	USBD_DET_PS4,
	USBD_DET_PS5,
	USBD_DET_MAX,					//9
	USBD_DET_NULL,
}usbd_det_t;



extern usbd_det_t m_usbd_auto_type;				//自动识别到的设备





/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_auto_reset(uint8_t id);
error_t usbd_auto_suspend(uint8_t id);
uint16_t usbd_auto_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_auto_control_request_process(uint8_t id, usbd_class_t *pclass,  usbd_req_t* const preq);
error_t usbd_auto_out_process(uint8_t id, usbd_class_t *pclass);
void 	usbd_auto_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_auto_init(uint8_t id);
error_t usbd_auto_deinit(uint8_t id);

void usbd_auto_det_cb(uint8_t id, usbd_det_t dev);			//__WEAK
void usbd_auto_handler(uint8_t id, uint32_t period_10us);
void usbd_auto_task(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





