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


#ifndef _app_gamepad_key_h
#define _app_gamepad_key_h
#include "emf_typedef.h" 
#include "app/key_typedef.h"
#include "utils/emf_utils.h"
#include "app/app_key.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
	uint32_t key;
	axis2i_t stick_l;
	axis2i_t stick_r;
	int16_t l2;
	int16_t r2;			//16
	axis3i_t acc;
	axis3i_t gyro;
}app_gamepad_key_t;		//28

extern uint8_t  m_app_stick_key;
extern app_gamepad_key_t m_gamepad_key;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_gamepad_key_vendor_scan(app_gamepad_key_t *keyp);		//__WEAK 
void app_gamepad_trigger_check(app_gamepad_key_t* keyp);
void app_gamepad_key_swapl(app_gamepad_key_t* keyp);

bool app_gamepad_key_init(void);
bool app_gamepad_key_deinit(void);
void app_gamepad_key_scan_task(app_gamepad_key_t *pkey);
void app_gamepad_key_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





