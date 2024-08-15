/******************************************************************
* Senodia 6D IMU(Gyroscope+Accelerometer) SH3201(MCB) driver, By Senodia AE@20220512;
* SPI interface;
*
* 1. SH3201_init() : SH3201 initialize function
* 2. SH3201_GetImuData( ... ): SH3201 read gyro and accel data function
* 3. SH3201_GetTempData(): SH3201 read termperature function
* 4. delay_ms(x): delay x millisecond.
* 5. SH3201_initFIFO(): initialize FIFO function
* 6. SH3201_ReadFIFO(): read FIFO data
*	
******************************************************************/
#include "hw_config.h"
#ifdef  IMU_SH3201_ID
//#include <stdio.h>
#include "hw_board.h"
#include "api/api_iic_host.h"
#include "api/api_tick.h"
#include "SH3201.h"

#include "api/api_log.h"


/*==============================================================================

    Change Log:


    April 20 2023 Senodia 1.01.01
        - Add sh3201 version 1.01.01
    May 23 2023 Senodia 1.01.02
        - Add sh3201 data stabilization after odr changing version 1.01.02
    Dec 1 2023 Senodia 1.01.03
        - Optimized gyro starting.
    Jan 8 2024 Senodia 1.01.04
        - Optimized ADCReset.
    Jan 10 2024 Senodia 1.01.05
        - Optimized ADCReset and add Adc_Polarity.
    Jan 23 2024 Senodia 1.01.06
        - add var dym and cut the clock.
    March 1 2024 Senodia 1.01.07
        - Optimized sh3201 fifo read.
    April 18 2024 Senodia 1.01.08
        - Add en&dis channel and fifo reset for read fifo.
    May 7 2024 Senodia 1.01.09
        - Add SH3201_ADJUST_CF.
    May 13 2024 Senodia 1.01.10
        - Delete the SH3201_ADJUST_CF in init.
  ============================================================================*/


// 32-bit version number represented as major[31:16].minor[15:0]
#define SH3201_MAJOR        1
#define SH3201_MINOR       01
#define SH3201_REV         10
#define MCU_VERSION_SH3201  ((SH3201_MAJOR<<16) | (SH3201_MINOR<<8) | SH3201_REV)


/******************************************************************
* Description:	I2C or SPI bus interface functions	and delay time function
*
* Parameters: 
*   devAddr: I2C device address, modify the macro(SH3201_ADDRESS @SH3201.h) based on your I2C function.						  
*            If SPI interface, please ingnore the parameter.
*   regAddr: register address
*   readLen: data length to read
*   *readBuf: data buffer to read; If using FIFO function, the max buffer size is 1024; 
*   writeLen: data length to write
*   *writeBuf: data buffer to write
*	
* Note: I2C interface, define USER_INTERFACE_I2C (1)
*       SPI interface, define USER_INTERFACE_I2C (0)
*		SPI 3 Wire interface, define SH3201_SPI_3WIRE_MODE (1)
*		SPI 4 Wire interface, define SH3201_SPI_3WIRE_MODE (0)
*	
******************************************************************/
#define USER_INTERFACE_I2C		(1) //TODO

#if USER_INTERFACE_I2C

unsigned char I2C_ReadNBytes(	unsigned char devAddr, 
                                unsigned char regAddr, 
                                unsigned short readLen, 
                                unsigned char *readBuf)
{
	//your I2C interface code:
	#ifdef HW_IIC_MAP
  	return api_iic_host_read(IMU_SH3201_ID,devAddr,regAddr,readBuf,readLen);
  	#else
	return (SH3201_TRUE);
	#endif
}

unsigned char I2C_WriteNBytes(	unsigned char devAddr, 
                                unsigned char regAddr, 
                                unsigned short writeLen, 
                                unsigned char *writeBuf)
{
	//your I2C interface code:
	#ifdef HW_IIC_MAP
  	return api_iic_host_write(IMU_SH3201_ID,devAddr,(uint16_t)regAddr,(uint8_t const *)writeBuf,(uint16_t)writeLen);
	#else
	return (SH3201_TRUE);
	#endif
}																


/**
 * \brief
 * \return 0:读取失败 1:读取成功
 */
bool I2C_Isr_Read_NBytes(	unsigned char devAddr, 
                                unsigned char regAddr, 
                                unsigned char readLen, 
                                unsigned char *readBuf)
{
	//your I2C interface code:
	//......
	#ifdef HW_IIC_MAP
  	return api_iic_host_isr_read(IMU_SH3201_ID,(uint16_t)devAddr,regAddr,readBuf,(uint16_t)readLen);
	#else
	return true;
	#endif
}

IMU_read    SH3201_read     = I2C_ReadNBytes;
IMU_write   SH3201_write    = I2C_WriteNBytes;

#else

#define SH3201_SPI_3WIRE_MODE	(1)

unsigned char SPI_readNBytes (	unsigned char devAddr, 
                                unsigned char regAddr, 
                                unsigned short readLen, 
                                unsigned char *readBuf)
{
	//your SPI interface code:

	//For example:	
	//unsigned char u8Data;		
	//devAddr = devAddr;	
	//u8Data = (regAddr > 0x7F) ? 0x01 : 0x00;
	//SPIWrite(SH3201_SPI_REG_ACCESS, &u8Data, 1);
	
	//SPIRead((regAddr | 0x80), readBuf, readLen);	
	return (SH3201_TRUE);
}

unsigned char SPI_writeNBytes(	unsigned char devAddr, 
                                unsigned char regAddr, 
                                unsigned short writeLen, 
                                unsigned char *writeBuf)
{
	//your SPI interface code:
	
	//For example: 
	//unsigned char u8Data;	
	//devAddr = devAddr;	
	//u8Data = (regAddr > 0x7F) ? 0x01 : 0x00;
	//SPIWrite(SH3201_SPI_REG_ACCESS, &u8Data, 1);
	
	//SPIWrite((regAddr & 0x7F), writeBuf, writeLen);
	return (SH3201_TRUE);
}																

IMU_read 	SH3201_read		= SPI_readNBytes;
IMU_write	SH3201_write	= SPI_writeNBytes;

#endif


static void delay_ms(unsigned short mSecond)
{  
	//your delay code(mSecond: millisecond):
	//......
} 




/******************************************************************
* Description:	Local Variable		
******************************************************************/


/******************************************************************
* Description:	Local Function Prototypes		
******************************************************************/
static void SH3201_ModuleReset(void);
static void SH3201_Acc_Config(unsigned char accODR, 
                              unsigned char accRange, 
                              unsigned char accCutOffFreq,
                              unsigned char accFilter,
                              unsigned char accByPass);

static void SH3201_Gyro_Config(	unsigned char gyroODR, 
                                unsigned char gyroRange, 
                                unsigned char gyroCutOffFreq,
                                unsigned char gyroFilter,
                                unsigned char gyroByPass,
                                unsigned char gyroOnOff);
												
static void SH3201_Temp_Config(	unsigned char tempODR, 
                                unsigned char tempEnable);																									

/******************************************************************
* Description: SoftReset;
*
* Parameters: void	
*  						
* return:	void
*																
******************************************************************/
static void SH3201_SoftReset(void)
{

	unsigned char regData = 0;	
	
	// soft reset
	regData = 0x01;
	SH3201_write(SH3201_ADDRESS, 0x2B, 1, &regData);
	regData = 0x73;
	SH3201_write(SH3201_ADDRESS, 0x00, 1, &regData);

}


/******************************************************************
* Description: reset Drive Start;
*
* Parameters: void	
*  						
* return:	void
*																
******************************************************************/
static void SH3201_DriveStart(void)
{

  uint8_t regData = 0x0;
	
  regData = 0x01;
  SH3201_write(SH3201_ADDRESS, 0x2B, 1, &regData);

  delay_ms(2);

  regData = 0x00;
  SH3201_write(SH3201_ADDRESS, 0x2B, 1, &regData);


}


/******************************************************************
* Description: reset ADC module;
*
* Parameters: void	
*  						
* return:	void
*																
******************************************************************/
static void SH3201_ADCReset(void)
{

    unsigned char regData = 0;

	regData = 0x00;
	SH3201_write(SH3201_ADDRESS, 0xDD, 1, &regData);

	regData = 0x40;
	SH3201_write(SH3201_ADDRESS, 0xD3, 1, &regData);

	regData = 0x02;
	SH3201_write(SH3201_ADDRESS, 0xD5, 1, &regData);
	delay_ms(3);

	regData = 0x42;
	SH3201_write(SH3201_ADDRESS, 0xD3, 1, &regData);
	delay_ms(2);

	regData = 0x00;
	SH3201_write(SH3201_ADDRESS, 0xD5, 1, &regData);

}

/******************************************************************
* Description: reset CVA module;
*
* Parameters: void	
*  						
* return:	void
*																
******************************************************************/
static void SH3201_CVAReset(void)
{

  unsigned char regAddr[3] = { 0xB9, 0xBA, 0xBB};
  unsigned char regDataA[3] = {0x0C, 0x88, 0x0C};
  unsigned char regDataB[3] = {0x04, 0x80, 0x04};


  //CVA Reset
  SH3201_write(SH3201_ADDRESS, regAddr[0], 1, &regDataA[0]);
  SH3201_write(SH3201_ADDRESS, regAddr[1], 1, &regDataA[1]);
  SH3201_write(SH3201_ADDRESS, regAddr[2], 1, &regDataA[2]);

  delay_ms(1);

  SH3201_write(SH3201_ADDRESS, regAddr[0], 1, &regDataB[0]);
  SH3201_write(SH3201_ADDRESS, regAddr[1], 1, &regDataB[1]);
  SH3201_write(SH3201_ADDRESS, regAddr[2], 1, &regDataB[2]);

}



	
/******************************************************************
* Description: reset some internal modules;
*
* Parameters: void	
*  						
* return:	void
*																
******************************************************************/
static void SH3201_ModuleReset(void)
{
	unsigned char regData = 0;	
	
	// soft reset
	SH3201_SoftReset();

	delay_ms(20);

#if SH3201_SPI_3WIRE_MODE
	SH3201_SPI_Config(SH3201_SPI_3_WIRE);
#endif
	
	SH3201_DriveStart();	
	
	// ADC reset
	SH3201_ADCReset();
	
	// CVA reset
	SH3201_CVAReset();	

	delay_ms(250);
}	


/******************************************************************
* Description:	1.set accelerometer parameters;
*               2.accCutOffRreq = accODR * 0.40 or accODR * 0.25 or accODR * 0.11 or accODR * 0.04 or accODR * 0.02;
*
* Parameters: 	accODR              	accRange                accCutOffFreq       	accFilter				accByPass
*               SH3201_ACC_ODR_1000HZ	SH3201_ACC_RANGE_16G	SH3201_ACC_ODRX040		SH3201_ACC_FILTER_EN	SH3201_ACC_BYPASS_EN
*               SH3201_ACC_ODR_500HZ	SH3201_ACC_RANGE_8G		SH3201_ACC_ODRX025		SH3201_ACC_FILTER_DIS	SH3201_ACC_BYPASS_DIS
*               SH3201_ACC_ODR_250HZ	SH3201_ACC_RANGE_4G		SH3201_ACC_ODRX011
*               SH3201_ACC_ODR_125HZ	SH3201_ACC_RANGE_2G		SH3201_ACC_ODRX004
*               SH3201_ACC_ODR_63HZ								SH3201_ACC_ODRX002
*               SH3201_ACC_ODR_31HZ
*               SH3201_ACC_ODR_16HZ
*               SH3201_ACC_ODR_2000HZ
*               SH3201_ACC_ODR_4000HZ
*               SH3201_ACC_ODR_8000HZ
*  
* return:	void
*
******************************************************************/
static void SH3201_Acc_Config(unsigned char accODR, 
                              unsigned char accRange, 
                              unsigned char accCutOffFreq,
                              unsigned char accFilter,
                              unsigned char accByPass)
{
    unsigned char regData = 0;	

	// enable acc digital filter and bypass or not
	//SH3201_read(SH3201_ADDRESS, SH3201_ACC_CONF0, 1, &regData);
	regData = 0x29;
	regData = (regData & 0xFC) | accFilter | accByPass;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF0, 1, &regData);
	
	// set acc ODR
	//SH3201_read(SH3201_ADDRESS, SH3201_ACC_CONF1, 1, &regData);
	regData = 0x00;
	regData = (regData & 0xF0) | accODR;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF1, 1, &accODR);
	
	// set acc low pass filter cut-off frequency
	//SH3201_read(SH3201_ADDRESS, SH3201_ACC_CONF2, 1, &regData);
	regData = 0x81;
	regData = (regData & 0xF8) | accCutOffFreq;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF2, 1, &regData);
		
	// set acc Range
	//SH3201_read(SH3201_ADDRESS, SH3201_ACC_CONF3, 1, &regData);
	regData = 0x03;
	regData = (regData & 0xF8) | accRange;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF3, 1, &regData); 
}


/******************************************************************
* Description:	1.set gyroscope parameters;
*               2.gyroCutOffRreq is at Page 33 of SH3201 datasheet.
*
* Parameters:   gyroODR             	gyroRangeX,Y,Z          gyroCutOffFreq      gyroFilter				gyroByPass
*               SH3201_GYRO_ODR_800HZ	SH3201_GYRO_RANGE_125	SH3201_GYRO_ODRX00	SH3201_GYRO_FILTER_EN	SH3201_GYRO_BYPASS_EN
*               SH3201_GYRO_ODR_400HZ	SH3201_GYRO_RANGE_250	SH3201_GYRO_ODRX01	SH3201_GYRO_FILTER_DIS	SH3201_GYRO_BYPASS_DIS
*               SH3201_GYRO_ODR_200HZ	SH3201_GYRO_RANGE_500	SH3201_GYRO_ODRX02
*               SH3201_GYRO_ODR_100HZ	SH3201_GYRO_RANGE_1000  SH3201_GYRO_ODRX03
*               SH3201_GYRO_ODR_50HZ	SH3201_GYRO_RANGE_2000
*               SH3201_GYRO_ODR_25HZ
*               SH3201_GYRO_ODR_1600HZ
*               SH3201_GYRO_ODR_3200HZ
*               SH3201_GYRO_ODR_6400HZ
*               SH3201_GYRO_ODR_12800HZ
*               SH3201_GYRO_ODR_25600HZ
*  	 
*               gyroOnOff
*               SH3201_GYRO_OFF_INACT
*               SH3201_GYRO_ON_INACT
* return:	void
*
******************************************************************/
static void SH3201_Gyro_Config(	unsigned char gyroODR, 
                                unsigned char gyroRange, 
                                unsigned char gyroCutOffFreq,
                                unsigned char gyroFilter,
                                unsigned char gyroByPass,
                                unsigned char gyroOnOff)
{
    unsigned char regData = 0;	
	
	// enable gyro digital filter
	//SH3201_read(SH3201_ADDRESS, SH3201_GYRO_CONF0, 1, &regData);
	regData = 0x01;
	regData = (regData & 0x7C) | gyroFilter | gyroByPass | gyroOnOff;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF0, 1, &regData);
	
	// set gyro ODR
	//SH3201_read(SH3201_ADDRESS, SH3201_GYRO_CONF1, 1, &regData);
	regData = 0x00;
	regData = (regData & 0xF0) | gyroODR;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF1, 1, &regData);
	
	// set gyro low pass filter cut-off frequency
	//SH3201_read(SH3201_ADDRESS, SH3201_GYRO_CONF2, 1, &regData);
	regData = 0x00;
	regData = (regData & 0xFC) | gyroCutOffFreq;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF2, 1, &regData);
		
	// set gyro range
	//SH3201_read(SH3201_ADDRESS, SH3201_GYRO_CONF3, 1, &regData);
	regData = 0x06;
	regData = (regData & 0xF8) | gyroRange;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF3, 1, &regData);		 
}


/******************************************************************
* Description:	set temperature parameters;
*
* Parameters: 	tempODR                   tempEnable
*               SH3201_TEMP_ODR_500HZ	  SH3201_TEMP_EN
*               SH3201_TEMP_ODR_250HZ	  SH3201_TEMP_DIS
*               SH3201_TEMP_ODR_125HZ		
*               SH3201_TEMP_ODR_63HZ		
*
* return:	void
*  							
******************************************************************/
static void SH3201_Temp_Config(	unsigned char tempODR, 
                                unsigned char tempEnable)
{
    unsigned char regData = 0;	
	
	// enable temperature, set ODR
	//SH3201_read(SH3201_ADDRESS, SH3201_TEMP_CONF0, 1, &regData);
	regData = 0x00;
	regData = (regData & 0xF8) | tempODR | tempEnable;
	SH3201_write(SH3201_ADDRESS, SH3201_TEMP_CONF0, 1, &regData);
}


/******************************************************************
* Description:	read temperature parameters;
*
* Parameters: 	void	
*  		
* return:	temperature data(deg);
* 
******************************************************************/
float SH3201_GetTempData(void)
{
    unsigned char regData[2] = {0};	
    unsigned short int tempref[2] = {0};
	
	// read temperature data, unsigned 12bits;   SH3201_TEMP_CONF2..SH3201_TEMP_CONF1
	SH3201_read(SH3201_ADDRESS, SH3201_TEMP_CONF1, 2, &regData[0]);
	tempref[0] = ((unsigned short int)(regData[1] & 0x0F) << 8) | regData[0];
	
	SH3201_read(SH3201_ADDRESS, SH3201_TEMP_ZL, 2, &regData[0]);
	tempref[1] = ((unsigned short int)(regData[1] & 0x0F) << 8) | regData[0];	
	
	return ( (((float)(tempref[1] - tempref[0]))/16.0f) + 25.0f );
}


/******************************************************************
* Description:	enable or disable INT, mapping interrupt to INT pin or INT1 pin
*
* Parameters:   intType                 intEnable               intPinSel
*               SH3201_INT_LOWG         SH3201_INT_ENABLE       SH3201_INT_MAP_INT
*               SH3201_INT_HIGHG        SH3201_INT_DISABLE      SH3201_INT_MAP_INT1
*               SH3201_INT_INACT        		  
*               SH3201_INT_ACT
*               SH3201_INT_DOUBLE_TAP
*               SH3201_INT_SINGLE_TAP
*               SH3201_INT_FLAT
*               SH3201_INT_ORIENTATION
*               SH3201_INT_FIFO_WATERMARK
*               SH3201_INT_GYRO_READY
*               SH3201_INT_ACC_READY
*               SH3201_INT_FREE_FALL
*               SH3201_INT_UP_DOWN_Z
* return:	void
* 
******************************************************************/
void SH3201_INT_Enable(	unsigned short int intType, 
                        unsigned char intEnable, 
                        unsigned char intPinSel)
{
  unsigned char regData[2] = {0};	
  unsigned short int u16IntVal = 0; 
	
	// Z axis change between UP to DOWN
	if((intType & 0x0040) == SH3201_INT_UP_DOWN_Z)
	{
		//SH3201_read(SH3201_ADDRESS, SH3201_ORIEN_INTCONF0, 1, &regData[0]);
		regData[0] = 0x00;
		regData[0] = (intEnable == SH3201_INT_ENABLE) \
                     ? (regData[0] & 0xBF) : (regData[0] | 0x40);
		SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INTCONF0, 1, &regData[0]);
	}	
	
	if(intType & 0xFF0F)
	{	
		// enable or disable INT
		//SH3201_read(SH3201_ADDRESS, SH3201_INT_ENABLE0, 2, &regData[0]);
        //SH3201_read(SH3201_ADDRESS, SH3201_INT_ENABLE0, 1, &regData[0]);  
        //SH3201_read(SH3201_ADDRESS, SH3201_INT_ENABLE1, 1, &regData[1]);  
		regData[0] = 0x00;
		regData[1] = 0x00;
    
		u16IntVal = ((unsigned short int)regData[0] << 8) | regData[1]; 
		
		u16IntVal = (intEnable == SH3201_INT_ENABLE) \
                    ? (u16IntVal | intType) : (u16IntVal & ~intType);		
                
		regData[0] = (unsigned char)(u16IntVal >> 8);
		regData[1] = (unsigned char)(u16IntVal);		
		//SH3201_write(SH3201_ADDRESS, SH3201_INT_ENABLE0, 2, &regData[0]);
        SH3201_write(SH3201_ADDRESS, SH3201_INT_ENABLE0, 1, &regData[0]);
        SH3201_write(SH3201_ADDRESS, SH3201_INT_ENABLE1, 1, &regData[1]);
                              							
		// mapping interrupt to INT pin or INT1 pin
		//SH3201_read(SH3201_ADDRESS, SH3201_INT_PIN_MAP0, 2, &regData[0]);
        //SH3201_read(SH3201_ADDRESS, SH3201_INT_PIN_MAP0, 1, &regData[0]);
        //SH3201_read(SH3201_ADDRESS, SH3201_INT_PIN_MAP1, 1, &regData[1]);
		regData[0] = 0x00;
		regData[1] = 0x00;
		u16IntVal = ((unsigned short int)regData[0] << 8) | regData[1];
    
		u16IntVal = u16IntVal & 0xFF1F;
        u16IntVal = (intPinSel == SH3201_INT_MAP_INT1) \
                    ? (u16IntVal | intType) : (u16IntVal & ~intType);	
		
		regData[0] = (unsigned char)(u16IntVal >> 8);
		regData[1] = (unsigned char)(u16IntVal);
		//SH3201_write(SH3201_ADDRESS, SH3201_INT_PIN_MAP0, 2, &regData[0]); 
        SH3201_write(SH3201_ADDRESS, SH3201_INT_PIN_MAP0, 1, &regData[0]);
        SH3201_write(SH3201_ADDRESS, SH3201_INT_PIN_MAP1, 1, &regData[1]);
	}	
}


/******************************************************************
* Description:	1.config INT function
*               2.intCount is valid only when intLatch is equal to SH3201_INT_NO_LATCH;
*
* Parameters:   int0Level					intLatch             	  int1Level                 intClear					
*               SH3201_INT0_LEVEL_HIGH		SH3201_INT_NO_LATCH		  SH3201_INT1_LEVEL_HIGH    SH3201_INT_CLEAR_ANY
*               SH3201_INT0_LEVEL_LOW	  	SH3201_INT_LATCH		  SH3201_INT1_LEVEL_LOW     SH3201_INT_CLEAR_STATUS
*
*
*               int1Mode					int0Mode				intTime
*               SH3201_INT1_NORMAL			SH3201_INT0_NORMAL		Unit: 2mS
*               SH3201_INT1_OD		  		SH3201_INT0_OD
*
* return:	void
* 
******************************************************************/
void SH3201_INT_Config(	unsigned char int0Level,
                        unsigned char intLatch,
                        unsigned char int1Level,  
                        unsigned char intClear,
                        unsigned char int1Mode,
                        unsigned char int0Mode,
                        unsigned char intTime)
{
	unsigned char regData = 0;	 
	
	//SH3201_read(SH3201_ADDRESS, SH3201_INT_CONF, 1, &regData);
	
	// INT1 and INT0 output
	regData |= 0x05; 
	
	regData = (int0Level == SH3201_INT0_LEVEL_LOW) \
              ? (regData | SH3201_INT0_LEVEL_LOW) : (regData & SH3201_INT0_LEVEL_HIGH);
	
	regData = (intLatch == SH3201_INT_NO_LATCH) \
              ? (regData | SH3201_INT_NO_LATCH) : (regData & SH3201_INT_LATCH);
  
	regData = (int1Level == SH3201_INT1_LEVEL_LOW) \
              ? (regData | SH3201_INT1_LEVEL_LOW) : (regData & SH3201_INT1_LEVEL_HIGH);
	
	regData = (intClear == SH3201_INT_CLEAR_ANY) \
              ? (regData | SH3201_INT_CLEAR_ANY) : (regData & SH3201_INT_CLEAR_STATUS);
	
	regData = (int1Mode == SH3201_INT1_NORMAL) \
              ? (regData | SH3201_INT1_NORMAL) : (regData & SH3201_INT1_OD);	

	regData = (int0Mode == SH3201_INT0_NORMAL) \
              ? (regData | SH3201_INT0_NORMAL) : (regData & SH3201_INT0_OD);			
	
	SH3201_write(SH3201_ADDRESS, SH3201_INT_CONF, 1, &regData);	 
	
	if(intLatch == SH3201_INT_NO_LATCH)
	{
		regData = intTime;
		SH3201_write(SH3201_ADDRESS, SH3201_INT_LIMIT, 1, &regData);	
	} 
}		


/******************************************************************
* Description:	orientation config;
*
* Parameters: 	orientBlockMode				orientMode			    	orientTheta
*               SH3201_ORIENT_BLOCK_MODE0	SH3201_ORIENT_SYMM			(1 Byte)
*               SH3201_ORIENT_BLOCK_MODE1	SH3201_ORIENT_HIGH_ASYMM
*               SH3201_ORIENT_BLOCK_MODE2	SH3201_ORIENT_LOW_ASYMM
*               SH3201_ORIENT_BLOCK_MODE3
*
*               orientG1point5				orientSlope					orientHyst
*               (2 Bytes)					(2 Bytes)					(2 Bytes)
*  		
* return:	void
* 
******************************************************************/
void SH3201_INT_Orient_Config(	unsigned char 	orientBlockMode,
                                unsigned char 	orientMode,
                                unsigned char	  orientTheta,
                                unsigned short	orientG1point5,
                                unsigned short 	orientSlope,
                                unsigned short 	orientHyst)
{
	unsigned char regData[2] = {0};	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ORIEN_INTCONF0, 1, &regData[0]);
	regData[0] = 0x00;
	regData[0] |= (regData[0] & 0xC0) | (orientTheta & 0x3F); 
	SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INTCONF0, 1, &regData[0]);

	//SH3201_read(SH3201_ADDRESS, SH3201_ORIEN_INTCONF1, 1, &regData[0]);
	regData[0] = 0x00;
	regData[0] |= (regData[0] & 0xF0) | (orientBlockMode & 0x0C) | (orientMode & 0x03);
	SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INTCONF1, 1, &regData[0]);
	
	regData[0] = (unsigned char)orientG1point5;
	regData[1] = (unsigned char)(orientG1point5 >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INT_1G5_LOW, 2, &regData[0]);
	
	regData[0] = (unsigned char)orientSlope;
	regData[1] = (unsigned char)(orientSlope >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INT_SLOPE_LOW, 2, &regData[0]);	
	
	regData[0] = (unsigned char)orientHyst;
	regData[1] = (unsigned char)(orientHyst >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INT_HYST_LOW, 2, &regData[0]);	 
}


/******************************************************************
* Description:	flat INT time threshold, unit: mS;
*
* Parameters: 	flatTimeTH					flatTanHeta2
*               SH3201_FLAT_TIME_200MS		0 ~ 63
*               SH3201_FLAT_TIME_400MS
*               SH3201_FLAT_TIME_800MS
*               SH3201_FLAT_TIME_000MS
*  		
* return:	void
* 
******************************************************************/
void SH3201_INT_Flat_Config(unsigned char flatTimeTH, unsigned char flatTanHeta2)
{
	unsigned char regData = 0;	
  
    //SH3201_read(SH3201_ADDRESS, SH3201_ORIEN_INTCONF1, 1, &regData);	
	regData = 0x00;
    regData = (regData & 0xF3) | SH3201_ORIENT_BLOCK_MODE1;
    SH3201_write(SH3201_ADDRESS, SH3201_ORIEN_INTCONF1, 1, &regData);	
  
    //SH3201_read(SH3201_ADDRESS, SH3201_FLAT_INT_CONF, 1, &regData);	
	regData = 0x40;
	regData =  (flatTimeTH & 0xC0) | (flatTanHeta2 & 0x3F);
	SH3201_write(SH3201_ADDRESS, SH3201_FLAT_INT_CONF, 1, &regData);		
}	


/******************************************************************
* Description:	activity INT config;
*				actIntThres: 1 LSB is 0.24mg@+/-2G, 0.48mg@+/-4G, 0.97mg@+/-8G, 1.95mg@+/-16G
*
* Parameters: 	actEnDisIntX			actEnDisIntY			actEnDisIntZ
*               SH3201_ACT_X_INT_EN		SH3201_ACT_Y_INT_EN		SH3201_ACT_Z_INT_EN
*               SH3201_ACT_X_INT_DIS	SH3201_ACT_Y_INT_DIS	SH3201_ACT_Z_INT_DIS
*								
*								
*               actIntMode				actTimeNum		actIntThres		actLinkStatus				actIntClear						
*               SH3201_ACT_AC_MODE		(1 Byte)		(2 Byte2)		SH3201_LINK_PRE_STA			SH3201_ACT_INACT_CLR_STATUS1
*               SH3201_ACT_DC_MODE										SH3201_LINK_PRE_STA_NO		SH3201_ACT_INACT_CLR_STATUS3
*
* return:	void
* 
******************************************************************/
void SH3201_INT_Act_Config(	unsigned char actEnDisIntX,
                            unsigned char actEnDisIntY,
                            unsigned char actEnDisIntZ,
                            unsigned char actIntMode,
                            unsigned char actTimeNum,
                            unsigned short actIntThres,
                            unsigned char actLinkStatus,
                            unsigned char actIntClear)
{
	unsigned char regData[2] = {0};	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ACT_INACT_INT_CONF, 1, &regData[0]);	
	regData[0] = 0x00;
	regData[0] |= (regData[0] & 0x0F) | actEnDisIntX | actEnDisIntY | actEnDisIntZ | actIntMode;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INACT_INT_CONF, 1, &regData[0]);	
	
	regData[0] = (unsigned char)actIntThres;
	regData[1] = (unsigned char)(actIntThres >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INT_THRESHOLDL, 2, &regData[0]);	

	regData[0] = actTimeNum;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INT_TIME, 1, &regData[0]);
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData[0]);
	regData[0] = 0x00;	
	regData[0] |= (regData[0] & 0x7E) | actLinkStatus | actIntClear;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData[0]);	   
}	


/******************************************************************
* Description:	inactivity INT config;
*
* Parameters: 	inactEnDisIntX              inactEnDisIntY				inactEnDisIntZ
*               SH3201_INACT_X_INT_EN		SH3201_INACT_Y_INT_EN		SH3201_INACT_Z_INT_EN
*               SH3201_INACT_X_INT_DIS	    SH3201_INACT_Y_INT_DIS	    SH3201_INACT_Z_INT_DIS
*								
*								
*               inactIntMode				inactLinkStatus				inactTimeMs		
*               SH3201_INACT_AC_MODE		SH3201_LINK_PRE_STA			Unit: mS									
*               SH3201_INACT_DC_MODE		SH3201_LINK_PRE_STA_NO
*
*
*               inactIntThres				inactG1						inactIntClear
*               (3 Bytes)					(2 Bytes)					SH3201_ACT_INACT_CLR_STATUS1
*																		SH3201_ACT_INACT_CLR_STATUS3
* return:	void
* 
******************************************************************/
void SH3201_INT_Inact_Config(	unsigned char inactEnDisIntX,
                                unsigned char inactEnDisIntY,
                                unsigned char inactEnDisIntZ,
                                unsigned char inactIntMode,
                                unsigned char inactLinkStatus,	
                                unsigned char inactTimeMs,
                                unsigned int inactIntThres,
                                unsigned short int inactG1,
                                unsigned char inactIntClear)
{
	unsigned char regData = 0;	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ACT_INACT_INT_CONF, 1, &regData);
	regData = 0x00;		
	regData |= (regData & 0xF0) | inactEnDisIntX | inactEnDisIntY | inactEnDisIntZ | inactIntMode;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INACT_INT_CONF, 1, &regData);	
	
	regData = (unsigned char)inactIntThres;
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_THRESHOLDL, 1, &regData);
	regData = (unsigned char)(inactIntThres >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_THRESHOLDM, 1, &regData);
	regData = (unsigned char)(inactIntThres >> 16);
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_THRESHOLDH, 1, &regData);	
		
	regData = inactTimeMs;
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_TIME, 1, &regData);	
	
	regData = (unsigned char)inactG1;
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_1G_REFL, 1, &regData);
	regData = (unsigned char)(inactG1 >> 8);
	SH3201_write(SH3201_ADDRESS, SH3201_INACT_INT_1G_REFH, 1, &regData);	
	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData);	
	regData = 0x00;
	regData |= (regData & 0x7E) | inactLinkStatus | inactIntClear;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData);		
}	


/******************************************************************
* Description:	1.tap INT config;
*               2.tapWaitTimeWindowMs is more than tapWaitTimeMs;
*
* Parameters: 	tapEnDisIntX				tapEnDisIntY				tapEnDisIntZ
*               SH3201_TAP_X_INT_EN			SH3201_TAP_Y_INT_EN			SH3201_TAP_Z_INT_EN
*               SH3201_TAP_X_INT_DIS		SH3201_TAP_Y_INT_DIS		SH3201_TAP_Z_INT_DIS
*								
*								
*               tapIntThres		tapTimeMs	    tapWaitTimeMs	    tapWaitTimeWindowMs
*               (1 Byte)		Unit: mS		Unit: mS			Unit: 2mS
*								
* return:	void
* 
******************************************************************/
void SH3201_INT_Tap_Config(	unsigned char tapEnDisIntX,
                            unsigned char tapEnDisIntY,
                            unsigned char tapEnDisIntZ,
                            unsigned char tapIntThres,
                            unsigned char tapTimeMs,
                            unsigned char tapWaitTimeMs,
                            unsigned char tapWaitTimeWindowMs)
{
	unsigned char regData = 0;		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData);
	regData = 0x00;	
	regData |= (regData & 0xF1) | tapEnDisIntX | tapEnDisIntY | tapEnDisIntZ;
	SH3201_write(SH3201_ADDRESS, SH3201_ACT_INACT_INT_LINK, 1, &regData);																			
																			
	regData = (unsigned char)tapIntThres; 
	SH3201_write(SH3201_ADDRESS, SH3201_TAP_INT_THRESHOLDL, 1, &regData);
	regData = (unsigned char)(tapIntThres >> 8); 
	SH3201_write(SH3201_ADDRESS, SH3201_TAP_INT_THRESHOLDH, 1, &regData);
	
	regData = tapTimeMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_TAP_INT_DURATION, 1, &regData);	
	
	regData = tapWaitTimeMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_TAP_INT_LATENCY, 1, &regData);	

	regData = tapWaitTimeWindowMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_DTAP_INT_WINDOW, 1, &regData);	  
}


/******************************************************************
* Description:	1.highG INT config;
*               2.highGThres: x=0.5mG@2G, x=1mG@4G, x=2mG@8G, x=4mG@16G
*
* Parameters: 	highGEnDisIntX              highGEnDisIntY              highGEnDisIntZ				
*               SH3201_HIGHG_X_INT_EN		SH3201_HIGHG_Y_INT_EN		SH3201_HIGHG_Z_INT_EN		
*               SH3201_HIGHG_X_INT_DIS		SH3201_HIGHG_Y_INT_DIS		SH3201_HIGHG_Z_INT_DIS
*			
*
*               highGEnDisIntAll			highGThres					highGTimsMs
*               SH3201_HIGHG_ALL_INT_EN		Unit: x mG(*8)			    Unit: 2mS
*               SH3201_HIGHG_ALL_INT_DIS
*
* return:	void
* 
******************************************************************/
void SH3201_INT_HighG_Config(	unsigned char highGEnDisIntX,
                                unsigned char highGEnDisIntY,
                                unsigned char highGEnDisIntZ,
                                unsigned char highGEnDisIntAll,
                                unsigned short highGThres,
                                unsigned char highGTimsMs)
{
	unsigned char regData[2] = {0};		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_HIGHLOW_G_INT_CONF, 1, &regData[0]);	
	regData[0] = 0x00;
	regData[0] |= (regData[0] & 0x0F) | highGEnDisIntX | highGEnDisIntY | highGEnDisIntZ | highGEnDisIntAll;
	SH3201_write(SH3201_ADDRESS, SH3201_HIGHLOW_G_INT_CONF, 1, &regData[0]);		
				
	//SH3201_read(SH3201_ADDRESS, SH3201_HIGHG_INT_THRESHOLDL, 2, &regData[0]);		
	regData[0] = (unsigned char)highGThres; 
	regData[1] = (regData[1] & 0xE0) | (unsigned char)(highGThres >> 8); 
	SH3201_write(SH3201_ADDRESS, SH3201_HIGHG_INT_THRESHOLDL, 2, &regData[0]);	
			
	regData[0] = highGTimsMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_HIGHG_INT_TIME, 1, &regData[0]);	 
}


/******************************************************************
* Description:	1.lowG INT config;
*               2.lowGThres: x=0.5mG@2G or x=1mG@4G or x=2mG@8G or x=4mG@16G
*
* Parameters: 	lowGEnDisIntAll	            lowGThres		lowGTimsMs
*               SH3201_LOWG_ALL_INT_EN		Unit: x mG		Unit: 2mS
*               SH3201_LOWG_ALL_INT_DIS
*
* return:	void
* 
******************************************************************/
void SH3201_INT_LowG_Config(	unsigned char lowGEnDisIntAll,
                                unsigned short lowGThres,
                                unsigned char lowGTimsMs)
{
	unsigned char regData[2] = {0};		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_HIGHLOW_G_INT_CONF, 1, &regData[0]);
	regData[0] = 0x00;	
	regData[0] |= (regData[0] & 0xFE) | lowGEnDisIntAll;
	SH3201_write(SH3201_ADDRESS, SH3201_HIGHLOW_G_INT_CONF, 1, &regData[0]);	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_LOWG_INT_THRESHOLDL, 2, &regData[0]);	
	regData[0] = (unsigned char)lowGThres; 
	regData[1] = (unsigned char)(lowGThres >> 8) & 0x1F;
	SH3201_write(SH3201_ADDRESS, SH3201_LOWG_INT_THRESHOLDL, 2, &regData[0]);	
	
	regData[0] = lowGTimsMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_LOWG_INT_TIME, 1, &regData[0]);		
}


/******************************************************************
* Description:	1.freeFall INT config;
*               2.freeFallThres: x=0.5mG@2G or x=1mG@4G or x=2mG@8G or x=4mG@16G
*
* Parameters: 	freeFallThres	freeFallTimsMs
*               Unit: x mG		Unit: 2mS
*
* return:	void
* 
******************************************************************/
void SH3201_INT_FreeFall_Config(	unsigned char freeFallThres,
                                    unsigned char freeFallTimsMs)
{
	unsigned char regData = 0;
	
	regData = freeFallThres; 
	SH3201_write(SH3201_ADDRESS, SH3201_FREEFALL_INT_THRES, 1, &regData);	
	
	regData = freeFallTimsMs; 
	SH3201_write(SH3201_ADDRESS, SH3201_FREEFALL_INT_TIME, 1, &regData);																						
}



/******************************************************************
* Description:	1.read INT status0
*
* Parameters: 	void
*
* return:	unsigned short
*			bit 12: Tap, Single or Double Tap Interrupt status	
*           bit 11: FIFO Watermark Interrupt status 							
*           bit 10: Gyro Ready Interrupt status
*           bit 9: Acc Data Ready Interrupt status
*           bit 8: Free-fall Interrupt status 
*           bit 7: Low-G Interrupt status
*           bit 6: High-G Interrupt status
*           bit 5: Inactivity Interrupt status
*           bit 4: Activity Interrupt status
*           bit 3: Double Tap Interrupt status 
*           bit 2: Single Tap Interrupt status
*           bit 1: Flat Interrupt status
*           bit 0: Orientation Interrupt status 
*               0: Not Active
*               1: Active
*
******************************************************************/
unsigned short SH3201_INT_Read_Status01(void)
{
	unsigned char regData[2] = {0};	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_INT_STA0, 2, &regData[0]);	
    SH3201_read(SH3201_ADDRESS, SH3201_INT_STA0, 1, &regData[0]);	
    SH3201_read(SH3201_ADDRESS, SH3201_INT_STA1, 1, &regData[1]);	
    
	return( ((unsigned short)(regData[1] & 0x1F) << 8) | regData[0] );
}


/******************************************************************
* Description:	1.read INT status2
*
* Parameters: 	void
*
* return:	unsigned char
*           bit 7: High-G Sign
*               0: Positive
*               1: Negative
*           bit 6: High-G X-axis Interrupt status
*           bit 5: High-G Y-axis Interrupt status
*           bit 4: High-G Z-axis Interrupt status
*               0: No Active
*               1: Active
*           bit 3: Reserved
*           bit 2: Reserved
*           bit 1: Reserved
*           bit 0: Low-G Interrupt status
*               0: Not Active
*               1: Active
*
******************************************************************/
unsigned char SH3201_INT_Read_Status2(void)
{
	unsigned char regData = 0;	
	
	SH3201_read(SH3201_ADDRESS, SH3201_INT_STA2, 1, &regData);	

	return( (regData & 0xF1));
}


/******************************************************************
* Description:	1.read INT status3
*
* Parameters: 	void
*
* return:	unsigned char
*           bit 7: Activity-Inactivity Sign
*               0: Positive
*               1: Negative
*           bit 6: Activity-Inactivity X-axis Interrupt Status
*           bit 5: Activity-Inactivity Y-axis Interrupt Status
*           bit 4: Activity-Inactivity Z-axis Interrupt Status
*               0: No Active
*               1: Active
*			bit 3: Sign of acceleration that trigger Single or Double Tap Interrup
*               0: Positive
*               1: Negative
*			bit 2: Whether Single or Double Tap Interrupt is triggered by X axis
*			bit 1: Whether Single or Double Tap Interrupt is triggered by Y axis
*			bit 0: Whether Single or Double Tap Interrupt is triggered by Z axis
*               0: No
*               1: Yes
******************************************************************/
unsigned char SH3201_INT_Read_Status3(void)
{
	unsigned char regData = 0;	
	
	SH3201_read(SH3201_ADDRESS, SH3201_INT_STA3, 1, &regData);	

	return(regData);
}


/******************************************************************
* Description:	1.read INT status4
*
* Parameters: 	void
*
* return:	unsigned char
*           bit 7: Reserved
*           bit 6: Reserved
*           bit 5: Reserved
*           bit 4: Reserved
*           bit 3: Reserved
*           bit 2: Orientation Interrupt Value of Z-axis
*               0: Upward
*               1: Downward
*           bit 1..0: Orientation Interrupt Value of X and Y-axis
*               00: Landscape left
*               01: Landscape right
*               10: Portrait upside down
*               11: Portrait upright
*
******************************************************************/
unsigned char SH3201_INT_Read_Status4(void)
{
	unsigned char regData = 0;	
	
	SH3201_read(SH3201_ADDRESS, SH3201_INT_STA4, 1, &regData);	

	return(regData & 0x07);
}


/******************************************************************
* Description:	reset FIFO controller;
*
* Parameters: 	fifoMode
*               SH3201_FIFO_MODE_DIS
*               SH3201_FIFO_MODE_FIFO
*               SH3201_FIFO_MODE_STREAM
*               SH3201_FIFO_MODE_TRIGGER
*
* return:	void
* 
******************************************************************/
void SH3201_FIFO_Reset(unsigned char fifoMode)
{
	unsigned char regData = 0;		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_FIFO_CONF0, 1, &regData);
	regData |= 0x80;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF0, 1, &regData);
	regData &= 0x7F;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF0, 1, &regData);
	
	regData = fifoMode & 0x03;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF0, 1, &regData);
}


/******************************************************************
* Description:	1.FIFO down sample frequency config;
*               2.fifoAccFreq:	accODR*1/2, *1/4, *1/8, *1/16, *1/32, *1/64, *1/128, *1/256
*               3.fifoGyroFreq:	gyroODR*1/2, *1/4, *1/8, *1/16, *1/32, *1/64, *1/128, *1/256
*
* Parameters: 	fifoAccDownSampleEnDis		fifoAccFreq/fifoGyroFreq	fifoGyroDownSampleEnDis
*               SH3201_FIFO_ACC_DOWNS_EN	SH3201_FIFO_FREQ_X1_2		SH3201_FIFO_GYRO_DOWNS_EN
*               SH3201_FIFO_ACC_DOWNS_DIS	SH3201_FIFO_FREQ_X1_4		SH3201_FIFO_GYRO_DOWNS_DIS
*											SH3201_FIFO_FREQ_X1_8
*											SH3201_FIFO_FREQ_X1_16
*											SH3201_FIFO_FREQ_X1_32
*											SH3201_FIFO_FREQ_X1_64
*											SH3201_FIFO_FREQ_X1_128
*											SH3201_FIFO_FREQ_X1_256
* return:	void
* 
******************************************************************/
void SH3201_FIFO_Freq_Config(	unsigned char fifoAccDownSampleEnDis,
                                unsigned char fifoAccFreq,
                                unsigned char fifoGyroDownSampleEnDis,
                                unsigned char fifoGyroFreq)
{
	unsigned char regData = 0;		
	
	regData |= fifoAccDownSampleEnDis | fifoGyroDownSampleEnDis;
	regData |= (fifoAccFreq << 4) | fifoGyroFreq;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF4, 1, &regData); 
}


/******************************************************************
* Description:	1.config data type in FIFO;
*               2.fifoWaterMarkLevel <= channelNum*2*(INT(1024/(channelNum*2))-1)
*                 eg. fifomode = SH3201_FIFO_ACC_X_EN | SH3201_FIFO_ACC_Y_EN | SH3201_FIFO_ACC_Z_EN
*                     then fifoWaterMarkLevel maximum is 1014
*
* Parameters: 	fifoMode				          fifoWaterMarkLevel
*               SH3201_FIFO_EXT_Z_EN	          <=1024
*               SH3201_FIFO_EXT_Y_EN
*               SH3201_FIFO_EXT_X_EN							
*               SH3201_FIFO_TEMPERATURE_EN							
*               SH3201_FIFO_GYRO_Z_EN							
*               SH3201_FIFO_GYRO_Y_EN						
*               SH3201_FIFO_GYRO_X_EN
*               SH3201_FIFO_ACC_Z_EN
*               SH3201_FIFO_ACC_Y_EN
*               SH3201_FIFO_ACC_X_EN
*               SH3201_FIFO_ALL_DIS
*															
* return:	void
* 
******************************************************************/
void SH3201_FIFO_Data_Config(	unsigned short fifoMode,
                                unsigned short fifoWaterMarkLevel)
{
	unsigned char regData = 0;
	
	if(fifoWaterMarkLevel > 1024)
	{
		fifoWaterMarkLevel = 1024;
	}
	
	//SH3201_read(SH3201_ADDRESS, SH3201_FIFO_CONF2, 1, &regData);
	regData = (regData & 0xC8) \
              | ((unsigned char)(fifoMode >> 8) & 0x30) \
              | (((unsigned char)(fifoWaterMarkLevel >> 8)) & 0x07);
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF2, 1, &regData);

	regData = (unsigned char)fifoMode;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF3, 1, &regData);
	
	regData = (unsigned char)fifoWaterMarkLevel;
	SH3201_write(SH3201_ADDRESS, SH3201_FIFO_CONF1, 1, &regData);            
}



/******************************************************************
* Description:	1. read Fifo status and fifo entries count
*
* Parameters: 	*fifoEntriesCount, store fifo entries count, less than or equal to 1024;
*
*															
* return:	unsigned char
*           bit 7: 0
*           bit 6: 0
*           bit 5: Whether FIFO Watermark has been reached
*           bit 4: Whether FIFO is full
*           bit 3: Whether FIFO is empty
*               0: No
*               1: Yes
*           bit 2: 0
*           bit 1: 0
*           bit 0: 0
*
*
******************************************************************/
unsigned char SH3201_FIFO_Read_Status(unsigned short int *fifoEntriesCount)
{
	unsigned char regData[2] = {0};		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_FIFO_STA0, 2, &regData[0]);
    SH3201_read(SH3201_ADDRESS, SH3201_FIFO_STA0, 1, &regData[0]);
    SH3201_read(SH3201_ADDRESS, SH3201_FIFO_STA1, 1, &regData[1]); 
    
	*fifoEntriesCount = ((unsigned short int)(regData[1] & 0x07) << 8) | regData[0];
	
	return (regData[1] & 0x38);
}


/******************************************************************
* Description:	1. read Fifo data
*
* Parameters: 	*fifoReadData		fifoDataLength
*               data				data length
*															
* return:	void
*
*
******************************************************************/
void SH3201_FIFO_Read_Data(unsigned char *fifoReadData, unsigned short int fifoDataLength)
{
    if(fifoDataLength == 0)
	{
		return;
	} 
    
	if(fifoDataLength > 1024)
	{
		fifoDataLength = 1024;
	} 
	
	//SH3201_read(SH3201_ADDRESS, SH3201_FIFO_DATA, fifoDataLength, fifoReadData);
    while(fifoDataLength--)
	{
		SH3201_read(SH3201_ADDRESS, SH3201_FIFO_DATA, 1, fifoReadData);
		fifoReadData++;
	}  
}	


/******************************************************************
* Description:	reset Mater I2C;
*
* Parameters: 	void
*															
* return:	void
* 
******************************************************************/
void SH3201_MI2C_Reset(void)
{
	unsigned char regData = 0;		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	regData |= 0x80;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	regData &= 0x7F;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);	
	
}


/******************************************************************
* Description:	1.master I2C config;
*               2.master I2C clock frequency is (1MHz/(6+3*mi2cFreq));
*               3.mi2cSlaveAddr: slave device address, 7 bits;
*
*
* Parameters: 	mi2cReadMode					mi2cODR							mi2cFreq
*               SH3201_MI2C_READ_MODE_AUTO		SH3201_MI2C_READ_ODR_200HZ		<=15
*               SH3201_MI2C_READ_MODE_MANUAL	SH3201_MI2C_READ_ODR_100HZ
*				SH3201_MI2C_READ_ODR_50HZ		
*				SH3201_MI2C_READ_ODR_25HZ																							
* return:	void
* 
******************************************************************/
void SH3201_MI2C_Bus_Config(	unsigned char mi2cReadMode,
                                unsigned char mi2cODR,
                                unsigned char mi2cFreq)
{
	unsigned char regData = 0;		
	
	if(mi2cFreq > 15)
	{
		mi2cFreq = 15;
	}
	
	//SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF1, 1, &regData);
	regData = (regData & 0xC0) | (mi2cODR & 0x30) | mi2cFreq;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF1, 1, &regData);	
	
	//SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	regData= 0x00;
	regData = (regData & 0xBF) | mi2cReadMode;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);		
}


/******************************************************************
* Description:	1.master I2C address and command config;
*               2.mi2cSlaveAddr: slave device address, 7 bits;
*
* Parameters: 	mi2cSlaveAddr		mi2cSlaveCmd		mi2cReadMode
*               (1 Byte)			(1 Byte)			SH3201_MI2C_READ_MODE_AUTO
*                                                       SH3201_MI2C_READ_MODE_MANUAL
*																						
* return:	void
* 
******************************************************************/
void SH3201_MI2C_Cmd_Config(	unsigned char mi2cSlaveAddr,
                                unsigned char mi2cSlaveCmd,
                                unsigned char mi2cReadMode)
{
	unsigned char regData = 0;	

	//SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	regData = (regData & 0xBF) | mi2cReadMode;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	
	//SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CMD0, 1, &regData);
	regData = 0x00;
	regData |= (mi2cSlaveAddr << 1);
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CMD0, 1, &regData);
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CMD1, 1, &mi2cSlaveCmd);																																							
}


/******************************************************************
* Description:	1.master I2C write data fucntion;
*               2.mi2cWriteData: write data;
*
* Parameters: 	mi2cWriteData
*																						
* return:	SH3201_TRUE or SH3201_FALSE
* 
******************************************************************/
unsigned char SH3201_MI2C_Write(unsigned char mi2cWriteData)
{
	unsigned char regData = 0;	
	unsigned char i = 0;

	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_WR, 1, &mi2cWriteData);
		
	//Master I2C enable, write-operation
	SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	regData = (regData & 0xFC) | 0x02;
	SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	
	// wait write-operation to end
	while(i++ < 20)
	{	
		SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
		if(regData & 0x30)
			break;
	}
		
	if((regData & 0x30) == SH3201_MI2C_SUCCESS)
	{
		return (SH3201_TRUE);	
	}	
	else
	{
		return (SH3201_FALSE);	
	}		
}																


/******************************************************************
* Description:	1.master I2C read data fucntion;
*               2.*mi2cReadData: read data;
*
* Parameters: 	*mi2cReadData
*																						
* return:	SH3201_TRUE or SH3201_FALSE
* 
******************************************************************/
unsigned char SH3201_MI2C_Read(unsigned char *mi2cReadData)
{
	unsigned char regData = 0;			
	unsigned char i = 0;
		
	
	SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	if((regData & 0x40) == 0)
	{	
		//Master I2C enable, read-operation
		regData |= 0x03;
		SH3201_write(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	}
	
	// wait read-operation to end
	while(i++ < 20)
	{	
		SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
		if(regData & 0x30)
			break;
	}	

	SH3201_read(SH3201_ADDRESS, SH3201_MI2C_CONF0, 1, &regData);
	if((regData & 0x30) == SH3201_MI2C_SUCCESS)
	{
		SH3201_read(SH3201_ADDRESS, SH3201_MI2C_RD, 1, &regData);
		*mi2cReadData = regData;
		
		return (SH3201_TRUE);	
	}	
	else
	{
		return (SH3201_FALSE);	
	}		
}	


/******************************************************************
* Description:	1.SPI interface config;  Default: SH3201_SPI_4_WIRE
*
* Parameters: 	spiInterfaceMode
*               SH3201_SPI_3_WIRE
*               SH3201_SPI_4_WIRE
*
* return:	void
* 
******************************************************************/
void SH3201_SPI_Config(	unsigned char spiInterfaceMode)
{
	unsigned char regData = 0;		
	
	//SH3201_read(SH3201_ADDRESS, SH3201_SPI_CONF, 1, &regData);	
	regData = (regData & 0xFE) | spiInterfaceMode;
	SH3201_write(SH3201_ADDRESS, SH3201_SPI_CONF, 1, &regData);
} 





/******************************************************************
* Description:	Switch SH3201 power mode:
*               1.SH3201_NORMAL_MODE: 1.74mA; 
* 				2.SH3201_ACC_HP_MODE
*				3.SH3201_POWERDOWN_MODE: 14uA
*
* Parameters:   powerMode
*               SH3201_NORMAL_MODE
*   			SH3201_ACC_HP_MODE
*               SH3201_POWERDOWN_MODE
*
* return:	SH3201_FALSE or SH3201_TRUE
*
******************************************************************/
unsigned char SH3201_SwitchPowerMode(unsigned char powerMode)
{
   unsigned char regData[2] = {0};
   int i = 0;
   unsigned char reg_data = 0;
   unsigned char reg_addr[SH3001_POWMODE_REG_NUM] = { 0x20, 0x6A, 0x75, 0x80, 0xCE, 0xD5, 0xD6, 0xD7, 0xDD};
   unsigned char reg_data_normal[SH3001_POWMODE_REG_NUM] = {0x29, 0x4, 0x4, 0x4, 0x98, 0x0, 0x0, 0x0, 0x00};
   unsigned char reg_data_acc_hp[SH3001_POWMODE_REG_NUM] = {0x29, 0x4, 0x4, 0x4, 0x99, 0x7A, 0xFF, 0xFC, 0x20};
   unsigned char reg_data_powerdown[SH3001_POWMODE_REG_NUM] = {0x29, 0x4, 0x4, 0x4, 0x98, 0x0, 0x0, 0x0, 0xE0};

	if(powerMode > 0x04)
	{
		return (SH3201_FALSE);
	}

	switch(powerMode)
	{
	case SH3201_NORMAL_MODE:

	     for(i = 0; i < SH3001_POWMODE_REG_NUM; i++)
	     {
	      SH3201_write(SH3201_ADDRESS, reg_addr[i], 1, &reg_data_normal[i]);
	     }
			
      	//Driver start
      	SH3201_DriveStart();

		 //ADC reset
      	SH3201_ADCReset();
	
		//CVA Reset
		SH3201_CVAReset();

		delay_ms(250);
	
		return (SH3201_TRUE);

	case SH3201_ACC_HP_MODE:

	     for(i = 0; i < SH3001_POWMODE_REG_NUM; i++)
	     {
	      SH3201_write(SH3201_ADDRESS, reg_addr[i], 1, &reg_data_acc_hp[i]); 
	     }

		 return (SH3201_TRUE);
	
		
	case SH3201_POWERDOWN_MODE:

	     for(i = 0; i < SH3001_POWMODE_REG_NUM; i++)
	     {
	      SH3201_write(SH3201_ADDRESS, reg_addr[i], 1, &reg_data_powerdown[i]); 
	     }

		 return (SH3201_TRUE);
	
	default:
		 return (SH3201_FALSE);
	}
}


/******************************************************************
* Description:	1. fast compensate offset
*
* Parameters:   sh3201Positon
*               SH3201_X_UP
*               SH3201_X_DOWN
*				SH3201_Y_UP
*               SH3201_Y_DOWN
*				SH3201_Z_UP
*               SH3201_Z_DOWN
*
* return:	SH3201_FALSE or SH3201_TRUE
*
******************************************************************/
void SH3201_FastOffsetComp(unsigned char sh3201Positon)
{
  unsigned char regData = 0;
    
  switch(sh3201Positon)
    {
  case SH3201_X_UP:
    regData = 0x90;
    break;
  case SH3201_X_DOWN:
    regData = 0x91;
    break;  
  case SH3201_Y_UP:
    regData = 0x92;
    break;
  case SH3201_Y_DOWN:
    regData = 0x93;
    break;     
  case SH3201_Z_UP:
    regData = 0x94;
    break;
  case SH3201_Z_DOWN:
    regData = 0x95;
    break; 
  default:
    regData = 0x94;
    break;
  }
  
  SH3201_write(SH3201_ADDRESS, SH3201_FAST_COMP, 1, &regData);
}  



/******************************************************************
* Description: 	1. SH3201 initialization FIFO
*               2. INT mode
*               3. Enable ACC_X, ACC_Y, ACC_Z, GYRO_X, GYRO_Y, GYRO_Z
*
* Parameters:	void
*
* return:	void
*
******************************************************************/
void SH3201_initFIFO(void)
{  
	SH3201_INT_Config(SH3201_INT0_LEVEL_HIGH,
                    SH3201_INT_LATCH, 		  
                    SH3201_INT1_LEVEL_HIGH,
                    SH3201_INT_CLEAR_STATUS,   
                    SH3201_INT1_NORMAL,
                    SH3201_INT0_NORMAL,
                    1);	 

	SH3201_FIFO_Reset(SH3201_FIFO_MODE_FIFO);
  
	SH3201_FIFO_Freq_Config(SH3201_FIFO_ACC_DOWNS_DIS,
                            SH3201_FIFO_FREQ_X1_32,
                            SH3201_FIFO_GYRO_DOWNS_DIS,
                            SH3201_FIFO_FREQ_X1_32);                                                   
                            
    SH3201_FIFO_Data_Config(SH3201_FIFO_ACC_X_EN 
                            | SH3201_FIFO_ACC_Y_EN 
                            | SH3201_FIFO_ACC_Z_EN
                            | SH3201_FIFO_GYRO_X_EN
                            | SH3201_FIFO_GYRO_Y_EN
                            | SH3201_FIFO_GYRO_Z_EN,
                            1008); 

	SH3201_INT_Enable(SH3201_INT_FIFO_WATERMARK, 			
                      SH3201_INT_ENABLE, 
                      SH3201_INT_MAP_INT);  
}


/******************************************************************
* Description: 	1. SH3201 read ACC_X, ACC_Y, ACC_Z, GYRO_X, GYRO_Y, GYRO_Z @FIFO
*               2. clear FIFO Watermark INT
*
* Parameters:	void
*
* return:	void
*
******************************************************************/
void SH3201_ReadFIFO(void)
{
	unsigned short fifoEntriesCount = 0;
	unsigned char fifoData[SH3201_FIFO_BUFFER] = {0};
	unsigned short i = 0, j = 0, dataGroup = 0;
	short accData[3], gyroData[3];
    unsigned char regData = 0;

	SH3201_FIFO_Data_Config(SH3201_FIFO_ALL_DIS, 0);

	// read FIFO entries count
	SH3201_FIFO_Read_Status(&fifoEntriesCount);
	printf("fc %d\r\n", fifoEntriesCount);

	dataGroup = (unsigned short)(fifoEntriesCount/SH3201_WATERMARK_DIV); 
	fifoEntriesCount = dataGroup * SH3201_WATERMARK_DIV;

	SH3201_FIFO_Read_Data(fifoData, fifoEntriesCount);
    
	for(i = 0; i < dataGroup; i++) 
	{				
		accData[0] = ((short)fifoData[j+1] << 8) | fifoData[j];
		accData[1] = ((short)fifoData[j+3] << 8) | fifoData[j+2];
		accData[2] = ((short)fifoData[j+5] << 8) | fifoData[j+4];	
		gyroData[0] = ((short)fifoData[j+7] << 8) | fifoData[j+6];
		gyroData[1] = ((short)fifoData[j+9] << 8) | fifoData[j+8];
		gyroData[2] = ((short)fifoData[j+11] << 8) | fifoData[j+10];	

        printf("%d %d %d %d %d %d\n", accData[0], accData[1], accData[2],gyroData[0], gyroData[1], gyroData[2]);
       
		j = j + SH3201_WATERMARK_DIV;
	}

    SH3201_FIFO_Data_Config(SH3201_FIFO_ACC_X_EN
                            | SH3201_FIFO_ACC_Y_EN
                            | SH3201_FIFO_ACC_Z_EN
                            | SH3201_FIFO_GYRO_X_EN
                            | SH3201_FIFO_GYRO_Y_EN
                            | SH3201_FIFO_GYRO_Z_EN,
                            120);

	// reset FIFO mode
	SH3201_FIFO_Reset(SH3201_FIFO_MODE_FIFO);

    // clear INT
    //SH3201_INT_Read_Status01();
}  

static void SH3201_IMU_Data_Stablize(void)
{
	unsigned char regData = 0;

	//TODO Three times maybe
	//Acc
	SH3201_read(SH3201_ADDRESS, SH3201_ACC_CONF0, 1, &regData);
	regData &= 0xFE;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF0, 1, &regData);
	regData |= 0x1;
	SH3201_write(SH3201_ADDRESS, SH3201_ACC_CONF0, 1, &regData);

	//Gyro
	SH3201_read(SH3201_ADDRESS, SH3201_GYRO_CONF0, 1, &regData);
	regData &= 0xFE;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF0, 1, &regData);
	regData |= 0x1;
	SH3201_write(SH3201_ADDRESS, SH3201_GYRO_CONF0, 1, &regData);

}

#if SH3201_ADJUST_CF
void SH3201_Adjust_Cf(void)
{
	unsigned char reg_addr[15] = { 0x8B, 0x8C, 0x8D, 0x8E, 0x97, 0x98, 0x99, 0x9A, 0xA3, 0xA4, 0xA5, 0xA6, 0xB0, 0xB1, 0xB2};
	unsigned char regDataold[15];
	unsigned char regDatanew[15];
	short OldData[6];
	short NewData[6];
	int i = 0;

	SH3201_read(SH3201_ADDRESS, reg_addr[12], 1, &regDataold[12]);
	//printf("Adjust read %x= %x\r\n", reg_addr[12], regDataold[12]);
	if(regDataold[12] != 0x06)
		return;

	for(i = 0; i < 15; i++)
	{
		SH3201_read(SH3201_ADDRESS, reg_addr[i], 1, &regDataold[i]);
		//printf("Adjust read = %x\r\n", regDataold[i]);
	}

	OldData[0] = (short)(regDataold[1] << 8 | regDataold[0]);
	OldData[1] = (short)(regDataold[3] << 8 | regDataold[2]);
	OldData[2] = (short)(regDataold[5] << 8 | regDataold[4]);
	OldData[3] = (short)(regDataold[7] << 8 | regDataold[6]);
	OldData[4] = (short)(regDataold[9] << 8 | regDataold[8]);
	OldData[5] = (short)(regDataold[11] << 8 | regDataold[10]);

	NewData[0] = OldData[0] * 1.687;
	NewData[1] = OldData[1] * 0.593 - 223;
	NewData[2] = OldData[2] * 1.687;
	NewData[3] = OldData[3] * 0.593 - 334;
	NewData[4] = OldData[4] * 1.687;
	NewData[5] = OldData[5] * 0.593 - 248;

	//printf("OldData %d %d %d %d %d %d \r\n",OldData[0], OldData[1], OldData[2], OldData[3], OldData[4], OldData[5]);
	//printf("NewData %d %d %d %d %d %d \r\n",NewData[0], NewData[1], NewData[2], NewData[3], NewData[4], NewData[5]);

	regDatanew[0] = NewData[0] & 0xFF;
	regDatanew[1] = (NewData[0] >> 8) & 0xFF;
	regDatanew[2] = NewData[1] & 0xFF;
	regDatanew[3] = (NewData[1] >> 8) & 0xFF;
	regDatanew[4] = NewData[2] & 0xFF;
	regDatanew[5] = (NewData[2] >> 8) & 0xFF;
	regDatanew[6] = NewData[3] & 0xFF;
	regDatanew[7] = (NewData[3] >> 8) & 0xFF;
	regDatanew[8] = NewData[4] & 0xFF;
	regDatanew[9] = (NewData[4] >> 8) & 0xFF;
	regDatanew[10] = NewData[5] & 0xFF;
	regDatanew[11] = (NewData[5] >> 8) & 0xFF;
	regDatanew[12] = 0x04;
	regDatanew[13] = 0x04;
	regDatanew[14] = 0x0C;

	for(i = 0; i < 15; i++)
	{
		//printf("Adjust write = %x\r\n", regDatanew[i]);
		SH3201_write(SH3201_ADDRESS, reg_addr[i], 1, &regDatanew[i]);
	}
}
#endif

/******************************************************************
* Description: 	SH3201 initialization function
*
* Parameters:	void
*																						
* return:	SH3201_TRUE or SH3201_FALSE
*
******************************************************************/
unsigned char SH3201_init(uint8_t acc_range, uint8_t gyro_range) //TODO
{
	unsigned char regData = 0;
	unsigned char i = 0;
	unsigned char accr, gyror;

#if SH3201_SPI_3WIRE_MODE  			
	SH3201_SPI_Config(SH3201_SPI_3_WIRE);
#endif	

	// SH3201 chipID = 0x63;	
	while((regData != 0x63) && (i++ < 3))
	{
		SH3201_read(SH3201_ADDRESS, SH3201_CHIP_ID, 1, &regData);	
		if((i == 3) && (regData != 0x63))
		{
			printf("SH3201 init fail 0x%x\n", regData);
			return 0;
		}	
	}

	switch(acc_range){
		case ACC_RANGE_2G:
			accr = SH3201_ACC_RANGE_2G;
			break;
		case ACC_RANGE_4G:
			accr = SH3201_ACC_RANGE_4G;
			break;
		case ACC_RANGE_8G:
			accr = SH3201_ACC_RANGE_8G;
			break;
		case ACC_RANGE_16G:
			accr = SH3201_ACC_RANGE_16G;
			break;
		default:
			accr = SH3201_ACC_RANGE_8G;
			break;
	}

	switch(gyro_range){
		case GYRO_RANGE_125:
			gyror = SH3201_GYRO_RANGE_125;
			break;
		case GYRO_RANGE_250:
			gyror = SH3201_GYRO_RANGE_250;
			break;
		case GYRO_RANGE_500:
			gyror = SH3201_GYRO_RANGE_500;
			break;
		case GYRO_RANGE_1000:
			gyror = SH3201_GYRO_RANGE_1000;
			break;
		case GYRO_RANGE_2000:
			gyror = SH3201_GYRO_RANGE_2000;
			break;
		default:
			gyror = SH3201_GYRO_RANGE_1000;
			break;
	}

	// reset internal module, don't modify it
	SH3201_ModuleReset();

	// 500Hz, 16G, cut off Freq(BW)=500*0.25Hz=125Hz, enable filter;
	SH3201_Acc_Config(SH3201_ACC_ODR_125HZ, 
                    accr, //SH3201_ACC_RANGE_16G  //TODO
                    SH3201_ACC_ODRX025,
                    SH3201_ACC_FILTER_EN,
                    SH3201_ACC_BYPASS_DIS); 
  
	// 800Hz, X\Y\Z 2000deg/s, cut off Freq(BW)=291Hz, enable filter;
	SH3201_Gyro_Config(	SH3201_GYRO_ODR_400HZ, 
                      gyror, //SH3201_GYRO_RANGE_2000  //TODO
                      SH3201_GYRO_ODRX00,
                      SH3201_GYRO_FILTER_EN,
                      SH3201_GYRO_BYPASS_DIS,
                      SH3201_GYRO_ON_INACT); 
	
	// temperature ODR is 63Hz, enable temperature measurement
	SH3201_Temp_Config(SH3201_TEMP_ODR_63HZ, SH3201_TEMP_EN);


	SH3201_IMU_Data_Stablize();

	return 1;
}



/******************************************************************
* Description: 	Read SH3201 gyroscope and accelerometer data
*
* Parameters:	accData[3]: acc X,Y,Z;  gyroData[3]: gyro X,Y,Z;
*																						
* return:	void
* 
******************************************************************/
bool SH3201_GetImuData( short accData[3], short gyroData[3] ,unsigned char isr)
{
	bool ret;	
	unsigned char regData[12]={0};	

	if(isr){	
		ret = I2C_Isr_Read_NBytes(SH3201_ADDRESS, SH3201_ACC_XL, 12, regData); 
	}else{
		ret = I2C_ReadNBytes(SH3201_ADDRESS, SH3201_ACC_XL, 12, regData); 
	}
  
	accData[0] = ((short)regData[1] << 8) | regData[0];
	accData[1] = ((short)regData[3] << 8) | regData[2];
	accData[2] = ((short)regData[5] << 8) | regData[4];		
	
	gyroData[0] = ((short)regData[7] << 8) | regData[6];
	gyroData[1] = ((short)regData[9] << 8) | regData[8];
	gyroData[2] = ((short)regData[11] << 8) | regData[10];
		
	// printf("%d %d %d %d %d %d\n", accData[0], accData[1], accData[2], gyroData[0], gyroData[1], gyroData[2]);
	return ret;
}


#define  SH5001_GYRO_ADC_S1_REGNUM 5
#define  SH5001_GYRO_ADC_S2_REGNUM 1
#define  SH5001_GYRO_ADC__DEBUG
int adc_count = 0;
int err_count = 0;
void SH3201_Get_Adc_Polarity(short gyroPolarity[3])
{
	unsigned char regAddrS1[SH5001_GYRO_ADC_S1_REGNUM] = {0xA2, 0xB2, 0xB8, 0xBB, 0xBE};
	unsigned char regInitS1[SH5001_GYRO_ADC_S1_REGNUM] = {0xFF, 0x08, 0x29, 0x18, 0x70};
	unsigned char regAddrS2[SH5001_GYRO_ADC_S2_REGNUM] = {0xBE};
	unsigned char regInitS2[SH5001_GYRO_ADC_S2_REGNUM] = {0x60};
	unsigned char regBackS1[SH5001_GYRO_ADC_S1_REGNUM] = {0};
	unsigned char gyroTemp[6] = {0};
	signed int gyroSum[3] = {0};
	signed short gyroAvg1[3] = {0}, gyroAvg2[3] = {0};
	unsigned char i = 0;
	//signed short gyroPolarity[3] = {0};

	for(i=0; i<SH5001_GYRO_ADC_S1_REGNUM; i++)
	{
		SH3201_read(SH3201_ADDRESS, regAddrS1[i], 1, &regBackS1[i]);
	}

	regInitS1[4] = regBackS1[4] | 0x10;
	regInitS2[0] = regBackS1[4] & 0xEF;
	for(i=0; i<SH5001_GYRO_ADC_S1_REGNUM; i++)
	{
		SH3201_write(SH3201_ADDRESS, regAddrS1[i], 1, &regInitS1[i]);
	}

	HAL_Delay(50);

	i = 0;
	while(i++ < 20)
	{
		SH3201_read(SH3201_ADDRESS, SH3201_GYRO_XL, 6, gyroTemp);
		gyroSum[0] += (signed short)((short)gyroTemp[1] << 8) | gyroTemp[0];
		gyroSum[1] += (signed short)((short)gyroTemp[3] << 8) | gyroTemp[2];
		gyroSum[2] += (signed short)((short)gyroTemp[5] << 8) | gyroTemp[4];
		#ifdef SH5001_GYRO_ADC__DEBUG
		printf("bf %d %d %d\r\n", (signed short)(((short)gyroTemp[1] << 8) | gyroTemp[0]), (signed short)(((short)gyroTemp[3] << 8) | gyroTemp[2]), (signed short)(((short)gyroTemp[5] << 8) | gyroTemp[4]));
		#endif
	}
	gyroAvg1[0] = gyroSum[0]/20;
	gyroAvg1[1] = gyroSum[1]/20;
	gyroAvg1[2] = gyroSum[2]/20;

	for(i=0; i<SH5001_GYRO_ADC_S2_REGNUM; i++)
	{
		SH3201_write(SH3201_ADDRESS, regAddrS2[i], 1, &regInitS2[i]);
	}

	HAL_Delay(50);

	gyroSum[0] = 0;
	gyroSum[1] = 0;
	gyroSum[2] = 0;
	i = 0;

	while(i++ < 20)
	{
		SH3201_read(SH3201_ADDRESS, SH3201_GYRO_XL, 6, gyroTemp);
		gyroSum[0] += (signed short)((short)gyroTemp[1] << 8) | gyroTemp[0];
		gyroSum[1] += (signed short)((short)gyroTemp[3] << 8) | gyroTemp[2];
		gyroSum[2] += (signed short)((short)gyroTemp[5] << 8) | gyroTemp[4];

		#ifdef SH5001_GYRO_ADC__DEBUG
		printf("af %d %d %d\r\n", (signed short)(((short)gyroTemp[1] << 8) | gyroTemp[0]), (signed short)(((short)gyroTemp[3] << 8) | gyroTemp[2]), (signed short)(((short)gyroTemp[5] << 8) | gyroTemp[4]));
		#endif
	}

	gyroAvg2[0] = gyroSum[0]/20;
	gyroAvg2[1] = gyroSum[1]/20;
	gyroAvg2[2] = gyroSum[2]/20;

	for(i=0; i<SH5001_GYRO_ADC_S1_REGNUM; i++)
	{
		SH3201_write(SH3201_ADDRESS, regAddrS1[i], 1, &regBackS1[i]);
	}

	gyroPolarity[0] = gyroAvg2[0] - gyroAvg1[0];
	gyroPolarity[1] = gyroAvg2[1] - gyroAvg1[1];
	gyroPolarity[2] = gyroAvg2[2] - gyroAvg1[2];
	adc_count++;
	#ifdef SH5001_GYRO_ADC__DEBUG
	printf("gyroPolarity %d %d %d %d %d\r\n", adc_count, err_count, gyroPolarity[0],gyroPolarity[1] ,gyroPolarity[2]);
	#endif

	if(gyroPolarity[2] < 0)
	{
		err_count++;
	}

}

void sh3201_adc_phase(void)
{
	unsigned char addr = 0xBE;
	unsigned char regData;

	SH3201_read(SH3201_ADDRESS, addr, 1, &regData);
	//printf("0xBE = 0x%x\r\n", regData);

	if((regData & 0x10) == 0x10)
		regData &= 0xEF;
	else
		regData |= 0x10;
	SH3201_write(SH3201_ADDRESS, addr, 1, &regData);
	//printf("0xBE = 0x%x\r\n", regData);

}

#define SENODIA_GYRO_CHECK 0
#if SENODIA_GYRO_CHECK
#define SH3201_MAX_NUM 50
float dym_x = 0.0;
float dym_y = 0.0;
float dym_z = 0.0;
float avg_x = 0.0;
float avg_y = 0.0;
float avg_z = 0.0;
float var_x = 0.0;
float var_y = 0.0;
float var_z = 0.0;
float mean_x = 0.0;
float mean_y = 0.0;
float mean_z = 0.0;
int num = 0;
int gyro_check = 0;
int sh3201_var_dym(float x,float y,float z)
{

	mean_x = mean_x + (x * x);
	mean_y = mean_y + (y * y);
	mean_z = mean_z + (z * z);
	dym_x = dym_x + x;
	dym_y = dym_y + y;
	dym_z = dym_z + z;
	num ++;
	//printf("sh3201_var_dym = %f %f %f %f %f %f %f %f %f %f\r\n", x, y, z, mean_x, mean_y, mean_z, dym_x, dym_y, dym_z, num);

	if(num == SH3201_MAX_NUM)
	{
		avg_x = (float)dym_x / SH3201_MAX_NUM;
		avg_y = (float)dym_y / SH3201_MAX_NUM;
		avg_z = (float)dym_z / SH3201_MAX_NUM;

		mean_x = (float)mean_x / SH3201_MAX_NUM;
		mean_y = (float)mean_y / SH3201_MAX_NUM;
		mean_z = (float)mean_z / SH3201_MAX_NUM;

		var_x = (float)(mean_x  - (avg_x * avg_x));
		var_y = (float)(mean_y  - (avg_y * avg_y));
		var_z = (float)(mean_z  - (avg_z * avg_z));
		//printf("var_x = %f %f %f\r\n", var_x, var_y, var_z);

		if((var_x == 0) || (var_y == 0) || (var_z == 0))
		{
			gyro_check = 1;
		}
		else
		{
			gyro_check = 0;
		}

		dym_x = 0.0;
		dym_y = 0.0;
		dym_z = 0.0;
		avg_x = 0.0;
		avg_y = 0.0;
		avg_z = 0.0;
		var_x = 0.0;
		var_y = 0.0;
		var_z = 0.0;
		mean_x = 0.0;
		mean_y = 0.0;
		mean_z = 0.0;
		num = 0;
	}

	return gyro_check;
}

int sh3201_gyro_ac_check(float x, float y, float z)
{
	unsigned char regData;
	int ret = -1;

	ret = sh3201_var_dym(x, y, z);
	if(ret == 1)
	{
		SH3201_read(SH3201_ADDRESS, 0x14, 1, &regData);
		if((regData & 0x1F) == 0x1F)
		{
			regData = 0x99;
			SH3201_write(SH3201_ADDRESS, 0xCE, 1, &regData);
			ret = 2;
		}
	}

	return ret;
}
#endif

int sh3201_get_version(void)
{
	return MCU_VERSION_SH3201;
}
#endif
