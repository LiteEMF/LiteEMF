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

#include "hw_config.h"
#ifdef HW_UART_MAP
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_uart.h"
#include  "api/api_log.h"
#include  "api/api_tick.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t pin_map_t m_uart_map[] = HW_UART_MAP;
uint8c_t m_uart_num = countof(m_uart_map);

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
#if UART0_RX_FIFO_LEN
uint8_t uart0_rx_buf[UART0_RX_FIFO_LEN];
#endif
#if UART1_RX_FIFO_LEN
uint8_t uart1_rx_buf[UART1_RX_FIFO_LEN];
#endif
#if UART2_RX_FIFO_LEN
uint8_t uart2_rx_buf[UART2_RX_FIFO_LEN];
#endif
app_fifo_t uart_rx_fifo[countof(m_uart_map)];

#if UART_TX_FIFO_ENABLED
#if UART0_TX_FIFO_LEN
uint8_t uart0_tx_buf[UART0_TX_FIFO_LEN];
#endif
#if UART1_TX_FIFO_LEN
uint8_t uart1_tx_buf[UART1_TX_FIFO_LEN];
#endif
#if UART2_TX_FIFO_LEN
uint8_t uart2_tx_buf[UART2_TX_FIFO_LEN];
#endif
app_fifo_t uart_tx_fifo[countof(m_uart_map)];
#endif
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_uart_set_baud(uint8_t id, uint32_t baud)
{
	bool ret = false;
	if(id < m_uart_num){
		ret = hal_uart_set_baud(id,baud);
	}
	return ret;
}

bool api_uart_tx(uint8_t id,void * buf,uint16_t len)
{
	return hal_uart_tx(id,buf,len);
}

bool api_uart_fifo_tx(uint8_t id,void * buf,uint16_t len)
{
	bool ret = false;
    uint16_t fifo_len = len;

	if(id >= m_uart_num) return false;

	#if UART_TX_FIFO_ENABLED
	if(ERROR_SUCCESS == app_fifo_write(&uart_tx_fifo[id], buf, &fifo_len)){
		ret = true;
	}
	#endif
	return ret;
}
__WEAK bool api_uart_rx_hook(uint8_t id,uint8_t* buf,uint16_t len)
{
	uint8_t err;
	uint16_t fifo_len = len;
	app_fifo_t* fifop;

	if(id >= m_uart_num) return false;
	
	fifop = &uart_rx_fifo[id];
	err = app_fifo_write(fifop,buf,&fifo_len);
	if((ERROR_SUCCESS != err) || (fifo_len != len) ){
		logd("uo\n");
		// if(ERROR_SUCCESS == err){
		//     logd("write:%d %d,fifo:%d %d %d %d\n",fifo_len, len,
		//         fifop->buf_size_max,
		//         fifop->read_pos,
		//         fifop->write_pos,
		//         fifop->fifo_stu);
		// }
	}
	return (ERROR_SUCCESS == err);
}
app_fifo_t* api_uart_get_rx_fifo(uint8_t id)
{
	if(id < m_uart_num){
		return &uart_rx_fifo[id];
	}
	return NULL;
}


bool uart_get_pa(uint8_t id,uart_pa_t *pa)
{
	bool ret = false;

	if(id >= m_uart_num) return false;
	switch(id){
	case 0:
		pa->baudrate = UART0_BADU;
		pa->tx_buf_len = UART0_TX_FIFO_LEN;
		pa->rx_buf_len = UART0_RX_FIFO_LEN;
		#if UART0_RX_FIFO_LEN
		pa->tx_buf = uart0_rx_buf;
		#endif
		#if UART0_TX_FIFO_LEN
		pa->rx_buf = uart0_tx_buf;
		#endif
		ret = true;
		break;
	case 1:
		pa->baudrate = UART1_BADU;
		pa->tx_buf_len = UART1_TX_FIFO_LEN;
		pa->rx_buf_len = UART1_RX_FIFO_LEN;
		#if UART1_RX_FIFO_LEN
		pa->tx_buf = uart1_rx_buf;
		#endif
		#if UART1_TX_FIFO_LEN
		pa->rx_buf = uart1_tx_buf;
		#endif
		ret = true;
		break;
	case 2:
		pa->baudrate = UART2_BADU;
		pa->tx_buf_len = UART2_TX_FIFO_LEN;
		pa->rx_buf_len = UART2_RX_FIFO_LEN;
		#if UART2_RX_FIFO_LEN
		pa->tx_buf = uart2_rx_buf;
		#endif
		#if UART2_TX_FIFO_LEN
		pa->rx_buf = uart2_tx_buf;
		#endif
		ret = true;
		break;
	}
	return ret;
}
bool api_uart_init(uint8_t id)
{
	bool ret;
	uart_pa_t uart_pa;

	if(id >= m_uart_num) return false;

	uart_get_pa(id,&uart_pa);
	app_fifo_init(&uart_rx_fifo[id], (uint8_t*)uart_pa.rx_buf, uart_pa.rx_buf_len);
	#if UART_TX_FIFO_ENABLED
	app_fifo_init(&uart_tx_fifo[id], (uint8_t*)uart_pa.tx_buf, uart_pa.tx_buf_len);
	#endif

	ret = hal_uart_init(id,uart_pa.baudrate);

	return ret;
}

bool api_uart_deinit(uint8_t id)
{
	bool ret;

	if(id >= m_uart_num) return false;

	app_fifo_flush(&uart_rx_fifo[id]);
	#if UART_TX_FIFO_ENABLED
	app_fifo_flush(&uart_tx_fifo[id]);
	#endif

	ret = hal_uart_deinit(id);
	return ret;
}

//为防止串口漏电, debug 串口单独初始化
void api_uarts_init(void)
{
	uint8_t id;
	for(id=0; id<m_uart_num; id++){
		if(UART_DEBUG_ID != id){
			api_uart_init(id);
		}
	}   
}

void api_uarts_deinits(void)
{
	uint8_t id;
	for(id=0; id<m_uart_num; id++){
		api_uart_deinit(id);
	}   
}

void api_uart_tx_task(void* pa)
{
	#if UART_TX_FIFO_ENABLED
	uint8_t id;
	uint8_t buf[UART_TX_FIFO_MTU];
	uint16_t len=UART_TX_FIFO_MTU;

	for(id=0; id<m_uart_num; id++){
		if(ERROR_SUCCESS == app_fifo_read(&uart_tx_fifo[id], (uint8_t*)&buf, &len)){
			api_uart_tx(id,buf,len);
		}
	}
	UNUSED_PARAMETER(pa);
	#endif
}



#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_uart_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_uart_tx_task(NULL);
	}
}
#endif


#endif

