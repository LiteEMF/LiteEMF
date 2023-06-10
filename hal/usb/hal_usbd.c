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

#include  "api/usb/usb_typedef.h"

#include "api/api_log.h"

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

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/

error_t hal_usbd_endp_dma_init(uint8_t id)
{
	return 0;
}
error_t hal_usbd_endp_open(uint8_t id, usb_endp_t *pendp)
{
	return 0;
}
error_t hal_usbd_endp_close(uint8_t id, uint8_t ep)
{
	return 0;
}
error_t hal_usbd_endp_ack(uint8_t id, uint8_t ep, uint16_t len)
{
	return 0;
}
error_t hal_usbd_endp_nak(uint8_t id, uint8_t ep)
{
	return 0;
}
error_t hal_usbd_clear_endp_stall(uint8_t id, uint8_t ep)
{
	return 0;
}
error_t hal_usbd_endp_stall(uint8_t id, uint8_t ep)
{
	return 0;
}

uint8_t *hal_usbd_get_endp_buffer(uint8_t id, uint8_t ep)
{
	return NULL;
}
/*******************************************************************
** Parameters:
** Returns:
** Description: 注意: 端点0 发送需要处理usbd_req_t
*******************************************************************/
error_t hal_usbd_in(uint8_t id, uint8_t ep, uint8_t* buf,uint16_t len)
{
	return 0;
}
error_t hal_usbd_out(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t* plen)
{
	return 0;
}
error_t hal_usbd_reset(uint8_t id)
{
	return 0;
}
error_t hal_usbd_set_address(uint8_t id,uint8_t address)
{
	return 0;
}

error_t hal_usbd_init(uint8_t id)
{
	return 0;
}
error_t hal_usbd_deinit(uint8_t id)
{
	return 0;
}





