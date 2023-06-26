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


#ifndef _api_uart_h
#define _api_uart_h
#include "utils/emf_typedef.h" 
#include "hw_config.h"
#include "hal_uart.h"
#include "api/api_gpio.h"
#include "utils/emf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef UART_TX_FIFO_ENABLED			//can cut code size
#define UART_TX_FIFO_ENABLED     0
#endif
#ifndef UART_TX_FIFO_MTU				//fix by hardware os
#define UART_TX_FIFO_MTU		16
#endif


#ifndef UART_BADU_DEFAULT
#define UART_BADU_DEFAULT        1000000
#endif


#ifndef UART_DEBUG_ID							//debug uart 单独初始化
#define UART_DEBUG_ID		0
#endif


//uart att
#define UART_BAUD_POS		(0)
#define UART_BAUD_MASK		0X00FFFFFF
#define UART_PRI_POS		(24)				/*是否开启中断, 设置中断优先级Priority*/
#define UART_PRI_MASK		0X01000000

#ifndef UART_BAUD_ATT
#define UART_BAUD_ATT(id)  		(FLD2VAL(UART_BAUD, m_uart_map[id].att))
#endif
#ifndef UART_PRI_ATT
#define UART_PRI_ATT(id)  		(FLD2VAL(UART_PRI, m_uart_map[id].att))
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	pin_t tx;
	pin_t rx;
	uint16_t tx_buf_len;
	uint16_t rx_buf_len;
	uint32_t peripheral;
	uint32_t att;
} uart_map_t;


extern uint8c_t m_uart_num;
extern const_t uart_map_t m_uart_map[];
/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_uart_set_baud(uint8_t id, uint32_t baud);
bool api_uart_tx(uint8_t id,void * buf,uint16_t len);
bool api_uart_fifo_tx(uint8_t id,void * buf,uint16_t len);			//UART_TX_FIFO_ENABLED
void api_uart_rx_hook(uint8_t id,uint8_t* buf,uint16_t len);		//WEAK
app_fifo_t* api_uart_get_rx_fifo(uint8_t id);

bool api_uart_init(uint8_t id);
bool api_uart_deinit(uint8_t id);
void api_uarts_init(void);
void api_uarts_deinits(void);
void api_uart_tx_task(void* pa);
void api_uart_tx_handler(uint32_t period_10us);

//hal
bool hal_uart_set_baud(uint8_t id, uint32_t baud);
bool hal_uart_tx(uint8_t id,void * buf,uint16_t len);
bool hal_uart_init(uint8_t id,uint32_t baudrate);
bool hal_uart_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





