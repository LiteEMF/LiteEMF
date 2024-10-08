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


#ifndef _app_gamepad_h
#define _app_gamepad_h
#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/app_rumble.h"



#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK
#include  "app/gamepad/switch_controller.h"
#endif
#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
#include  "app/gamepad/ps_controller.h"
#endif
#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK
#include  "app/gamepad/xbox_controller.h"
#endif
#include  "app/gamepad/gamepad_controller.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
hid_type_t app_gamepad_get_hidtype(uint16_t hid_types);
hid_type_t app_gamepad_get_vid_pid(trp_t trp, uint16_t hid_types, uint16_t* vidp, uint16_t* pidp);

uint8_t app_gamepad_get_vendor_map(trp_handle_t *phandle,uint32_t(**mapp)[2]);			//__WEAK
uint8_t app_gamepad_get_map(trp_handle_t *phandle,uint32_t(**mapp)[2]);
uint32_t app_gamepad_map_convert(uint32_t key, uint32_t(* map)[2], uint8_t map_count);
uint32_t app_gamepad_map_invconvert(uint32_t key, uint32_t(* map)[2], uint8_t map_count);
uint32_t app_gamepad_key_convert(trp_handle_t *phandle, uint32_t key_in);
uint32_t app_gamepad_key_invconvert(trp_handle_t *phandle,uint32_t key_in);

bool app_gamepad_key_send(trp_handle_t *phandle,app_gamepad_key_t *keyp);
bool app_gamepad_std_trp_send(uint8_t id, trp_t trp, app_gamepad_key_t *keyp);
bool app_gamepad_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len);
bool app_gamepad_rumble_send(trp_handle_t *phandle, rumble_t const *prumble);
bool app_gamepad_host_process(trp_handle_t* phandle,app_gamepad_key_t *pkey, uint8_t *buf, uint8_t len);
bool app_gamepad_init( trp_handle_t *phandle );
bool app_gamepad_deinit( trp_handle_t *phandle );
void app_gamepad_task(trp_handle_t *phandle);

#ifdef __cplusplus
}
#endif
#endif





