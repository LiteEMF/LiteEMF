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

#ifndef	_ps_crypt_h
#define	_ps_crypt_h
#include "hw_config.h"
#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "api/api_tick.h"
#include "app/gamepad/app_gamepad_key.h"
#include "app/gamepad/p2.h"
#include "app/gamepad/nxp7105.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/

#if defined PS_P2_ENCRYPT_ENABLED
#define PS_READ_SECURITY_LEN		256
#elif defined PS_7105_ENCRYPT_ENABLED
#define PS_READ_SECURITY_LEN		1040
#endif

typedef enum _ps_encrypt_step_t{
	PS_IDLE = 0,
	PS_CHALLENGE,				//主机发送加密 KEY
	PS_WRITE_SECURITY,			//写加密KEY 到加密芯片
	PS_READ_SECURITY,			//读取加密后的KEY
	PS_CALCULATE_PHASE2,		//7105
	PS_ANSWER,					//回复主机
}ps_encrypt_step_t;


typedef struct {
	ps_encrypt_step_t step;
	uint16_t			len;
	uint8_t				index;
	uint8_t				cmd_index;
}ps_encrypt_t;
extern ps_encrypt_t	ps_encrypt;
extern uint8_t ps_encrypt_buf[1040];

/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
bool os_ps_task_en(bool en);				//weak
bool ps_encrypt_start(uint8_t cmd_index);
bool ps_encrypt_stop(void);
void ps_encrypt_task(void *pa);
bool ps_encrypt_init(void);

#endif

