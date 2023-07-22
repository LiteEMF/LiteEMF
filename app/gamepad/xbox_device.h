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
#ifndef	_xbox_commander_h
#define	_xbox_commander_h

#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/app_rumble.h"

#include "app/gamepad/xbox_typedef.h"
#include "app/gamepad/xbox_command.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
uint16_t xbox_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len);

void get_xbox_info(xbox_info_t* infp);
bool xbox_earphone_enable(trp_handle_t *phandle,xbox_command_t* cmdp,bool enable);
bool xbox_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len);

void xbox_device_init(trp_handle_t *phandle);
void xbox_device_deinit(trp_handle_t *phandle);
void xbox_device_task(trp_handle_t *phandle);


#endif

