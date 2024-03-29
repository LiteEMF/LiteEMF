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
#include  "utils/filter.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/



/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:		outp: 注意必须传入上一次的输出值
** Returns:	
** Description:		First-order low-pass filter
					used LPF_1ST_FACTOR(deltaT, Fcut) get lpf_factor
*******************************************************************/
void lpf_1st(float* outp, float measure, float lpf_factor)
{
	*outp = *outp * (1 - lpf_factor) + measure * lpf_factor;
}
void lpf_1st_axis2f(axis2f_t* outp, const axis2f_t* axis_inp, float lpf_factor)
{
	lpf_1st(&outp->x, axis_inp->x, lpf_factor);
    lpf_1st(&outp->y, axis_inp->y, lpf_factor);
}
void lpf_1st_axis3f(axis3f_t* outp, const axis3f_t* axis_inp, float lpf_factor)
{
	lpf_1st(&outp->x, axis_inp->x, lpf_factor);
    lpf_1st(&outp->y, axis_inp->y, lpf_factor);
    lpf_1st(&outp->z, axis_inp->z, lpf_factor);
}


/*******************************************************************
** Parameters:	kalmanp: Klaman filter structure
** Returns:	
** Description:		kalman filter
ref: https://github.com/xiahouzuoxin/kalman_filter/blob/master/kalman_filter.c
*******************************************************************/
void kalman_filter_init(kalman_t* kalmanp,float q, float r)
{
    kalmanp->out = 0;
    kalmanp->p = 0;
    kalmanp->q = q;
    kalmanp->r = r;
}
void kalman_axis2f_filter_init(kalman_axis2f_t* kalmanp,float q, float r)
{
    kalman_filter_init(&kalmanp->x,q,r);
	kalman_filter_init(&kalmanp->y,q,r);
}
void kalman_axis3f_filter_init(kalman_axis3f_t* kalmanp,float q, float r)
{
    kalman_filter_init(&kalmanp->x,q,r);
	kalman_filter_init(&kalmanp->y,q,r);
	kalman_filter_init(&kalmanp->z,q,r);
}

void kalman_filter(kalman_t* kalmanp, float measure)
{
	float gain;
    /* Predict */
    kalmanp->out = KALMAN_A * kalmanp->out;
    kalmanp->p = KALMAN_A * KALMAN_A * kalmanp->p + kalmanp->q;  /* p(n|n-1)=A^2*p(n-1|n-1)+q */

    /* Measurement */
    gain = kalmanp->p * KALMAN_H / (kalmanp->p * KALMAN_H * KALMAN_H + kalmanp->r);
    kalmanp->out = kalmanp->out + gain * (measure - KALMAN_H * kalmanp->out);
    kalmanp->p = (1 - gain * KALMAN_H) * kalmanp->p;
}

void kalman_axis2f_filter(kalman_axis2f_t* kalmanp, const axis2f_t* measurep)
{
	kalman_filter(&kalmanp->x, measurep->x);
	kalman_filter(&kalmanp->y, measurep->y);
}

void kalman_axis3f_filter(kalman_axis3f_t* kalmanp, const axis3f_t* measurep)
{
	kalman_filter(&kalmanp->x, measurep->x);
	kalman_filter(&kalmanp->y, measurep->y);
	kalman_filter(&kalmanp->z, measurep->z);
}



/*******************************************************************
** Parameters:	imp:fir impulse response
** Returns:	
** Description:		fir filter
*******************************************************************/
void fir_fiter_init(firf_t *firp,float* imp,uint8_t imp_size)
{
	uint8_t i; 

	memset(firp,0,sizeof(firf_t));
	firp->imp_size = MIN(imp_size,FIR_FILTER_MAX_LENGTH);
	if(0 == firp->imp_size) firp->imp_size = FIR_FILTER_MAX_LENGTH;	 //防止imp_size为0

	#if !SAMPLE_FIR_FILTER
	for(i=0; i<firp->imp_size; i++){			//for default value
		fir->impulse_response[i] = 1/firp->imp_size;
	}
	if(NULL != imp){
		memcpy(fir->impulse_response,imp,firp->imp_size);
	}
	#endif
	
	UNUSED_VARIABLE(i);
}
void fir_axis2f_fiter_init(firf_axis2f_t *firp,float* imp,uint8_t imp_size)
{
	fir_fiter_init(&firp->x,imp,imp_size);
	fir_fiter_init(&firp->y,imp,imp_size);
}
void fir_axis3f_fiter_init(firf_axis3f_t *firp,float* imp,uint8_t imp_size)
{
	fir_fiter_init(&firp->x,imp,imp_size);
	fir_fiter_init(&firp->y,imp,imp_size);
	fir_fiter_init(&firp->z,imp,imp_size);
}
void fir_fiter(firf_t *firp, int32_t measure)
{
	uint8_t n;
	uint8_t sum_index;
	
	UNUSED_VARIABLE(n);
	UNUSED_VARIABLE(sum_index);
	
	#if SAMPLE_FIR_FILTER
		firp->sum += measure - firp->buf[firp->index];
		firp->out = firp->sum / firp->imp_size;
	#endif
	
	/* Store latest sample in buffer */
	firp->buf[firp->index] = measure;
	firp->index++;
	if (firp->index == firp->imp_size) {
		firp->index = 0;
	}

	#if !SAMPLE_FIR_FILTER
		/* Compute new output sample (via convolution) */
		fir->sum = 0;
		sum_index = fir->index;

		for ( n = 0; n < firp->imp_size; n++) {
			/* Decrement index and wrap if necessary */
			if (sum_index > 0) {
				sum_index--;
			} else {
				sum_index = firp->imp_size - 1;
			}
			/* Multiply impulse response with shifted input sample and add to output */
			fir->sum += fir->impulse_response[n] * fir->buf[sum_index];
		}
		fir->out = fir->sum;
    #endif
}
void fir_axis2f_fiter(firf_axis2f_t *firp, const axis2l_t* measurep)
{
	fir_fiter(&firp->x, measurep->x);
	fir_fiter(&firp->y, measurep->y);
}
void fir_axis3f_fiter(firf_axis3f_t *firp, const axis3l_t* measurep)
{
	fir_fiter(&firp->x, measurep->x);
	fir_fiter(&firp->y, measurep->y);
	fir_fiter(&firp->z, measurep->z);
}


