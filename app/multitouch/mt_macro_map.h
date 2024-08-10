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


#ifndef _mt_macro_map_h
#define _mt_macro_map_h
#include "emf_typedef.h" 
#include "app/multitouch/mt_map_setting.h"

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
mt_map_t* find_macro_map_by_id(uint16_t macro_id, uint16_t index, api_storage_map_t* macro_map);
mt_map_t* get_next_macro(mt_map_t* mapp, api_storage_map_t* macro_map);
bool macro_map_add(mt_map_t* macrop, api_storage_map_t* macro_map);
bool macro_map_remove(uint16_t macro_id, api_storage_map_t* macro_map);


#ifdef __cplusplus
}
#endif
#endif





