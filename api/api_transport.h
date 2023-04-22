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


#ifndef _api_transporte_h
#define _api_transporte_h
#include "emf_typedef.h"
#include "hw_config.h"			//TODO
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#ifndef BLE_CMD_MTU
#define BLE_CMD_MTU			48
#endif
#ifndef EDR_CMD_MTU
#define EDR_CMD_MTU			64
#endif
#ifndef BLEC_CMD_MTU
#define BLEC_CMD_MTU		48
#endif
#ifndef EDRC_CMD_MTU
#define EDRC_CMD_MTU		64
#endif
#ifndef BLE_RF_CMD_MTU
#define BLE_RF_CMD_MTU		48
#endif
#ifndef BLEC_RF_CMD_MTU
#define BLEC_RF_CMD_MTU		48
#endif
#ifndef RF_CMD_MTU
#define RF_CMD_MTU			32
#endif
#ifndef RFC_CMD_MTU
#define RFC_CMD_MTU			32
#endif
#ifndef USBD_CMD_MTU
#define USBD_CMD_MTU		64
#endif
#ifndef USBH_CMD_MTU
#define USBH_CMD_MTU		64
#endif
#ifndef UART_CMD_MTU
#define UART_CMD_MTU		10
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

typedef struct {
	trp_t trp;
	uint8_t id;
	uint16_t index;	//high 8bit:dev_type_t low 8bit:sub_type
}trp_handle_t;


extern uint16_t m_trps;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_trp_is_usb(trp_t trp);
bool api_trp_is_bt(trp_t trp);

uint16_t api_transport_get_mtu(trp_handle_t* phandle);
bool api_transport_tx(trp_handle_t* phandle, uint8_t* buf,uint16_t len);
bool api_trp_init(void);
bool api_trp_deinit(void);

#ifdef __cplusplus
}
#endif
#endif





