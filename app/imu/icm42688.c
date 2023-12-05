/************************************************************************************************************
**	FileName:	storage.c
**	Description:
**	Major Function:
**
--------------------------------------------------------------------------------------------------------
**	Software:
**	Hareware:
**
**	Company:	NGDS
--------------------------------------------------------------------------------------------------------
**	Modified:
**	Author 			Date 			 Comment
_________________________________________________________________________________________________________
**	Cesar.Li			2014-09-16		Original
**
**
************************************************************************************************************/

#include "hw_config.h"
#ifdef  IMU_ICM42688_ID
#include "hw_board.h"
#include "api/api_soft_spi.h"
#include "api/api_tick.h"
#include "icm42688.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define PI        	3.1415926f
#define ABS(a) 		(((0 - (a)) > 0) ? (-(a)) : (a))

XYZ_ACCE_T AcceXYZ;
XYZ_ACCE_T AcceOffset={0,0,0};
XYZ_GYRO_T GyroXYZ;
XYZ_GYRO_T GyroOffset={0,0,0};


static float accSensitivity   = 0.244f;   //加速度的最小分辨率 mg/LSB
static float gyroSensitivity  = 32.8f;    //陀螺仪的最小分辨率
float bsp_Icm42688GetAres(uint8_t Ascale)
{
    switch(Ascale)
    {
    case AFS_2G:
        accSensitivity = 2000 / 32768.0f;
        break;
    case AFS_4G:
        accSensitivity = 4000 / 32768.0f;
        break;
    case AFS_8G:
        accSensitivity = 8000 / 32768.0f;
        break;
    case AFS_16G:
        accSensitivity = 16000 / 32768.0f;
        break;
    }

    return accSensitivity;
}

float bsp_Icm42688GetGres(uint8_t Gscale)
{
    switch(Gscale)
    {
    case GFS_125DPS:
        gyroSensitivity = 125.0f / 32768.0f;
        break;
    case GFS_250DPS:
        gyroSensitivity = 250.0f / 32768.0f;
        break;
    case GFS_500DPS:
        gyroSensitivity = 500.0f / 32768.0f;
        break;
    case GFS_1000DPS:
        gyroSensitivity = 1000.0f / 32768.0f;
        break;
    case GFS_2000DPS:
        gyroSensitivity = 2000.0f / 32768.0f;
        break;
    }
    return gyroSensitivity;
}
/*********************************************************************************
** Function:
** Parameters:
** Returns:
** details:
** Modified:
**
*********************************************************************************/
uint8_t ICM42688_init(acc_range_t acc_range, gyro_range_t gyro_range)
{
	uint8_t ret,id;
	uint8_t accr, gyror;
	uint8_t reg_val;

	ret = api_spi_host_read(0,0x80|MPUREG_WHO_AM_I, &id, 1);
	logd("id = 0x%x\n", id);
	if(id == ICM42688_ID)
	{
		reg_val = 1;
		api_spi_host_write(0,MPUREG_REG_BANK_SEL, &reg_val, 1); 	//设置bank 1区域寄存器
		reg_val = 0x02;
        api_spi_host_write(0,MPUREG_INTF_CONFIG4_B1, &reg_val, 1); 	//设置为4线SPI通信

		reg_val = 0;
        api_spi_host_write(0,MPUREG_REG_BANK_SEL, &reg_val, 1); 	//设置bank 0区域寄存器
        reg_val = 0x02;
		api_spi_host_write(0,MPUREG_INT_CONFIG, &reg_val, 1);

		switch(acc_range){
			case ACC_RANGE_2G:
				accr = AFS_2G;
				break;
			case ACC_RANGE_4G:
				accr = AFS_4G;
				break;
			case ACC_RANGE_8G:
				accr = AFS_8G;
				break;
			case ACC_RANGE_16G:
				accr = AFS_16G;
				break;
			default:
				accr = AFS_8G;
				break;
		}
        bsp_Icm42688GetAres(accr);
        reg_val = 0;	
        reg_val |= (accr << 5);   	//量程 
        reg_val |= (AODR_1000Hz);     //输出速率 1000HZ
        api_spi_host_write(0,MPUREG_ACCEL_CONFIG0, &reg_val, 1);	//page79

		switch(gyro_range){
			case GYRO_RANGE_125:
				gyror = GFS_125DPS;
				break;
			case GYRO_RANGE_250:
				gyror = GFS_250DPS;
				break;
			case GYRO_RANGE_500:
				gyror = GFS_500DPS;
				break;
			case GYRO_RANGE_1000:
				gyror = GFS_1000DPS;
				break;
			case GYRO_RANGE_2000:
				gyror = GFS_2000DPS;
				break;
			default:
				gyror = GFS_1000DPS;
				break;
		}
        bsp_Icm42688GetGres(gyror);
        reg_val = 0;
        reg_val |= (gyror << 5);   		//量程 
        reg_val |= (GODR_1000Hz);     	//输出速率 1000HZ
        api_spi_host_write(0,MPUREG_GYRO_CONFIG0, &reg_val, 1);	//page78

        reg_val = 0;
        reg_val |= (1 << 5);		//关闭温度测量
        reg_val |= ((3) << 2);		//设置GYRO_MODE  0:关闭 1:待机 2:预留 3:低噪声
        reg_val |= (3);				//设置ACCEL_MODE 0:关闭 1:关闭 2:低功耗 3:低噪声
        api_spi_host_write(0,MPUREG_PWR_MGMT_0, &reg_val, 1);	//page77
        delay_us(500); 		//操作完PWR—MGMT0寄存器后 200us内不能有任何读写寄存器的操作

	}
	return ret;
}

void ICM42688_Sleep(void)
{
	uint8_t reg_val;

	reg_val = 0x01;
	api_spi_host_write(0,MPUREG_DEVICE_CONFIG, &reg_val, 1);		//软件复位后1ms内不要操作寄存器
}


uint8_t  SensorSendbuf[8];
void  SendUart_Wave(void)
{
	SensorSendbuf[0] = 0xAA;					//数据接收回复
	SensorSendbuf[1] = 0xBB;
	
//	r_UartTX(SensorSendbuf,8);							//开始发送数据

}

/****************************************************************
** 函数名称：	Led_Flicker
** 功能描述：	LED闪烁处理
** 输入参数：	del:闪烁时间  cnt:闪烁次数  brn:亮度
** 返回参数：	无
****************************************************************/
bool ICM42688_GetImuData(int16_t *accData, int16_t *gyroData)
{
	uint8_t ret,temp[12];

	ret = api_spi_host_read(0,0x80|MPUREG_ACCEL_DATA_X0_UI,temp,12);
	if(ret == true)
	{
		accData[0] = (int16_t)(((temp[0]&0X00FF)<< 8) | (temp[1]&0X00FF));	//拼接数据
		accData[1] = (int16_t)(((temp[2]&0X00FF)<< 8) | (temp[3]&0X00FF));
		accData[2] = (int16_t)(((temp[4]&0X00FF)<< 8) | (temp[5]&0X00FF));
		gyroData[0] = (int16_t)(((temp[6]&0X00FF)<< 8) | (temp[7]&0X00FF));	//拼接数据
		gyroData[1] = (int16_t)(((temp[8]&0X00FF)<< 8) | (temp[9]&0X00FF));
		gyroData[2] = (int16_t)(((temp[10]&0X00FF)<< 8) | (temp[11]&0X00FF));

	//	logd("AX = %d  AY = %d  AZ = %d\r\n", AcceXYZ.AcceX,AcceXYZ.AcceY,AcceXYZ.AcceZ);
	//	logd("GX = %d  GY = %d  GZ = %d\r\n", GyroXYZ.GyroX,GyroXYZ.GyroY,GyroXYZ.GyroZ);
	}
	return ret;
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

#define Kp 2.0f			// 比例增益支配率收敛到加速度计/磁强计
#define Ki 0.005f		// 积分增益支配率的陀螺仪偏见的衔接
// #define Kp 		10.0f               // 这里的KpKi是用于调整加速度计修正陀螺仪的速度
// #define Ki 		0.008f                        
#define halfT 	0.0025f             	// 采样周期的一半，用于求解四元数微分方程时计算角增量
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    	// 初始姿态四元数，由上篇博文提到的变换四元数公式得来
float exInt = 0, eyInt = 0, ezInt = 0;    	//当前加计测得的重力加速度在三轴上的分量
                                	//与用当前姿态计算得来的重力在三轴上的分量的误差的积分
void ICM42688_IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)		//g表陀螺仪，a表加计  
{
	float norm; //矢量的模或四元数的范数
	float vx, vy, vz;//当前姿态计算得来的重力在三轴上的分量
	float ex, ey, ez;//当前加计测得的重力加速度在三轴上的分量,与用当前姿态计算得来的重力在三轴上的分量的误差
	float q0_last = q0;		//四元数暂存变量，求解微分方程时要用
	float q1_last = q1;	
	float q2_last = q2;	
	float q3_last = q3;	    
	float gztemp = gz;	
	float gxtemp = gx;	 

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
	q0 = q0 * norm;
	q1 = q1 * norm;
	q2 = q2 * norm;
	q3 = q3 * norm;

}

#endif
