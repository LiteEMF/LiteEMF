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
#ifndef	_xbox_host_h
#define	_xbox_host_h

#include "utils/emf_typedef.h" 
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/app_rumble.h"

#include "app/gamepad/xbox_typedef.h"
#include  "app/gamepad/xbox_command.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#ifndef XBOX_HOST_AUDIO_ENABLE
#define XBOX_HOST_AUDIO_ENABLE	1
#endif 


extern bool is_xbox_uac_open;			//是否开启音频
extern uint8_t m_xbox_enum_step;
/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
#include "api/usb/usb_typedef.h"
//xbox device
void xbox_spk_transfer(uint8_t id, usb_endp_t *endpp, uint8_t* buf, uint16_t frame_len);
bool xbox_rumble_send(trp_handle_t *phandle, rumble_t const *prumble);
bool xbox_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp);
bool xbox_in_process(trp_handle_t* phandle, uint8_t* buf,uint8_t len);
void xbox_enum_process(trp_handle_t* phandle);

void xbox_host_init(trp_handle_t *phandle);
void xbox_host_deinit(trp_handle_t *phandle);
void xbox_host_task(trp_handle_t *phandle);

#endif

