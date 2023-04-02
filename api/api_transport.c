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
#include  "api/api_transport.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t m_trps = DEV_TRPS_DEFAULT;



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
bool api_transporte_init(void)
{
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_trp_is_usb(trp_t trp)
{	
	return ((TR_USBD == trp) || (TR_USBH == trp));
}

bool api_trp_is_bt(trp_t trp)
{	
	return (TR_RFC >= trp);
}

uint16_t api_get_transport_mtu(trp_handle_t* phandle)
{
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_transport_tx(trp_handle_t* phandle, uint8_t* buf,uint16_t size)
{
	bool ret = true;
	if (size <= 0)			return false;
	if (NULL == buf)		return false;

	switch(phandle->trp){
		case TR_BLE	:
			break;
		case TR_EDR	:
			break;
		case TR_BLEC:
			break;	
		case TR_EDRC:
			break;	
		case TR_BLE_RF:
			break;
		case TR_BLEC_RF:
			break;
		case TR_RF:
			break;
		case TR_RFC:
			break;
		case TR_USBD:
			break;	
		case TR_USBH:
			break;	
		case TR_UART:
			break;	
		default:
			break;
	}
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_trp_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_trp_deinit(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_trp_handler(void)
{

}




