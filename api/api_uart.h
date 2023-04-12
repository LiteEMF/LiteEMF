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
#include "emf_typedef.h"
#include "hw_config.h"
#include "hal/hal_uart.h"
#include "api/api_gpio.h"
#include "emf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef UART_TX_FIFO_ENABLED			//can cut code size
#define UART_TX_FIFO_ENABLED     1
#endif
#ifndef UART_TX_FIFO_MTU				//fix by hardware os
#define UART_TX_FIFO_MTU		16
#endif


#ifndef UART0_BADU
#define UART0_BADU			1000000
#endif
#ifndef UART0_TX_FIFO_LEN
#define UART0_TX_FIFO_LEN	0
#endif
#ifndef UART0_RX_FIFO_LEN
#define UART0_RX_FIFO_LEN	0
#endif

#ifndef UART1_BADU
#define UART1_BADU			1000000
#endif
#ifndef UART1_TX_FIFO_LEN
#define UART1_TX_FIFO_LEN	0
#endif
#ifndef UART1_RX_FIFO_LEN
#define UART1_RX_FIFO_LEN	0
#endif

#ifndef UART2_BADU
#define UART2_BADU			1000000
#endif
#ifndef UART2_TX_FIFO_LEN
#define UART2_TX_FIFO_LEN	0
#endif
#ifndef UART2_RX_FIFO_LEN
#define UART2_RX_FIFO_LEN	0
#endif

#ifndef UART_DEBUG_ID
#define UART_DEBUG_ID		0
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint32_t baudrate;
	uint8_t *tx_buf;
	uint8_t *rx_buf;
	uint16_t tx_buf_len;
	uint16_t rx_buf_len;
}uart_pa_t;



extern uint8c_t m_uart_num;
extern const_t pin_map_t m_uart_map[];
/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_uart_set_baud(uint8_t id, uint32_t baud);
bool api_uart_tx(uint8_t id,void * buf,uint16_t len);
bool api_uart_fifo_tx(uint8_t id,void * buf,uint16_t len);			//UART_TX_FIFO_ENABLED
bool api_uart_rx_hook(uint8_t id,uint8_t* buf,uint16_t len);		//WEAK
app_fifo_t* api_uart_get_rx_fifo(uint8_t id);
bool api_uart_init(uint8_t id);
bool api_uart_deinit(uint8_t id);
void api_uarts_init(void);
void api_uarts_deinits(void);
void api_uart_handler(uint32_t period_10us);

//hal
bool hal_uart_set_baud(uint8_t id, uint32_t baud);
bool hal_uart_tx(uint8_t id,void * buf,uint16_t len);
bool hal_uart_init(uint8_t id,uint32_t baudrate);
bool hal_uart_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





