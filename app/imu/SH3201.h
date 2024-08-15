#ifndef __SH3201_H 
#define __SH3201_H

#include "emf_typedef.h" 
#include "app/imu/imu_driver.h"
/******************************************************************
*	SH3201 I2C address Macro Definition 
*
*   (7bit):    (0x37)011 0111@SDO=1;    (0x36)011 0110@SDO=0;
******************************************************************/
#define SH3201_ADDRESS				(0x36<<1)		


/******************************************************************
*	SH3201 Registers Macro Definitions 
******************************************************************/
#define SH3201_ACC_XL				(0x00)
#define SH3201_ACC_XH				(0x01)
#define SH3201_ACC_YL				(0x02)
#define SH3201_ACC_YH				(0x03)
#define SH3201_ACC_ZL				(0x04)
#define SH3201_ACC_ZH				(0x05)
#define SH3201_FAST_COMP            (0x06)
#define SH3201_GYRO_XL				(0x06)
#define SH3201_GYRO_XH				(0x07)
#define SH3201_GYRO_YL				(0x08)
#define SH3201_GYRO_YH				(0x09)
#define SH3201_GYRO_ZL				(0x0A)
#define SH3201_GYRO_ZH				(0x0B)
#define SH3201_TEMP_ZL				(0x0C)
#define SH3201_TEMP_ZH				(0x0D)
#define SH3201_INT_STA0				(0x16)
#define SH3201_INT_STA1				(0x17)
#define SH3201_INT_STA2				(0x18)
#define SH3201_INT_STA3				(0x19)
#define SH3201_INT_STA4				(0x1A)
#define SH3201_FIFO_STA0			(0x1B)
#define SH3201_FIFO_STA1			(0x1C)
#define SH3201_FIFO_DATA			(0x1D)

#define SH3201_CHIP_ID				(0x1F)

#define SH3201_ACC_CONF0			(0x20)
#define SH3201_ACC_CONF1			(0x21)
#define SH3201_ACC_CONF2			(0x22)
#define SH3201_ACC_CONF3			(0x23)

#define SH3201_GYRO_CONF0			(0x24)
#define SH3201_GYRO_CONF1			(0x25)
#define SH3201_GYRO_CONF2			(0x26)
#define SH3201_GYRO_CONF3			(0x27)

#define SH3201_TEMP_CONF0			(0x28)
#define SH3201_TEMP_CONF1			(0x29)
#define SH3201_TEMP_CONF2			(0x2A)

#define SH3201_SPI_CONF				(0x32)
#define SH3201_FIFO_CONF0			(0x35)
#define SH3201_FIFO_CONF1			(0x36)
#define SH3201_FIFO_CONF2			(0x37)
#define SH3201_FIFO_CONF3			(0x38)
#define SH3201_FIFO_CONF4			(0x39)

#define SH3201_MI2C_CONF0			(0x3A)
#define SH3201_MI2C_CONF1			(0x3B)
#define SH3201_MI2C_CMD0			(0x3C)
#define SH3201_MI2C_CMD1			(0x3D)
#define SH3201_MI2C_WR				(0x3E)
#define SH3201_MI2C_RD				(0x3F)

#define SH3201_INT_ENABLE0			(0x40)
#define SH3201_INT_ENABLE1			(0x41)
#define SH3201_INT_CONF				(0x42)
#define SH3201_INT_LIMIT			(0x43)
#define SH3201_ORIEN_INTCONF0		(0x44)
#define SH3201_ORIEN_INTCONF1		(0x45)
#define SH3201_ORIEN_INT_1G5_LOW	(0x46)
#define SH3201_ORIEN_INT_1G5_HIGH	(0x47)
#define SH3201_ORIEN_INT_SLOPE_LOW	(0x48)
#define SH3201_ORIEN_INT_SLOPE_HIGH	(0x49)
#define SH3201_ORIEN_INT_HYST_LOW	(0x4A)
#define SH3201_ORIEN_INT_HYST_HIGH	(0x4B)
#define SH3201_FLAT_INT_CONF		(0x4C)

#define SH3201_ACT_INACT_INT_CONF	(0x4D)
#define SH3201_ACT_INACT_INT_LINK	(0x4E)
#define SH3201_ACT_INT_THRESHOLDL	(0x54)
#define SH3201_ACT_INT_THRESHOLDH	(0x55)
#define SH3201_ACT_INT_TIME			(0x56)
#define SH3201_INACT_INT_THRESHOLDL	(0x57)
#define SH3201_INACT_INT_THRESHOLDM	(0x58)
#define SH3201_INACT_INT_THRESHOLDH	(0x59)
#define SH3201_INACT_INT_TIME		(0x5A)
#define SH3201_INACT_INT_1G_REFL	(0x5B)
#define SH3201_INACT_INT_1G_REFH	(0x5C)

#define SH3201_TAP_INT_THRESHOLDL	(0x4F)
#define SH3201_TAP_INT_THRESHOLDH	(0x50)
#define SH3201_TAP_INT_DURATION		(0x51)
#define SH3201_TAP_INT_LATENCY		(0x52)
#define SH3201_DTAP_INT_WINDOW		(0x53)

#define SH3201_HIGHLOW_G_INT_CONF	(0x5D)
#define SH3201_HIGHG_INT_THRESHOLDL	(0x5E)
#define SH3201_HIGHG_INT_THRESHOLDH	(0x5F)
#define SH3201_HIGHG_INT_TIME		(0x60)

#define SH3201_LOWG_INT_THRESHOLDL	(0x61)
#define SH3201_LOWG_INT_THRESHOLDH	(0x62)
#define SH3201_LOWG_INT_TIME		(0x63)

#define SH3201_FREEFALL_INT_THRES	(0x64)
#define SH3201_FREEFALL_INT_TIME	(0x65)
#define SH3201_INT_PIN_MAP0			(0x66)
#define SH3201_INT_PIN_MAP1			(0x67)

#define SH3201_SPI_REG_ACCESS		(0x7F)
#define SH3201_AUX_I2C_CONF			(0xFD)


/******************************************************************
*	ACC Config Macro Definitions 
******************************************************************/
#define SH3201_ACC_ODR_1000HZ	(0x00)
#define SH3201_ACC_ODR_500HZ	(0x01)
#define SH3201_ACC_ODR_250HZ	(0x02)
#define SH3201_ACC_ODR_125HZ	(0x03)
#define SH3201_ACC_ODR_63HZ		(0x04)
#define SH3201_ACC_ODR_31HZ		(0x05)
#define SH3201_ACC_ODR_16HZ		(0x06)
#define SH3201_ACC_ODR_2000HZ	(0x08)
#define SH3201_ACC_ODR_4000HZ	(0x09)
#define SH3201_ACC_ODR_8000HZ	(0x0A)

#define SH3201_ACC_RANGE_2G		(0x00)
#define SH3201_ACC_RANGE_4G		(0x01)
#define SH3201_ACC_RANGE_8G		(0x02)
#define SH3201_ACC_RANGE_16G	(0x03)

#define SH3201_ACC_ODRX040		(0x00)
#define SH3201_ACC_ODRX025		(0x01)
#define SH3201_ACC_ODRX011		(0x02)
#define SH3201_ACC_ODRX004		(0x03)
#define SH3201_ACC_ODRX002		(0x04)

#define SH3201_ACC_FILTER_EN	(0x01)
#define SH3201_ACC_FILTER_DIS	(0x00)
#define SH3201_ACC_BYPASS_EN	(0x02)
#define SH3201_ACC_BYPASS_DIS	(0x00)


/******************************************************************
*	GYRO Config Macro Definitions 
******************************************************************/
#define SH3201_GYRO_ODR_800HZ	(0x00)
#define SH3201_GYRO_ODR_400HZ	(0x01)
#define SH3201_GYRO_ODR_200HZ	(0x02)
#define SH3201_GYRO_ODR_100HZ	(0x03)
#define SH3201_GYRO_ODR_50HZ	(0x04)
#define SH3201_GYRO_ODR_25HZ	(0x05)
#define SH3201_GYRO_ODR_1600HZ	(0x08)
#define SH3201_GYRO_ODR_3200HZ	(0x09)
#define SH3201_GYRO_ODR_6400HZ	(0x0A)
#define SH3201_GYRO_ODR_12800HZ	(0x0B)
#define SH3201_GYRO_ODR_25600HZ	(0x0C)

#define SH3201_GYRO_RANGE_125	(0x02)
#define SH3201_GYRO_RANGE_250	(0x03)
#define SH3201_GYRO_RANGE_500	(0x04)
#define SH3201_GYRO_RANGE_1000	(0x05)
#define SH3201_GYRO_RANGE_2000	(0x06)

#define SH3201_GYRO_ODRX00		(0x00)
#define SH3201_GYRO_ODRX01		(0x01)
#define SH3201_GYRO_ODRX02		(0x02)
#define SH3201_GYRO_ODRX03		(0x03)

#define SH3201_GYRO_FILTER_EN	(0x01)
#define SH3201_GYRO_FILTER_DIS	(0x00)
#define SH3201_GYRO_BYPASS_EN	(0x02)
#define SH3201_GYRO_BYPASS_DIS	(0x00)

#define SH3201_GYRO_OFF_INACT   (0x80)
#define SH3201_GYRO_ON_INACT    (0x00)
/******************************************************************
*	Temperature Config Macro Definitions 
******************************************************************/
#define SH3201_TEMP_ODR_500HZ	(0x00)
#define SH3201_TEMP_ODR_250HZ	(0x02)
#define SH3201_TEMP_ODR_125HZ	(0x04)
#define SH3201_TEMP_ODR_63HZ	(0x06)

#define SH3201_TEMP_EN			(0x01)
#define SH3201_TEMP_DIS			(0x00)

/******************************************************************
*	INT Config Macro Definitions 
******************************************************************/
#define SH3201_INT_LOWG				(0x8000)
#define SH3201_INT_HIGHG			(0x4000)
#define SH3201_INT_INACT			(0x2000)
#define SH3201_INT_ACT				(0x1000)
#define SH3201_INT_DOUBLE_TAP	  	(0x0800)
#define SH3201_INT_SINGLE_TAP	  	(0x0400)
#define SH3201_INT_FLAT				(0x0200)
#define SH3201_INT_ORIENTATION		(0x0100)
#define SH3201_INT_FIFO_WATERMARK	(0x0008)
#define SH3201_INT_GYRO_READY		(0x0004)
#define SH3201_INT_ACC_READY		(0x0002)
#define SH3201_INT_FREE_FALL		(0x0001)
#define SH3201_INT_UP_DOWN_Z    	(0x0040)

#define SH3201_INT_ENABLE			(0x01)
#define SH3201_INT_DISABLE			(0x00)

#define SH3201_INT_MAP_INT1			(0x01)
#define SH3201_INT_MAP_INT			(0x00)

#define SH3201_INT0_LEVEL_LOW		(0x80)
#define SH3201_INT0_LEVEL_HIGH		(0x7F)
#define SH3201_INT_NO_LATCH			(0x40)
#define SH3201_INT_LATCH			(0xBF)
#define SH3201_INT1_LEVEL_LOW		(0x20)
#define SH3201_INT1_LEVEL_HIGH		(0xDF)
#define SH3201_INT_CLEAR_ANY		(0x10)
#define SH3201_INT_CLEAR_STATUS	    (0xEF)
#define SH3201_INT1_NORMAL	    	(0x04)
#define SH3201_INT1_OD				(0xFB)
#define SH3201_INT0_NORMAL			(0x01)
#define SH3201_INT0_OD				(0xFE)

/******************************************************************
*	Orientation Blocking Config Macro Definitions 
******************************************************************/
#define SH3201_ORIENT_BLOCK_MODE0	(0x00)
#define SH3201_ORIENT_BLOCK_MODE1	(0x04)
#define SH3201_ORIENT_BLOCK_MODE2	(0x08)
#define SH3201_ORIENT_BLOCK_MODE3	(0x0C)

#define SH3201_ORIENT_SYMM			(0x00)
#define SH3201_ORIENT_HIGH_ASYMM	(0x01)
#define SH3201_ORIENT_LOW_ASYMM		(0x02)


/******************************************************************
*	Flat Time Config Macro Definitions 
******************************************************************/
#define SH3201_FLAT_TIME_200MS		(0x00)
#define SH3201_FLAT_TIME_400MS		(0x40)
#define SH3201_FLAT_TIME_800MS		(0x80)
#define SH3201_FLAT_TIME_000MS		(0xC0)

/******************************************************************
*	ACT and INACT Int Config Macro Definitions 
******************************************************************/
#define SH3201_ACT_AC_MODE		(0x80)
#define SH3201_ACT_DC_MODE		(0x00)
#define SH3201_ACT_X_INT_EN		(0x40)
#define SH3201_ACT_X_INT_DIS	(0x00)
#define SH3201_ACT_Y_INT_EN		(0x20)
#define SH3201_ACT_Y_INT_DIS	(0x00)
#define SH3201_ACT_Z_INT_EN		(0x10)
#define SH3201_ACT_Z_INT_DIS	(0x00)


#define SH3201_INACT_AC_MODE	(0x08)
#define SH3201_INACT_DC_MODE	(0x00)
#define SH3201_INACT_X_INT_EN	(0x04)
#define SH3201_INACT_X_INT_DIS	(0x00)
#define SH3201_INACT_Y_INT_EN	(0x02)
#define SH3201_INACT_Y_INT_DIS	(0x00)
#define SH3201_INACT_Z_INT_EN	(0x01)
#define SH3201_INACT_Z_INT_DIS	(0x00)


#define SH3201_LINK_PRE_STA		(0x01)
#define SH3201_LINK_PRE_STA_NO	(0x00)

#define SH3201_ACT_INACT_CLR_STATUS1	(0x00)
#define SH3201_ACT_INACT_CLR_STATUS3	(0x80)

/******************************************************************
*	TAP Int Config Macro Definitions 
******************************************************************/
#define SH3201_TAP_X_INT_EN		(0x08)
#define SH3201_TAP_X_INT_DIS	(0x00)
#define SH3201_TAP_Y_INT_EN		(0x04)
#define SH3201_TAP_Y_INT_DIS	(0x00)
#define SH3201_TAP_Z_INT_EN		(0x02)
#define SH3201_TAP_Z_INT_DIS	(0x00)


/******************************************************************
*	HIGHG Int Config Macro Definitions 
******************************************************************/
#define SH3201_HIGHG_ALL_INT_EN		(0x80)
#define SH3201_HIGHG_ALL_INT_DIS	(0x00)
#define SH3201_HIGHG_X_INT_EN		(0x40)
#define SH3201_HIGHG_X_INT_DIS		(0x00)
#define SH3201_HIGHG_Y_INT_EN		(0x20)
#define SH3201_HIGHG_Y_INT_DIS		(0x00)
#define SH3201_HIGHG_Z_INT_EN		(0x10)
#define SH3201_HIGHG_Z_INT_DIS		(0x00)


/******************************************************************
*	LOWG Int Config Macro Definitions 
******************************************************************/
#define SH3201_LOWG_ALL_INT_EN		(0x01)
#define SH3201_LOWG_ALL_INT_DIS		(0x00)


/******************************************************************
*	SPI Interface Config Macro Definitions 
******************************************************************/
#define SH3201_SPI_3_WIRE		(0x01)
#define SH3201_SPI_4_WIRE		(0x00)



/******************************************************************
*	FIFO Config Macro Definitions 
******************************************************************/
#define SH3201_FIFO_MODE_DIS		(0x00)
#define SH3201_FIFO_MODE_FIFO		(0x01)
#define SH3201_FIFO_MODE_STREAM		(0x02)
#define SH3201_FIFO_MODE_TRIGGER	(0x03)

#define SH3201_FIFO_ACC_DOWNS_EN	(0x80)
#define SH3201_FIFO_ACC_DOWNS_DIS	(0x00)
#define SH3201_FIFO_GYRO_DOWNS_EN	(0x08)
#define SH3201_FIFO_GYRO_DOWNS_DIS	(0x00)

#define SH3201_FIFO_FREQ_X1_2		(0x00)
#define SH3201_FIFO_FREQ_X1_4		(0x01)
#define SH3201_FIFO_FREQ_X1_8		(0x02)
#define SH3201_FIFO_FREQ_X1_16		(0x03)
#define SH3201_FIFO_FREQ_X1_32		(0x04)
#define SH3201_FIFO_FREQ_X1_64		(0x05)
#define SH3201_FIFO_FREQ_X1_128		(0x06)
#define SH3201_FIFO_FREQ_X1_256		(0x07)

#define SH3201_FIFO_EXT_Z_EN		(0x2000)
#define SH3201_FIFO_EXT_Y_EN		(0x1000)
#define SH3201_FIFO_EXT_X_EN		(0x0080)
#define SH3201_FIFO_TEMPERATURE_EN	(0x0040)
#define SH3201_FIFO_GYRO_Z_EN		(0x0020)
#define SH3201_FIFO_GYRO_Y_EN		(0x0010)
#define SH3201_FIFO_GYRO_X_EN		(0x0008)
#define SH3201_FIFO_ACC_Z_EN		(0x0004)
#define SH3201_FIFO_ACC_Y_EN		(0x0002)
#define SH3201_FIFO_ACC_X_EN		(0x0001)
#define SH3201_FIFO_ALL_DIS			(0x0000)

/******************************************************************
*	AUX I2C Config Macro Definitions 
******************************************************************/
#define SH3201_MI2C_NORMAL_MODE			(0x00)
#define SH3201_MI2C_BYPASS_MODE			(0x01)

#define SH3201_MI2C_READ_ODR_200HZ	    (0x00)
#define SH3201_MI2C_READ_ODR_100HZ	    (0x10)
#define SH3201_MI2C_READ_ODR_50HZ		(0x20)
#define SH3201_MI2C_READ_ODR_25HZ		(0x30)

#define SH3201_MI2C_FAIL				(0x20)
#define SH3201_MI2C_SUCCESS				(0x10)

#define SH3201_MI2C_READ_MODE_AUTO		(0x40)
#define SH3201_MI2C_READ_MODE_MANUAL	(0x00)


/******************************************************************
*	Fast offset compensation Macro Definitions 
******************************************************************/
#define SH3201_X_UP     (0x00)
#define SH3201_X_DOWN   (0x01)
#define SH3201_Y_UP     (0x02)
#define SH3201_Y_DOWN   (0x03)
#define SH3201_Z_UP     (0x04)
#define SH3201_Z_DOWN   (0x05)


/******************************************************************
*	Other Macro Definitions 
******************************************************************/
#define SH3201_TRUE		(0)
#define SH3201_FALSE	(1)

#define SH3001_POWMODE_REG_NUM  9

#define SH3201_NORMAL_MODE		(0x00)
#define SH3201_ACC_HP_MODE		(0x01)
#define SH3201_SLEEP_MODE		(0x02)
#define SH3201_SLEEP_LP_MODE		(0x03)
#define SH3201_POWERDOWN_MODE		(0x04)

// FIFO Mode DIV = FIFO channel * 2;
#define SH3201_WATERMARK_DIV    (12)     		
#define SH3201_FIFO_BUFFER      (1024)

#define SH3201_MAX_TRY_NUM                 5
#define SH3201_TRY_0                       0
#define SH3201_TRY_1                       1
#define SH3201_TRY_2                       2
#define SH3201_TRY_3                       3
#define SH3201_TRY_4                       4

#define SH3201_GYRO_AC_REG      	(0x14)
#define SH3201_AC_VALUE    		(0x1F)

/***************************************************************************
*       Type Definitions                                              
****************************************************************************/
typedef unsigned char (*IMU_read)(	unsigned char devAddr, 
                                    unsigned char regAddr, 
                                    unsigned short readLen, 
                                    unsigned char *readBuf);

typedef unsigned char (*IMU_write)(	unsigned char devAddr, 
                                    unsigned char regAddr, 
                                    unsigned short writeLen, 
                                    unsigned char *writeBuf);
													

/***************************************************************************
        Exported Functions                                           
****************************************************************************/

extern unsigned char SH3201_init(uint8_t acc_range, uint8_t gyro_range);//TODO
extern bool SH3201_GetImuData( short accData[3], short gyroData[3] ,unsigned char isr);
extern float SH3201_GetTempData(void);																	
extern unsigned char SH3201_SwitchPowerMode(unsigned char powerMode);		
																																																																																						
																		

// interrupt related fucntions
extern void SH3201_INT_Enable(	unsigned short int intType, 
								unsigned char intEnable, 
								unsigned char intPinSel);

extern void SH3201_INT_Config(	unsigned char int0Level,
								unsigned char intLatch,
								unsigned char int1Level,  
								unsigned char intClear,
								unsigned char int1Mode,
								unsigned char int0Mode,
								unsigned char intTime);
								
extern void SH3201_INT_Orient_Config(	unsigned char 	orientBlockMode,
										unsigned char 	orientMode,
										unsigned char	orientTheta,
										unsigned short	orientG1point5,
										unsigned short 	orientSlope,
										unsigned short 	orientHyst);								
								
extern void SH3201_INT_Flat_Config(	unsigned char flatTimeTH, 
									unsigned char flatTanHeta2);								
								
extern void SH3201_INT_Act_Config(	unsigned char actEnDisIntX,
									unsigned char actEnDisIntY,
									unsigned char actEnDisIntZ,
									unsigned char actIntMode,
									unsigned char actTimeNum,
									unsigned short actIntThres,
									unsigned char actLinkStatus,
									unsigned char actIntClear);								

extern void SH3201_INT_Inact_Config(	unsigned char inactEnDisIntX,
										unsigned char inactEnDisIntY,
										unsigned char inactEnDisIntZ,
										unsigned char inactIntMode,
										unsigned char inactLinkStatus,	
										unsigned char inactTimeMs,
										unsigned int inactIntThres,
										unsigned short int inactG1,
										unsigned char inactIntClear);

extern void SH3201_INT_Tap_Config(	unsigned char tapEnDisIntX,
									unsigned char tapEnDisIntY,
									unsigned char tapEnDisIntZ,
									unsigned char tapIntThres,
									unsigned char tapTimeMs,
									unsigned char tapWaitTimeMs,
									unsigned char tapWaitTimeWindowMs);

extern void SH3201_INT_HighG_Config(	unsigned char highGEnDisIntX,
										unsigned char highGEnDisIntY,
										unsigned char highGEnDisIntZ,
										unsigned char highGEnDisIntAll,
										unsigned short highGThres,
										unsigned char highGTimsMs);

extern void SH3201_INT_LowG_Config(	unsigned char lowGEnDisIntAll,
									unsigned short lowGThres,
									unsigned char lowGTimsMs);

extern void SH3201_INT_FreeFall_Config(	unsigned char freeFallThres,
										unsigned char freeFallTimsMs);

extern unsigned short SH3201_INT_Read_Status01(void);

extern unsigned char SH3201_INT_Read_Status2(void);

extern unsigned char SH3201_INT_Read_Status3(void);

extern unsigned char SH3201_INT_Read_Status4(void);
								
			
// Fifo related fucntions
extern void SH3201_FIFO_Reset(unsigned char fifoMode);

extern void SH3201_FIFO_Freq_Config(	unsigned char fifoAccDownSampleEnDis,
										unsigned char fifoAccFreq,
										unsigned char fifoGyroDownSampleEnDis,
										unsigned char fifoGyroFreq);
										
extern void SH3201_FIFO_Data_Config(	unsigned short fifoMode,
										unsigned short fifoWaterMarkLevel);										
										
extern unsigned char SH3201_FIFO_Read_Status(unsigned short int *fifoEntriesCount);										
										
extern void SH3201_FIFO_Read_Data(	unsigned char *fifoReadData, 
									unsigned short int fifoDataLength);										
			

// Master I2C related fucntions
extern void SH3201_MI2C_Reset(void);

extern void SH3201_MI2C_Bus_Config(	unsigned char mi2cReadMode,
									unsigned char mi2cODR,
									unsigned char mi2cFreq);			
			
extern void SH3201_MI2C_Cmd_Config(	unsigned char mi2cSlaveAddr,
									unsigned char mi2cSlaveCmd,
									unsigned char mi2cReadMode);			
			
extern unsigned char SH3201_MI2C_Write(unsigned char mi2cWriteData);			
			
extern unsigned char SH3201_MI2C_Read(unsigned char *mi2cReadData);

																																																		
// SPI interface related fucntions
extern void SH3201_SPI_Config(unsigned char spiInterfaceMode);

// fast offset compensation function
extern void SH3201_FastOffsetComp(unsigned char sh3201Positon);                  
 
    
// Read FIFO data functions
extern void SH3201_initFIFO(void);                                    
    
extern void SH3201_ReadFIFO(void);      
                    
#endif

