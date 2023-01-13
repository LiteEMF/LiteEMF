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


#ifndef _app_bt_h
#define _app_bt_h
#include "emf_typedef.h"
#include "bt_typedef.h"
#include "api/bt/api_bt.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef RF_FIFO_LEN		
#define RF_FIFO_LEN		(0x100)
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	app_fifo_t	fifo;
	uint8_t		tx_busy;
	uint16_t 	tx_mtu;
	uint16_t 	tx_len;
	uint8_t* 	tx_buf;
}bt_tx_fifo_t;

typedef struct{
	uint8_t init_ok:1;
	uint8_t enable:1;
	uint8_t res:6;
	uint16_t inteval;	//1.25ms
	bt_sta_t sta;
	hid_types_t types;
	hid_types_t hid_types;

	bt_tx_fifo_t* fifo_txp;
}app_bt_ctb_t;

extern bt_t app_bt_trs;		//bt_t
#if BLE_ENABLE					//ble peripheral
extern app_bt_ctb_t m_ble;
#endif

#if EDR_ENABLE					//edr peripheral
extern app_bt_ctb_t m_edr;
#endif

#if BLEC_ENABLE					//ble central
extern app_bt_ctb_t m_blec;
extern bt_evt_scan_t blec_scan_result;
#endif

#if EDRC_ENABLE					//edr central
extern app_bt_ctb_t m_edrc;
extern bt_evt_scan_t edrc_scan_result;
#endif

#if RF_ENABLE
extern app_bt_ctb_t m_rf;
#endif

#if RFC_ENABLE
extern app_bt_ctb_t m_rfc;
#endif

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bt_evt_scan_t* app_bt_get_scan_result(bt_t bt);
app_bt_ctb_t* app_bt_get_ctb(bt_t bt);
bool app_bt_get_mac(bt_t bt, uint8_t *buf );		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
uint8_t app_bt_get_name(bt_t bt, char *buf, uint8_t len );
void app_bt_tx_fifo_fush(bt_t bt);
bool app_bt_uart_tx(bt_t bt,uint8_t *buf, uint16_t len);
bool app_bt_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len);
error_t os_bt_rx(bt_t bt, bt_evt_rx_t* pa);								//__WEAK
void app_bt_rx(bt_t bt, bt_evt_rx_t* pa);								//__WEAK
bool app_bt_event_weak(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa);	//__WEAK
void app_bt_event(bt_t bt, bt_evt_t const event, bt_evt_pa_t* pa);
static bool app_bt_ctb_init(bt_t bt);
bool app_bt_init(void);
bool app_bt_deinit(void);
void app_bt_handler(void);

#ifdef __cplusplus
}
#endif
#endif





