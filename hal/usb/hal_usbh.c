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
#if API_USBH_BIT_ENABLE
#include  "api/usb/usb_typedef.h"
#include  "api/usb/host/usbh.h"

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
error_t hal_usbh_port_en(uint8_t id,uint8_t en, usb_speed_t* pspeed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(en);
	UNUSED_PARAMETER(pspeed);
	return ERROR_SUCCESS;
}
error_t hal_usbh_set_speed(uint8_t id, usb_speed_t speed)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(speed);
	return ERROR_SUCCESS;
}
error_t hal_usbh_port_reset(uint8_t id, uint8_t reset_ms)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(reset_ms);
	return ERROR_SUCCESS;
}
error_t hal_usbh_set_addr(uint8_t id,uint8_t addr)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(addr);
	return ERROR_SUCCESS;
}
error_t hal_usbh_endp_unregister(uint8_t id,usb_endp_t *endpp)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(endpp);
	return ERROR_SUCCESS;
}
error_t hal_usbh_endp_register(uint8_t id,usb_endp_t *endpp)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(endpp);
	return ERROR_SUCCESS;
}
error_t hal_usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(preq);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(plen);
	return ERROR_SUCCESS;
}
error_t hal_usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(endpp);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(plen);
	UNUSED_PARAMETER(timeout_ms);
	return ERROR_SUCCESS;
}
error_t hal_usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(endpp);
	UNUSED_PARAMETER(buf);
	UNUSED_PARAMETER(len);
	return ERROR_SUCCESS;
}
error_t hal_usbh_driver_init(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}
error_t hal_usbh_driver_deinit(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}
void hal_usbh_driver_task(uint32_t dt_ms)
{
	UNUSED_PARAMETER(dt_ms);
}

#endif


