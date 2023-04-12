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

#ifndef	_switch_nfc_h
#define	_switch_nfc_h

#include "emf_typedef.h"
#include "app/gamepad/switch_typedef.h"
#include "api/nfc/api_nfc.h"

typedef struct {
	api_nfc_state_t nfc_ic_state;
    switch_mcu_report_id_t mcu_report_id;               //nfc MCU report id
    mcu_state_t mcu_state;
    nfc_state_t nfc_state;
    uint16_t stable_num;                            //发送请求指令后需多次读取/发送等待稳定
    uint8_t mcu_resum;
}switch_nfc_t;

/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
void switch_nfc_replies_pack(switch_nfc_t *pnfc, switch_large_report_t* switchp);
void switch_nfc_req_process(switch_nfc_t *pnfc,uint8_t* buf,uint16_t len);
bool switch_nfc_emun_process(switch_nfc_t *pnfc,switch_enum_t* enump, uint8_t* buf,uint16_t len);
void switch_nfc_init(switch_nfc_t *pnfc);

#endif

