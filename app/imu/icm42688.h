#ifndef __ICM42688_H
#define __ICM42688_H

#include "emf_typedef.h" 
#include "app/imu/imu_driver.h"


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define ICM42688_ID			0x47

/* Bank 0 */
#define MPUREG_DEVICE_CONFIG      0x11
#define MPUREG_CHIP_CONFIG        MPUREG_DEVICE_CONFIG // Retro-compatibility
#define MPUREG_DRIVE_CONFIG       0x13
#define MPUREG_INT_CONFIG         0x14
#define MPUREG_FIFO_CONFIG        0x16
#define MPUREG_TEMP_DATA0_UI      0x1D
#define MPUREG_ACCEL_DATA_X0_UI   0x1F
#define MPUREG_GYRO_DATA_X0_UI    0x25
#define MPUREG_TMST_FSYNCH        0x2B
#define MPUREG_TMST_FSYNC1        MPUREG_TMST_FSYNCH // Retro-compatibility
#define MPUREG_INT_STATUS         0x2D
#define MPUREG_FIFO_COUNTH        0x2E
#define MPUREG_FIFO_BYTE_COUNT1   MPUREG_FIFO_COUNTH // Retro-compatibility
#define MPUREG_FIFO_COUNTL        0x2F
#define MPUREG_FIFO_BYTE_COUNT2   MPUREG_FIFO_COUNTL // Retro-compatibility
#define MPUREG_FIFO_DATA          0x30
#define MPUREG_APEX_DATA0         0x31
#define MPUREG_APEX_DATA1         0x32
#define MPUREG_APEX_DATA2         0x33
#define MPUREG_APEX_DATA3         0x34
#define MPUREG_APEX_DATA4         0x35
#define MPUREG_APEX_DATA5         0x36
#define MPUREG_INT_STATUS2        0x37
#define MPUREG_INT_STATUS3        0x38
#define MPUREG_SIGNAL_PATH_RESET  0x4B
#define MPUREG_INTF_CONFIG0       0x4C
#define MPUREG_INTF_CONFIG1       0x4D
#define MPUREG_PWR_MGMT_0         0x4E
#define MPUREG_GYRO_CONFIG0       0x4F
#define MPUREG_ACCEL_CONFIG0      0x50
#define MPUREG_GYRO_CONFIG1       0x51
#define MPUREG_ACCEL_GYRO_CONFIG0 0x52
#define MPUREG_ACCEL_CONFIG1      0x53
#define MPUREG_TMST_CONFIG        0x54
#define MPUREG_APEX_CONFIG0       0x56
#define MPUREG_SMD_CONFIG         0x57
#define MPUREG_FIFO_CONFIG1       0x5F
#define MPUREG_FIFO_CONFIG2       0x60
#define MPUREG_FSYNC_CONFIG       0x62
#define MPUREG_INT_CONFIG0        0x63
#define MPUREG_INT_CONFIG1        0x64
#define MPUREG_INT_SOURCE0        0x65
#define MPUREG_INT_SOURCE1        0x66
#define MPUREG_INT_SOURCE2        0x67
#define MPUREG_INT_SOURCE3        0x68
#define MPUREG_INT_SOURCE4        0x69
#define MPUREG_INT_SOURCE5        0x6A
#define MPUREG_FIFO_LOST_PKT0     0x6C
#define MPUREG_SELF_TEST_CONFIG   0x70
#define MPUREG_WHO_AM_I           0x75
#define MPUREG_SCAN0              0x71
#define MPUREG_MEM_BANK_SEL       0x72
#define MPUREG_MEM_START_ADDR     0x73
#define MPUREG_MEM_R_W            0x74
#define MPUREG_REG_BANK_SEL       0x76

/* Bank 1 */
#define MPUREG_SENSOR_CONFIG1_B1      0x04
#define MPUREG_GYRO_CONFIG_STATIC2_B1 0x0B
#define MPUREG_GYRO_CONFIG_STATIC3_B1 0x0C
#define MPUREG_GYRO_CONFIG_STATIC4_B1 0x0D
#define MPUREG_GYRO_CONFIG_STATIC5_B1 0x0E
#define MPUREG_XG_ST_DATA_B1          0x5F
#define MPUREG_YG_ST_DATA_B1          0x60
#define MPUREG_ZG_ST_DATA_B1          0x61
#define MPUREG_TMST_VAL0_B1           0x62
#define MPUREG_INTF_CONFIG4_B1        0x7A			//默认为4线SPI模式
#define MPUREG_INTF_CONFIG5_B1        0x7B
#define MPUREG_INTF_CONFIG6_B1        0x7C
#define MPUREG_INTF_CONFIG3_B1        0x79

/* Bank 2 */
#define MPUREG_ACCEL_CONFIG_STATIC2_B2 0x03
#define MPUREG_ACCEL_CONFIG_STATIC3_B2 0x04
#define MPUREG_ACCEL_CONFIG_STATIC4_B2 0x05
#define MPUREG_ACCEL_CONFIG_STATIC0_B2 0x39
#define MPUREG_XA_ST_DATA_B2           0x3B
#define MPUREG_YA_ST_DATA_B2           0x3C
#define MPUREG_ZA_ST_DATA_B2           0x3D
/* Only accessible from AUX1 */
#define MPUREG_OIS1_CONFIG1_B2         0x44
#define MPUREG_OIS1_CONFIG2_B2         0x45
#define MPUREG_OIS1_CONFIG3_B2         0x46
#define MPUREG_ACCEL_DATA_X0_OIS1_B2   0x49
#define MPUREG_GYRO_DATA_X0_OIS1_B2    0x4F
#define MPUREG_INT_STATUS_OIS1_B2      0x57
/* End of Only accessible from AUX1 */
/* Only accessible from AUX2 */
#define MPUREG_OIS2_CONFIG1_B2         0x59
#define MPUREG_OIS2_CONFIG2_B2         0x5A
#define MPUREG_OIS2_CONFIG3_B2         0x5B
#define MPUREG_ACCEL_DATA_X0_OIS2_B2   0x5E
#define MPUREG_GYRO_DATA_X0_OIS2_B2    0x64
#define MPUREG_INT_STATUS_OIS2_B2      0x6C
/* End of Only accessible from AUX2 */
#define MPUREG_TMD4_B2                 0x70
#define MPUREG_TMD5_B2                 0x71
#define MPUREG_TMD6_B2                 0x72
#define MPUREG_TMD7_B2                 0x73

/* Bank 3 */
#define MPUREG_PU_PD_CONFIG1_B3   0x06
#define MPUREG_PU_PD_CONFIG2_B3   0x0E

/* Bank 4 */
#define MPUREG_FDR_CONFIG_B4      0x09
#define MPUREG_APEX_CONFIG1_B4    0x40
#define MPUREG_APEX_CONFIG2_B4    0x41
#define MPUREG_APEX_CONFIG3_B4    0x42
#define MPUREG_APEX_CONFIG4_B4    0x43
#define MPUREG_APEX_CONFIG5_B4    0x44
#define MPUREG_APEX_CONFIG6_B4    0x45
#define MPUREG_APEX_CONFIG7_B4    0x46
#define MPUREG_APEX_CONFIG8_B4    0x47
#define MPUREG_APEX_CONFIG9_B4    0x48
#define MPUREG_APEX_CONFIG10_B4   0x49
#define MPUREG_ACCEL_WOM_X_THR_B4 0x4A
#define MPUREG_ACCEL_WOM_Y_THR_B4 0x4B
#define MPUREG_ACCEL_WOM_Z_THR_B4 0x4C
#define MPUREG_INT_SOURCE6_B4     0x4D
#define MPUREG_INT_SOURCE7_B4     0x4E
#define MPUREG_INT_SOURCE8_B4     0x4F
#define MPUREG_INT_SOURCE9_B4     0x50
#define MPUREG_INT_SOURCE10_B4    0x51
#define MPUREG_OFFSET_USER_0_B4   0x77
#define MPUREG_OFFSET_USER_1_B4   0x78
#define MPUREG_OFFSET_USER_2_B4   0x79
#define MPUREG_OFFSET_USER_3_B4   0x7A
#define MPUREG_OFFSET_USER_4_B4   0x7B
#define MPUREG_OFFSET_USER_5_B4   0x7C
#define MPUREG_OFFSET_USER_6_B4   0x7D
#define MPUREG_OFFSET_USER_7_B4   0x7E
#define MPUREG_OFFSET_USER_8_B4   0x7F

#define ACC_1G               512


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
	int16_t AcceX;				//acceleration x
	int16_t AcceY;				//acceleration y
	int16_t AcceZ;				//acceleration z
} XYZ_ACCE_T;
typedef struct {
	int16_t GyroX;				//gyro x
	int16_t GyroY;				//gyro y
	int16_t GyroZ;				//gyro z
} XYZ_GYRO_T;
typedef struct {
	float X;				
	float Y;				
	float Z;				
} XYZ_ANGLE_T;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t ICM42688_init(acc_range_t acc_range, gyro_range_t gyro_range);
void ICM42688_Sleep(void);
bool ICM42688_GetImuData(int16_t *accData, int16_t *gyroData);
uint8_t MotionState_Process();


#endif
