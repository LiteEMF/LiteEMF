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
#if APP_CMD_ENABLE
#include "app/app_command.h"
#include "app/emf.h"

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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_uart.h"
#include  "api/api_log.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifdef HW_UART_MAP
uint8_t uart_sbuf[UART_CMD_MTU];
uint8_t uart2_sbuf[UART_CMD_MTU];
command_rx_t uart_cmd_rx = {{uart_sbuf,0},{NULL,0}};
command_rx_t uart2_cmd_rx = {{uart2_sbuf,0},{NULL,0}};
#endif

#if API_BT_ENABLE
#if BT_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))					//ble peripheral
uint8_t ble_sbuf[BLE_CMD_MTU];
command_rx_t ble_cmd_rx = {{ble_sbuf,0},{NULL,0}};;
#endif

#if BT_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))					//ble central
uint8_t blec_sbuf[BLEC_CMD_MTU];
command_rx_t blec_cmd_rx = {{blec_sbuf,0},{NULL,0}};;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RF)
uint8_t rf_sbuf[RF_CMD_MTU];
command_rx_t rf_cmd_rx = {{rf_sbuf,0},{NULL,0}};
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RFC)
uint8_t rfc_sbuf[RFC_CMD_MTU];
command_rx_t rfc_cmd_rx = {{rfc_sbuf,0},{NULL,0}};
#endif
#endif

#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR))
command_rx_t usbd_cmd_rx = {{NULL,0},{NULL,0}};
#endif
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR))
command_rx_t usbh_cmd_rx = {{NULL,0},{NULL,0}};
#endif
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
** Parameters:	通讯协议处理
** Returns:	
** Description:		
*******************************************************************/
bool app_command_std_decode(trp_handle_t *phandle,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	command_head_t *phead = (command_head_t*)buf;

	switch(phead->cmd){
	/*---- 0x00	设备信息 ----*/
	case CMD_GET_EMF_VERSION:
		break;
	case CMD_GET_UUID:
		break;
	case CMD_GET_DEV_VERSION:
		break;
	case CMD_GET_MODE:
		break;
	case CMD_GET_MTU:
		break;
	//case //6,7
	case CMD_DEV_PID_VID:
		break;
	case CMD_DEV_SN:
		break;
	case CMD_DEV_MAC:
		break;
	case CMD_DEV_EID:
		break;
	case CMD_DEV_NAME:
		break;

	/*---- 0x10	设备信息 ----*/
	case CMD_DEV_MODE:
		break;
	case CMD_TEST_MODE:
		break;
	case CMD_DEV_CTRL:
		break;
	case CMD_STORAGE_SYNC:
		break;
	case CMD_HEART_BEAT:
		break;
	case CMD_RECOVER_FACTORY:
		break;
	default:
		break;
	}

	#if API_USBH_BIT_ENABLE && USBH_SOCKET_ENABLE
	if(!ret){
		ret = usbh_socket_decode(phandle,phead->cmd, buf+COM_HEAD_LEN, len - (COM_HEAD_LEN+1));
	}
	#endif

	#if API_USBD_BIT_ENABLE && USBD_SOCKET_ENABLE
	if(!ret){
		ret = usbd_socket_decode(phandle,phead->cmd, buf+COM_HEAD_LEN, len - (COM_HEAD_LEN+1));
	}
	#endif

	return ret;
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	注意: 数据长度必须使用len , buf 中的长度不准确
*******************************************************************/
__WEAK bool app_command_vendor_decode(trp_handle_t *phandle,uint8_t* buf,uint16_t len)
{
	return app_command_std_decode(phandle,buf,len);
}




command_rx_t* app_get_command_rx(trp_handle_t* phandle)
{
	switch(phandle->trp){
	#if API_BT_ENABLE
	#if BT_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
	case TR_BLE		:
	case TR_BLE_RF	:
		return &ble_cmd_rx;
	#endif
	#if BT_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
	case TR_BLEC	:
	case TR_BLE_RFC	:
		return &blec_cmd_rx;
	#endif
	#if BT_SUPPORT & BIT_ENUM(TR_RF)
	case TR_RF		:
		return &rf_cmd_rx;
	#endif
	#if BT_SUPPORT & BIT_ENUM(TR_RFC)
	case TR_RFC		:
		return &rfc_cmd_rx;
	#endif
	#endif

	#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR))
	case TR_USBD	:
		return &usbd_cmd_rx;
	#endif
	#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR))
	case TR_USBH	:
		return &usbh_cmd_rx;
	#endif
	#ifdef HW_UART_MAP
	case TR_UART	:
		if(0 == phandle->id){
			return &uart_cmd_rx;
		}else{
			return &uart2_cmd_rx;
		}
	#endif
	default:
		return NULL;
		break;
	}
	
	return NULL;
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description:	从输入流中通过字节包解包并获取完整数据指令包
	注意:采用动态内存方式
*******************************************************************/
bool app_command_rx_byte(trp_handle_t *phandle, uint8_t c)
{	
	uint8_t ret = false;
	command_rx_t* cmd_rxp = app_get_command_rx(phandle);
	uint8_t mtu = api_transport_get_mtu(phandle);

	if(NULL == cmd_rxp) return false;
	if(command_frame_rx(&cmd_rxp->sbuf, mtu, c)){
		ret = api_command_rx(&cmd_rxp->cmds, cmd_rxp->sbuf.buf,cmd_rxp->sbuf.len);
		cmd_rxp->sbuf.len = 0;
		app_command_vendor_decode(phandle, cmd_rxp->cmds.buf, cmd_rxp->cmds.len);
		emf_free_and_clear(cmd_rxp->cmds.buf);
	}

	return ret;
}

bool app_command_rx_fifo(trp_handle_t *phandle, app_fifo_t* fifop)
{	
	uint8_t ret = false;
	uint8_t c;
	command_rx_t* cmd_rxp = app_get_command_rx(phandle);
	uint8_t mtu = api_transport_get_mtu(phandle);
	
	if(NULL == cmd_rxp) return false;
	while(ERROR_SUCCESS == app_fifo_get(fifop, &c)){
		if(command_frame_rx(&cmd_rxp->sbuf, mtu, c)){
			ret = api_command_rx(&cmd_rxp->cmds, cmd_rxp->sbuf.buf, cmd_rxp->sbuf.len);
			cmd_rxp->sbuf.len = 0;

			app_command_vendor_decode(phandle, cmd_rxp->cmds.buf, cmd_rxp->cmds.len);
			emf_free_and_clear(cmd_rxp->cmds.buf);
		}
	}

	return ret;
}


void app_command_init(void)
{
}


#endif






