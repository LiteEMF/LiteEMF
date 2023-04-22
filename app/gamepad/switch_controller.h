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

#ifndef	_switch_controller_h
#define	_switch_controller_h

#include "emf_typedef.h"
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/app_rumble.h"

#include "app/gamepad/switch_host.h"
#include "app/gamepad/switch_device.h"




extern switch_ctb_t switch_dev_ctb;
/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
bool switch_controller_init(trp_handle_t *phandle);
bool switch_controller_deinit(trp_handle_t *phandle);
void switch_controller_task(trp_handle_t *phandle);



#endif

