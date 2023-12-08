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
#include  "app/drv2605.h"
#include  "api/api_iic_host.h"
#include  "api/api_tick.h"
#include  "api/api_log.h"
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
**	static Function
******************************************************************************************************/
/**
 * \brief
 * \return 0:读取失败 1:读取成功
 */
bool I2C_Read_NBytes(uint8_t devAddr, 
                    uint8_t regAddr, 
                    uint8_t readLen, 
                    uint8_t *readBuf)
{
	//your I2C interface code:
	#ifdef HW_IIC_MAP
  	return api_iic_host_read(IIC_DRV2605_ID,devAddr,regAddr,readBuf,readLen);
  	#else
	return true;
	#endif
}

/**
 * \brief
 * \return 0:读取失败 1:读取成功
 */
bool I2C_Write_NBytes(uint8_t devAddr, 
                    uint8_t regAddr, 
                    uint8_t writeLen, 
                    uint8_t *writeBuf)
{
	//your I2C interface code:
	#ifdef HW_IIC_MAP
  	return api_iic_host_write(IIC_DRV2605_ID,devAddr,(uint16_t)regAddr,(uint8_t const *)writeBuf,(uint16_t)writeLen);
	#else
	return true;
	#endif
}																


/**
 * \brief
 * \return 0:读取失败 1:读取成功
 */
bool I2C_Isr_Read_NBytes(	uint8_t devAddr, 
                                uint8_t regAddr, 
                                uint8_t readLen, 
                                uint8_t *readBuf)
{
	//your I2C interface code:
	#ifdef HW_IIC_MAP
  	return api_iic_host_isr_read(IIC_DRV2605_ID,(uint16_t)devAddr,regAddr,readBuf,(uint16_t)readLen);
	#else
	return true;
	#endif
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

bool drv2605_AutoCal(void)		//自动校准
{
    uint8_t temp = 0x00;
    uint8_t regData;
	
	regData = 0x50;
	I2C_Write_NBytes(DRV2605_ADDRESS, RATED_VOLTAGE_Reg, 1, &regData);		//Set rate voltage	
	regData = 0x89;
	I2C_Write_NBytes(DRV2605_ADDRESS, OD_CLAMP_Reg, 1, &regData);			//Set overdrive voltage
	regData = 0xB6;
	I2C_Write_NBytes(DRV2605_ADDRESS, FB_CON_Reg, 1, &regData);				//Setup feedback control and control registers
	regData = 0x93;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL1_Reg, 1, &regData);
	regData = 0xF5;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL2_Reg, 1, &regData);
	regData = 0x80;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL3_Reg, 1, &regData);	
	regData = 0x07;
	I2C_Write_NBytes(DRV2605_ADDRESS, MODE_Reg, 1, &regData);				//Put the DRV2605 device in auto calibration mode
	regData = 0x20;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL4_Reg, 1, &regData);
	regData = 0x01;
	I2C_Write_NBytes(DRV2605_ADDRESS, GO_Reg, 1, &regData);					//Begin auto calibration

	while ((temp & 0x01) != 0x01)		//Poll the GO register until least significant bit is set
	{
		I2C_Read_NBytes(DRV2605_ADDRESS, GO_Reg, 1, &temp);		//判断校准是否完成
	}

	return 1;
}

bool drv2605_Play_Waveform(uint8_t ucEffect)
{
	uint8_t regData = 0;
    
	regData = 0x00;
	I2C_Write_NBytes(DRV2605_ADDRESS, MODE_Reg, 1, &regData);			//Put the DRV2605 device in active mode
	regData = ucEffect;
	I2C_Write_NBytes(DRV2605_ADDRESS, WAV_SEQ1_Reg, 1, &regData);
	regData = 0x00;
	I2C_Write_NBytes(DRV2605_ADDRESS, WAV_SEQ2_Reg, 1, &regData);
	regData = 0x01;
	I2C_Write_NBytes(DRV2605_ADDRESS, GO_Reg, 1, &regData);

	return 1;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool drv2605_init(void)
{
	uint8_t regData = 0;
	uint8_t i = 0;

    while((regData != 0xE0) && (i++ < 3))
	{
		I2C_Read_NBytes(DRV2605_ADDRESS, STATUS_Reg, 1, &regData);	
		logd("id = 0x%x\n", regData);
		if((i == 3) && ((regData&0xE0) != 0xE0))
		{
			return false;
		}	
	}
	regData = 0x40;
	I2C_Write_NBytes(DRV2605_ADDRESS, MODE_Reg, 1, &regData);			//Enter standby mode
	regData = 0x50;
	I2C_Write_NBytes(DRV2605_ADDRESS, RATED_VOLTAGE_Reg, 1, &regData);	//Set rate voltage
	regData = 0x89;
	I2C_Write_NBytes(DRV2605_ADDRESS, OD_CLAMP_Reg, 1, &regData);		//Set overdrive voltage
	regData = 0xB6;
	I2C_Write_NBytes(DRV2605_ADDRESS, FB_CON_Reg, 1, &regData);			//Setup feedback control and control registers
	regData = 0x13;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL1_Reg, 1, &regData);
	regData = 0xF5;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL2_Reg, 1, &regData);
	regData = 0x80;
	I2C_Write_NBytes(DRV2605_ADDRESS, CONTRL3_Reg, 1, &regData);	
	regData = 0x06;
	I2C_Write_NBytes(DRV2605_ADDRESS, LIB_SEL_Reg, 1, &regData);		//Select the LRA Library
	regData = 0x00;
	I2C_Write_NBytes(DRV2605_ADDRESS, MODE_Reg, 1, &regData);			//Put the DRV2605 device in active mode

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool drv2605_deinit(void)
{
	
	return true;
}




