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

//产品模式
//TODO注意: 以下类型用于产品类型分类, 具体到不同传输类型上在工程中根据不同产品自定义修改
//后期遇到不同传输类型需要不同模式的产品再修改适配
uint16_t m_trps = DEV_TRPS_DEFAULT;		
uint16_t m_dev_mode = USBD_TYPE_SUPPORT | BT_TYPE_SUPPORT;
uint16_t m_hid_mode = HIDD_SUPPORT;					

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

bool api_transport_set_type(uint8_t id, trp_t trp, uint16_t dev_types, uint16_t hid_types)
{
	bool ret = false;

	if(TR_USBD == trp){
		#if API_USBD_BIT_ENABLE
		dev_types &= USBD_TYPE_SUPPORT;
		hid_types &= USBD_HID_SUPPORT;
		m_usbd_types[id] = dev_types;
		m_usbd_hid_types[id] = hid_types;
		ret = true;
		#endif
	}else if(TR_USBH == trp){			//usb host 修改的是AOA/IAP2模式
		#if API_USBH_BIT_ENABLE
		dev_types &= USBH_TYPE_SUPPORT;
		m_usbh_types = dev_types;
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
		if(dev_types & BIT_ENUM(DEV_TYPES_AOA)){
			m_aoa_hid_typess = hid_types & AOA_HID_SUPPORT;
		}
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
		if(dev_types & BIT_ENUM(DEV_TYPES_IAP2)){
			m_iap_hid_types = hid_types & IAP2_HID_SUPPORT;
		}
		#endif
		hid_types &= IAP2_HID_SUPPORT | AOA_HID_SUPPORT;
		#endif
	}else if(TR_RFC >= trp){	//bt and 2.4g
		#if API_BT_ENABLE
		api_bt_ctb_t* bt_ctbp;
		bt_ctbp = api_bt_get_ctb(trp);
		if(NULL != bt_ctbp){
			switch(trp){
			case TR_BLE		:
				dev_types &= BLE_TYPE_SUPPORT;
				hid_types &= BLE_HID_SUPPORT;
				bt_ctbp->types = dev_types;
				bt_ctbp->hid_types = hid_types;
				ret = true;
				break;
			case TR_EDR		:
				dev_types &= EDR_TYPE_SUPPORT;
				hid_types &= EDR_HID_SUPPORT;
				bt_ctbp->types = dev_types;
				bt_ctbp->hid_types = hid_types;
				ret = true;
				break;
			case TR_BLEC	:
			case TR_BLE_RF	:
			case TR_BLE_RFC :
			case TR_EDRC	:
			case TR_RF		:
			case TR_RFC		:
				bt_ctbp->hid_types = hid_types;
				ret = true;
				break;
			default:
				break;
			}
		}
		#endif
	}

	if(0 == (hid_types | dev_types)){
		ret = false;
	}
	UNUSED_PARAMETER(id);
	return ret;
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
				ready = bt_ctbp->hid_ready;
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
		if((NULL != pdev) && (TUSB_STA_CONFIGURED == pdev->state)){
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
	if(NULL == phandle) 	return 0;
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
	static timer_t err_timer;			//for test

	if(NULL == phandle) 	return false;
	if (len <= 0)			return false;
	if (NULL == buf)		return false;

	if((TR_RF != phandle->trp) && (TR_RFC != phandle->trp)){
		// logd("trp=%d,id=%d,index=0x%x,dt=%ld,len=%d:",(uint16_t)phandle->trp, (uint16_t)phandle->id,(uint16_t)phandle->index,m_systick-tx_timer,len); tx_timer = m_systick; //dumpd(buf,len);
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
				if(TR_EDR == phandle->trp){
					ret = api_bt_hid_tx(phandle->id,phandle->trp, EDR_HID_REQ_TYPE_DATA|HID_REPORT_TYPE_INPUT, pbuf, len);
				}else if(TR_EDRC == phandle->trp){
					ret = api_bt_hid_tx(phandle->id,phandle->trp, EDR_HID_REQ_TYPE_DATA|HID_REPORT_TYPE_OUTPUT, pbuf, len);
				}else{
					ret = api_bt_hid_tx(phandle->id,phandle->trp, HID_REPORT_TYPE_INVALID, pbuf, len);
				}
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
	if(!ret) {
		logd("trp%d len=%d err dt=%d\n",(uint16_t)phandle->trp,len,m_systick-err_timer);err_timer = m_systick;
		//dumpd(buf,len);
	}

	UNUSED_VARIABLE(tx_timer);
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_trp_init(void)
{
	//判断和设置默认模式
	#if API_STORAGE_ENABLE

	if(m_storage.trps & ~(BIT(TR_MAX) - 1)){
		m_storage.trps = DEV_TRPS_DEFAULT;
	}
	if(m_storage.dev_mode & ~(USBD_TYPE_SUPPORT | BT_TYPE_SUPPORT)){
		m_storage.dev_mode = USBD_TYPE_SUPPORT | BT_TYPE_SUPPORT;
	}
	if(m_storage.hid_mode & ~HIDD_SUPPORT){
		m_storage.hid_mode = HIDD_SUPPORT;
	}
	m_trps = m_storage.trps;
	m_dev_mode = m_storage.dev_mode;
	m_hid_mode = m_storage.hid_mode;
	#endif

	logd_g("trp init dev trps=%x, modes=%x hids=%x\n",m_trps,m_dev_mode,m_hid_mode);
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






