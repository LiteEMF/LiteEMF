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
#ifdef HW_UART_MAP

#include  "api/api_uart.h"
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
bool hal_uart_set_baud(uint8_t id, uint32_t baud)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(baud);
	return true;
}
bool hal_uart_tx(uint8_t id,void * buf,uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return true;
}
bool hal_uart_init(uint8_t id,uint32_t baudrate)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(baudrate);
	return false;
}
bool hal_uart_deinit(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return false;
}


#endif





