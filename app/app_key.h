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
#include "hw_config.h"
#include "emf_typedef.h" 
#include "key_typedef.h"
#include "io_key.h"
#include "utils/emf_utils.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef KEY_PERIOD_DEFAULT					//*ms
#define KEY_PERIOD_DEFAULT 			(1)			
#endif
#ifndef KEY_LONG_LONG_TIME					//*(KEY_PERIOD_DEFAULT)ms
#define KEY_LONG_LONG_TIME 			(5000 / ( KEY_PERIOD_DEFAULT) )
#endif
#ifndef KEY_LONG_TIME
#define KEY_LONG_TIME 				(2000 / ( KEY_PERIOD_DEFAULT) )		
#endif
#ifndef KEY_SHORT_TIME						//short and pressed_b limte time	
#define KEY_SHORT_TIME 	    		(1000 / ( KEY_PERIOD_DEFAULT) )				
#endif
#ifndef KEY_DOUBLE_B_TIME					//double_b time, pressed_b limte time
#define KEY_DOUBLE_B_TIME			(200 / ( KEY_PERIOD_DEFAULT) )			
#endif
#ifndef KEY_PRESSED_B_DELAY					// pressed_b,double_b,three_b event delay time
#define KEY_PRESSED_B_DELAY 		(60 / ( KEY_PERIOD_DEFAULT) )
#endif


#ifndef KEY_DUMP_ENABLE
#define KEY_DUMP_ENABLE 			0
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint32_t pressed;
	uint32_t press_short;
	uint32_t pressed_b;
	uint32_t press_long;
	uint32_t long_long;
	uint32_t double_b;	
	uint32_t pre_double_b;
	uint32_t three_b;	
	uint32_t pre_three_b;
}app_key_t;


extern uint32_t m_key_scan;
extern bool m_key_power_on;
extern app_key_t m_app_key;



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_key_dump(uint32_t key);
void app_key_vendor_scan(uint32_t *pkey);			//__WEAK 
void app_key_event(void);							//__WEAK
bool app_key_init(void);
bool app_key_deinit(void);
void app_key_scan_task(void *pa);
void app_key_decode_task(uint32_t key_scan);
void app_key_handler(uint32_t period_10us);
void app_key_decode_handler(uint32_t period_10us,uint32_t key_scan);

#ifdef __cplusplus
}
#endif
#endif





