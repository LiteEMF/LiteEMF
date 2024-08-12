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
#include "app/app_commander.h"
#if API_OS_TIMER_ENABLE
#include "api/api_os_timer.h"
#endif

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


#if BT_SUPPORT & BIT_ENUM(TR_EDR)
uint8_t edr_sbuf[EDR_CMD_MTU];
command_rx_t edr_cmd_rx = {{edr_sbuf,0},{NULL,0}};
#endif
#endif

#if API_USBD_BIT_ENABLE
command_rx_t usbd_cmd_rx = {{NULL,0},{NULL,0}};
#endif
#if API_USBH_BIT_ENABLE
command_rx_t usbh_cmd_rx = {{NULL,0},{NULL,0}};
#endif
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
trp_t m_test_mode_trp = TR_NULL;

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
	uint8_t replay[16];

	switch(phead->cmd){
	/*---- 0x00	设备信息 ----*/
	case CMD_GET_EMF_VERSION:
		replay[0] = EMF_CMD_VERSION;
		app_command_tx(phandle,phead->cmd, replay, 1);
		ret = true;
		break;
	case CMD_GET_UUID:
		if(hal_get_uuid(replay, 16)){
			app_command_tx(phandle,phead->cmd, replay, 16);
			ret = true;
		}
		break;
	case CMD_GET_DEV_VERSION:
		#ifdef SW_VERSION
		replay[0] = (uint8_t)SW_VERSION>>8;
		replay[1] = (uint8_t)SW_VERSION;
		replay[2] = 0;
		replay[3] = 0;
		app_command_tx(phandle,phead->cmd, replay, 4);
		ret = true;
		#endif
		break;
	case CMD_GET_MODEL:
		#ifdef DEFAULT_MODEL
		app_command_tx(phandle,phead->cmd, DEFAULT_MODEL, strlen(DEFAULT_MODEL));
		ret = true;
		#endif
		break;
	case CMD_GET_MTU:
		//获取指令当前传输协议mtu
		replay[0] = phandle->trp;
		replay[1] = api_transport_get_mtu(phandle);	
		app_command_tx(phandle,phead->cmd, replay, 2);
		ret = true;
		break;
	case CMD_DEV_PID_VID:
		break;
	case CMD_DEV_SN:
		break;
	case CMD_DEV_MAC:
		#if API_BT_ENABLE
		if(len == (CMD_PACK_LEN+1)){
			replay[0] = phandle->id;
			replay[1] = phead->buf[0];
			if(api_bt_get_mac(replay[0], replay[1], replay+2)){
				app_command_tx(phandle,phead->cmd, replay, 8);
				ret = true;
			}
		}
		#endif
		break;
	case CMD_DEV_EID:
		break;
	case CMD_DEV_NAME:	
		#if TCFG_USER_BLE_ENABLE
		char device_name[29];
		uint8_t device_name_len = 0;
		if(len == CMD_PACK_LEN){		//read	
			device_name_len = api_bt_get_name(phandle->id,phandle->trp,device_name,sizeof(device_name) );       //获取BLE蓝牙名称
			app_command_tx(phandle,phead->cmd, device_name, device_name_len);
			ret = true;
		}else if(len > CMD_PACK_LEN){				//write
			#if API_STORAGE_ENABLE && BT_MODIFY_NAME_ENABLE
			memset(device_name,0,sizeof(device_name));
			device_name_len = api_bt_get_name(phandle->id,phandle->trp,device_name,sizeof(device_name) );       //获取BLE蓝牙名称
			if(memcmp(device_name, phead->buf, len-CMD_PACK_LEN)){
				device_name_len = len-CMD_PACK_LEN;
				if (device_name_len > BT_NAME_LEN_MAX) {
					device_name_len = BT_NAME_LEN_MAX;
				}
				memcpy(device_name,phead->buf,device_name_len);
				ble_comm_set_config_name(device_name, 0);	
				m_storage.device_name_len = device_name_len;	
				memcpy(m_storage.device_name,device_name,device_name_len);
				api_storage_auto_sync();
				api_reset();
			}
			#endif
		}
		#else
		#ifdef DEFAULT_NAME
		if(len == CMD_PACK_LEN){		//read	
			app_command_tx(phandle,phead->cmd, DEFAULT_NAME, strlen(DEFAULT_NAME));
			ret = true;
		}else if(len > CMD_PACK_LEN){				//write
			

		}
		#endif
		#endif
		break;

	/*---- 0x10	设备信息 ----*/
	case CMD_DEV_MODE:			//TODO 功能待完善修改
		if(len >= CMD_PACK_LEN + 8){
		//	uint16_t trps = U16(phead->buf[0],phead->buf[1]);
			m_dev_mode = U16(phead->buf[4],phead->buf[5]);
			m_hid_mode = U16(phead->buf[6],phead->buf[7]);
		}
		replay[0] = m_trps>>8;
		replay[1] = m_trps;
		replay[2] = TYPED_SUPPORT>>8;
		replay[3] = TYPED_SUPPORT & 0xff;
		replay[4] = m_dev_mode>>8;
		replay[5] = m_dev_mode;
		replay[6] = m_hid_mode>>8;
		replay[7] = m_hid_mode;
		app_command_tx(phandle,phead->cmd, replay, 10);
		ret = true;
		break;
	case CMD_TEST_MODE:
		if(len > CMD_PACK_LEN){
			m_test_mode_trp = (trp_t)phead->buf[0];
		}else{
			m_test_mode_trp = (trp_t)phandle->trp;
		}
		ret = true;
		break;
	case CMD_DEV_CTRL:
		#if API_PM_ENABLE
		if(len > CMD_PACK_LEN){
			dev_ctrl_t ctrl = phead->buf[0];
			switch(ctrl){
			#if API_PM_ENABLE
		    case CTRL_RESET:
				api_reset();
				ret = true;
				break;
			case CTRL_STOP:
				api_sleep();
				ret = true;
				break;
			#endif	
			case CTRL_BOOT:
				if(len > 6){
					api_boot(phead->buf[1]);
				}else{
					api_boot(0);
				}
				ret = true;
				break;
			}
			if(ret){
				replay[0] = ctrl;
				replay[1] = 0;
				app_command_tx(phandle,phead->cmd, replay, 2);
			}
		}
		#endif
		break;
	case CMD_STORAGE_SYNC:
		#if API_STORAGE_ENABLE
		ret = api_storage_sync();
		app_command_tx(phandle,phead->cmd, &ret, 1);
		#endif
		break;
	case CMD_HEART_BEAT:
		app_command_tx(phandle,phead->cmd, &ret, 1);
		break;
	case CMD_RECOVER_FACTORY:
		app_command_tx(phandle,phead->cmd, &ret, 1);
		break;
	default:
		break;
	}

	#if API_USBH_BIT_ENABLE && USBH_SOCKET_ENABLE
	if(!ret){
		ret = usbh_socket_decode(phandle,phead->cmd, phead->buf, len - (CMD_HEAD_LEN+1));
	}
	#endif

	#if API_USBD_BIT_ENABLE && USBD_SOCKET_ENABLE
	if(!ret){
		ret = usbd_socket_decode(phandle,phead->cmd, phead->buf, len - (CMD_HEAD_LEN+1));
	}
	#endif

	return ret;
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	注意: 数据长度必须使用len , buf 中的长度不准确
*******************************************************************/
#if WEAK_ENABLE
__WEAK bool app_command_vendor_decode(trp_handle_t *phandle,uint8_t* buf,uint16_t len)
{
	return app_command_std_decode(phandle,buf,len);
}
#endif



static void app_command_tx_fill(command_tx_t *txp, trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len)
{
	txp->cmd = cmd;
	txp->handle = *phandle;
	txp->buf = buf;
	txp->len = len;
	txp->index = 0;
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description:	用于类似蓝牙大数据发送, 需要定时拆分发送数据包
*******************************************************************/
#if API_OS_TIMER_ENABLE
static void app_command_timer_cb(command_tx_t *txp)
{
	bool ret = false;
	if((NULL != txp->buf) || (0 == txp->len)){
		ret = command_frame_tx(txp);

		if(ret && (NULL == txp->buf || txp->index == txp->len)){
			emf_free_and_clear(txp->buf);
			api_os_timer_stop((api_os_timer_t*)txp->ptimer);
		}
	}

}

/*******************************************************************
** Parameters: txp: 传入静态变量 buf:可以是临时变量, 内部会缓存
** Returns:	
** Description:	长包定时拆分发送, 注意: 使用到了软件定时器, 该接口不适用于51平台
*******************************************************************/
bool app_command_timer_tx(command_tx_t *txp, trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len, uint32_t ms)
{
	bool ret = false;
	api_os_timer_t *tx_timer;
	uint8_t *p;
	uint8_t mtu = api_transport_get_mtu(phandle);

	if(NULL != txp->buf) {
		loge("txp buf is not NULL\n");
		return ret;
	}
	
	p = emf_malloc(len);
	if(NULL != p) {
		memcpy(p, buf, len);
	}

	app_command_tx_fill(txp, phandle, cmd, p, len);

	tx_timer = api_os_timer_create((timer_cb_t)&app_command_timer_cb,(void*)txp,ms,0);
	if(NULL != tx_timer){
		txp->ptimer = tx_timer;
		ret = !api_os_timer_start(tx_timer);
	}else{
		emf_free_and_clear(txp->buf);
	}
	
	return ret;
}
#endif

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	while(1)循环发送指令
*******************************************************************/
bool app_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len)
{
	bool ret = false;
	command_tx_t tx;
	uint8_t mtu = api_transport_get_mtu(phandle);

	app_command_tx_fill(&tx, phandle, cmd, buf, len);
	
	do{
		ret = command_frame_tx(&tx);
		if(!ret) break;
	}while(tx.index < tx.len);

	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	注意为了接收空间没有将所以的通讯都定义出来, 如果有冲突需要修改!!	
*******************************************************************/
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
	#if BT_SUPPORT & BIT_ENUM(TR_EDR)
	case TR_EDR		:
		return &edr_cmd_rx;
	#endif
	
	#endif

	#if API_USBD_BIT_ENABLE
	case TR_USBD	:
		return &usbd_cmd_rx;
	#endif
	#if API_USBH_BIT_ENABLE
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

		break;
	}
	
	return NULL;
}

/*******************************************************************
** Parameters:	buf: 必须是完整端包指令
** Returns:	
** Description:	解析短包指令, 
*******************************************************************/
bool app_command_rx(trp_handle_t *phandle,uint8_t *buf, uint8_t len)
{	
	uint8_t ret = false;
	command_rx_t* cmd_rxp = app_get_command_rx(phandle);
	uint8_t mtu = api_transport_get_mtu(phandle);

	if(NULL == cmd_rxp) return false;

	ret = api_command_rx(&cmd_rxp->cmds, buf,len);
	if(ret){
		cmd_rxp->sbuf.len = 0;
		app_command_vendor_decode(phandle, cmd_rxp->cmds.buf, cmd_rxp->cmds.len);
		emf_free_and_clear(cmd_rxp->cmds.buf);
	}

	return ret;
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
		if(ret){
			cmd_rxp->sbuf.len = 0;
			app_command_vendor_decode(phandle, cmd_rxp->cmds.buf, cmd_rxp->cmds.len);
			emf_free_and_clear(cmd_rxp->cmds.buf);
		}
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
			
			if(ret){
				cmd_rxp->sbuf.len = 0;
				app_command_vendor_decode(phandle, cmd_rxp->cmds.buf, cmd_rxp->cmds.len);
				emf_free_and_clear(cmd_rxp->cmds.buf);
			}
		}
	}

	return ret;
}


void app_command_init(void)
{
}


#endif






