/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/


#ifndef _emf_utils_h
#define _emf_utils_h
#include "emf_typedef.h"
#include "utils/emf_mem.h"
#include "utils/app_fifo.h"
#include "utils/at_commander.h"
#include "utils/filter.h"
#include "utils/vector.h"
#include "utils/crc/crc.h"
#include "utils/check_sum.h"
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
int32_t remap(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
void swap_buf(uint8_t *buf, uint16_t len);
int8_t constrain_int8(int32_t val);
int16_t constrain_int16(int32_t val);
int8_t negative_int8(int8_t val);
int16_t negative_int16(int16_t val);
void int_to_bit12(uint8_t* buf, int16_t x, int16_t y);
void  bit12_to_uint(uint8_t* buf, uint16_t* px, uint16_t*py);
void  bit12_to_int(uint8_t* buf, int16_t* px, int16_t*py);
uint8_t get_bit_pos(uint32_t val);

#ifdef __cplusplus
}
#endif
#endif





