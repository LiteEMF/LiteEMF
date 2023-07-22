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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/

#ifndef	_switch_device_h
#define	_switch_device_h

#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/app_rumble.h"
#if API_NFC_ENABLE
#include "app/gamepad/switch_nfc.h"
#endif

#include "app/gamepad/switch_typedef.h"

#if API_NFC_ENABLE
extern switch_nfc_t switch_dev_nfc;
#endif
extern switch_ctb_t switch_dev_ctb;
/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
uint16_t switch_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len);
uint8_t switch_rumble_decode(switch_rumble_bit_t* prumble_bit);
bool switch_dev_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len);		//switch 手柄数据处理


bool switch_device_init(trp_handle_t *phandle);
bool switch_device_deinit(trp_handle_t *phandle);
void switch_device_task(trp_handle_t *phandle);
#endif

