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
#ifndef	_gamepad_controller_H
#define	_gamepad_controller_H

#include "emf_typedef.h"
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/app_key.h"
#include "app/app_rumble.h"
#include "api/hid/hid_dev_desc.h"

#include "app/gamepad/gamepad_typedef.h"

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/

/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
uint16_t gamepad_key_pack(trp_handle_t *phandle,app_key_t *gpadp, uint8_t* buf,uint16_t len);
bool gamepad_rumble_send(trp_handle_t *phandle, rumble_t const *prumble);
bool gamepad_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_key_t*keyp);
uint8_t gamepad_key_to_hatswitch(uint32_t value);
uint32_t gamepad_hatswitch_to_key(uint8_t hat_switch);

bool gamepad_in_process(trp_handle_t* phandle, uint8_t *buf, uint8_t len);
void gamepad_controller_init(trp_handle_t *phandle);
void gamepad_controller_deinit(trp_handle_t *phandle);
void gamepad_controller_handler(trp_handle_t *phandle);



#endif

