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


#ifndef _app_key_h
#define _app_key_h
#include "emf_typedef.h"
#include "key_typedef.h"
#include "io_key.h"
#include "emf_utils.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef KEY_PERIOD_DEFAULT
#define KEY_PERIOD_DEFAULT 			(10)			//10ms
#endif
#ifndef KEY_LONG_LONG_TIME
#define KEY_LONG_LONG_TIME 			(500)
#endif
#ifndef KEY_LONG_TIME
#define KEY_LONG_TIME 				(200)		
#endif
#ifndef KEY_SHORT_TIME
#define KEY_SHORT_TIME 	    		(100)			//short and pressed_b limte time		
#endif
#ifndef KEY_DOUBLE_B_TIME					//double_b time, pressed_b event delay time
#define KEY_DOUBLE_B_TIME			(20)			
#endif
#ifndef KEY_PRESSED_B_DELAY
#define KEY_PRESSED_B_DELAY 		(6)
#endif




/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
	uint32_t key;
	axis2i_t stick_l;
	axis2i_t stick_r;
	axis2i_t trigger;		//x:l, y:r
	axis3i_t acc;
	axis3i_t gyro;	
}app_key_t;

extern uint8_t  m_app_stick_key;
extern app_key_t m_app_key;

extern uint32_t m_key_pressed;
extern uint32_t m_key_short;
extern uint32_t m_key_pressed_b;
extern uint32_t m_key_long;
extern uint32_t m_key_long_long;
extern uint32_t m_key_double_b;	
extern uint32_t m_key_pre_double_b;
extern bool m_key_power_on;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_key_vendor_scan(app_key_t *keyp);		//__WEAK 
void app_key_event(void);						//__WEAK
void app_key_trigger_std(app_key_t* keyp);		//standard
void app_key_swapl(app_key_t* keyp);

bool app_key_init(void);
bool app_key_deinit(void);
void app_key_handler(void);

#ifdef __cplusplus
}
#endif
#endif





