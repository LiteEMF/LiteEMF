/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/


#ifndef _filter_h
#define _filter_h
#include "emf_typedef.h" 
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/	
// deltaT: 	采样时间, (s秒)
// Fcut:	截止频率 (HZ) 	frequency cout off
#define LPF_1ST_FACTOR(deltaT, Fcut)	(deltaT) / ((deltaT) + 1 / (2 * pi * (Fcut)))

#ifndef KALMAN_A
#define KALMAN_A				1	/* x(n)=A*x(n-1)+u(n),u(n)~N(0,q) */
#endif
#ifndef KALMAN_H
#define KALMAN_H				1	/* z(n)=H*x(n)+w(n),w(n)~N(0,r)   */
#endif




/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

	
/*******************************************************************
*	Q: Process noise,Q increases, dynamic response becomes faster, convergence stability becomes worse
*	R: Measurement noise, R increases, dynamic response slows down, convergence stability becomes better
*	smoot = 100 * ((Q+R)/Q)
*	R = smoot;
*	Q = 100 - smoot;
*	smoot: 0 ~ 100	, smoot increases is smoother
*******************************************************************/
typedef struct {
	float out;					//out
	float p;					/* process(predict) noise convariance */
	float q;					/* measure noise convariance */		
	float r;					/* estimated error convariance*/		
}kalman_t;

typedef struct {
	kalman_t x;	
	kalman_t y;
}kalman_axis2f_t;

typedef struct {
	kalman_t x;	
	kalman_t y;
	kalman_t z;
}kalman_axis3f_t;



typedef struct {
	uint8_t index;
	uint8_t imp_size;
	float 	out;
	float 	sum;
	float*  pbuf;
	float* 	pimpulse_response;		//imp[0] is newest data
} firf_t;


typedef struct {
	firf_t x;	
	firf_t y;
}firf_axis2_t;

typedef struct {
	firf_t x;	
	firf_t y;
	firf_t z;
}firf_axis3_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern void lpf_1st(float* outp, float measure, float lpf_factor);
extern void lpf_1st_axis2f(axis2f_t* outp, const axis2f_t* axis_inp, float lpf_factor);
extern void lpf_1st_axis3f(axis3f_t* outp, const axis3f_t* axis_inp, float lpf_factor);

extern void kalman_filter_init(kalman_t* kalmanp,float q, float r);
extern void kalman_axis2f_filter_init(kalman_axis2f_t* kalmanp,float q, float r);
extern void kalman_axis3f_filter_init(kalman_axis3f_t* kalmanp,float q, float r);
extern void kalman_filter(kalman_t* kalmanp, float measure);
extern void kalman_axis2f_filter(kalman_axis2f_t* kalmanp, const axis2f_t* measurep);
extern void kalman_axis3f_filter(kalman_axis3f_t* kalmanp, const axis3f_t* measurep);

extern void fir_fiter_init(firf_t *firp,float* imp,float* fbuf, uint8_t imp_size);
extern void fir_axis2_fiter_init(firf_axis2_t *firp,float* imp,float* fbuf,uint8_t imp_size);
extern void fir_axis3_fiter_init(firf_axis3_t *firp,float* imp,float* fbuf,uint8_t imp_size);
extern void fir_fiter(firf_t *firp, float measure);
extern void fir_axis2l_fiter(firf_axis2_t *firp, const axis2l_t* measurep);
extern void fir_axis3l_fiter(firf_axis3_t *firp, const axis3l_t* measurep);
void fir_axis2f_fiter(firf_axis2_t *firp, const axis2f_t* measurep);
void fir_axis3f_fiter(firf_axis3_t *firp, const axis3f_t* measurep);

extern int32_t variance_calculate(int16_t value, int16_t* s_buf, uint8_t size);
#ifdef __cplusplus
}
#endif
#endif





