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


#ifndef _imu_driver_h
#define _imu_driver_h
#include "emf_typedef.h"
#include "utils/emf_utils.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

typedef enum{
	ACC_RANGE_1G = 0,
	ACC_RANGE_2G,
	ACC_RANGE_4G,
	ACC_RANGE_8G,
	ACC_RANGE_16G,
}acc_range_t;

typedef enum{
	GYRO_RANGE_125 = 0,
	GYRO_RANGE_250,
	GYRO_RANGE_500,
	GYRO_RANGE_1000,
	GYRO_RANGE_2000,
}gyro_range_t;


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool imu_driver_get_raw(axis3i_t* accp,axis3i_t* gyrop);
bool imu_driver_init(acc_range_t acc_range,gyro_range_t gyro_range);
bool imu_driver_deinit(void);
void imu_driver_handler(void);

#ifdef __cplusplus
}
#endif
#endif





