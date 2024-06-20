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


#ifndef _usbh_hub_h
#define _usbh_hub_h
#include "emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/usb/usb_hub_typedef.h"

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
error_t usbh_hub_get_desc(uint8_t id, descriptor_hub_desc_t* pdesc);
error_t usbh_hub_port_clear_feature(uint8_t id, uint8_t feature);  
error_t usbh_hub_port_set_feature(uint8_t id, uint8_t feature); 
error_t usbh_hub_port_get_status(uint8_t id, uint8_t* pstatus);  
error_t usbh_hub_port_disable(uint8_t id);
error_t usbh_hub_port_reset(uint8_t id);


bool usbh_hub_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_hub( uint8_t id, usbh_class_t *pclass);
error_t usbh_hub_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_hub_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_hub_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_hub_task(uint8_t id, usbh_class_t *pclass);



#ifdef __cplusplus
}
#endif
#endif





