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

#ifndef	_ps_controller_h
#define	_ps_controller_h
#include "hw_config.h"
#include "emf_typedef.h"
#include "api/api_transport.h"
#include "app/app_rumble.h"
#include "app/gamepad/app_gamepad_key.h"

#include "app/gamepad/ps_typedef.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/

/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
uint32_t ps4_hatswitch_to_key(uint8_t hat_switch);
uint8_t ps4_key_to_hatswitch(uint32_t ps_key);
void ps_get_bt_mac(ps_bt_mac_t *pmac);
void ps_set_bt_link(ps_bt_link_t *plink);
void ps_touch_pack(app_gamepad_key_t *keyp);			//__WEAK
bool ps_touch_event_fill_id(uint8_t id,uint8_t active,int16_t x, int16_t y);
uint16_t ps_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len);
bool ps_dev_process(trp_handle_t* phandle, uint8_t* buf,uint8_t len);

bool ps_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp);
bool ps_rumble_send(trp_handle_t *phandle, rumble_t const *prumble);
bool ps_in_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len);

void ps_controller_init(trp_handle_t *phandle);
void ps_controller_deinit(trp_handle_t *phandle);
void ps_controller_task(trp_handle_t *phandle);


#endif

