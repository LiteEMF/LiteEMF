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


#ifndef _app_km_h
#define _app_km_h
#include "emf_typedef.h"
#include "km_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct 
{
	uint8_t id;
	uint8_t but;
	int16_t x;	
	int16_t y;	
	int8_t w;	
	int8_t res;		//占位
} app_mouse_t;


typedef struct 
{
	uint8_t id;
	uint8_t but;
	int16_t x;	
	int16_t y;	
	int8_t w;	
	int8_t res;		//占位

    uint8_t fn;
    uint8_t key_res;
	uint8_t key[6];
} app_km_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_kb_vendor_scan(kb_bit_t* keyp);			//__WEAK
void app_mouse_vendor_scan(app_mouse_t* pmouse);	//__WEAK

bool app_km_init(void);
bool app_km_deinit(void);
void app_kb_scan_task(void*pa);
void app_mouse_scan_task(void*pa);
void app_km_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





