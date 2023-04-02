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


#ifndef _usbh_adb_h
#define _usbh_adb_h
#include "emf_typedef.h"
#include "api/usb/usb_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define ADB_CLASS              0xff
#define ADB_SUBCLASS           0x42
#define ADB_PROTOCOL           0x1


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbh_adb_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len);
error_t usbh_match_adb( uint8_t id, usbh_class_t *pclass);
error_t usbh_adb_open( uint8_t id, usbh_class_t *pclass);
error_t usbh_adb_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len);
error_t usbh_adb_deinit( uint8_t id, usbh_class_t *pclass); 
void usbh_adb_handler( uint8_t id, usbh_class_t *pclass);

#ifdef __cplusplus
}
#endif
#endif





