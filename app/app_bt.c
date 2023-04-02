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
#if APP_BT_ENABLE
#include  "app_bt.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
bt_t app_bt_trs;				//bt_t

#if BLE_ENABLE					//ble peripheral
app_bt_ctb_t m_ble;
#endif

#if BLEC_ENABLE					//ble central
app_bt_ctb_t m_blec;
bt_evt_scan_t blec_scan_result;
#endif

#if BLE_RF_ENABLE
app_bt_ctb_t m_ble_rf;
#endif

#if BLEC_RF_ENABLE					//ble central
app_bt_ctb_t m_blec_rf;
#endif

#if EDR_ENABLE					//edr peripheral
app_bt_ctb_t m_edr;
#endif

#if EDRC_ENABLE					//edr central
app_bt_ctb_t m_edrc;
bt_evt_scan_t edrc_scan_result;
#endif

#if RF_ENABLE
app_bt_ctb_t m_rf;
static bt_tx_fifo_t app_rf_tx;
#endif

#if RFC_ENABLE
app_bt_ctb_t m_rfc;
static bt_tx_fifo_t app_rfc_tx;
#endif


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
bool bt_tx_fifo_init(bt_tx_fifo_t* txp, uint16_t mtu,uint16_t fifo_len)
{
	uint8_t *fifo_buf, *tx_buf;

	fifo_buf = (uint8_t*)emf_malloc(fifo_len);
	tx_buf = (uint8_t*)emf_malloc(mtu);
	if((NULL == fifo_buf) || (NULL == tx_buf)){
		emf_free((void*)fifo_buf);
		emf_free((void*)tx_buf);
		return false;
	}

	memset(txp,0,sizeof(bt_tx_fifo_t));
	app_fifo_init(&txp->fifo, fifo_buf, sizeof(fifo_len));
	txp->tx_mtu = mtu;
	txp->tx_buf = tx_buf;

	return true;
}
bool bt_tx_fifo_deinit(bt_tx_fifo_t* txp, uint16_t mtu,uint16_t fifo_len)
{
	emf_free(txp->tx_buf);
	emf_free(txp->fifo.p_buf);
	memset(&txp,0,sizeof(bt_tx_fifo_t));
	return true;
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		输入bt_t 输出对应数据
*******************************************************************/
bt_evt_scan_t* app_bt_get_scan_result(bt_t bt)
{
	bt_evt_scan_t* resultp = NULL;
	bool ret = false;

	switch(bt){
		#if BLEC_ENABLE
		case BT_BLEC:
			resultp = &blec_scan_result;
			break;
		#endif
		#if EDRC_ENABLE
		case BT_EDRC:
			resultp = &edrc_scan_result;
			break;
		#endif
		#if RFC_ENABLE
		case BT_RFC:
			break;
		#endif
		default:
			break;
	}
	return resultp;
}

app_bt_ctb_t* app_bt_get_ctb(bt_t bt)
{
	bool ret = false;
	app_bt_ctb_t* app_btp = NULL;
	switch(bt){
		#if BLE_ENABLE
		case BT_BLE:
			app_btp = &m_ble;
			break;
		#endif
		#if BLEC_ENABLE
		case BT_BLEC:
			app_btp = &m_blec;
			break;
		#endif
		#if BLE_RF_ENABLE
		case BT_BLE:
			app_btp = &m_ble_rf;
			break;
		#endif
		#if BLEC_RF_ENABLE
		case BT_BLEC:
			app_btp = &m_blec_rf;
			break;
		#endif
		#if EDR_ENABLE
		case BT_EDR:
			app_btp = &m_edr;
			break;
		#endif
		#if EDRC_ENABLE
		case BT_EDRC:
			app_btp = &m_edrc;
			break;
		#endif
		#if RF_ENABLE
		case BT_RF:
			app_btp = &m_rf;
			break;
		#endif
		#if RFC_ENABLE
		case BT_RFC:
			app_btp = &m_rfc;
			break;
		#endif
		default:
			break;
	}

	return app_btp;
}
/*******************************************************************
** Parameters: base_mac	:输入基地址
** Returns:		base_mac:输出计算后的地址
** Description:	蓝牙MAC地址, 使用大端地址
 * add[3~5]为高厂商地址,add[0~2]为低产品地址(NRF工具显示的地址前面是高位,后面是低位)
 * add[0] 用于产品后面随机数,经典蓝牙地址是ble地址(ADD[0]+1)
 * add[1] 用于产品升级时候进入boot蓝牙地址+1
 * add[2] 用于产品不同模式下不同mac地址
 * add[3~5]一般不建议修改
*******************************************************************/
bool app_bt_get_mac(bt_t bt, uint8_t *buf )		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
{
	bool ret = false;
	uint8_t base_mac[6];
	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);

	if(NULL == bt_ctbp) return false;
	if(!api_bt_get_mac(bt,base_mac)) return false;	//get base_mac

 	#if (BT_TYPE_SUPPORT & BIT(TYPE_HID))					//优化代码,不支持HID不编译
	if(bt_ctbp->types & BIT(TYPE_HID)){
		switch(bt_ctbp->hid_types){
			case BIT_ENUM(HID_TYPE_GAMEPADE):
				base_mac[2] += HID_TYPE_GAMEPADE;
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

	return ret;
}

/*******************************************************************
** Parameters:	len:buf 长度,
** Returns:		名称字符长度	
** Description:	获取蓝牙名称,蓝牙名称会根据模式和蓝牙地址不而不同
*******************************************************************/
uint8_t app_bt_get_name(bt_t bt, char *buf, uint8_t len )
{
	bool ret = false;
	uint8_t name[30] = {0};
    uint8_t bt_addr[6];
    char chr[3];
	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return false;

	memset(buf,0,len);
    memset(name,0,sizeof(name));

	#if (BT_TYPE_SUPPORT & BIT_ENUM(TYPE_HID))
	if(bt_ctbp->types & BIT_ENUM(TYPE_HID)){
		switch(bt_ctbp->hid_types){
			#if (BT_HID_SUPPORT & HID_SWITCH_MASK)
			case BIT_ENUM(HID_TYPE_SWITCH):
				memcpy(name,"Pro Controller",sizeof("Pro Controller"));
				ret = true;
				break;
			#endif
			#if (BT_HID_SUPPORT & HID_PS_MASK)
			case BIT_ENUM(HID_TYPE_PS3)	:
			case BIT_ENUM(HID_TYPE_PS4)	:
			case BIT_ENUM(HID_TYPE_PS5)	:
				memcpy(name,"Wireless Controller",sizeof("Wireless Controller"));
				ret = true;
				break;
			#endif
			#if (BT_HID_SUPPORT & HID_XBOX_MASK)
			case BIT_ENUM(HID_TYPE_X360)	:
			case BIT_ENUM(HID_TYPE_XBOX)	:
				memcpy(name,"Xbox Wireless Controller",sizeof("Xbox Wireless Controller"));
				ret = true;
				break;
			#endif
			default :
				break;
		}
	}
	#endif

	if(!ret){
		memcpy(name,DEFAULT_NAME,sizeof(DEFAULT_NAME));
		
		#if BT_RANDOM_NAME_ENABLE
		app_bt_get_mac(bt,bt_addr);
		sprintf(chr, "%02x", bt_addr[0]);
		memcpy(name+strlen(name),chr,3);
		#endif
	}
	
    len = MIN(len-1,(uint8_t)strlen(name));
    memcpy(buf, name, len);
	return len;
}


void app_bt_tx_fifo_fush(bt_t bt)
{
	#if RF_ENABLE
	if(bt & BT_RF){
		app_fifo_flush(&m_rf.fifo_txp->fifo);	
	}
	#endif
	#if RFC_ENABLE
	if(bt & BT_RFC){
		app_fifo_flush(&m_rfc.fifo_txp->fifo);	
	}
	#endif
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description: 根据不同设备选择fifo_tx 或者直接tx发送
*******************************************************************/
bool app_bt_uart_tx(bt_t bt,uint8_t *buf, uint16_t len)
{
	bool ret = false;
	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok || (BT_STA_READY !=  bt_ctbp->sta)) return ret;
	if(NULL != bt_ctbp->fifo_txp){
		ret = api_bt_uart_fifo_tx(bt, &bt_ctbp->fifo_txp->fifo,buf, len);
	}else{
		ret = api_bt_uart_tx(bt, buf, len);
	}
	return ret;
}

/*******************************************************************
** Function: 	
** Parameters: id:高8bit为参数:, 低8bit为:id //TODO
** Returns:
*******************************************************************/
bool app_bt_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len)
{
	bool ret = false;
	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return ret;
	if(!bt_ctbp->init_ok || (BT_STA_READY !=  bt_ctbp->sta)) return ret;
	ret = api_bt_hid_tx(bt, id, buf, len);
	
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:			
** Description: 用于BT_EVT_TX 事件触发发送
*******************************************************************/
static void bt_tx_event_process(bt_tx_fifo_t* txp, bt_evt_tx_t* pa)
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
			
			if(api_bt_uart_tx(BT_RF,txp->tx_buf,txp->tx_len)){
				txp->tx_len = 0;
			}
		}
	}
}


/*******************************************************************
** Parameters:		
** Returns:			
** Description: 蓝牙从机接收数据,用于在中断外处理蓝牙数据
*******************************************************************/
__WEAK error_t os_bt_rx(bt_t bt, bt_evt_rx_t* pa)	
{
	return ERROR_UNSUPPORT;
}

__WEAK void app_bt_rx(bt_t bt, bt_evt_rx_t* pa)
{
    logd("weak bt(%d) rx:%d:",bt,pa->len);    //dumpd(pa->buf,pa->len);

	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;
	if(BT_UART == pa->bts){			//uart
		// if(api_get_command(bt,buffer, length)){
		//     device_decode(bt,m_ble_cmd.rx_buf,m_ble_cmd.rx_len);
		// }
	}else{							//hid and other
		
	}
}




static void bt_event(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	app_bt_ctb_t* bt_ctbp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;

	switch(event){
		case BT_EVT_INIT:			//蓝牙SDK自动广播
			bt_ctbp->init_ok = true;
			api_bt_stack_init_ok(bt);
			if(NULL != bt_ctbp->fifo_txp) bt_ctbp->fifo_txp->tx_busy = false;
	
			bt_ctbp->sta = BT_STA_IDLE;
			api_bt_enable(bt,bt_ctbp->enable);
			if(api_bt_is_bonded(bt)){					//TODO
				bt_ctbp->sta = BT_STA_DIR_ADV;
			}else{
				bt_ctbp->sta = BT_STA_ADV;
			}
			break;
		case BT_EVT_CONNECTED:
			logd("bt(%d) connect...\n",bt);
			if(BT_STA_READY != bt_ctbp->sta){	//防止被改回去
				bt_ctbp->sta = BT_STA_CONN;
			}
			break;		
		case BT_EVT_READY:
			bt_ctbp->sta = BT_STA_READY;
			logd("bt(%d) ready...\n",bt);
			break;		
		case BT_EVT_IDLE:
			bt_ctbp->sta = BT_STA_IDLE;
			break;			
		case BT_EVT_ADV:
			bt_ctbp->sta = BT_STA_ADV;
			break;
		case BT_EVT_ADV_DIR:
			bt_ctbp->sta = BT_STA_DIR_ADV;
			break;
		case BT_EVT_DISCONNECTED:
			logd("bt(%d) disconnected...\n",bt);
			api_bt_enable(bt,bt_ctbp->enable);
			if(bt_ctbp->enable){
				if(api_bt_is_bonded(bt)){
					bt_ctbp->sta = BT_STA_DIR_ADV;
				}else{
					bt_ctbp->sta = BT_STA_ADV;
				}
			}else{
				bt_ctbp->sta = BT_STA_IDLE;
			}
			break;
		case BT_EVT_RX:
			if(ERROR_UNSUPPORT == os_bt_rx(bt, (bt_evt_rx_t*)pa)){
				app_bt_rx(bt, (bt_evt_rx_t*)pa);
			}
			break;
		case BT_EVT_TX:
			if(NULL != bt_ctbp->fifo_txp){
				bt_tx_event_process(bt_ctbp->fifo_txp,(bt_evt_tx_t*)pa);
			}
			break;
		case BT_EVT_TX_COMPLETE:
			if(NULL != bt_ctbp->fifo_txp) bt_ctbp->fifo_txp->tx_busy = false;
			break;
		default:
			break;
	}
}


static void btc_event(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	app_bt_ctb_t* bt_ctbp;
	bt_evt_scan_t* bt_scan_resultp;

	bt_ctbp = app_bt_get_ctb(bt);
	if(NULL == bt_ctbp) return;

	bt_scan_resultp = app_bt_get_scan_result(bt);

	switch(event){
		case BT_EVT_INIT:
			bt_ctbp->init_ok = true;
			api_bt_stack_init_ok(bt);

			bt_ctbp->sta = BT_STA_IDLE;
			api_bt_enable(bt,bt_ctbp->enable);
			if(api_bt_is_bonded(bt)){					//TODO
				bt_ctbp->sta = BT_STA_DIR_ADV;
			}else{
				bt_ctbp->sta = BT_STA_ADV;
			}
			break;
		case BT_EVT_CONNECTED:
			logd("btc(%d) connect...\n",bt);
			if(BT_STA_READY != bt_ctbp->sta){		//防止被改回去
				bt_ctbp->sta = BT_STA_CONN;
			}
			break;		
		case BT_EVT_READY:
			bt_ctbp->sta = BT_STA_READY;
			logd("btc(%d) ready...\n",bt);
			break;		
		case BT_EVT_IDLE:
			bt_ctbp->sta = BT_STA_IDLE;
			break;
		case BT_EVT_SCAN:
			bt_ctbp->sta = BT_STA_SCAN;
			#ifdef BTC_SEARCH_NAME
			/*
			1. 底层默认开启扫描地址,名字的广播,筛选由此处理
			2. 当有绑定信息,使用的是扫描地址,否则扫面的是名字.
			*/
			if(pa != NULL){
				logi("btc scan name: %s\n", pscanning->name);
				if(!memcmp(pa->scan.name, BTC_SEARCH_NAME, strlen(BTC_SEARCH_NAME))
					#ifdef BTC_SEARCH_RSSI
					&& (pa->scan.rssi >= BTC_SEARCH_RSSI)
					#endif
				){
					*bt_scan_resultp = &(pa->scan);
					logd("btc(%d) match rissi=%d, name=%s",bt,pa->scan.rssi,pa->scan.name);
				}
			}else{
				memset(bt_scan_resultp,0,sizeof(bt_evt_scan_t));
			}
            #endif
			break;
		case BT_EVT_SCAN_DIR:
			bt_ctbp->sta = BT_STA_DIR_ADV;
			break;
		case BT_EVT_DISCONNECTED:
			logd("btc(%d) disconnected...\n",bt);
			api_bt_enable(bt,bt_ctbp->enable);

			if(bt_ctbp->enable){
				if(api_bt_is_bonded(bt)){
					bt_ctbp->sta = BT_STA_DIR_SCAN;
				}else{
					bt_ctbp->sta = BT_STA_SCAN;
				}
			}else{
				bt_ctbp->sta = BT_STA_IDLE;
			}
			break;
		case BT_EVT_RX:
			if(ERROR_UNSUPPORT == os_bt_rx(bt, (bt_evt_rx_t*)pa)){
				app_bt_rx(bt, (bt_evt_rx_t*)pa);
			}
			break;
		case BT_EVT_TX:
			if(NULL != bt_ctbp->fifo_txp){
				bt_tx_event_process(bt_ctbp->fifo_txp,(bt_evt_tx_t*)pa);
			}
			break;
		case BT_EVT_TX_COMPLETE:
			if(NULL != bt_ctbp->fifo_txp) bt_ctbp->fifo_txp->tx_busy = false;
			break;
		default:
			break;
	}
}


/*******************************************************************
** Parameters:		
** Returns:		true: 用户自定义处理, false: 会走通用处理	
** Description: 蓝牙事件用户处理
*******************************************************************/
__WEAK bool app_bt_event_weak(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	return false;
}


void app_bt_event(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa)
{
	
	if(app_bt_event_weak(bt, event, pa)){				//用于自定义处理
		return;
	}

	if((BT_BLE | BT_EDR) & bt){
		bt_event(bt, event, pa);
	}else if((BT_BLEC | BT_EDRC) & bt){
		btc_event(bt, event, pa);
	}else{
	}
}

static bool app_bt_ctb_init(bt_t bt)
{
	uint8_t i;
	bt_t bt_id;
	app_bt_ctb_t* bt_ctbp;

	for(i = 0; i < BT_TR_NUM; i++){
		bt_id = BIT(i);
		if(bt_id & bt){
			bt_ctbp = app_bt_get_ctb(bt_id);
			if(NULL != bt_ctbp){
				memset(bt_ctbp,0,sizeof(app_bt_ctb_t));
				bt_ctbp->enable = true;
				bt_ctbp->inteval = 12;

				#if RF_ENABLE
				if(BT_RF == bt_id){
					bt_ctbp->fifo_txp = &app_rf_tx;
					bt_tx_fifo_init(&app_rf_tx,RF_TX_LL_MTU,RF_FIFO_LEN);
				}
				#endif
				#if RFC_ENABLE
				if(BT_RFC == bt_id){
					bt_ctbp->fifo_txp = &app_rfc_tx;
					bt_tx_fifo_init(&app_rfc_tx,RF_TX_LL_MTU,RF_FIFO_LEN);
				}
				#endif
			}
		}
	}
	
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_bt_init(void)				//TODO
{
	app_bt_ctb_init(BT_TRS_MASK);
	api_bt_init();
	
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_bt_deinit(void)
{
	bool ret = false;
	
	ret = api_bt_deinit();
	app_bt_ctb_init(BT_TRS_MASK);

	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_bt_handler(void)
{
	api_bt_handler();
}

#endif






