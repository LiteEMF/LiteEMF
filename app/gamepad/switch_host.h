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

#ifndef	_switch_host_h
#define	_switch_host_h

#include "emf_typedef.h"
#include "api/api_transport.h"
#include "app/app_key.h"
#include "app/app_rumble.h"
#if APP_NFC_ENABLE
#include "app/gamepad/switch_nfc.h"
#endif
#include "app/gamepad/switch_typedef.h"

extern switch_cal_t m_switch_cal;
extern switch_ctb_t switch_host_ctb;
#if APP_NFC_ENABLE
extern switch_nfc_t switch_host_nfc;
#endif
/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
bool switch_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_key_t*keyp);
uint8_t switch_motor_decode(switch_motor_bit_t* pmotor_bit);


bool switch_in_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len);

bool switch_host_init(trp_handle_t *phandle);
bool switch_host_deinit(trp_handle_t *phandle);
void switch_host_handler(trp_handle_t *phandle);


#endif

