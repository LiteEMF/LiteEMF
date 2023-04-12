/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
* 
*/


#ifndef _app_imu_h
#define _app_imu_h
#include "emf_typedef.h"
#include "imu_driver.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef IMU_FILTER_ENABLE
#define IMU_FILTER_ENABLE	    1
#endif
#ifndef ACC_FILTER_KALMAN
#define ACC_FILTER_KALMAN	    0
#endif

#ifndef ACC_LPFP
#define ACC_LPFP		LPF_1ST_FACTOR(0.004/(1000.0f), 50)
#endif
#ifndef ACC_KALMAN_Q
#define ACC_KALMAN_Q        0.2
#endif
#ifndef ACC_KALMAN_Q
#define ACC_KALMAN_R        0.80
#endif

#ifndef IMU_CAL_TIMEOUT
#define IMU_CAL_TIMEOUT		6000
#endif

#ifndef GYRO_LEVE1_DPS
#define GYRO_LEVE1_DPS		7
#endif
#ifndef ACC_LEVE1_DPS
#define ACC_LEVE1_DPS		300
#endif

#ifndef GYRO_LEVE2_DPS
#define GYRO_LEVE2_DPS		14
#endif
#ifndef GYRO_LEVE2_DPS
#define ACC_LEVE2_DPS		600
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
    IMU_CAL_NONE,
    IMU_CAL_START,
    IMU_CAL_ING,
    IMU_CAL_SUCCEED,
    IMU_CAL_FAILED,
} imu_cal_sta_t;

typedef enum {
	IMU_MOVE_NONE = 0,
	IMU_MOVE_LEVE1,
	IMU_MOVE_LEVE2,
}imu_move_t;	

typedef struct{
    axis3i_t gyro;
	axis3i_t acc;
} imu_cal_t;	//12

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_imu_get_raw(axis3i_t* accp,axis3i_t* gyrop);			//__WEAK
void app_imu_event(imu_cal_sta_t event);			//__WEAK
void app_imu_cal_start(void);
bool app_imu_get_val(axis3i_t *accp, axis3i_t *gyrop);
bool app_imu_init(void);
bool app_imu_deinit(void);
void app_imu_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





