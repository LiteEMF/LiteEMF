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

#ifndef _emf_math_h
#define _emf_math_h
#include "emf_typedef.h" 
#include "hw_config.h" 
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef SIN8_ENABLE
#define SIN8_ENABLE				0
#endif
#ifndef SIN8_TABLE_ENABLE
#define SIN8_TABLE_ENABLE		0
#endif

#ifndef SIN16_ENABLE
#define SIN16_ENABLE				0
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
int8_t sin8( uint8_t theta);
int8_t cos8( uint8_t theta);
int16_t sin16( uint16_t theta);
int16_t cos16( uint16_t theta);

#ifdef __cplusplus
}
#endif
#endif





