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


#ifndef _api_transporte_h
#define _api_transporte_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef uint8_t 	trp_t;			//see @ref TR_BLE
typedef uint16_t 	trps_t;			//see @ref TR_BLE
typedef uint8_t 	trp_type_t;		//sedd @ref TYPE_VENDOR
typedef uint16_t 	trp_types_t;	//sedd @ref TYPE_VENDOR

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_is_usb_trp(trp_t trp);
bool api_is_bt_trp(trp_t trp);

bool api_trp_init(void);
bool api_trp_deinit(void);
void api_trp_handler(void);

#ifdef __cplusplus
}
#endif
#endif





