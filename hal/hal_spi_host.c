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
#ifdef HW_SPI_HOST_MAP

#include  "api/api_spi_host.h"
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
** Description:		
*******************************************************************/
bool hal_spi_host_write(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
	return false;
}
bool hal_spi_host_read(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
	return false;
}
bool hal_spi_host_isr_write(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
	return false;
}
bool hal_spi_host_isr_read(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
	return false;
}
bool hal_spi_host_init(uint8_t id)
{
	return false;
}
bool hal_spi_host_deinit(uint8_t id)
{
	return false;
}

#endif




