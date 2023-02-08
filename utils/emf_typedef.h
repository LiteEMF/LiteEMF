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
#include "hal/hal_typedef.h"
//std include
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stddef.h"

#include "utils/emf_error.h"


#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
	
#ifndef FALSE 
#define FALSE	false
#endif
#ifndef TRUE
#define TRUE 	true
#endif
#define NOT(a)	 ((bool)!(a))
#ifndef BIT
#define BIT(n)              (1UL << (n))
#endif
#ifndef _STR
#define _STR(arg)  #arg      //转义宏,防止宏定义提前展开
#endif
#ifndef STR
#define STR(arg)  _STR(arg)     //将任意输入转换为字符串
#endif
#ifndef BOOL_SET
#define BOOL_SET(a)		( (0 == (a))? false:true)
#endif
#ifndef MAX
#define MAX(a,b)	( ( (a) > (b) )? (a):(b) )
#endif
#ifndef MIN
#define MIN(a,b)	( ( (a) > (b) )? (b):(a) )
#endif
#ifndef IS_POWER_OF_TWO                     // is a = 2^n
#define IS_POWER_OF_TWO(a) ( ((a) != 0) && ((((a) - 1) & (a)) == 0) )
#endif
#ifndef countof
#define countof(a) (sizeof(a) / sizeof(*(a)))
#endif
/*
Constrains a number to be within a range return:
x: if x is between a and b.
a(low): if x is less than a.
b(high): if x is greater than b.
*/
#ifndef CONSTRAIN
#define CONSTRAIN(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x))) 
#endif
#ifndef REMAP
#define REMAP(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif
#ifndef VAL2FLD
#define VAL2FLD(field_str, value) (((uint32_t)(value) << (field_str##_POS)) & (field_str##_MASK))
#endif
#ifndef FLD2VAL
#define FLD2VAL(field_str, value) (((uint32_t)(value)&(field_str##_MASK)) >> (field_str##_POS))
#endif



#ifndef SWAP16
#define SWAP16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))  
#endif
#ifndef SWAP24           
#define SWAP24(l) (((l) & 0x00ff0000) >> 16)  | \
			   (((l) & 0x000000ff) << 16) 
#endif
#ifndef SWAP32
#define SWAP32(l) (((l) >> 24) | \
           (((l) & 0x00ff0000) >> 8)  | \
           (((l) & 0x0000ff00) << 8)  | \
           ((l) << 24))  
#endif



/* This structure is used for 'bytes' arrays.
 * It has the number of bytes in the beginning, and after that an array.
 * Note that actual structs used will have a different length of bytes array.
 */
#define BYTES_ARRAY_SIZE(n) ((uint16_t)n + offsetof(pb_bytes_array_t, bytes))
typedef struct _bytes_array_t {
    uint16_t size;
    uint8_t bytes[1];
}bytes_array_t;


#ifdef __BIG_ENDIAN
    #ifndef SWAP16_L
    #define SWAP16_L(x) SWAP16((uint16_t)(x))
    #endif
    #ifndef SWAP32_L
    #define SWAP32_L(x) SWAP32((uint32_t)(x))
    #endif
    #ifndef SWAP16_H
    #define SWAP16_H(x) (x)
    #endif
    #ifndef SWAP32_H
    #define SWAP32_H(x) (x)
    #endif
#else
    #ifndef SWAP16_L
    #define SWAP16_L(x) (x)
    #endif
    #ifndef SWAP32_L
    #define SWAP32_L(x) (x)
    #endif
    #ifndef SWAP16_H
    #define SWAP16_H(x) SWAP16((uint16_t)(x))
    #endif
    #ifndef SWAP32_H
    #define SWAP32_H(x) SWAP32((uint32_t)(x))
    #endif
#endif

#ifndef UNUSED_VARIABLE
	#if defined(__C51__)
	#define UNUSED_VARIABLE(X)  (X = X)
	#else
	#define UNUSED_VARIABLE(X)  ((void)(X))
	#endif
#endif
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(X) UNUSED_VARIABLE(X)
#endif

#ifndef MODULE_ENABLE
#define MODULE_ENABLE(module) (defined(##module) && (##module))
#endif


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

