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


#ifndef _hid_desc_h
#define _hid_desc_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "api/api_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

enum {					//为了节省蓝牙服务通过宏定义枚举类型
	DEFAULT_REPORT_ID = 0,
	#if HID_SUPPORT & BIT(HID_TYPE_GAMEPADE)
	GAMEPAD_REPORT_ID,
	#endif
	#if HID_SUPPORT & BIT(HID_TYPE_KB)
	KB_REPORT_ID,
	#endif
	#if HID_SUPPORT & BIT(HID_TYPE_MOUSE)
	MOUSE_REPORT_ID,
	#endif
	#if HID_SUPPORT & BIT(HID_TYPE_CONSUMER)
	CONSUMER_REPORT_ID,
	#endif
	#if HID_SUPPORT & BIT(HID_TYPE_MT)
	MT_REPORT_ID,
	#endif
	#if HID_SUPPORT & BIT(HID_TYPE_VENDOR)
	VENDOR_REPORT_ID,
	#endif
	REPORT_ID_NUM,
};


#ifndef KB_MAX_NUM		
#define KB_MAX_NUM					6	//stadend keyboard report key num
#endif
#ifndef KB_BIT_MAP_ENABLE		
#define KB_BIT_MAP_ENABLE			1
#endif

#ifndef MT_CONTACT_NUM		
#define MT_CONTACT_NUM				5
#endif
#ifndef MT_X_LOGICAL_MAX	
#define MT_X_LOGICAL_MAX			4096
#endif
#ifndef MT_Y_LOGICAL_MAX	
#define MT_Y_LOGICAL_MAX			4096
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef uint8_t 	hid_type_t;		//see @ref HID_TYPE_NONE
typedef uint16_t 	hid_types_t;	//see @ref HID_TYPE_NONE



/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t get_hid_desc_map(trp_t trp, hid_type_t hid_type ,uint8_t** ppmap);

bool hid_report_desc_init(void);
bool hid_report_desc_deinit(void);

#ifdef __cplusplus
}
#endif
#endif





