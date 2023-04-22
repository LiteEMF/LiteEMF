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

#if API_BT_ENABLE
#include  "api/bt/api_bt.h"
#endif
#if APP_USBH_ENABLE
#include "api/usb/host/usbh.h"
#endif
#if APP_USBD_ENABLE
#include "api/usb/device/usbd.h"
#endif
#ifdef HW_UART_MAP
#include  "api/api_uart.h"
#endif


#include  "api/api_log.h"
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
bool api_trp_is_usb(trp_t trp)
{	
	return ((TR_USBD == trp) || (TR_USBH == trp));
}

bool api_trp_is_bt(trp_t trp)
{	
	return (TR_RFC >= trp);
}

uint16_t api_transport_get_mtu(trp_handle_t* phandle)
{
	switch(phandle->trp){
	case TR_BLE		:
		return BLE_CMD_MTU;
		break;
	case TR_EDR		:
		return EDR_CMD_MTU;
		break;
	case TR_BLEC	:
		return BLEC_CMD_MTU;
		break;
	case TR_EDRC	:
		return EDRC_CMD_MTU;
		break;
	case TR_BLE_RF	:
		return BLE_RF_CMD_MTU;
		break;
	case TR_BLEC_RF	:
		return BLEC_RF_CMD_MTU;
		break;
	case TR_RF		:
		return RF_CMD_MTU;
		break;
	case TR_RFC		:
		return RFC_CMD_MTU;
		break;
	case TR_USBD	:
		return USBD_CMD_MTU;
		break;
	case TR_USBH	:
		return USBH_CMD_MTU;
		break;
	case TR_UART	:
		return UART_CMD_MTU;
		break;
	default:
		break;
	}
	
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_transport_tx(trp_handle_t* phandle, uint8_t* buf,uint16_t len)
{
	bool ret = true;
	static timer_t tx_timer;			//for test
	if (len <= 0)			return false;
	if (NULL == buf)		return false;

	logd("trp(%d) id(%d) index(0x%x) dt=%d ret=%d:",phandle->trp, phandle->id,phandle->index,m_systick-tx_timer,ret); tx_timer = m_systick;dumpd(buf,len);
	switch(phandle->trp){
		#if API_BT_ENABLE
		case TR_BLE	:
		case TR_EDR	:
		case TR_BLEC:	
		case TR_EDRC:
		case TR_BLE_RF:
		case TR_BLEC_RF:
		case TR_RF:
		case TR_RFC:
			if(DEV_TYPE_VENDOR == (phandle->index >> 8)){
				ret = api_bt_uart_tx(phandle->id,phandle->trp, buf, len);
			}else if(DEV_TYPE_HID == (phandle->index >> 8)){
				ret = api_bt_hid_tx(phandle->id,phandle->trp, buf, len);
			}
			break;
		#endif
		#if APP_USBD_ENABLE
		case TR_USBD:
			ret = !usbd_class_in(phandle->id, phandle->index>>8, phandle->index&0xFF, buf, len);
			break;	
		#endif
		#if APP_USBH_ENABLE
		case TR_USBH:
			ret = !usbh_class_out(phandle->index>>8, phandle->index&0xFF, buf, len);
			break;	
		#endif
		#ifdef HW_UART_MAP
		case TR_UART:
			ret = api_uart_tx(phandle->id, buf, len);
			break;	
		#endif
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
	m_trps = DEV_TRPS_DEFAULT;
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






