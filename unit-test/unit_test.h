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


#ifndef _unit_test_h
#define _unit_test_h
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



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_commander_test(void);
void api_soft_timer_test(void);
void crc_test(void);
void hid_desc_parser_test(void);
void list_test(void);
bool protobuf_test(void);
void mem_test(void);

#ifdef __cplusplus
}
#endif
#endif





