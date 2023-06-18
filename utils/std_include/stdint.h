/* Copyright (C) ARM Ltd., 1999,2014 */
/* All rights reserved */

/*
 * RCS $Revision: 185525 $
 * Checkin $Date: 2014-05-29 12:44:48 +0100 (Thu, 29 May 2014) $
 * Revising $Author: agrant $
 */

#ifndef __stdint_h
#define __stdint_h





/*
 * 'signed' is redundant below, except for 'signed char' and if
 * the typedef is used to declare a bitfield.
 */

    /* 7.18.1.1 */

    /* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed  short int16_t;
typedef   signed           long int32_t;


    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned      short uint16_t;
typedef unsigned           long uint32_t;


    /* 7.18.1.2 */

    /* smallest type of at least n bits */
    /* minimum-width signed integer types */
typedef   signed          char int_least8_t;
typedef   signed      short int_least16_t;
typedef   signed           long int_least32_t;


    /* minimum-width unsigned integer types */
typedef unsigned          char uint_least8_t;
typedef unsigned      short uint_least16_t;
typedef unsigned           long uint_least32_t;


    /* 7.18.1.3 */

    /* fastest minimum-width signed integer types */
typedef   signed           char int_fast8_t;
typedef   signed           short int_fast16_t;
typedef   signed           long int_fast32_t;


    /* fastest minimum-width unsigned integer types */
typedef unsigned           char uint_fast8_t;
typedef unsigned           short uint_fast16_t;
typedef unsigned           long uint_fast32_t;



    /* minimum values of exact-width signed integer types */
#define INT8_MIN                   -128
#define INT16_MIN                -32768
#define INT32_MIN          (~0x7fffffff)   /* -2147483648 is unsigned */


    /* maximum values of exact-width signed integer types */
#define INT8_MAX                    127
#define INT16_MAX                 32767
#define INT32_MAX            2147483647


    /* maximum values of exact-width unsigned integer types */
#define UINT8_MAX                   255
#define UINT16_MAX                65535
#define UINT32_MAX           4294967295u


    /* 7.18.2.2 */

    /* minimum values of minimum-width signed integer types */
#define INT_LEAST8_MIN                   -128
#define INT_LEAST16_MIN                -32768
#define INT_LEAST32_MIN          (~0x7fffffff)


    /* maximum values of minimum-width signed integer types */
#define INT_LEAST8_MAX                    127
#define INT_LEAST16_MAX                 32767
#define INT_LEAST32_MAX            2147483647


    /* maximum values of minimum-width unsigned integer types */
#define UINT_LEAST8_MAX                   255
#define UINT_LEAST16_MAX                65535
#define UINT_LEAST32_MAX           4294967295u


    /* 7.18.2.3 */

    /* minimum values of fastest minimum-width signed integer types */
#define INT_FAST8_MIN           (~0x7fffffff)
#define INT_FAST16_MIN          (~0x7fffffff)
#define INT_FAST32_MIN          (~0x7fffffff)


    /* maximum values of fastest minimum-width signed integer types */
#define INT_FAST8_MAX             2147483647
#define INT_FAST16_MAX            2147483647
#define INT_FAST32_MAX            2147483647


    /* maximum values of fastest minimum-width unsigned integer types */
#define UINT_FAST8_MAX            4294967295u
#define UINT_FAST16_MAX           4294967295u
#define UINT_FAST32_MAX           4294967295u


#endif /* __stdint_h */

/* end of stdint.h */
