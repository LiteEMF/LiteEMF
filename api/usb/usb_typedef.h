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


#ifndef _usb_typedef_h
#define _usb_typedef_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
// @ref:https://www.usb.org/defined-class-codes
#define CLASS_AUDIO     			0x01
#define CLASS_COMMUNIC  			0x02
#define CLASS_HID       			0x03
#define CLASS_MONITOR   			0x04
#define CLASS_PHYSIC_IF 			0x05
#define CLASS_IMG	    			0x06
#define CLASS_PRINTER   			0x07
#define CLASS_MSD		   			0x08		//mass storage devices
#define CLASS_HUB       			0x09
#define CLASS_CDC					0x0a
#define CLASS_SMART_CARD			0x0B
#define CLASS_CONTENT_SECURITY		0x0C
#define CLASS_VIDEO					0x0E
#define CLASS_Health_care			0x0F		//保健
#define CLASS_AUDIO_VIDEO			0x10
#define CLASS_Billboard				0x11		//广告牌设备类
#define CLASS_USB_TYPECB			0x12
#define CLASS_i3c					0x3C
#define CLASS_Diagnostic			0xdc		//诊断设备
#define CLASS_WIRE_CTRL				0xE0		//无线控制器
#define CLASS_MIS					0xEF		//杂项
#define CLASS_Specific				0xFE		
#define CLASS_VENDOR    			0xFF

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool template_init(void);
bool template_deinit(void);
void template_handler(void);

#ifdef __cplusplus
}
#endif
#endif





