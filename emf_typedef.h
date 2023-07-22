/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/

#ifndef	_EMF_TYPEDEF_H
#define	_EMF_TYPEDEF_H
#include "compiler_abstraction.h"
#include "hal_typedef.h"
//std include
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "stddef.h"

#include "utils/emf_defined.h"
#include "utils/emf_error.h"


#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
#if defined(__C51__)
	#ifndef const_t
	#define const_t  code
	#endif

	#ifndef int8d_t
	typedef char  data             int8d_t;
	#endif
	#ifndef int16d_t
	typedef short data             int16d_t;
	#endif
	#ifndef int32d_t
	typedef long  data             int32d_t;
	#endif
	#ifndef int8i_t
	typedef char  idata            int8i_t;
	#endif
	#ifndef int16i_t
	typedef short idata            int16i_t;
	#endif
	#ifndef int32i_t
	typedef long  idata            int32i_t;
	#endif
	#ifndef uint8x_t
	typedef unsigned char  xdata   	uint8x_t;
	#endif
	#ifndef uint16x_t
	typedef unsigned short xdata     uint16x_t;
	#endif
	#ifndef uint32x_t
	typedef unsigned long  xdata     uint32x_t;
	#endif
	#ifndef uint8d_t
	typedef unsigned char  data     uint8d_t;
	#endif
	#ifndef uint16d_t
	typedef unsigned short data     uint16d_t;
	#endif
	#ifndef uint32d_t
	typedef unsigned long  data     uint32d_t;
	#endif
	#ifndef uint8i_t
	typedef unsigned char  idata    uint8i_t;
	#endif
	#ifndef uint16i_t
	typedef unsigned short idata    uint16i_t;
	#endif
	#ifndef uint32i_t
	typedef unsigned long  idata	uint32i_t;
	#endif
	#ifndef uint8c_t
	typedef const unsigned char  code	uint8c_t;
	#endif
	#ifndef uint16i_t
	typedef const unsigned short code	uint16c_t;
	#endif
	#ifndef uint32i_t
	typedef const unsigned long  code	uint32c_t;
	#endif
	#ifndef floatc_t
	typedef const float  code	floatc_t;
	#endif
	#ifndef doublec_t
	typedef const double  code	doublec_t;
	#endif
	
	#ifndef uintptr_t
	typedef unsigned long uintptr_t;
	#endif


#else
    #ifndef const_t
	#define const_t const
	#endif
	#ifndef int8d_t
	typedef char			int8d_t;
	#endif
	#ifndef int16d_t
	typedef short			int16d_t;
	#endif
	#ifndef int32d_t
	typedef long			int32d_t;
	#endif
	#ifndef int8i_t
	typedef char			int8i_t;
	#endif
	#ifndef int16i_t
	typedef short			int16i_t;
	#endif
	#ifndef int32i_t
	typedef long			int32i_t;
	#endif
	#ifndef uint8x_t
	typedef unsigned char		uint8x_t;
	#endif
	#ifndef uint8d_t
	typedef unsigned char		uint8d_t;
	#endif
	#ifndef uint16d_t
	typedef unsigned short		uint16d_t;
	#endif
	#ifndef uint32d_t
	typedef unsigned long		uint32d_t;
	#endif
	#ifndef uint8i_t
	typedef unsigned char		uint8i_t;
	#endif
	#ifndef uint16i_t
	typedef unsigned short		uint16i_t;
	#endif
	#ifndef uint32i_t
	typedef unsigned long		uint32i_t;
	#endif
	#ifndef uint8c_t
	typedef const unsigned char		uint8c_t;
	#endif
	#ifndef uint16i_t
	typedef const unsigned short	uint16c_t;
	#endif
	#ifndef uint32i_t
	typedef const unsigned long		uint32c_t;
	#endif
	#ifndef floatc_t
	typedef const float  floatc_t;
	#endif
	#ifndef doublec_t
	typedef const double  doublec_t;
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

