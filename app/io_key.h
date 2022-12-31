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


#ifndef _io_key_h
#define _io_key_h
#include "emf_typedef.h"
#include "key_typedef.h"
#include "hal/hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef KEY_ACTIVE				//io key
#define KEY_ACTIVE 0  			//high level trigger , normal low level trigger
#endif
#ifndef MATRIX_KEY_ACTIVE		//matrix key
#define MATRIX_KEY_ACTIVE	0
#endif
#ifndef MATRIX_KEY_DELAY
#define MATRIX_KEY_DELAY()		delay_ns(0);
#endif


//power key
#ifndef POWER_KEY_ACTIVE
#define POWER_KEY_ACTIVE	0
#endif
#ifndef POWER_KEY_PULL
#define POWER_KEY_PULL		PIN_PULLNONE		//pin_pull_t 
#endif
#ifndef POWER_SWITCH_KEY
#define POWER_SWITCH_KEY	0					//used switch key
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint32_t io_key_scan(void);
bool io_key_init(void);
bool io_key_deinit(void);
void io_key_handler(void);

#ifdef __cplusplus
}
#endif
#endif





