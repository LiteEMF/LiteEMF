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


#ifndef _io_keyboard_h
#define _io_keyboard_h
#include "emf_typedef.h" 
#include "api/api_gpio.h"
#include "km_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef IO_KB_ACTIVE				//high level trigger , normal low level trigger
#define IO_KB_ACTIVE 0  			
#endif
#ifndef MATRIX_KB_ACTIVE			//matrix key
#define MATRIX_KB_ACTIVE	0
#endif
#ifndef MATRIX_KB_DELAY
#define MATRIX_KB_DELAY()	api_delay_ns(0);
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t io_keyboard_scan(app_kb_t* pkey, app_kb_bit_t *pkey_bit);
bool io_keyboard_init(void);
bool io_keyboard_deinit(void);


#ifdef __cplusplus
}
#endif
#endif





