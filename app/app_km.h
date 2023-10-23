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
#include "api/api_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef APP_BIT_KB_ENABLE			//全键无冲使能
#define APP_BIT_KB_ENABLE			0
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct 
{
	uint8_t active; 		//数据有改变
	uint8_t res;
	app_mouse_t mouse;
	app_kb_t kb;
} app_km_t;


extern app_km_t m_app_km;

#if APP_BIT_KB_ENABLE
extern kb_bit_t m_kb_bit;			//全键无冲突
#endif

/*****************************************************************************************************
**  Function
******************************************************************************************************/
__WEAK void app_km_vendor_scan(app_km_t* pkey, kb_bit_t *pkey_bit);			//__WEAK
void app_km_clean(app_km_t* pkey);
bool app_km_fill(app_km_t* pkey, app_km_t* pkey_in);
bool app_km_cache(app_km_t* pkey, app_km_t* pkey_in);
bool app_mouse_key_send(trp_handle_t *phandle,app_mouse_t *pmouse);
bool app_kb_key_send(trp_handle_t *phandle,app_kb_t *pkey);
bool app_km_init(void);
bool app_km_deinit(void);
void app_km_scan_task(void*pa);
void app_km_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





