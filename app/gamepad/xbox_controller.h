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
#ifndef	_xbox_controller_H
#define	_xbox_controller_H

#include "emf_typedef.h"
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/app_key.h"
#include "app/app_rumble.h"

#include "app/gamepad/xbox_device.h"
#include "app/gamepad/xbox_host.h"

#include  "app/gamepad/xbox_command.h"

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


extern bool is_xbox_uac_open;			//是否开启音频
extern uint8_t m_xbox_enum_step;
/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/

void xbox_controller_init(trp_handle_t *phandle);
void xbox_controller_deinit(trp_handle_t *phandle);
void xbox_controller_handler(trp_handle_t *phandle);


#endif

