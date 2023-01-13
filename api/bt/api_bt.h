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
#include "api/hid/hid_desc.h"
#include "api/bt/driver_bt.h"
#include "hal_bt.h"
#include "utils/emf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef BLE_VID
#define BLE_VID			CFG_VID
#endif
#ifndef EDR_VID
#define EDR_VID			CFG_VID
#endif
#ifndef BLE_PID
#define BLE_PID			0X000d
#endif
#ifndef EDR_PID
#define EDR_PID			0X000e
#endif
#ifndef BLE_ICON
#define BLE_ICON		BLE_APPEARANCE_HID_KEYBOARD
#endif
#ifndef EDR_ICON
#define EDR_ICON		BD_CLASS_KEYBOARD
#endif

#ifndef EDR_SNIFF_ENABLE
#define EDR_SNIFF_ENABLE			1
#endif
#ifndef BT_ADV_INTERVAL
#define BT_ADV_INTERVAL             (160) 		//(unit:0.625ms)
#endif
#ifndef BLE_MULT_HID_SERVICES					//如果pc打开多服务,但是不初始化report map 会有感叹号
#define BLE_MULT_HID_SERVICES		1
#endif

#ifndef BT_RANDOM_NAME_ENABLE
#define BT_RANDOM_NAME_ENABLE		1
#endif


/*******************************************************************
** Description: 2.4g数据发送说明,2.4G没有more data不能按照蓝牙的方式发送数据, 
2.4G有两种发送数据方式:
	1.调用api_bt_tx函数发送: 这种方式可以发送随机的指令或者非高频的数据,
		如果发送高频数据太快超过2.4g连接间隔要有fifo堆满和延时的风险(2.4g丢包或者多包)
	2. RF_EVT_TX 事件中发送数据, 为了优化发送不阻塞和冗余,实时数据(比如按键)最好在 RF_EVT_TX 事件中处理,
RF_EVT_TX事件接收后可以根据fifo长度来判断是否填充按键数据	
*******************************************************************/

/*******************************************************************
 * RF_TX_LL_MTU : rf链路层MTU, 长度约大越容易被干扰
 * 如果 发送单包数据长度>RF_TX_LL_MTU,底层就需要分包发送,占用更多缓存	
 * 如果 发送单包数据长度<RF_TX_LL_MTU 就不需要底层分包	
*******************************************************************/
#ifndef RF_TX_LL_MTU
#define RF_TX_LL_MTU		32
#endif
#ifndef RF_RX_LL_MTU
#define RF_RX_LL_MTU		RF_TX_LL_MTU
#endif


/*****************************************************************************************************
** 蓝牙空中包数据格式:
	Preamble 	Access_Address			PDU				CRC
		|			|			Header	Payload	MIC		|
		|			|		  		2	251	  	 4		|
		1			4					257				3

* PDU_Payload	range(27~251)
* PDU_Payload =  APP_BT_LL_MTU + 3 + 4	 	Link layout 底层MTU
* PDU_Payload  = APP_BT_ATT_MTU + 3 + 4	ATT层MTU
******************************************************************************************************/

/*******************************************************************
 ** APP_BT_LL_MTU
 * APP_BT_LL_MTU 不一定要等于APP_BT_ATT_MTU,只有需要速度时候才需要增加
 * 如果 蓝牙发送单包数据长度>APP_BT_LL_MTU,底层就需要分包发送(分包间隔150us+其他包消耗)	
 * 如果 蓝牙发送单包数据长度<APP_BT_LL_MTU 就不需要底层分包
 * 范围 range(20~244) ,要求速度才需要开这个 APP_BT_LL_MTU feature，开了会多消耗RAM		
*******************************************************************/
#ifndef APP_BT_LL_MTU		// Link layout MTU	
#define APP_BT_LL_MTU		20
#endif

/*******************************************************************
 **	APP_BT_ATT_MTU
 *	APP_BT_ATT_MTU 决定了一包数据最大能发送多少字节数据,ATT MUT影响了整个ATT通讯MTU,超过的只能moredata或者下一个连接发送
 *	范围 range(20~244)
*******************************************************************/
#ifndef APP_BT_ATT_MTU		//hid数据通道的MTU一般不超过32字节
	#if BLE_CMD_MAX > 61
	#define APP_BT_ATT_MTU   (BLE_CMD_MAX)
	#else
	#define APP_BT_ATT_MTU   (61)					//不清楚为什么超过61会导致ios13.4鼠标描述符无作用
	#endif
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
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
    EDR_NON_CONNECTABLE,			//可发现不可连接
    EDR_LIMIT_DISVOVERABLE, 		//受限可发现可连接
    EDR_GENERAL_DISVOVERABLE,		//通用可发现可连接
}bt_adv_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_bt_get_mac(bt_t bt, uint8_t *buf );		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
bool api_bt_is_bonded(bt_t bt);
bool api_bt_debond(bt_t bt);
bool api_bt_disconnect(bt_t bt);
bool api_bt_enable(bt_t bt,bool en);
bool api_bt_uart_fifo_tx(bt_t bt,app_fifo_t * fifop, uint8_t *buf, uint16_t len);
bool api_bt_uart_tx(bt_t bt,uint8_t *buf, uint16_t len);
bool api_bt_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len);
bool api_bt_stack_init_ok(bt_t bt);
bool api_bt_init(void);
bool api_bt_deinit(void);
void api_bt_handler(void);

// bt extern dev driver
bool bt_driver_get_mac(bt_t bt, uint8_t *buf );
bool bt_driver_is_bonded(bt_t bt);
bool bt_driver_debond(bt_t bt);
bool bt_driver_disconnect(bt_t bt);
bool bt_driver_enable(bt_t bt,bool en);
bool bt_driver_uart_tx(bt_t bt,uint8_t *buf, uint16_t len);
bool bt_driver_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len);
bool bt_driver_init(void);
bool bt_driver_deinit(bt_t bt);
void bt_driver_handler(void);

/*******************************************************************************************************************
**	hal驱动接口
********************************************************************************************************************/
bool hal_bt_get_mac(bt_t bt, uint8_t *buf );		//这里高3byte buf[3]是public地址
bool hal_bt_is_bonded(bt_t bt);
bool hal_bt_debond(bt_t bt);
bool hal_bt_disconnect(bt_t bt);
bool hal_bt_enable(bt_t bt,bool en);
bool hal_bt_uart_tx(bt_t bt,uint8_t *buf, uint16_t len);
bool hal_bt_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len);
bool hal_bt_init(void);
bool hal_bt_deinit(bt_t bt);
void hal_bt_handler(void);

#ifdef __cplusplus
}
#endif
#endif

