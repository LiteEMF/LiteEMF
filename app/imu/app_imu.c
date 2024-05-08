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
#include  "hw_config.h"
#if APP_IMU_ENABLE
#include  "app/imu/app_imu.h"
#include  "utils/emf_utils.h"
#include  "api/api_tick.h"
#include  "api/api_log.h"
#include  "api/api_storage.h"



//产品正面放置(何为正面?两手握手柄,左手正按住左摇杆,右手正按右摇杆,游戏按键均朝面)
//IMU 3轴 标准形态在产品正面放置是的形态示意图
//不同的硬件，芯片的焊接位置和方向不同，但为了方便软件开发，使用app_imu_set_standard_direction将数据转成统一的方向

/////////////////////
//加速度计的统一方向
/////////////////////
//(可以用张开的右手(掌心朝下,大拇指朝视线方向)，X轴正方向为拇指的指向，Y轴的正方向为4指的指向，Z轴的正方向为掌背到掌心的方向)
// 下图x表示箭尾
//  (x)x---------→(y)
//     |
//     |
//     |
//     ↓(z)

//roll->acc.x, pitch->acc.y, yaw->acc.z

//////////////////////
///gyro陀螺仪的统一方向
//////////////////////
//（以右手拇指指向的该轴加速度计的正方向，握住形成拳头，手指弯曲的方向为该轴陀螺仪的正方向）
//GYRO Z 轴（全局右陀螺仪） 绕Z轴正方向旋转,值增大，反之，值减小（用于左右控制）
//GYRO X 轴（全局右陀螺仪） 绕X轴正方向旋转,值增大，反之，值减小（用于左右控制）
//GYRO Y 轴（全局右陀螺仪） 绕Y轴正方向旋转,值增大，反之，值减小（用于上下控制）

/******************************************************************************************************
** Defined
*******************************************************************************************************/
	
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
axis3i_t m_gyro;
axis3i_t m_acc;
imu_cal_sta_t imu_cal_sta;
imu_move_t is_imu_move;
bool imu_auto_cal;
float accSensitivity   = 0.244f;   //加速度的最小分辨率 mg/LSB
float gyroSensitivity  = 32.8f;    //陀螺仪的最小分辨率
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
#if IMU_FILTER_ENABLE
	static firf_axis3_t gyro_filter;
	static float gyro_filter_buf[4];

	#if ACC_FILTER_KALMAN
	static kalman_axis3f_t acc_filter;
	#else
	static axis3f_t acc_filter;
	#endif
#endif

#if API_STORAGE_ENABLE
imu_cal_t *const imu_calp = (imu_cal_t*)(m_storage.imu_cal);
#else
imu_cal_t *const imu_calp = NULL;
#endif
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK bool app_imu_get_raw(axis3i_t* accp,axis3i_t* gyrop)	//这里可以根据工程调整imu方向
{
	bool ret = false;
   	ret = imu_driver_get_raw(accp,gyrop);
	return ret;
}
__WEAK void app_imu_event(imu_cal_sta_t event)
{
}
#endif

/*******************************************************************
** Parameters:	in: accp, gyrop raw data
** Returns:	
** Description:		imu raw filter
*******************************************************************/
#if IMU_FILTER_ENABLE
static void app_imu_filter_init(void)
{
	fir_axis3_fiter_init(&gyro_filter,NULL,gyro_filter_buf,countof(gyro_filter_buf));
	#if ACC_FILTER_KALMAN
	kalman_axis3f_filter_init(&acc_filter,ACC_KALMAN_Q,ACC_KALMAN_R);
	#endif
}

static void app_imu_filter(axis3i_t* accp,axis3i_t* gyrop)
{
	axis3f_t acc;
	axis3l_t gyro;

	AXIS3_COPY(&gyro,gyrop);
	AXIS3_COPY(&acc,accp);

	#if ACC_FILTER_KALMAN
	kalman_axis3f_filter(&acc_filter, &acc);
	m_acc.x = acc_filter.x.out;
	m_acc.y = acc_filter.y.out;
	m_acc.z = acc_filter.z.out;
	#else
	lpf_1st_axis3f(&acc_filter, &acc,ACC_LPFP);
	AXIS3_COPY(&m_acc,&acc_filter);
	#endif

	fir_axis3l_fiter(&gyro_filter, &gyro);
	m_gyro.x = gyro_filter.x.out;
	m_gyro.x = gyro_filter.x.out;
	m_gyro.x = gyro_filter.x.out;
}
#endif

static imu_move_t imu_static_check(void)
{
	bool move1 = true, move2 = true;
	axis3i_t d_gyro,d_acc;
	static timer_t timer;
	static axis3i_t s_gyro,s_acc;

	d_gyro = m_gyro;
	axis3i_sub(&d_gyro,&s_gyro);

	if( (abs(d_gyro.x) > GYRO_LEVE2_DPS) || (abs(d_gyro.x) > GYRO_LEVE2_DPS) || (abs(d_gyro.x) > GYRO_LEVE2_DPS) ){
		move2 = false;
		move1 = false;
	}

	if(move2){
		d_acc = m_acc;
		axis3i_sub(&d_acc,&s_acc);
		if( (abs(d_acc.x) > ACC_LEVE1_DPS) || (abs(d_acc.x) > ACC_LEVE1_DPS) || (abs(d_acc.x) > ACC_LEVE1_DPS) ){
			move2 = false;
			move1 = false;
		}
	}

	if(move1){
		if( (abs(d_gyro.x) > GYRO_LEVE1_DPS) || (abs(d_gyro.x) > GYRO_LEVE1_DPS) || (abs(d_gyro.x) > GYRO_LEVE1_DPS) ){
			move1 = false;
		}
	}
	if(move1){
		d_acc = m_acc;
		axis3i_sub(&d_acc,&s_acc);
		if( (abs(d_acc.x) > ACC_LEVE1_DPS) || (abs(d_acc.x) > ACC_LEVE1_DPS) || (abs(d_acc.x) > ACC_LEVE1_DPS) ){
			move1 = false;
		}
	}
	s_gyro = m_gyro;
	s_acc = m_acc;

	if(move1){
		if(m_systick - timer > 3000){
			timer = m_systick;
			is_imu_move = IMU_MOVE_NONE;
		}
	}else if(move2){
		if(m_systick - timer > 3000){
			timer = m_systick;
			is_imu_move = IMU_MOVE_LEVE1;
		}
	}else{
		timer = m_systick;
		is_imu_move = IMU_MOVE_LEVE2;
	}
	return is_imu_move;
}

static void imu_do_cal(void)
{
	bool is_static;
    static axis3l_t gyro_sum,acc_sum;
	static timer_t timer;
    static uint8_t s_count = 0;
	static imu_cal_sta_t s_cal_sta = IMU_CAL_NONE;


    if(s_cal_sta != imu_cal_sta){
        s_cal_sta = imu_cal_sta;
        if(!imu_auto_cal) app_imu_event(imu_cal_sta);
    }

    switch (imu_cal_sta){
    case IMU_CAL_START:
		s_count = 0;
		timer = m_systick;
		memset(&gyro_sum,0,sizeof(gyro_sum));
		memset(&acc_sum,0,sizeof(acc_sum));
		imu_cal_sta = IMU_CAL_ING;
        break;
    case IMU_CAL_ING:
		if(imu_auto_cal){
			is_static = (IMU_MOVE_NONE == is_imu_move);
		}else{
			is_static = (IMU_MOVE_LEVE1 >= is_imu_move);
		}

		if(is_static){
			s_count++;
			AXIS3_ADD(&gyro_sum, &m_gyro);
			AXIS3_ADD(&acc_sum, &m_acc);

			if(s_count >= 100){
				axis3l_div(&gyro_sum,s_count);
				axis3l_div(&acc_sum,s_count);
				acc_sum.z -= 4096;		//TODO

				AXIS3_COPY(&(imu_calp->gyro),&gyro_sum);
				AXIS3_COPY(&(imu_calp->acc),&acc_sum);
				#if API_STORAGE_ENABLE
				if(!imu_auto_cal) api_storage_auto_sync();
				#endif
				imu_cal_sta = IMU_CAL_SUCCEED;
			}
		}else{
			s_count = 0;
			memset(&gyro_sum,0,sizeof(gyro_sum));
			memset(&acc_sum,0,sizeof(acc_sum));

			if(m_systick - timer >= IMU_CAL_TIMEOUT){
				imu_cal_sta = IMU_CAL_FAILED;
			}
		}
        break;
    case IMU_CAL_SUCCEED:
	case IMU_CAL_FAILED:
        imu_cal_sta = IMU_CAL_NONE;
		imu_auto_cal = false;
        break;
    default:
        break;
    }
}
void app_imu_cal_start(void)
{
	imu_cal_sta = IMU_CAL_START;
	imu_auto_cal =  false;
}


#define Kp 2.0f			// 比例增益支配率收敛到加速度计/磁强计
#define Ki 0.005f		// 积分增益支配率的陀螺仪偏见的衔接
// #define Kp 		10.0f               // 这里的KpKi是用于调整加速度计修正陀螺仪的速度
// #define Ki 		0.008f                        
#define halfT 	0.0025f             	// 采样周期的一半，用于求解四元数微分方程时计算角增量
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    	// 初始姿态四元数，由上篇博文提到的变换四元数公式得来
float exInt = 0, eyInt = 0, ezInt = 0;    	//当前加计测得的重力加速度在三轴上的分量
                                	//与用当前姿态计算得来的重力在三轴上的分量的误差的积分
axis4f_t m_imu_quaternion;		//四元数
void app_imu_update(float gx, float gy, float gz, float ax, float ay, float az)		//g表陀螺仪，a表加计  
{
	float norm; //矢量的模或四元数的范数
	float vx, vy, vz;//当前姿态计算得来的重力在三轴上的分量
	float ex, ey, ez;//当前加计测得的重力加速度在三轴上的分量,与用当前姿态计算得来的重力在三轴上的分量的误差
	float q0_last = q0;		//四元数暂存变量，求解微分方程时要用
	float q1_last = q1;	
	float q2_last = q2;	
	float q3_last = q3;	    

	// 陀螺仪数据需乘一个系数将其转化为弧度制角速度   (GX*gyroSensitivity)*2*PI/360   gyroSensitivity(度/S) = 量程 / 0x8000
	gx = gx * gyroSensitivity * 0.017453f;		//转化为弧度制角速度  弧度/S  
	gy = gy * gyroSensitivity * 0.017453f;
	gz = gz * gyroSensitivity * 0.017453f;

	// if(ax*ay*az==0)//加计处于自由落体状态时不进行姿态解算，因为会产生分母无穷大的情况
	// 	return;
	norm = invSqrt(ax*ax + ay*ay + az*az);//单位化加速度计，
	ax = ax * norm;		// 这样变更了量程也不需要修改KP参数，因为这里归一化了
	ay = ay * norm;
	az = az * norm;
	//用当前姿态计算出重力在三个轴上的分量，
	//参考坐标n系转化到载体坐标b系的用四元数表示的方向余弦矩阵第三列即是（博文一中有提到）
	vx = 2*(q1*q3 - q0*q2);
	vy = 2*(q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
	if(vz > 1) vz = 1;
	if(vz < -1) vz = -1;
	//计算测得的重力与计算得重力间的误差，向量外积可以表示这一误差
	//原因我理解是因为两个向量是单位向量且sin0等于0
	//不过要是夹角是180度呢~这个还没理解
	ex = (ay*vz - az*vy);                                                                  
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	exInt = exInt + ex * Ki;                                           //对误差进行积分
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;
	// adjusted gyroscope measurements
	gx = gx + Kp*ex + exInt;  //将误差PI后补偿到陀螺仪，即补偿零点漂移
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;    //这里的gz由于没有观测者进行矫正会产生漂移，表现出来的就是积分自增或自减
	//下面进行姿态的更新，也就是四元数微分方程的求解
	//采用一阶毕卡解法
	q0 = q0_last + (-q1_last*gx - q2_last*gy -q3_last*gz)*halfT;
	q1 = q1_last + (q0_last*gx + q2_last*gz -q3_last*gy)*halfT;
	q2 = q2_last + (q0_last*gy - q1_last*gz +q3_last*gx)*halfT;
	q3 = q3_last + (q0_last*gz + q1_last*gy -q2_last*gx)*halfT;
	//单位化四元数在空间旋转时不会拉伸，仅有旋转角度，这类似线性代数里的正交变换
	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	m_imu_quaternion.x = q0 * norm;
	m_imu_quaternion.y = q1 * norm;
	m_imu_quaternion.z = q2 * norm;
	m_imu_quaternion.w = q3 * norm;

}

bool app_imu_get_val(axis3i_t *accp, axis3i_t *gyrop)
{
	if(NULL != imu_calp){
		if (accp != NULL){
			accp->x = constrain_int16((int32_t)m_acc.x - imu_calp->acc.x);
			accp->y = constrain_int16((int32_t)m_acc.y - imu_calp->acc.y);
			accp->z = constrain_int16((int32_t)m_acc.z - imu_calp->acc.z);
		}
		if (gyrop != NULL){
			gyrop->x = constrain_int16((int32_t)m_gyro.x - imu_calp->gyro.x);
			gyrop->y = constrain_int16((int32_t)m_gyro.y - imu_calp->gyro.y);
			gyrop->z = constrain_int16((int32_t)m_gyro.z - imu_calp->gyro.z);
		}
	}else{
		if (accp != NULL){
			*accp = m_acc;
		}
		if (gyrop != NULL){
			*gyrop = m_gyro;
		}
	}
    
	return true;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_imu_init(void)
{
	imu_driver_init(ACC_RANGE_8G,GYRO_RANGE_2000);
	#if IMU_FILTER_ENABLE
	app_imu_filter_init();
	#endif
	imu_auto_cal = false;
	is_imu_move = IMU_MOVE_LEVE2;
	imu_cal_sta = IMU_CAL_NONE;
	memset(&m_gyro,0,sizeof(m_gyro));
	memset(&m_acc,0,sizeof(m_acc));

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_imu_deinit(void)
{
	imu_driver_deinit();
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_imu_task(void* pa)
{
	axis3i_t acc, gyro;
	static timer_t atuo_cal_timer;

	if(app_imu_get_raw(&acc, &gyro)){
	
		#if IMU_FILTER_ENABLE
		app_imu_filter(&acc, &gyro);
		#else
		m_gyro = gyro;
		m_acc = acc;
		#endif
		app_imu_update(gyro.x,gyro.y,gyro.z, acc.x,acc.y,acc.z);
				
		// // logd("imu: %d, %d, %d, %d, %d, %d\n",acc.x, acc.y, acc.z, gyro.x, gyro.y, gyro.z );
		imu_static_check();
		imu_do_cal();

		//动态校准
		if((IMU_CAL_NONE == imu_cal_sta) && IMU_MOVE_NONE == is_imu_move){
			if(m_systick - atuo_cal_timer > 6000){
				imu_cal_sta = IMU_CAL_START;
				imu_auto_cal =  true;
			}
		}else{
			atuo_cal_timer = m_systick;
		}
		
	}
	UNUSED_PARAMETER(pa);
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_imu_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_imu_task(NULL);
	}
}
#endif

#endif






