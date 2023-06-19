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


#ifndef _usbh_iap2_h
#define _usbh_iap2_h
#include "utils/emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/usb/host/usbh_core.h"

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
void usbh_iap2_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_iap2( uint8_t id, usbh_class_t *pclass);
error_t usbh_iap2_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_iap2_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_iap2_deinit( uint8_t id, usbh_class_t *pclass);
void usbh_iap2_task( uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





