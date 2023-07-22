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


#ifndef _check_sum_h
#define _check_sum_h
#include "emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern uint32_t check_sum(const void *buf, uint32_t len);
extern uint32_t check_sum_negative(const void *buf, uint32_t len);
#ifdef __cplusplus
}
#endif
#endif





