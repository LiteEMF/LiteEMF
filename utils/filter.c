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
二、常见滤波算法介绍：
1.限幅滤波
2.滑动平均滤波（递推平均滤波）
3.中值滤波
4.加权平均滤波
5.算术平均滤波
6.惯性滤波法（一阶滞后滤波法）
7.零相位滤波
8.卡尔曼滤波
9.粒子滤波
10.复合算法
a.中位值平均滤波法
b.限幅均值滤波法
复合滤波:
1.限幅滤波：
2.中值+滑动均值滤波：
3.Kalman滤波

ref: https://blog.csdn.net/qq_46280347/article/details/120743250
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
				fbuf: static tmp buf
				imp_size: imp size == fbuf size
** Returns:	
** Description:		fir filter
*******************************************************************/
void fir_fiter_init(firf_t *firp,float* imp,float* fbuf,uint8_t imp_size)
{
	memset(firp,0,sizeof(firf_t));
	EMF_ASSERT(imp_size);
	EMF_ASSERT(NULL != buf);

	firp->pbuf = fbuf;
	firp->pimpulse_response = imp;
}
void fir_axis2_fiter_init(firf_axis2_t *firp,float* imp,float* fbuf,uint8_t imp_size)
{
	fir_fiter_init(&firp->x,fbuf,imp,imp_size);
	fir_fiter_init(&firp->y,fbuf,imp,imp_size);
}
void fir_axis3_fiter_init(firf_axis3_t *firp,float* imp,float* fbuf,uint8_t imp_size)
{
	fir_fiter_init(&firp->x,fbuf,imp,imp_size);
	fir_fiter_init(&firp->y,fbuf,imp,imp_size);
	fir_fiter_init(&firp->z,fbuf,imp,imp_size);
}
void fir_fiter(firf_t *firp, float measure)
{
	uint8_t n;
	uint8_t sum_index;
	
	UNUSED_VARIABLE(n);
	UNUSED_VARIABLE(sum_index);

	if(NULL == firp->pbuf){				//防止参数异常
		firp->out = measure;
		return;
	}
	
	if(NULL == firp->pimpulse_response){		
		firp->sum += measure - firp->pbuf[firp->index];	//优化节省算力获取sum
		firp->out = firp->sum / firp->imp_size;
	}
	
	/* Store latest sample in buffer */
	firp->pbuf[firp->index] = measure;
	firp->index++;
	if (firp->index == firp->imp_size) {
		firp->index = 0;
	}

	if(NULL != firp->pimpulse_response){
		/* Compute new output sample (via convolution) */
		firp->sum = 0;
		sum_index = firp->index;

		for ( n = 0; n < firp->imp_size; n++) {
			/* Decrement index and wrap if necessary */
			if (sum_index > 0) {
				sum_index--;
			} else {
				sum_index = firp->imp_size - 1;
			}
			/* Multiply impulse response with shifted input sample and add to output */
			firp->sum += firp->pimpulse_response[n] * firp->pbuf[sum_index];
		}
		firp->out = firp->sum;
    }
}

void fir_axis2l_fiter(firf_axis2_t *firp, const axis2l_t* measurep)
{
	fir_fiter(&firp->x, (float)measurep->x);
	fir_fiter(&firp->y, (float)measurep->y);
}
void fir_axis3l_fiter(firf_axis3_t *firp, const axis3l_t* measurep)
{
	fir_fiter(&firp->x, (float)measurep->x);
	fir_fiter(&firp->y, (float)measurep->y);
	fir_fiter(&firp->z, (float)measurep->z);
}

void fir_axis2f_fiter(firf_axis2_t *firp, const axis2f_t* measurep)
{
	fir_fiter(&firp->x, measurep->x);
	fir_fiter(&firp->y, measurep->y);
}
void fir_axis3f_fiter(firf_axis3_t *firp, const axis3f_t* measurep)
{
	fir_fiter(&firp->x, measurep->x);
	fir_fiter(&firp->y, measurep->y);
	fir_fiter(&firp->z, measurep->z);
}



/*******************************************************************
** Parameters:	pbuf,size: 缓存数据	
** Returns:	
** Description:	计算方差
@ref: https://github.dev/FreeJoy-Team/FreeJoy IsDynamicDeadbandHolding	
*******************************************************************/
int32_t variance_calculate(int16_t value, int16_t* pbuf, uint8_t size)
{
	int32_t	disp = 0;
	int32_t mean = 0;
    uint8_t i;
	
    // shift buffer data and add a new value
	for (i=size-1; i>0; i--){
		pbuf[i] = pbuf[i-1];
	}
	pbuf[0] = value;
	
	// calculating mean value
	for (i=0; i<size; i++){
		mean += pbuf[i];
	}
	mean = mean/size;
	
	// calculating dispercy
	for (i=0; i<size; i++){
		disp += (pbuf[i] - mean)*(pbuf[i] - mean);
	}
	disp = disp/size;
	
	return disp;
}




/*******************************************************************
** Parameters:	pbuf,size: 数据, 
	window_num: 取中间窗口大小, 这里为了处理简单window_num = size-6 ~ size
** Returns:	
** Description:	去最大最小值取平均滤波值,中值+滑动均值滤波
*******************************************************************/
int16_t average2i_filter(int16_t* pbuf, uint8_t size, uint8_t window_num)
{
    int16_t max[3], min[3];
	uint8_t i, k, exclude_num = size - window_num;
    int32_t sum = 0;

	if(exclude_num > 2*countof(max)) exclude_num = 2*countof(max); //这里简单处理, 排除最大最小数据支持最大6个数据排除

	memcpy(max,pbuf,countof(max));
	memcpy(min,pbuf,countof(min));
	for(i=0; i<size; i++){
		for(k=0; k<exclude_num/2; k++){
			if (pbuf[i] > max[k]){
				max[k] = pbuf[i];
				break;
			}else if(pbuf[i] < min[k]){
				min[k] = pbuf[i];
				break;
			}
		}
		sum += pbuf[i];
	}
	for(k=0; k<exclude_num/2; k++){
		sum -= max[k] + min[k];
	}
	sum = sum / window_num;
	return sum;
}

