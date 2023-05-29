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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "app/imu/imu_driver.h"
#include "api/api_tick.h"

#if	defined IMU_SH3001_ID
#include "app/imu/sh3001.h"
#endif
#if defined IMU_QMI8658_ID
#endif


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
bool m_imu_init = false;    //imu初始化完成,大部分sensor 初始化画的时间比较长,需要在handle中实现




/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static timer_t imu_init_timer;          //imu定时器
static uint16_t imu_delay_time;                 //imu上电延时时间

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool imu_driver_get_raw(axis3i_t* accp,axis3i_t* gyrop)
{
	bool ret = false;

	if(!m_imu_init) return false;

	#if IMU_ISR_READ_ENABLE
		#if	defined IMU_SH3001_ID
		ret = SH3001_GetImuCompData((int16_t *)accp, (int16_t *)gyrop,1);
		#endif
		#if defined IMU_ICM20600_ID
		ret = zw_get_imu_data((int16_t *)accp, (int16_t *)gyrop);
		#endif
		#if defined IMU_QMI8658_ID
		ret = Qmi8658_read_xyz_raw((int16_t *)accp, (int16_t *)gyrop, NULL);
		#endif
	#else
		#if	defined IMU_SH3001_ID
		ret = SH3001_GetImuCompData((int16_t *)accp, (int16_t *)gyrop,0);
		#endif
		#if defined IMU_ICM20600_ID
		ret = zw_get_imu_data((int16_t *)accp, (int16_t *)gyrop);
		#endif
		#if defined IMU_QMI8658_ID
		ret = Qmi8658_read_xyz_raw((int16_t *)accp, (int16_t *)gyrop, NULL);
		#endif
	#endif

	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool imu_driver_init(acc_range_t acc_range,gyro_range_t gyro_range)	//TODO 待优化接口
{
	m_imu_init = false;
	imu_init_timer = m_systick;          //imu定时器

	#if	defined IMU_SH3001_ID
    imu_delay_time = 300;
    #endif
    #if defined IMU_QMI8658_ID
    imu_delay_time = 2000;
    #endif

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool imu_driver_deinit(void)
{
	return true;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	大部分的imu初始都有delay延时函数, 建议单独放到单独的一个低优先级任务中初始化	
*******************************************************************/
void imu_driver_task(void* pa)
{

	if(m_imu_init) return;

    if(m_systick - imu_init_timer >= (uintptr_t)pa){
        imu_init_timer = m_systick;
	
        #if	defined IMU_SH3001_ID
        m_imu_init = SH3001_init(ACC_RANGE_8G, GYRO_RANGE_2000);
        #endif

        #if defined IMU_ICM20600_ID
        m_imu_init = InitICM20600(ACC_RANGE_8G, GYRO_RANGE_2000);
        #endif

        #if defined IMU_QMI8658_ID
        m_imu_init = Qmi8658_init(ACC_RANGE_8G, GYRO_RANGE_2000);
        #endif

        if(!m_imu_init) loge_r("imu driver init error!\n");
	}

	UNUSED_PARAMETER(pa);
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void imu_driver_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		imu_driver_task((void*)imu_delay_time);
	}
}
#endif






