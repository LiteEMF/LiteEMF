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

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

typedef enum{
	TR_BLE		= 0,
	TR_EDR		= 1,
	TR_BLEC		= 2,
	TR_EDRC		= 3,
	TR_BLE_RF	= 4,				//BLE模拟2.4G	
	TR_BLEC_RF	= 5,
	TR_RF		= 6,
	TR_RFC		= 7,
	TR_USBD		= 8,
	TR_USBH		= 9,
	TR_UART		= 10,
}trp_t;

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

uint16_t api_get_transport_mtu(trp_handle_t* phandle);
bool api_transport_tx(trp_handle_t* phandle, uint8_t* buf,uint16_t size);
bool api_trp_init(void);
bool api_trp_deinit(void);
void api_trp_handler(void);

#ifdef __cplusplus
}
#endif
#endif





