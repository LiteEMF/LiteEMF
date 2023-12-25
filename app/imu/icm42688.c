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


extern float accSensitivity;   	//加速度的最小分辨率 mg/LSB
extern float gyroSensitivity;    //陀螺仪的最小分辨率
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
        api_spi_host_write(0,MPUREG_ACCEL_CONFIG0, &reg_val, 1);	

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
        api_spi_host_write(0,MPUREG_GYRO_CONFIG0, &reg_val, 1);	

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

#endif
