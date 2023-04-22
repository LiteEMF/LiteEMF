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


#ifndef _usbh_aoa_h
#define _usbh_aoa_h
#include "emf_typedef.h"
#include "api/usb/usb_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#define AOA_CLASS              0xff
#define AOA_SUBCLASS           0xff
#define AOA_PROTOCOL           0x00


#define GOOGLE_VID                     0x18D1
#define GOOGLE_PID_ACCESSORY           0x2D00
#define GOOGLE_PID_ACCESSORY_ADB       0x2D01
#define GOOGLE_PID_AUDIO               0x2D02
#define GOOGLE_PID_AUDIO_ADB           0x2D03
#define GOOGLE_PID_ACCESSORY_AUDIO     0x2D04
#define GOOGLE_PID_ACCESSORY_AUDIO_ADB 0x2D05


#define ACCESSORY_GET_PROTOCOL_VERSION 51  			/*0x33*/
#define ACCESSORY_SEND_STRING          52			/*0x34*/
#define ACCESSORY_START                53			/*0x35*/
#define ACCESSORY_REGISTER_HID         54			/*0x36*/
#define ACCESSORY_UNREGISTER_HID       55			/*0x37*/
#define ACCESSORY_SET_HID_REDPORT_DESC 56			/*0x38*/
#define ACCESSORY_SEND_HID_EVENT       57			/*0x39*/
#define ACCESSORY_SET_AUDIO_MODE       58			/*0x3A*/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern uint16_t aoa_hid_types;



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_aoa_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_aoa( uint8_t id, usbh_class_t *pclass);
error_t usbh_aoa_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_aoa_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_aoa_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_aoa_task( uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





