/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "emf_utils.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/



/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
int32_t remap(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    if(x <= in_min) return out_min;
    if(x >= in_max) return out_max;
	return REMAP(x,in_min,in_max, out_min, out_max);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		swap buf
*******************************************************************/
void swap_buf(uint8_t *buf, uint16_t len)
{
    uint8_t temp;
    uint16_t i;
    for(i=0; i<len/2; i++)
    {
        temp = buf[i];
        buf[i] = buf[len -i -1];
        buf[len -i -1] = temp;
    }
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		swap buf
*******************************************************************/
int8_t constrain_int8(int32_t val)
{
    return CONSTRAIN(val,INT8_MIN,INT8_MAX);
}
int16_t constrain_int16(int32_t val)
{
	return CONSTRAIN(val,INT16_MIN,INT16_MAX);
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description: -128-->127, 127-->-128,miss -127
*******************************************************************/
int8_t negative_int8(int8_t val)
{
    int8_t ret;

    if(val >= INT8_MAX){
        ret = INT8_MIN;
    }else if(val <= INT8_MIN){
        ret = INT8_MAX;
    }else{
        ret = -val;
    }

	return (int8_t)ret;
}

/*******************************************************************
** Returns:	
** Description:
** Parameters:	-32768-->32767, 32767-->-32768,miss:-32767
*******************************************************************/
int16_t negative_int16(int16_t val)
{
	int16_t ret = val;

    if(val >= INT16_MAX){
        ret = INT16_MIN;
    }else if(val <= INT16_MIN){
        ret = INT16_MAX;
    }else{
        ret = -val;
    }

	return (int16_t)ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	默认小端 int <-> 3byte
*******************************************************************/
void int_to_bit12(uint8_t* buf, int16_t x, int16_t y)
{
	buf[0] = x;
	buf[1] = (x >> 8) + ((y & 0x0f)<<4);
	buf[2] = y >> 4;
}
void  bit12_to_uint(uint8_t* buf, uint16_t* px, uint16_t*py)
{
	int16_t x,y;
	
	x = ((uint16_t)(buf[1]&0x0f)<<8) + buf[0];
	y = (buf[1]>>4) + ((uint16_t)buf[2]<<4);

	*px = x;
	*py = y;
}
void  bit12_to_int(uint8_t* buf, int16_t* px, int16_t*py)
{
	int16_t x,y;
	
	x = ((uint16_t)(buf[1]&0x0f)<<8) + buf[0];
	y = (buf[1]>>4) + ((uint16_t)buf[2]<<4);

    if(x & 0X800) x |= 0XF000;
    if(y & 0X800) y |= 0XF000;

	*px = x;
	*py = y;
}

uint8_t get_bit_pos(uint32_t val)
{
    uint8_t i;
    uint32_t tmp = 0x01;
    for(i = 0; i < 32; i++){
        if(tmp & val){
            return i;
        }
        tmp <<= 1;
    }
    return i;
}

