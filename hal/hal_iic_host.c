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
#include "hw_config.h"
#include "hw_board.h"
#ifdef HW_IIC_MAP

#include  "api/api_iic_host.h"
#include  "api/api_gpio.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	需要使用硬件iic扫描外设时才需要适配该接口
*******************************************************************/
bool hal_iic_scan_addr(uint8_t id,uint8_t dev_addr)
{
	uint8_t ret = false;
	
	// start 
	// tx byte 
	// stop 
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(dev_addr);
	return ret;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool hal_iic_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(dev_addr);
	UNUSED_PARAMETER(addr);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return false;
}
bool hal_iic_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(dev_addr);
	UNUSED_PARAMETER(addr);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return false;
}
bool hal_iic_isr_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(dev_addr);
	UNUSED_PARAMETER(addr);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return false;
}
bool hal_iic_isr_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(dev_addr);
	UNUSED_PARAMETER(addr);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return false;
}
bool hal_iic_init(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return false;
}
bool hal_iic_deinit(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return false;
}

#endif





