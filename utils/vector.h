/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/


#ifndef _victor_h
#define _victor_h
#include "emf_typedef.h"
#include "math.h"

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

#define pi 			(3.141592f)
#define RtA 		(180/pi)		//幅度转角度

typedef struct{
	float pitch;			//Pitch为俯仰角 y旋转，表示机头正方向与水平线的夹角，范围-90到+90；
	float roll;				//Roll为翻滚角 x旋转，表示机翼与水平线的夹角，范围：-180到+180。
	float yaw;				//Yaw为航向角 z旋转，表示机头偏离正北方多少度，范围-180到+180；
}angle_t;					//angle

typedef struct{				//axis
	int8_t x;
	int8_t y;
}axis2_t;

typedef struct{ 
	int16_t x;
	int16_t y;
}axis2i_t;

typedef struct{
	int32_t x;
	int32_t y;
}axis2l_t;

typedef struct{
	float x;
	float y;
}axis2f_t;


typedef struct{
	int8_t x;
	int8_t y;
	int8_t z;
}axis3_t;

typedef struct{
	int16_t x;	
	int16_t y;
	int16_t z;
}axis3i_t;

typedef struct{
	int32_t x;	
	int32_t y;
	int32_t z;
}axis3l_t;

typedef struct{
	float x;
	float y;
	float z;
}axis3f_t;


typedef struct{			//vector
	int8_t x;			//x sin_a
	int8_t y;			//y cos_a
	int8_t r;
}vector2_t;


typedef struct{			//vector
	int16_t x;			//x sin_a
	int16_t y;			//y cos_a
	int16_t r;
}vector2i_t;

typedef struct{			//vector
	int32_t x;			//x sin_a
	int32_t y;			//y cos_a
	int32_t r;
}vector2l_t;

typedef struct{			//vector
	float x;			//x sin_a
	float y;			//y cos_a
	float r;
}vector2f_t;

typedef struct{			//vector
	int8_t x;			//x sin_a
	int8_t y;			//y cos_a
	int8_t z;
	int8_t r;
}vector3_t;

typedef struct{			//vector
	int16_t x;			//x sin_a
	int16_t y;			//y cos_a
	int16_t z;
	int16_t r;
}vector3i_t;			//向量

typedef struct{			//vector
	int32_t x;			//x sin_a
	int32_t y;			//y cos_a
	int32_t z;
	int32_t r;
}vector3l_t;

typedef struct{			//vector
	float x;			//x sin_a
	float y;			//y cos_a
	float z;
	float r;
}vector3f_t;


void  axis3i_swapl( axis3i_t* p);
void  axis2i_swapl( axis2i_t* p);
void  axis3i_swaph( axis3i_t* p);
void  axis2i_swaph( axis2i_t* p);
void  axis3l_swapl( axis3l_t* p);
void  axis2l_swapl( axis2l_t* p);
void  axis3l_swaph( axis3l_t* p);
void  axis2l_swaph( axis2l_t* p);

void  axis2i_add( axis2i_t* outp, axis2i_t a, axis2i_t b);
void  axis2i_sub( axis2i_t* outp, axis2i_t a, axis2i_t b);
void  axis3i_add( axis3i_t* outp, axis3i_t a, axis3i_t b);
void  axis3i_sub( axis3i_t* outp, axis3i_t a, axis3i_t b);
void  axis2l_add( axis2l_t* outp, axis2l_t a, axis2l_t b);
void  axis2l_sub( axis2l_t* outp, axis2l_t a, axis2l_t b);
void  axis3l_add( axis3l_t* outp, axis3l_t a, axis3l_t b);
void  axis3l_sub( axis3l_t* outp, axis3l_t a, axis3l_t b);

void vector2f_normalization(vector2f_t* vectorp);
void vector3f_normalization(vector3f_t* vectorp);








#ifdef __cplusplus
}
#endif
#endif
