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


#ifndef _usbh_h
#define _usbh_h
#include "utils/emf_typedef.h" 
#include "utils/emf_utils.h"
#include "utils/list.h"
#include "api/usb/host/usbh_core.h"
#include "api/usb/host/usbh_class.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/




extern uint16_t m_usbh_types;
extern uint16_t m_usbh_hid_types;
extern uint16_t usbh_iap_hid_types;

/*****************************************************************************************************
**  Function
******************************************************************************************************/

void usbh_reset_event(uint8_t id);						//__WEAK 
void usbh_endp_in_event(uint8_t id, uint8_t ep);		//__WEAK 
void usbh_endp_out_event(uint8_t id, uint8_t ep);		//__WEAK 

void usbh_det_event(uint8_t id, uint8_t attached );
error_t usbh_disconnect(uint8_t id);
error_t usbh_reset(uint8_t id,uint8_t reset_ms);
error_t usbh_set_address(uint8_t id,uint8_t addr);
uint8_t usbh_find_by_status(uint8_t usb_stas);   
error_t usbh_set_status(uint8_t id, usb_state_t usb_sta, uint8_t addr) ; 
error_t usbh_init( uint8_t id );
error_t usbh_deinit(uint8_t id);
void usbhs_init(void);
void usbhs_deinit(void);
void usbh_task(uint32_t dt_ms);
void usbh_handler(uint32_t period_10us);



#ifdef __cplusplus
}
#endif
#endif





