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
/*******************************************************************
** Description:	
	BLE: 	ble peripheral
	EDR: 	edr peripheral
	BT:		ble+edr peripheral
	BLEC:	ble central	
	EDRC:	edr central
	RF:		rf peripheral
	BTC:	ble+edr	central
	RFC:	rf central
	APP_BT: ble+edr+rf peripheral and central
*******************************************************************/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "hw_config.h"
#if API_BT_ENABLE
#include  "api/bt/api_bt.h"
#include  "api/api_tick.h"

#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad.h"
#endif


#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

#if BT_SUPPORT & BIT_ENUM(TR_BLE)					//ble peripheral
api_bt_ctb_t m_ble = {BLE_TYPE_SUPPORT, BLE_HID_SUPPORT, true};
bool m_ble_adv_discoverable = true;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_BLEC)					//ble central
api_bt_ctb_t m_blec = {BIT(DEV_TYPE_VENDOR), 0, true};
bt_evt_scan_t blec_scan_result;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_BLE_RF)
api_bt_ctb_t m_ble_rf = {BIT(DEV_TYPE_VENDOR), 0, true};
#endif

#if BT_SUPPORT & BIT_ENUM(TR_BLE_RFC)					//ble central
api_bt_ctb_t m_ble_rfc = {BIT(DEV_TYPE_VENDOR), 0, true};
bt_evt_scan_t ble_rfc_scan_result;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_EDR)					//edr peripheral
bool edr_sniff_by_remote = false; 					//switch模式下由主机发起进入sniff
api_bt_ctb_t m_edr = {EDR_TYPE_SUPPORT, EDR_HID_SUPPORT, true};
#endif

#if BT_SUPPORT & BIT_ENUM(TR_EDRC)					//edr central
api_bt_ctb_t m_edrc = {BIT(DEV_TYPE_VENDOR), 0, true};
bt_evt_scan_t edrc_scan_result;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RF)
api_bt_ctb_t m_rf = {BIT(DEV_TYPE_VENDOR), 0, true};
static bt_tx_fifo_t app_rf_tx;
static uint8_t rf_tx_buf[RF_TX_LL_MTU];
static uint8_t rf_tx_fifo_buf[RF_FIFO_LEN];
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RFC)
api_bt_ctb_t m_rfc = {BIT(DEV_TYPE_VENDOR), 0, true};
static bt_tx_fifo_t app_rfc_tx;
static uint8_t rfc_tx_buf[RFC_TX_LL_MTU];
static uint8_t rfc_tx_fifo_buf[RF_FIFO_LEN];
#endif


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
** Description:		输入bt_t 输出对应数据
*******************************************************************/
bt_evt_scan_t* api_bt_get_scan_result(bt_t bt)
{
	bt_evt_scan_t* resultp = NULL;
	bool ret = false;

	switch(bt){
		#if BT_SUPPORT & BIT_ENUM(TR_BLEC)
		case BT_BLEC:
			resultp = &blec_scan_result;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_BLE_RFC)
		case BT_BLE_RFC:
			resultp = &ble_rfc_scan_result;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_EDRC)
		case BT_EDRC:
			resultp = &edrc_scan_result;
			break;
		#endif
		
		default:
			break;
	}
	return resultp;
}


api_bt_ctb_t* api_bt_get_ctb(bt_t bt)
{
	api_bt_ctb_t* api_btp = NULL;
	switch(bt){
		#if BT_SUPPORT & BIT_ENUM(TR_BLE)
		case BT_BLE:
			api_btp = &m_ble;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_BLEC)
		case BT_BLEC:
			api_btp = &m_blec;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_BLE_RF)
		case BT_BLE_RF:
			api_btp = &m_ble_rf;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_BLE_RFC)
		case BT_BLE_RFC:
			api_btp = &m_ble_rfc;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_EDR)
		case BT_EDR:
			api_btp = &m_edr;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_EDRC)
		case BT_EDRC:
			api_btp = &m_edrc;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_RF)
		case BT_RF:
			api_btp = &m_rf;
			break;
		#endif
		#if BT_SUPPORT & BIT_ENUM(TR_RFC)
		case BT_RFC:
			api_btp = &m_rfc;
			break;
		#endif
		default:
			break;
	}

	return api_btp;
}


static bool bt_tx_fifo_init(bt_tx_fifo_t* txp, uint8_t *tx_buf, uint16_t mtu,uint8_t *fifo_buf,uint16_t fifo_len)
{
	memset(txp,0,sizeof(bt_tx_fifo_t));
	app_fifo_init(&txp->fifo, fifo_buf, fifo_len);
	txp->tx_mtu = mtu;
	txp->tx_buf = tx_buf;

	return true;
}

bool api_bt_is_connected(bt_t bt)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;
	
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL != bt_ctbp){
		if(BT_STA_CONN == bt_ctbp->sta){
			ret = true;
		}
	}

	return ret;
}

#if WEAK_ENABLE
/*******************************************************************
** Description:	蓝牙MAC地址, 使用大端地址
 * add[3~5]为高厂商地址,add[0~2]为低产品地址(NRF工具显示的地址前面是高位,后面是低位)
*******************************************************************/
__WEAK bool api_bt_vendor_mac(uint8_t id, bt_t bt, uint8_t *pmac )
{
	return false;
}
/*******************************************************************
** Parameters:	len:	buf 长度,
** Returns:		名称字符长度, 0 表示不修改名称
** Description:	用户自定义蓝牙名称,蓝牙名称会根据模式和蓝牙地址不而不同
*******************************************************************/
__WEAK uint8_t api_bt_vendor_name(uint8_t id,bt_t bt, char *buf, uint8_t len )
{
	return MIN(len-1,(uint8_t)strlen(buf));
}
#endif

/*******************************************************************
** Parameters: id bt
** Returns:		获取mac成功
** Description:	蓝牙MAC地址, 使用大端地址
 * add[3~5]为高厂商地址,add[0~2]为低产品地址(NRF工具显示的地址前面是高位,后面是低位)
 * add[0] 用于产品后面随机数,经典蓝牙地址是ble地址(ADD[0]+1)
 * add[1] 用于产品升级时候进入boot蓝牙地址+1
 * add[2] 用于产品不同模式下不同mac地址
 * add[3~5]一般不建议修改
*******************************************************************/
bool api_bt_get_mac(uint8_t id, bt_t bt, uint8_t *pmac )		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
{
	bool ret = true;
	uint8_t base_mac[6];
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return false;
	
	if(id == BT_ID0){
		if(!hal_bt_get_mac(id, bt,base_mac)) return false;	//get base_mac
	}else{
		if(!hal_bt_get_mac(id, bt,base_mac)) return false;	//get base_mac
	}
	
	#if BLE_RANDOM_MAC_ENABLE
	if((BT_BLE == bt) || (BT_BLE_RF == bt)){
		memcpy(pmac, base_mac, 6);
		return ret;
	}
	#endif

	if((BT_BLE == bt) || (BT_BLE_RF == bt)){
		base_mac[0] += 1;								//edr address + 1
	}

 	#if (BT_TYPE_SUPPORT & BIT(DEV_TYPE_HID))					//优化代码,不支持HID不编译
	if(bt_ctbp->types & BIT(DEV_TYPE_HID)){
		switch(bt_ctbp->hid_types){
			case BIT_ENUM(HID_TYPE_GAMEPADE):
				base_mac[2] += HID_TYPE_GAMEPADE;
				break;
			case BIT_ENUM(HID_TYPE_DINPUT):
				base_mac[2] += HID_TYPE_DINPUT;
				break;
			case BIT_ENUM(HID_TYPE_SWITCH):
				base_mac[2] += HID_TYPE_SWITCH;
				break;
			case BIT_ENUM(HID_TYPE_PS3)	:
				base_mac[2] += HID_TYPE_PS3;
				break;
			case BIT_ENUM(HID_TYPE_PS4)	:
				base_mac[2] += HID_TYPE_PS4;
				break;
			case BIT_ENUM(HID_TYPE_PS5)	:
				base_mac[2] += HID_TYPE_PS5;
				break;
			case BIT_ENUM(HID_TYPE_X360)	:
				base_mac[2] += HID_TYPE_X360;
				break;
			case BIT_ENUM(HID_TYPE_XBOX)	:
				base_mac[2] += HID_TYPE_XBOX;
				break;
			case BIT_ENUM(HID_TYPE_KB) 	:
				base_mac[2] += HID_TYPE_KB;
				break;
			case BIT_ENUM(HID_TYPE_MOUSE) :
				base_mac[2] += HID_TYPE_MOUSE;
				break;
			case BIT_ENUM(HID_TYPE_CONSUMER)	:
				base_mac[2] += HID_TYPE_CONSUMER;
				break;
			case BIT_ENUM(HID_TYPE_MT)	:
				base_mac[2] += HID_TYPE_MT;
				break;
			case BIT_ENUM(HID_TYPE_TOUCH)	:
				base_mac[2] += HID_TYPE_TOUCH;
				break;
			default :			//comble device
				base_mac[2] += 16;
				break;
		}
	}
	#endif

	memcpy(pmac, base_mac, 6);
	api_bt_vendor_mac(id, bt, pmac );		//用于用户自定义mac地址

	return ret;
}

/*******************************************************************
** Parameters:	len:buf 长度,
** Returns:		名称字符长度	
** Description:	获取蓝牙名称,蓝牙名称会根据模式和蓝牙地址不而不同
*******************************************************************/
uint8_t api_bt_get_name(uint8_t id,bt_t bt, char *buf, uint8_t len )
{
	bool ret = false;
	uint8_t name[33], name_len;
    uint8_t bt_addr[6];
    char chr[3];
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return false;

    memset(name,0,sizeof(name));

	#if (BT_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID))
	if(bt_ctbp->types & BIT_ENUM(DEV_TYPE_HID)){
		switch(bt_ctbp->hid_types){
			#if (BT_HID_SUPPORT & HID_SWITCH_MASK)
			case BIT_ENUM(HID_TYPE_SWITCH):
				memcpy(name,"Pro Controller",strlen("Pro Controller"));
				ret = true;
				break;
			#endif
			#if (BT_HID_SUPPORT & HID_PS_MASK)
			case BIT_ENUM(HID_TYPE_PS3)	:
			case BIT_ENUM(HID_TYPE_PS4)	:
				memcpy(name,"Wireless Controller",strlen("Wireless Controller"));
				ret = true;
				break;
			case BIT_ENUM(HID_TYPE_PS5)	:
				// memcpy(name,"DualSense Edge Wireless Controlle",strlen("DualSense Edge Wireless Controlle"));
				memcpy(name,"DualSense Wireless Controller",strlen("DualSense Wireless Controller"));
				ret = true;
				break;
			#endif
			#if (BT_HID_SUPPORT & HID_XBOX_MASK)
			case BIT_ENUM(HID_TYPE_X360)	:
			case BIT_ENUM(HID_TYPE_XBOX)	:
				memcpy(name,"Xbox Wireless Controller",strlen("Xbox Wireless Controller"));
				ret = true;
				break;
			#endif
			default :
				break;
		}
	}
	#endif

	#ifdef DEFAULT_NAME
	// if(!ret){
		memcpy(name,DEFAULT_NAME,sizeof(DEFAULT_NAME));
		
		#if BT_RANDOM_NAME_ENABLE
		api_bt_get_mac(id, bt,bt_addr);
		sprintf(chr, "%02x", bt_addr[0]);
		memcpy(name+strlen(name),chr,3);
		#endif
	// }
	#endif
	
	#if API_STORAGE_ENABLE && BT_MODIFY_NAME_ENABLE
	if((m_storage.device_name_len > 0) && (m_storage.device_name_len <= BT_NAME_LEN_MAX)){
		memset(name,0,sizeof(name));
		memcpy(name,m_storage.device_name,m_storage.device_name_len);
	}
	#endif

	memset(buf,0,len);
    name_len = MIN(len-1,(uint8_t)strlen(name));
    memcpy(buf, name, name_len);

	name_len = api_bt_vendor_name(id,bt, buf, len);	//用户自定义名称

	return name_len;
}

bool api_bt_is_bonded(uint8_t id,bt_t bt)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok) return ret;

	if(id == BT_ID0){
		ret = hal_bt_is_bonded(id, bt);
	}else{
		ret = bt_driver_is_bonded(id, bt);
	}
	// logd("bt(%d) isdebond=%d\n",bt,ret);
	return ret;
}
bool api_bt_debond(uint8_t id,bt_t bt)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok) return ret;
	logd("bt(%d) debond bt=%d\n",id,bt);
	bt_ctbp->is_debonded = true;

	if(id == BT_ID0){
		ret = hal_bt_debond(id, bt);
	}else{
		ret = bt_driver_debond(id, bt);
	}
	
	return ret;
}
bool api_bt_disconnect(uint8_t id,bt_t bt)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok) return ret;

	api_bt_tx_fifo_fush(bt);				//使用fifo tx 清空fifo
	if(id == BT_ID0){
		ret = hal_bt_disconnect(id, bt);
	}else{
		ret = bt_driver_disconnect(id, bt);
	}
	
	return ret;
}

bool api_bt_enable(uint8_t id,bt_t bt,bool en)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;

	//BT_BLE_RF是基于BLE传输的, 如果同时支持这里对BT_BLE和BT_BLE_RF只处理一次
	#if (BIT(BT_BLE) | BIT(BT_BLE_RF)) == (BT0_SUPPORT & (BIT(BT_BLE) | BIT(BT_BLE_RF)))
	if((BT_BLE == bt) || (BT_BLE_RF == bt)){
		if(m_trps & (BIT(BT_BLE) | BIT(BT_BLE_RF))){
			if(0 == (m_trps & BIT(bt))){
				return true;		//忽略重复防止蓝牙被关闭
			}
		}
	}
	#endif
	#if (BIT(BT_BLEC) | BIT(BT_BLE_RFC)) == (BT0_SUPPORT & (BIT(BT_BLEC) | BIT(BT_BLE_RFC)))
	if((BT_BLEC == bt) || (BT_BLE_RFC == bt)){
		if(m_trps & (BIT(BT_BLEC) | BIT(BT_BLE_RFC))){
			if(0 == (m_trps & BIT(bt))){
				return true;		//忽略重复防止蓝牙被关闭
			}
		}
	}
	#endif
	
	logi("bt%d enable=%d\n", bt, en);	

	bt_ctbp->enable = en;
	if(!en) {
		api_bt_disconnect(id,bt);
		bt_ctbp->hid_ready = false;
		bt_ctbp->vendor_ready = false;
	}

	if(bt_ctbp->init_ok){
		if(id == BT_ID0){
			ret = hal_bt_enable(id, bt, en);
		}else{
			ret = bt_driver_enable(id, bt, en);
		}
	}
	
	return ret;
}


void api_bt_enable_all(bool en)
{
	uint8_t i;
	uint8_t bt_id;

	for(i=0; i<BT_ID_MAX; i++){
		for(bt_id = 0; bt_id < BT_MAX; bt_id++){
			if(NULL!= api_bt_get_ctb(bt_id)){
				api_bt_enable(i,bt_id, en);
			}
		}
	}
}



void api_bt_tx_fifo_fush(bt_t bt)
{
	#if BT_SUPPORT & BIT_ENUM(TR_RF)
	if(bt & BT_RF){
		app_fifo_flush(&m_rf.fifo_txp->fifo);	
	}
	#endif
	#if BT_SUPPORT & BIT_ENUM(TR_RFC)
	if(bt & BT_RFC){
		app_fifo_flush(&m_rfc.fifo_txp->fifo);	
	}
	#endif
}


bool api_bt_uart_fifo_tx(app_fifo_t * fifop, uint8_t *buf, uint16_t len)
{
	bool ret = false;
	uint16_t fifo_len = len;
	
	if(fifop->p_buf == NULL) return false;
	if(FIFO_EMPTY_LENGTH(fifop) < fifo_len){
		logd("fifo trp full %d %d %d!\n",fifop->buf_size_max,FIFO_EMPTY_LENGTH(fifop),fifo_len);
		ret = false;
	}else if(ERROR_SUCCESS == app_fifo_write(fifop, (uint8_t const *)buf, &fifo_len)){
		ret = true;
	}else{
		logi_r("fifo trp err!\n");
		ret = false;
	}

	return ret;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description: 根据不同设备选择fifo_tx 或者直接tx发送
*******************************************************************/
bool api_bt_uart_tx(uint8_t id, bt_t bt,uint8_t *buf, uint16_t len)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;

	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;

	if(!bt_ctbp->init_ok || !bt_ctbp->vendor_ready) return ret;
	if(NULL != bt_ctbp->fifo_txp){
		ret = api_bt_uart_fifo_tx(&bt_ctbp->fifo_txp->fifo,buf, len);
		if(ret) bt_ctbp->fifo_txp->tx_busy = true;
	}else{
		if(id == BT_ID0){
			ret = hal_bt_uart_tx(id, bt, buf, len);
		}else{
			ret = bt_driver_uart_tx(id, bt, buf, len);
		}
	}
	return ret;
}

/*******************************************************************
** Function: 	
** Parameters: 
** Returns:
*******************************************************************/
bool api_bt_hid_tx(uint8_t id, bt_t bt, uint8_t hid_requset, uint8_t*buf, uint16_t len)
{
	bool ret = false;
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return false;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok || !bt_ctbp->hid_ready || (BT_STA_CONN != bt_ctbp->sta)) return ret;

	if(id == BT_ID0){
		ret = hal_bt_hid_tx(id, bt, hid_requset, buf, len);
	}else{
		ret = bt_driver_hid_tx(id, bt, hid_requset, buf, len);
	}
	
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:			
** Description: 用于BT_EVT_TX 事件触发发送
*******************************************************************/
static void bt_tx_event_process(uint8_t id, bt_tx_fifo_t* txp, bt_evt_tx_t* pa)
{
	if(BT_UART & pa->bts){
		uint16_t fifo_len;

		if(txp->tx_len > txp->tx_mtu) txp->tx_len = 0;					//防止数据出错

		fifo_len = fifo_length(&txp->fifo);
		if(fifo_len || txp->tx_len){
			fifo_len = MIN(txp->tx_mtu - txp->tx_len, fifo_len);		//遗留数据和新数据要一起发送
			if(ERROR_SUCCESS == app_fifo_read(&txp->fifo, txp->tx_buf+txp->tx_len, &fifo_len)){
				txp->tx_len += fifo_len; 
			}
			
			if(id == BT_ID0){
				if(hal_bt_uart_tx(id, BT_RF,txp->tx_buf,txp->tx_len)){
					txp->tx_len = 0;
				}
			}else{
				if(bt_driver_uart_tx(id, BT_RF,txp->tx_buf,txp->tx_len)){
					txp->tx_len = 0;
				}
			}
		}
	}
}

#if WEAK_ENABLE
/*******************************************************************
** Parameters:		
** Returns:			
** Description: 蓝牙从机接收数据,用于在中断外处理蓝牙数据
*******************************************************************/
__WEAK error_t os_bt_rx(uint8_t id,bt_t bt, bt_evt_rx_t* pa)	
{
	// os post message 
	// return ERROR_SUCCESS;
	return ERROR_UNSUPPORT;
}

__WEAK void api_bt_rx(uint8_t id, bt_t bt, bt_evt_rx_t* pa)
{
    logd("weak bt%d rx:%d \n",bt,pa->len);    //dumpd(pa->buf,pa->len);

	api_bt_ctb_t* bt_ctbp;

	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;

	if(BT_HID & pa->bts){
		if(bt == BT_EDR){
			#if EDR_HID_SUPPORT && HID_GAMEPAD_MASK
			hid_type_t hid_type = app_gamepad_get_hidtype(bt_ctbp->hid_types);
			trp_handle_t handle = {bt,id,U16(DEV_TYPE_HID,hid_type)};
			app_gamepad_dev_process(&handle, pa->buf, pa->len);
			#elif APP_CMD_ENABLE
			trp_handle_t handle = {bt,id,U16(DEV_TYPE_HID, HID_TYPE_VENDOR)};
			app_command_rx(&handle,pa->buf+1, pa->len-1);	//丢弃 hid_report_type_t
			#endif
		}else{
			#if BLE_HID_SUPPORT &&  HID_GAMEPAD_MASK
			hid_type_t hid_type = app_gamepad_get_hidtype(bt_ctbp->hid_types);
			trp_handle_t handle = {bt,id,U16(DEV_TYPE_HID,hid_type)};
			app_gamepad_dev_process(&handle, pa->buf, pa->len);
			#elif APP_CMD_ENABLE
			trp_handle_t handle = {bt,id,U16(DEV_TYPE_HID, HID_TYPE_VENDOR)};
			app_command_rx(&handle,pa->buf, pa->len);
			#endif
		}
	}else if(BT_UART == pa->bts){					//uart
		#if APP_CMD_ENABLE
		uint8_t i;
		trp_handle_t handle = {bt,id,U16(DEV_TYPE_VENDOR, 0)};
		for(i=0; i<pa->len; i++){
			app_command_rx_byte(&handle, pa->buf[i]);
		}
		#endif
	}
}
#endif






static void bt_event(uint8_t id, bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	api_bt_ctb_t* bt_ctbp;

	if(id >= BT_ID_MAX) return;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;

	switch(event){
		case BT_EVT_INIT:			//蓝牙SDK自动广播
			if(0 == (m_trps & BIT(bt))){
				bt_ctbp->enable = 0;
			}
			bt_ctbp->init_ok = true;
			if(NULL != bt_ctbp->fifo_txp) bt_ctbp->fifo_txp->tx_busy = false;
	
			bt_ctbp->sta = BT_STA_IDLE;
			bt_ctbp->vendor_ready = false;
			bt_ctbp->hid_ready = false;
			bt_ctbp->is_debonded = false;
			api_bt_enable(id, bt,bt_ctbp->enable);
			if(bt_ctbp->enable){
				if(api_bt_is_bonded(id, bt) && !bt_ctbp->is_debonded){
					bt_ctbp->sta = BT_STA_DIR_ADV;
				}else{
					bt_ctbp->sta = BT_STA_ADV;
				}
			}else{
				bt_ctbp->sta = BT_STA_IDLE;
			}
			logd_g("bt(%d) init ok en=%d sta=%d...\n",bt, bt_ctbp->enable,bt_ctbp->sta);
			break;
		case BT_EVT_CONNECTED:
			logd_g("bt(%d) connect...\n",bt);
			bt_ctbp->sta = BT_STA_CONN;
			bt_ctbp->is_debonded = false;

			#if APP_GAMEAPD_ENABLE
			if(BIT(DEV_TYPE_HID) & bt_ctbp->types){
				hid_type_t hid_type = app_gamepad_get_hidtype(bt_ctbp->hid_types);
				trp_handle_t handle = {bt,id,U16(DEV_TYPE_HID,hid_type)};
				app_gamepad_init( &handle ) ;
			}
			#endif

			break;		
		case BT_EVT_READY:
			if(BT_UART == pa->ready.bts){
				bt_ctbp->vendor_ready = pa->ready.ready;
				logd("bt(%d) vendor ready=%d...\n",bt,pa->ready.ready);
			}else{
				bt_ctbp->hid_ready = pa->ready.ready;
				logd("bt(%d) hid ready=%d...\n",bt,pa->ready.ready);
			}
			break;		
		case BT_EVT_IDLE:
			bt_ctbp->sta = BT_STA_IDLE;
			break;			
		case BT_EVT_ADV:
			if(api_bt_is_bonded(id, bt) && !bt_ctbp->is_debonded){
				bt_ctbp->sta = BT_STA_DIR_ADV;
			}else{
				bt_ctbp->sta = BT_STA_ADV;
			}
			break;
		case BT_EVT_ADV_DIR:
			bt_ctbp->sta = BT_STA_DIR_ADV;
			break;
		case BT_EVT_DISCONNECTED:
		case BT_EVT_CONNECT_FAIL:
			logd("bt(%d) disconnected...\n",bt);
			bt_ctbp->vendor_ready = false;
			bt_ctbp->hid_ready = false;
			api_bt_enable(id, bt,bt_ctbp->enable);
			if(bt_ctbp->enable){
				if(api_bt_is_bonded(id, bt) && !bt_ctbp->is_debonded){
					bt_ctbp->sta = BT_STA_DIR_ADV;
				}else{
					bt_ctbp->sta = BT_STA_ADV;
				}
			}else{
				bt_ctbp->sta = BT_STA_IDLE;
			}
			break;
		case BT_EVT_RX:
			if(ERROR_UNSUPPORT == os_bt_rx(id, bt, (bt_evt_rx_t*)pa)){
				api_bt_rx(id, bt, (bt_evt_rx_t*)pa);
			}
			break;
		case BT_EVT_TX:
			if((NULL != bt_ctbp->fifo_txp) && (NULL != pa)){
				if(0 == pa->tx.ret){
					bt_ctbp->fifo_txp->tx_busy = false;
				}
				bt_tx_event_process(id, bt_ctbp->fifo_txp,(bt_evt_tx_t*)pa);
			}
			break;
		default:
			break;
	}
}


static void btc_event(uint8_t id, bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	api_bt_ctb_t* bt_ctbp;
	bt_evt_scan_t* bt_scan_resultp;

	if(id >= BT_ID_MAX) return;
	bt_ctbp = api_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;

	bt_scan_resultp = api_bt_get_scan_result(bt);

	switch(event){
		case BT_EVT_INIT:
			if(0 == (m_trps & BIT(bt))){
				bt_ctbp->enable = 0;
			}
			bt_ctbp->init_ok = true;
			bt_ctbp->sta = BT_STA_IDLE;
			bt_ctbp->vendor_ready = false;
			bt_ctbp->hid_ready = false;
			api_bt_enable(id, bt,bt_ctbp->enable);
			if(api_bt_is_bonded(id, bt)){					//TODO
				bt_ctbp->sta = BT_STA_DIR_ADV;
			}else{
				bt_ctbp->sta = BT_STA_ADV;
			}
			logd_g("bt(%d) init ok en=%d sta=%d...\n",bt, bt_ctbp->enable,bt_ctbp->sta);
			break;
		case BT_EVT_CONNECTED:
			logd_g("btc(%d) connect...\n",bt);
			bt_ctbp->sta = BT_STA_CONN;
		
			break;		
		case BT_EVT_READY:
			if(NULL != pa){
				if(BT_UART == pa->ready.bts){
					bt_ctbp->vendor_ready = pa->ready.ready;
					logd("btc(%d) vendor ready=%d...\n",bt,pa->ready.ready);
				}else{
					bt_ctbp->hid_ready = pa->ready.ready;
					logd("btc(%d) hid ready=%d...\n",bt,pa->ready.ready);
				}
			}
			break;
		case BT_EVT_IDLE:
			bt_ctbp->sta = BT_STA_IDLE;
			break;
		case BT_EVT_SCAN:
			if(api_bt_is_bonded(id, bt)){
				bt_ctbp->sta = BT_STA_DIR_SCAN;
			}else{
				bt_ctbp->sta = BT_STA_SCAN;
				/*
				1. 底层默认开启扫描地址,名字的广播,筛选由此处理
				2. 当有绑定信息,底层会根据配对地址直接匹配,否则扫面的是名字.
				*/
				if(pa != NULL){
					bt_evt_scan_t *scanp = (bt_evt_scan_t*)pa;
					
					#if defined BTC_SEARCH_MAC
					if(!memcmp(pa->scan.mac, BTC_SEARCH_MAC, 6)
						#ifdef BTC_SEARCH_RSSI
						&& (pa->scan.rssi >= BTC_SEARCH_RSSI)
						#endif
					){
						if(NULL != bt_scan_resultp) *bt_scan_resultp = *scanp;
						logd("btc(%d) match rissi=%d, name=%s",bt,pa->scan.rssi,pa->scan.name);
					}
					#elif defined BTC_SEARCH_NAME
					if(strlen(scanp->name)){
						logi("btc scan name: %s,mac:", scanp->name);dumpd(scanp->mac,6);
						if(!memcmp(pa->scan.name, BTC_SEARCH_NAME, strlen(BTC_SEARCH_NAME))
							#ifdef BTC_SEARCH_RSSI
							&& (pa->scan.rssi >= BTC_SEARCH_RSSI)
							#endif
						){
							if(NULL != bt_scan_resultp) *bt_scan_resultp = *scanp;
							logd("btc(%d) match rissi=%d, name=%s",bt,pa->scan.rssi,pa->scan.name);
						}
					}
					#endif
				}else if(NULL != bt_scan_resultp){
					memset(bt_scan_resultp,0,sizeof(bt_evt_scan_t));
				}
			}
			break;
		case BT_EVT_SCAN_DIR:
			logd("BT_EVT_SCAN_DIR");
			bt_ctbp->sta = BT_STA_DIR_ADV;
			break;
		case BT_EVT_DISCONNECTED:
		case BT_EVT_CONNECT_FAIL:
			logd("btc(%d) disconnected...\n",bt);
			api_bt_enable(id, bt,bt_ctbp->enable);
			bt_ctbp->vendor_ready = false;
			bt_ctbp->hid_ready = false;
			if(bt_ctbp->enable){
				if(api_bt_is_bonded(id, bt)){
					bt_ctbp->sta = BT_STA_DIR_SCAN;
				}else{
					bt_ctbp->sta = BT_STA_SCAN;
				}
			}else{
				bt_ctbp->sta = BT_STA_IDLE;
			}
			break;
		case BT_EVT_RX:
			if(ERROR_UNSUPPORT == os_bt_rx(id, bt, (bt_evt_rx_t*)pa)){
				api_bt_rx(id, bt, (bt_evt_rx_t*)pa);
			}
			break;
		case BT_EVT_TX:
			if((NULL != bt_ctbp->fifo_txp) && (NULL != pa)){
				if(0 == pa->tx.ret){
					bt_ctbp->fifo_txp->tx_busy = false;
				}
				bt_tx_event_process(id, bt_ctbp->fifo_txp, (bt_evt_tx_t*)pa);
			}
			break;
		default:
			break;
	}
}

#if WEAK_ENABLE
/*******************************************************************
** Parameters:		
** Returns:		true: 用户自定义处理, false: 会走通用处理	
** Description: 蓝牙事件用户处理
*******************************************************************/
__WEAK bool api_bt_event_weak(uint8_t id,bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	return false;
}
#endif

void api_bt_event(uint8_t id, bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	if(api_bt_event_weak(id, bt, event, pa)){				//用于自定义处理
		return;
	}

	switch(bt){
		case BT_BLE:
		case TR_BLE_RF:
		case BT_EDR:
		case BT_RF:
			bt_event(id, bt, event, pa);
			break;
		case BT_BLEC:
		case TR_BLE_RFC:
		case BT_EDRC:
		case BT_RFC:
			btc_event(id, bt, event, pa);
			break;
		default:
			break;
	}
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_bt_ctb_init(void)
{
	uint8_t bt;
	api_bt_ctb_t* bt_ctbp;

	for(bt = 0; bt < BT_MAX; bt++){
		bt_ctbp = api_bt_get_ctb(bt);
		if(NULL != bt_ctbp){
			bt_ctbp->init_ok = false;
			bt_ctbp->vendor_ready = false;
			bt_ctbp->hid_ready = false;
			bt_ctbp->is_debonded = false;
			bt_ctbp->bond_index = 0;
			bt_ctbp->bt_idx = 0;

			bt_ctbp->inteval_10us = 1500;
			bt_ctbp->sta = BT_STA_UNKNOW;
			bt_ctbp->fifo_txp = NULL;
			logd("bt%d type=%x %x", bt, bt_ctbp->types, bt_ctbp->hid_types);
			
			if(BT_EDR == bt){
				#if BT_SUPPORT & BIT_ENUM(TR_EDR)
				if(bt_ctbp->hid_types & BIT(HID_TYPE_SWITCH)){		//switch sniff by remote
					edr_sniff_by_remote = true;
				}else{
					edr_sniff_by_remote = false;
				}
				logd("edr_sniff_by_remote=%d",edr_sniff_by_remote);
				#endif
			}

			#if BT_SUPPORT & BIT_ENUM(TR_RF)
			if(BT_RF == bt){
				bt_ctbp->fifo_txp = &app_rf_tx;
				bt_tx_fifo_init(&app_rf_tx,rf_tx_buf,RF_TX_LL_MTU,rf_tx_fifo_buf,RF_FIFO_LEN);
			}
			#endif
			#if BT_SUPPORT & BIT_ENUM(TR_RFC)
			if(BT_RFC == bt){
				bt_ctbp->fifo_txp = &app_rfc_tx;
				bt_tx_fifo_init(&app_rfc_tx,rfc_tx_buf,RFC_TX_LL_MTU,rfc_tx_fifo_buf,RF_FIFO_LEN);
			}
			#endif
		}
	}
	
	return true;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	必须先调用蓝牙ctb参数初始化	api_bt_ctb_init();
*******************************************************************/
bool api_bt_init(void)
{
	uint8_t i;
	bt_t bt;

	/* 蓝牙初始化前发送BT_EVT_UNKNOW事件,可用于蓝牙参数初始化 */
	for(bt=0; bt<BT_MAX; bt++){
        if(BT0_SUPPORT & BIT(bt)){      //全部模式
            api_bt_event(BT_ID0, bt, BT_EVT_UNKNOW, NULL);
        }
    }
	for(i=0; i<BT_ID_MAX; i++){
		if(i == BT_ID0)hal_bt_init(BT_ID0);
		else bt_driver_init(i);
	}

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_bt_deinit(void)
{
	uint8_t i;

	for(i=0; i<BT_ID_MAX; i++){
		if(i == BT_ID0)hal_bt_deinit(BT_ID0);
		else bt_driver_deinit(i);
	}
	api_bt_ctb_init();

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_bt_task(void* pa)
{
	hal_bt_task(pa);
	bt_driver_task(pa);
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_bt_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_bt_task(NULL);
	}
}
#endif

#endif






