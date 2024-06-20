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

#ifndef	_api_bt_h
#define	_api_bt_h
#include "hw_config.h"
#include "bt_typedef.h"
#include "api/hid/hid_dev_desc.h"
#include "api/bt/bt_driver.h"
#include "api/api_transport.h"
#include "hal_bt.h"
#include "utils/emf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef BLE_VID
#define BLE_VID			VID_DEFAULT
#endif
#ifndef EDR_VID
#define EDR_VID			VID_DEFAULT
#endif
#ifndef BLE_PID
#define BLE_PID			(BLE_TYPE_SUPPORT | BLE_HID_SUPPORT)
#endif
#ifndef EDR_PID
#define EDR_PID			(EDR_TYPE_SUPPORT | EDR_HID_SUPPORT)
#endif
#ifndef BLE_ICON
#define BLE_ICON		BLE_APPEARANCE_HID_KEYBOARD
#endif
#ifndef EDR_ICON
#define EDR_ICON		BD_CLASS_KEYBOARD
#endif
#ifndef BLE_MANUFACTURER_COMPANY
#define BLE_MANUFACTURER_COMPANY			0xFFFE
#endif


#ifndef EDR_SNIFF_ENABLE
#define EDR_SNIFF_ENABLE			1
#endif
#ifndef BT_ADV_INTERVAL							//(unit:0.625ms)
#define BT_ADV_INTERVAL             (160) 		
#endif
#ifndef BLE_MULT_HID_SERVICES					//如果pc打开多服务,但是不初始化report map 会有感叹号
#define BLE_MULT_HID_SERVICES		1
#endif

#ifndef BT_RANDOM_NAME_ENABLE
#define BT_RANDOM_NAME_ENABLE		1
#endif
#ifndef BT_MODIFY_NAME_ENABLE
#define BT_MODIFY_NAME_ENABLE		0
#endif


#ifndef BLE_RANDOM_MAC_ENABLE					//BLE 使用随机地址, 可以解决ble解绑问题
#define BLE_RANDOM_MAC_ENABLE		0
#endif



// demo search defined
// #define BTC_SEARCH_NAME						"JL6321"
// #define BTC_SEARCH_RSSI						-50
// #define BTC_SEARCH_MAC						"\x01\x02\x03\x04\x05\x06"

// #define BTC_SEARCH_UUID16			    	0xae30
// #define BTC_SEARCH_WRITE_CHARA_UUID16		0xae01
// #define BTC_SEARCH_NOTIFY_CHARA_UUID16		0xae01

/*******************************************************************
** Description: 2.4g数据发送说明,2.4G没有more data不能按照蓝牙的方式发送数据, 
2.4G有两种发送数据方式:
	1.直接调用 api_bt_uart_tx 函数发送: 这种方式可以发送随机的指令或者非高频的数据,
		如果发送高频数据太快超过2.4g连接间隔要有fifo堆满和延时的风险(2.4g丢包或者多包)
	2. RF_EVT_TX 事件中发送数据, 为了优化发送不阻塞和冗余,实时数据(比如按键)最好在 RF_EVT_TX 事件中处理,
RF_EVT_TX 事件接收后可以根据fifo长度来判断是否填充按键数据
*******************************************************************/

/*******************************************************************
 * RF_TX_LL_MTU / RFC_TX_LL_MTU: rf链路层MTU, 长度越大越容易被干扰
 * 如果 发送单包数据长度>RF_TX_LL_MTU,底层就需要分包发送,占用更多缓存	
 * 如果 发送单包数据长度<RF_TX_LL_MTU 就不需要底层分包	
*******************************************************************/
#ifndef RF_TX_LL_MTU
#define RF_TX_LL_MTU		32
#endif
#ifndef RFC_TX_LL_MTU
#define RFC_TX_LL_MTU		32
#endif


/*****************************************************************************************************
** 蓝牙空中包数据格式:
	Preamble 	Access_Address			PDU				CRC
		|			|			Header	Payload	MIC		|
		|			|		  		2	251	  	 4		|
		1			4					257				3

* PDU_Payload	range(27~251)
* PDU_Payload =  API_BT_LL_MTU + 3 + 4	 	Link layout 底层MTU
* PDU_Payload  = API_BT_ATT_MTU + 3 + 4	ATT层MTU
******************************************************************************************************/

/*******************************************************************
 ** API_BT_LL_MTU
 * API_BT_LL_MTU 不一定要等于APP_BT_ATT_MTU,只有需要速度时候才需要增加
 * 如果 蓝牙发送单包数据长度>API_BT_LL_MTU,底层就需要分包发送(分包间隔150us+其他包消耗)	
 * 如果 蓝牙发送单包数据长度<API_BT_LL_MTU 就不需要底层分包
 * 范围 range(20~244) ,要求速度才需要开这个 API_BT_LL_MTU feature，开了会多消耗RAM		
*******************************************************************/
#ifndef API_BT_LL_MTU		// Link layout MTU	
#define API_BT_LL_MTU		20
#endif

/*******************************************************************
 **	API_BT_ATT_MTU
 *	API_BT_ATT_MTU 决定了一包数据最大能发送多少字节数据,ATT MUT影响了整个ATT通讯MTU,超过的只能moredata或者下一个连接发送
 *	范围 range(20~244)
*******************************************************************/
#ifndef API_BT_ATT_MTU		//hid数据通道的MTU一般不超过32字节
	#if BLE_CMD_MTU > 61
	#define API_BT_ATT_MTU   (BLE_CMD_MTU)
	#else					//不清楚为什么超过61会导致ios13.4鼠标描述符无作用
	#define API_BT_ATT_MTU   (61)					
	#endif
#endif



#ifndef RF_FIFO_LEN		
#define RF_FIFO_LEN		(0x100)
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	BT_ID0 = 0,				//默认内置蓝牙模块	
	BT_ID1 = 1,				//默认外部蓝牙模块
	BT_ID_MAX
}bt_id_t;
typedef enum {
	BT_BLE 		= DEF_TR_BLE,		
	BT_EDR 		= DEF_TR_EDR,		
	BT_BLEC 	= DEF_TR_BLEC,		
	BT_EDRC 	= DEF_TR_EDRC,		
	BT_BLE_RF 	= DEF_TR_BLE_RF,					//BLE模拟2.4G
	BT_BLE_RFC 	= DEF_TR_BLE_RFC,	
	BT_RF 		= DEF_TR_RF,			
	BT_RFC 		= DEF_TR_RFC,	
	BT_MAX 		= 8,
} bt_t;

typedef enum {
	BT_SERVER_NULL = 0,
	BT_UART = BIT(0), 
	BT_HID = BIT(1),		
	BT_HID_DESC = BIT(2),		
} bt_server_t;



//bt state machine
typedef enum  {
	BT_STA_UNKNOW	=	0,
	BT_STA_IDLE,
	BT_STA_ADV,
	BT_STA_DIR_ADV,
	BT_STA_SCAN		=	BT_STA_ADV,
	BT_STA_DIR_SCAN	=	BT_STA_DIR_ADV,
	BT_STA_CONN,
} bt_sta_t;


typedef enum {
	BT_EVT_UNKNOW=0,							//上电事件
    BT_EVT_INIT, 								//init (pa)
	BT_EVT_IDLE,								//idle
	
	BT_EVT_ADV,									//广播中(edr进入通用/限制可发现状态)
    BT_EVT_ADV_DIR,                     		//广播(edr 处于定向回连)
	BT_EVT_ADV_TIMOUT,							//广播超时
	BT_EVT_SCAN = BT_EVT_ADV,					//扫描	(pa)
	BT_EVT_SCAN_DIR = BT_EVT_ADV_DIR,			//定向扫描
	BT_EVT_SCAN_TIMEOUT = BT_EVT_ADV_TIMOUT,	//扫描超时

	BT_EVT_CONNECTED,							//连接成功	(pa)
	BT_EVT_DISCONNECTED,						//断开连接	(pa)
	BT_EVT_CONNECT_FAIL,						//(回连)连接失败	(pa)
	BT_EVT_READY,								//蓝牙notyif打开,可以接收数据

	BT_EVT_REMOTE_TYPE,							//蓝牙识别remote type event
	BT_EVT_RX,									//rx消息, (pa)
	BT_EVT_TX,									//READ 事件
} bt_evt_t;

typedef struct{
	uint8_t bonded; 
}bt_evt_init_t;
typedef struct{
	int8_t rssi; 		//RSSI值
	uint8_t mac[6];
	uint8_t name[31];
}bt_evt_scan_t;
typedef struct{
	uint8_t mac[6];
	uint8_t name[31];
}bt_evt_con_t;
typedef struct{
	uint8_t resion;
}bt_evt_discon_t,bt_evt_fail_t;

typedef struct{
	bt_server_t bts;
	uint8_t ready;		//0:success, 1:fail 2: txing
}bt_evt_ready_t;

typedef struct{
	bt_server_t bts;
	uint8_t* buf;
	uint16_t len;
}bt_evt_rx_t;
typedef struct{
	bt_server_t bts;
	uint8_t ret;		//0:success, 1:fail 2: txing
}bt_evt_tx_t;

typedef union{
	bt_evt_init_t init;
	bt_evt_scan_t scan;
	bt_evt_con_t con;
	bt_evt_discon_t discon;
	bt_evt_fail_t fail;
	bt_evt_rx_t rx;
	bt_evt_tx_t tx;
	bt_evt_ready_t ready;
}bt_evt_pa_t;


/*
BLE: ble 广播类型
	IND: An indication that doesn’t expect a reply
EDR: 广播类型
• Non-Discoverable Mode			: 0 不可发现,不可连接,用于关闭蓝牙(相当于关闭蓝牙)
• Non-Connectable Mode			: 1 可发现不可连接(蓝牙不可被连接)
• Limited Discoverable Mode		: 2 受限可发现可连接, 用于回连(可被已经配对过的设备连接,不被通用查询模式找到)
• General Discoverable Mode		: 3 通用可发现可连接, 用于连接新设备(只能能被通用查询找到)
*/
typedef enum {
    BLE_ADV_IND = 0,         /*Connectable and scannable undirected advertising,普通广播,超时后自动重新广播*/
    BLE_ADV_DIRECT_IND,      /*Connectable high duty cycle directed advertising,会超时退出到普通广播*/
    BLE_ADV_SCAN_IND,        /*Scannable undirected advertising*/
    BLE_ADV_NONCONN_IND,     /*Non connectable undirected advertising*/
    BLE_ADV_DIRECT_IND_LOW,  /*Connectable low duty cycle directed advertising, 超时后自动重新定向广播*/

	EDR_NON_DISVOVERABLE = 0,		//不可发现,不可连接
    EDR_LIMIT_DISVOVERABLE, 		//受限可发现可连接
    EDR_GENERAL_DISVOVERABLE,		//通用可发现可连接
    EDR_NON_CONNECTABLE,			//可发现不可连接
}bt_adv_t;




typedef struct{
	app_fifo_t	fifo;
	uint8_t		tx_busy;
	uint16_t 	tx_mtu;
	uint16_t 	tx_len;
	uint8_t* 	tx_buf;
}bt_tx_fifo_t;


typedef struct{
	uint16_t types;				//for default type, please fix in project
	uint16_t hid_types;			//for default type, please fix in project
	uint8_t enable:1;

	uint8_t init_ok:1;
	uint8_t hid_ready:1;		//ble edr hid ready
	uint8_t vendor_ready:1;		//ble vendor uuid, edr SPP ready
	uint8_t remote_ios:1;		//is ios remote type is ios
	uint8_t is_debonded:1; 		//解绑状态(目前只适配EDR)
	uint8_t res:2;
	uint8_t bond_index;			//蓝牙配对绑定序号(蓝牙不同模式可以有单独的配对列表)
	uint8_t bt_idx;				//蓝牙地址序号, 不同模式可以有不同的蓝牙mac地址
	uint16_t inteval_10us;		//10 us
	bt_sta_t sta;

	bt_tx_fifo_t* fifo_txp;
}api_bt_ctb_t;


#if BT_SUPPORT & BIT_ENUM(TR_BLE)					//ble peripheral
extern api_bt_ctb_t m_ble;
extern bool m_ble_adv_discoverable;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_EDR)					//edr peripheral
extern bool edr_sniff_by_remote;
extern api_bt_ctb_t m_edr;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_BLEC)					//ble central
extern api_bt_ctb_t m_blec;
extern bt_evt_scan_t blec_scan_result;
#endif
#if BT_SUPPORT & BIT_ENUM(TR_BLE_RF)
extern api_bt_ctb_t m_ble_rf;
#endif
#if BT_SUPPORT & BIT_ENUM(TR_BLE_RFC)					//ble central
extern api_bt_ctb_t m_ble_rfc;
extern bt_evt_scan_t ble_rfc_scan_result;
#endif
#if BT_SUPPORT & BIT_ENUM(TR_EDRC)					//edr central
extern api_bt_ctb_t m_edrc;
extern bt_evt_scan_t edrc_scan_result;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RF)
extern api_bt_ctb_t m_rf;
#endif

#if BT_SUPPORT & BIT_ENUM(TR_RFC)
extern api_bt_ctb_t m_rfc;
#endif


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bt_evt_scan_t* api_bt_get_scan_result(bt_t bt);
api_bt_ctb_t* api_bt_get_ctb(bt_t bt);
bool api_bt_is_connected(bt_t bt);
bool api_bt_vendor_mac(uint8_t id, bt_t bt, uint8_t *pmac );				//__WEAK
uint8_t api_bt_vendor_name(uint8_t id,bt_t bt, char *buf, uint8_t len );	//__WEAK
bool api_bt_get_mac(uint8_t id, bt_t bt, uint8_t *buf );		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
uint8_t api_bt_get_name(uint8_t id,bt_t bt, char *buf, uint8_t len );
bool api_bt_is_bonded(uint8_t id,bt_t bt);
bool api_bt_debond(uint8_t id,bt_t bt);
bool api_bt_disconnect(uint8_t id,bt_t bt);
bool api_bt_enable(uint8_t id,bt_t bt,bool en);
void api_bt_enable_all(bool en);
void api_bt_tx_fifo_fush(bt_t bt);
bool api_bt_uart_tx(uint8_t id, bt_t bt,uint8_t *buf, uint16_t len);
bool api_bt_hid_tx(uint8_t id, bt_t bt, uint8_t hid_requset, uint8_t*buf, uint16_t len);
error_t os_bt_rx(uint8_t id,bt_t bt, bt_evt_rx_t* pa);								//__WEAK
void api_bt_rx(uint8_t id,bt_t bt, bt_evt_rx_t* pa);								//__WEAK
bool api_bt_event_weak(uint8_t id,bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa);	//__WEAK
void api_bt_event(uint8_t id, bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa);
bool api_bt_ctb_init(void);
bool api_bt_init(void);
bool api_bt_deinit(void);
void api_bt_task(void* pa);
void api_bt_handler(uint32_t period_10us);



/*******************************************************************************************************************
**	hal驱动接口
********************************************************************************************************************/
bool bt_driver_select_mode(uint8_t id, uint16_t trps);
bool bt_driver_get_mac(uint8_t id,bt_t bt, uint8_t *buf );		//这里高3byte buf[3]是public地址
bool bt_driver_is_bonded(uint8_t id,bt_t bt);
bool bt_driver_debond(uint8_t id,bt_t bt);
bool bt_driver_disconnect(uint8_t id,bt_t bt);
bool bt_driver_enable(uint8_t id,bt_t bt,bool en);
bool bt_driver_uart_tx(uint8_t id,bt_t bt, uint8_t *buf, uint16_t len);
bool bt_driver_hid_tx(uint8_t id,bt_t bt, uint8_t hid_requset,uint8_t*buf, uint16_t len);
bool bt_driver_init(uint8_t id);
bool bt_driver_deinit(uint8_t id);
void bt_driver_task(void* pa);

bool hal_bt_get_mac(uint8_t id,bt_t bt, uint8_t *buf );		//这里高3byte buf[3]是public地址
bool hal_bt_is_bonded(uint8_t id,bt_t bt);
bool hal_bt_debond(uint8_t id,bt_t bt);
bool hal_bt_disconnect(uint8_t id,bt_t bt);
bool hal_bt_enable(uint8_t id,bt_t bt,bool en);
bool hal_bt_uart_tx(uint8_t id,bt_t bt, uint8_t *buf, uint16_t len);
bool hal_bt_hid_tx(uint8_t id,bt_t bt, uint8_t hid_requset, uint8_t*buf, uint16_t len);
bool hal_bt_init(uint8_t id);
bool hal_bt_deinit(uint8_t id);
void hal_bt_task(void* pa);

#ifdef __cplusplus
}
#endif

#endif