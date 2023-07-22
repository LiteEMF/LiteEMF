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


#ifndef _usbh_audio_h
#define _usbh_audio_h
#include "emf_typedef.h" 
#include "api/usb/usb_typedef.h"
#include "api/usb/usb_audio_typedef.h"
#include "api/usb/host/usbh_core.h"

#include "api/audio/api_audio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern api_audio_t usbh_audio_info;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_audio_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_audio( uint8_t id, usbh_class_t *pclass);
error_t usbh_audio_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_audio_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_audio_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_audio_task(uint8_t id, usbh_class_t *pclass);


#ifdef __cplusplus
}
#endif
#endif





