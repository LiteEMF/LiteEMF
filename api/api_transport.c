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
#include  "api/api_tick.h"
#if API_BT_ENABLE
#include  "api/bt/api_bt.h"
#endif
#if API_USBH_BIT_ENABLE
#include "api/usb/host/usbh.h"
#endif
#if API_USBD_BIT_ENABLE
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
uint16_t m_trps = DEV_TRPS_DEFAULT;		//在工程中修改,用于判断当前支持的传输类型



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
bool api_trp_is_slave(trp_t trp)
{	
	switch(trp){
		case TR_BLE:
		case TR_EDR:
		case TR_BLE_RF:
		case TR_RF:
		case TR_USBD:
		case TR_UART:
			return true;
	}
	return false;
}
bool api_trp_is_host(trp_t trp)
{	
	switch(trp){
		case TR_BLEC:
		case TR_EDRC:
		case TR_BLE_RFC:
		case TR_RFC:
		case TR_USBH:
		case TR_UART:
			return true;
	}
	return false;
}

bool api_transport_ready(trp_handle_t* phandle)
{
	bool ready = false;

	switch(phandle->trp){
	#if API_BT_ENABLE
	case TR_BLE	:      
	case TR_EDR	:
	case TR_BLEC	:
	case TR_EDRC	:
	case TR_BLE_RF:		
	case TR_BLE_RFC:
	case TR_RF	:
	case TR_RFC	:{
		api_bt_ctb_t* bt_ctbp = api_bt_get_ctb(phandle->trp);
		if(NULL == bt_ctbp) break;
		if(BT_STA_CONN == bt_ctbp->sta){
			if((phandle->index >> 8) == DEV_TYPE_HID){
				ready = true;
			}else{
				ready = bt_ctbp->vendor_ready;
			}
		}
		break;
	}
	#endif
	#if API_USBD_BIT_ENABLE
	case TR_USBD	:{
		usbd_dev_t *pdev = usbd_get_dev(phandle->id);
		ready = pdev->ready;
		break;
	}
	#endif
	#if API_USBH_BIT_ENABLE
	case TR_USBH	:{
		usbh_dev_t* pdev = get_usbh_dev(phandle->id);
		if(TUSB_STA_CONFIGURED == pdev->state){
			ready = true;
		}
		break;
	}
	#endif
	case TR_UART	:
		ready = true;
		break;
	default:
		break;
	}
	return ready;
}


uint16_t api_transport_get_mtu(trp_handle_t* phandle)
{
	switch(phandle->trp){
	case TR_BLE		:
	case TR_BLE_RF	:
		return BLE_CMD_MTU;
	case TR_EDR		:
		return EDR_CMD_MTU;
	case TR_BLEC	:
	case TR_BLE_RFC :
		return BLEC_CMD_MTU;
	case TR_EDRC	:
		return EDRC_CMD_MTU;
	case TR_RF		:
		return RF_CMD_MTU;
	case TR_RFC		:
		return RFC_CMD_MTU;
	case TR_USBD	:
		return USBD_CMD_MTU;
	case TR_USBH	:
		return USBH_CMD_MTU;
	case TR_UART	:
		return UART_CMD_MTU;
	default:
		break;
	}
	
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	
	phandle->index use U16(h,l)	set value
*******************************************************************/
bool api_transport_tx(trp_handle_t* phandle, void* buf,uint16_t len)
{
	bool ret = true;
	uint8_t* pbuf = buf;
	static timer_t tx_timer;			//for test

	if (len <= 0)			return false;
	if (NULL == buf)		return false;

	if((TR_RF != phandle->trp) && (TR_RFC != phandle->trp)){
		//logd("trp=%d,id=%d,index=0x%x,dt=%ld,len=%d:",(uint16_t)phandle->trp, (uint16_t)phandle->id,(uint16_t)phandle->index,m_systick-tx_timer,len); tx_timer = m_systick; //dumpd(buf,len);
	}
	switch(phandle->trp){
		#if API_BT_ENABLE
		case TR_BLE	:
		case TR_EDR	:
		case TR_BLEC:	
		case TR_EDRC:
		case TR_BLE_RF:
		case TR_BLE_RFC:
		case TR_RF:
		case TR_RFC:
			if(DEV_TYPE_VENDOR == (phandle->index >> 8)){
				ret = api_bt_uart_tx(phandle->id,phandle->trp, pbuf, len);
			}else if(DEV_TYPE_HID == (phandle->index >> 8)){
				ret = api_bt_hid_tx(phandle->id,phandle->trp, pbuf, len);
			}
			break;
		#endif
		#if API_USBD_BIT_ENABLE
		case TR_USBD:
			ret = !usbd_class_in(phandle->id, (dev_type_t)(phandle->index>>8), phandle->index&0xFF, pbuf, len);
			break;	
		#endif
		#if API_USBH_BIT_ENABLE
		case TR_USBH:
			ret = !usbh_class_out((dev_type_t)(phandle->index>>8), phandle->index&0xFF, pbuf, len);
			break;	
		#endif
		#ifdef HW_UART_MAP
		case TR_UART:
			ret = api_uart_tx(phandle->id, pbuf, len);
			break;	
		#endif
		default:
			break;
	}
	if(!ret) logd("trp%d len=%d err\n",(uint16_t)phandle->trp,len);
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






