/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/

#ifndef	_EMF_DEFINED_H
#define	_EMF_DEFINED_H


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
#ifndef _STR					//转义宏,防止宏定义提前展开
#define _STR(arg)  #arg
#endif
#ifndef STR						//将任意输入转换为字符串
#define STR(arg)  _STR(arg)
#endif
#define NOT(a)	 ((bool)!(a))
#ifndef BIT
#define BIT(n)              (1UL << (n))
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

// import from include/linux
#ifndef  offsetof
	#ifdef __compiler_offsetof
		#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
	#else
		#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
	#endif
#endif
/**
* container_of - cast a member of a structure out to the containing structure
* @ptr:        the pointer to the member.
* @type:       the type of the container struct this is embedded in.
* @member:     the name of the member within the struct.
* @example:
			test_t test;
			test_t *p = container_of(&test.list, test_t,list);
*
*/
#ifndef container_of
#define container_of(ptr, type, memb) \
		((type *)((char *)(ptr) - offsetof(type, memb)))
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
#ifndef _REMAP
#define _REMAP(x, in_min, in_max, out_min, out_max) (((x) - (in_min)) * ((out_max) - (out_min)) / ((in_max) - (in_min)) + (out_min))
#endif
#ifndef REMAP
#define REMAP(x, in_min, in_max, out_min, out_max) (x <= in_min)? (out_min) : ((x >= in_max)? (out_max) : _REMAP(x, in_min, in_max, out_min, out_max))
#endif

#ifndef VAL2FLD
#define VAL2FLD(field_str, value) (((uint32_t)(value) << (field_str##_POS)) & (field_str##_MASK))
#endif
#ifndef FLD2VAL
#define FLD2VAL(field_str, value) (((uint32_t)(value)&(field_str##_MASK)) >> (field_str##_POS))
#endif

//bit val to int. bit_len: bval bit len
#define BVAL2INT(bval, bit_len) ((bval) & (0X01UL << ((bit_len) - 1)))? (int32_t)((bval) | (0XFFFFFFFF << (bit_len))) : (int32_t)(bval);





#ifndef U16
#define U16(msb,lsb)	( ((uint16_t)(msb) << 8) | (uint16_t)(lsb) )
#endif
#ifndef U32
#define U32(msb,msbl,lsbh,lsb)	( ((uint32_t)(msb) << 24) | ((uint32_t)(msbl) << 16) | ((uint32_t)(lsbh) << 8) | (lsb) )
#endif

#ifndef U16_H
#define U16_H(pbuf)	 U16((pbuf)[0],(pbuf)[1])
#endif
#ifndef U32_H
#define U32_H(pbuf)	 U32((pbuf)[0],(pbuf)[1],(pbuf)[2],(pbuf)[3])
#endif
#ifndef U16_L
#define U16_L(pbuf)	 U16((pbuf)[1],(pbuf)[0])
#endif
#ifndef U32_L
#define U32_L(pbuf)	 U32((pbuf)[3],(pbuf)[2],(pbuf)[1],(pbuf)[0])
#endif

#ifndef SET_U16_H
#define SET_U16_H(pbuf)	 						\
do {                                      	\
        (pbuf)[0] = (uint8_t)((value) >> 8); \
        (pbuf)[1] = (uint8_t)((value) >> 0); \
    } while (0)
#endif
#ifndef SET_U24_H
#define SET_U24_H(pbuf, value)                 \
    do {                                       \
        (pbuf)[0] = (uint8_t)((value) >> 16); \
        (pbuf)[1] = (uint8_t)((value) >> 8);  \
        (pbuf)[2] = (uint8_t)((value) >> 0);  \
    } while (0)
#endif
#ifndef SET_U32_H
#define SET_U32_H(pbuf, value)                 \
    do {                                       \
        (pbuf)[0] = (uint8_t)((value) >> 24); \
        (pbuf)[1] = (uint8_t)((value) >> 16); \
        (pbuf)[2] = (uint8_t)((value) >> 8);  \
        (pbuf)[3] = (uint8_t)((value) >> 0);  \
    } while (0)
#endif

#ifndef SET_U16_L
#define SET_U16_L(pbuf)	 						\
do {                                      	\
        (pbuf)[1] = (uint8_t)((value) >> 8); \
        (pbuf)[0] = (uint8_t)((value) >> 0); \
    } while (0)
#endif
#ifndef SET_U24_L
#define SET_U24_L(pbuf, value)                 \
    do {                                       \
        (pbuf)[2] = (uint8_t)((value) >> 16); \
        (pbuf)[1] = (uint8_t)((value) >> 8);  \
        (pbuf)[0] = (uint8_t)((value) >> 0);  \
    } while (0)
#endif
#ifndef SET_U32_L
#define SET_U32_L(pbuf, value)                 \
    do {                                       \
        (pbuf)[3] = (uint8_t)((value) >> 24); \
        (pbuf)[2] = (uint8_t)((value) >> 16); \
        (pbuf)[1] = (uint8_t)((value) >> 8);  \
        (pbuf)[0] = (uint8_t)((value) >> 0);  \
    } while (0)
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


#ifdef __cplusplus
}
#endif

#endif

