/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/


#ifndef _pinmap_h
#define _pinmap_h
#include "utils/emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/* Private macro -------------------------------------------------------------*/
/* 消除waring: #1296-D:extended constant initialiser used*/
/* Suppress warning messages */
#if defined(__CC_ARM)
// Suppress warning message: extended constant initialiser used
#pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define PIN_NULL  (-1)

#define PA_00       0X00
#define PA_01       0X01
#define PA_02       0X02
#define PA_03       0X03
#define PA_04       0X04
#define PA_05       0X05
#define PA_06       0X06
#define PA_07       0X07
#define PA_08       0X08
#define PA_09       0X09
#define PA_10       0X0A
#define PA_11       0X0B
#define PA_12       0X0C
#define PA_13       0X0D
#define PA_14       0X0E
#define PA_15       0X0F

#define PB_00       0X10
#define PB_01       0X11
#define PB_02       0X12
#define PB_03       0X13
#define PB_04       0X14
#define PB_05       0X15
#define PB_06       0X16
#define PB_07       0X17
#define PB_08       0X18
#define PB_09       0X19
#define PB_10       0X1A
#define PB_11       0X1B
#define PB_12       0X1C
#define PB_13       0X1D
#define PB_14       0X1E
#define PB_15       0X1F

#define PC_00       0X20
#define PC_01       0X21
#define PC_02       0X22
#define PC_03       0X23
#define PC_04       0X24
#define PC_05       0X25
#define PC_06       0X26
#define PC_07       0X27
#define PC_08       0X28
#define PC_09       0X29
#define PC_10       0X2A
#define PC_11       0X2B
#define PC_12       0X2C
#define PC_13       0X2D
#define PC_14       0X2E
#define PC_15       0X2F

#define PD_00       0X30
#define PD_01       0X31
#define PD_02       0X32
#define PD_03       0X33
#define PD_04       0X34
#define PD_05       0X35
#define PD_06       0X36
#define PD_07       0X37
#define PD_08       0X38
#define PD_09       0X39
#define PD_10       0X3A
#define PD_11       0X3B
#define PD_12       0X3C
#define PD_13       0X3D
#define PD_14       0X3E
#define PD_15       0X3F

#define PE_00       0X40
#define PE_01       0X41
#define PE_02       0X42
#define PE_03       0X43
#define PE_04       0X44
#define PE_05       0X45
#define PE_06       0X46
#define PE_07       0X47
#define PE_08       0X48
#define PE_09       0X49
#define PE_10       0X4A
#define PE_11       0X4B
#define PE_12       0X4C
#define PE_13       0X4D
#define PE_14       0X4E
#define PE_15       0X4F

#define PF_00       0X50
#define PF_01       0X51
#define PF_02       0X52
#define PF_03       0X53
#define PF_04       0X54
#define PF_05       0X55
#define PF_06       0X56
#define PF_07       0X57
#define PF_08       0X58
#define PF_09       0X59
#define PF_10       0X5A
#define PF_11       0X5B
#define PF_12       0X5C
#define PF_13       0X5D
#define PF_14       0X5E
#define PF_15       0X5F

#define PG_00       0X60
#define PG_01       0X61
#define PG_02       0X62
#define PG_03       0X63
#define PG_04       0X64
#define PG_05       0X65
#define PG_06       0X66
#define PG_07       0X67
#define PG_08       0X68
#define PG_09       0X69
#define PG_10       0X6A
#define PG_11       0X6B
#define PG_12       0X6C
#define PG_13       0X6D
#define PG_14       0X6E
#define PG_15       0X6F

typedef uint8_t    pin_t;

/*
typedef enum{
    PA_00 = 0,
    PA_01,
    PA_02,
    PA_03,
    PA_04,
    PA_05,
    PA_06,
    PA_07,
    PA_08,
    PA_09,
    PA_10,
    PA_11,
    PA_12,
    PA_13,
    PA_14,
    PA_15,

    PB_00 = 0x10,
    PB_01,
    PB_02,
    PB_03,
    PB_04,
    PB_05,
    PB_06,
    PB_07,
    PB_08,
    PB_09,
    PB_10,
    PB_11,
    PB_12,
    PB_13,
    PB_14,
    PB_15,

    PC_00 = 0x20,
    PC_01,
    PC_02,
    PC_03,
    PC_04,
    PC_05,
    PC_06,
    PC_07,
    PC_08,
    PC_09,
    PC_10,
    PC_11,
    PC_12,
    PC_13,
    PC_14,
    PC_15,

    PD_00 = 0x30,
    PD_01,
    PD_02,
    PD_03,
    PD_04,
    PD_05,
    PD_06,
    PD_07,
    PD_08,
    PD_09,
    PD_10,
    PD_11,
    PD_12,
    PD_13,
    PD_14,
    PD_15,

    PE_00 = 0x40,
    PE_01,
    PE_02,
    PE_03,
    PE_04,
    PE_05,
    PE_06,
    PE_07,
    PE_08,
    PE_09,
    PE_10,
    PE_11,
    PE_12,
    PE_13,
    PE_14,
    PE_15,

    PF_00 = 0x50,
    PF_01,
    PF_02,
    PF_03,
    PF_04,
    PF_05,
    PF_06,
    PF_07,
    PF_08,
    PF_09,
    PF_10,
    PF_11,
    PF_12,
    PF_13,
    PF_14,
    PF_15,

    PG_00 = 0x60,
    PG_01,
    PG_02,
    PG_03,
    PG_04,
    PG_05,
    PG_06,
    PG_07,
    PG_08,
    PG_09,
    PG_10,
    PG_11,
    PG_12,
    PG_13,
    PG_14,
    PG_15,
} pin_t;
*/

typedef enum{
    PIN_PULLNONE = 0,
    PIN_PULLUP,
	PIN_PULLDOWN,
    PIN_PULL_OD,     //open drain开漏输出, 只有GPIO作为输出有效
}pin_pull_t;

typedef enum {
    PIN_OUT = 0,
    PIN_IN 	= 1,
}pin_dir_t;

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	pin_t pin;
	uint32_t peripheral;
	uint32_t att;
} pin_map_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint32_t get_pinmap_peripheral(pin_t pin, const pin_map_t* p_pinmap, uint8_t map_count);
uint32_t get_pinmap_att(pin_t pin, const pin_map_t* p_pinmap, uint8_t map_count);
uint8_t get_pinmap_index(pin_t pin,const pin_map_t *p_pinmap, uint8_t map_count);          

#ifdef __cplusplus
}
#endif
#endif





