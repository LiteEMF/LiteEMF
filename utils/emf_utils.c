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
#include  "utils/emf_utils.h"

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
** Parameters:	pcurve_shape: 0~100	, shape_cnt: 折线点个数
** Returns:	    
** Description:	Curve shaping 多点折线比例转换
@ref: https://github.dev/FreeJoy-Team/FreeJoy ShapeFunc	
*******************************************************************/
int16_t curve_shape_remap(int16_t value, uint8_t* pcurve_shape, uint8_t shape_cnt)
{
	int32_t out_min, out_max, step;
	int32_t in_min, in_max;
	uint8_t min_index;
	int16_t ret;

    int8_t i, shape_step = 100 / (shape_cnt-1);

    // check if is not linear shape_cnt-1必须能被10整除
	for(i=0; i<shape_cnt; i++){
        if(pcurve_shape[i] != i*shape_step){
            break;
        }
    }
    if(i == shape_cnt) return value;            // is line

    step = (0 - INT16_MIN) / (shape_cnt-1);
    min_index = value / step;
    
    if (min_index == shape_cnt-1) min_index = shape_cnt-2;
    
    in_min = min_index*step;
    in_max = (min_index+1)*step;
    
    out_min = ((int32_t)pcurve_shape[min_index] * (int32_t)(0 - INT16_MIN)/100);
    out_max = ((int32_t)pcurve_shape[min_index+1] * (int32_t)(0 - INT16_MIN)/100);
    
    ret = remap(value, in_min, in_max, out_min, out_max);

    ret = constrain_int16(ret);

	return(ret);
}

void curve_shape_remap_axis2i(axis2i_t* paxis, uint8_t* pcurve_shape, uint8_t shape_cnt)
{
    paxis->x = curve_shape_remap(paxis->x, pcurve_shape, shape_cnt);
    paxis->y = curve_shape_remap(paxis->y, pcurve_shape, shape_cnt);
}
void curve_shape_remap_axis3i(axis3i_t* paxis, uint8_t* pcurve_shape, uint8_t shape_cnt)
{
    paxis->x = curve_shape_remap(paxis->x, pcurve_shape, shape_cnt);
    paxis->y = curve_shape_remap(paxis->y, pcurve_shape, shape_cnt);
    paxis->z = curve_shape_remap(paxis->y, pcurve_shape, shape_cnt);
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

/*******************************************************************
** Parameters:	bit_offset: 数据bit偏移
                bit_lens: 数据 bit 长度, bit_lens < 32
                buf: bit 数据流
** Returns:	
** Description:	从bit 小端数据流中获取int数据	
*******************************************************************/
int32_t bits_to_int(uint16_t bit_offset, uint8_t bit_lens,  uint8_t* bbuf, uint8_t len)
{
    uint32_t mask, val = 0;
    int32_t ret = 0;
    uint8_t byte_offset = bit_offset/8;

    if(len > byte_offset){
        bit_offset %= 8;
        memcpy(&val, &bbuf[byte_offset], MIN(len - byte_offset, 4));
        val = SWAP32_L(val);            //fix BIG ENDP CUP
        mask = (0X01UL << bit_lens) - 1;

        ret = (val >> bit_offset) & mask;
        ret = BVAL2INT(ret,bit_lens);
    }
    return ret;
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

//快速求平方根倒数
float invSqrt(float x)
{
	// float halfx = 0.5f * x;
	// float y = x;
	// long i = *(long*)&y;

	// i = 0x5f3759df - (i>>1);
	// y = *(float*)&i;
	// y = y * (1.5f - (halfx * y * y));

	// return y;

	union{  
		int32_t i;  
		float   f; 
	} conv; 
	
	conv.f = x; 
	conv.i = 0x5f3759df - (conv.i >> 1); 
	return 0.5f * conv.f * (3.0f - x * conv.f * conv.f);
}
