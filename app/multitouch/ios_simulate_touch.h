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


#ifndef _ios_simulate_touch_h
#define _ios_simulate_touch_h
#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "km_typedef.h"
#include "utils/emf_utils.h"
#include "app/multitouch/multitouch.h"


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
bool ios_touch_init(void);
bool ios_touch_deinit(void);
uint8_t ios_simulate_touch_tx(trp_handle_t* phandle, multitouch_t* pmt, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif





