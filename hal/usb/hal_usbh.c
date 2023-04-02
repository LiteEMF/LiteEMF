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
error_t hal_usbh_port_disable(uint8_t id)
{
}
error_t hal_usbh_port_reset(uint8_t id)
{
}
error_t hal_usbh_set_addr(uint8_t id,uint8_t addr)
{
}
error_t hal_usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen)
{
}
error_t hal_usbh_endp_unregister(uint8_t id,usb_endp_t *endpp)
{
}
error_t hal_usbh_endp_register(uint8_t id,usb_endp_t *endpp)
{
}
error_t hal_usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen,uint16_t timeout_ms)
{
}
error_t hal_usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len)
{
}
void 	hal_usbh_driver_handler(void)
{
}
error_t hal_usbh_driver_init(uint8_t id)
{
}
error_t hal_usbh_driver_deinit(uint8_t id)
{
}




