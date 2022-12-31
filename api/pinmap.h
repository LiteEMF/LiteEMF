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
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define PIN_NC  (-1)

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

typedef enum{
    PIN_PULLNONE = 0,
    PIN_PULLUP,
	PIN_PULLDOWN,
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





