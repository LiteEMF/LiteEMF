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

static const_t uart_pa_t uart_pa[] = {		//TODO api HAL
	UART0_PA,
	UART1_PA,
	UART2_PA
};


app_fifo_t* s_uart_rx_fifop[countof(m_uart_map)];
static uint8_t* uart_rx_buf[countof(m_uart_map)];
#if UART_TX_FIFO_ENABLED
app_fifo_t* s_uart_tx_fifop[countof(m_uart_map)];
static uint8_t* uart_tx_buf[countof(m_uart_map)];
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
#if UART_TX_FIFO_ENABLED
bool api_uart_fifo_tx(uint8_t id,void * buf,uint16_t len)
{
	bool ret = false;
    uint16_t fifo_len = len;
	if(ERROR_SUCCESS == app_fifo_write(s_uart_tx_fifop[id], buf, &fifo_len)){
		ret = true;
	}
	return ret;
}
#endif
__WEAK bool api_uart_rx(uint8_t id,uint8_t* buf,uint16_t len)
{
	uint8_t err;
	uint16_t fifo_len = len;
	app_fifo_t* fifop = s_uart_rx_fifop[id];
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
		return s_uart_rx_fifop[id];
	}
	return NULL;
}

bool api_uart_init(uint8_t id)
{
	bool ret;

	ret = hal_uart_init(id,uart_pa[id].baudrate);
	if(ret){
		if(uart_pa[id].rx_buf_len){		//rx fifo init
			s_uart_rx_fifop[id] = emf_malloc(sizeof(app_fifo_t*));
			uart_rx_buf[id] = emf_malloc(uart_pa[id].rx_buf_len);
			app_fifo_init(s_uart_rx_fifop[id], (uint8_t*)uart_rx_buf, uart_pa[id].rx_buf_len);
		}

		#if UART_TX_FIFO_ENABLED
		if(uart_pa[id].tx_buf_len){		//tx fifo init
			s_uart_tx_fifop[id] = emf_malloc(sizeof(app_fifo_t*));
			uart_tx_buf[id] = emf_malloc(uart_pa[id].tx_buf_len);
			app_fifo_init(s_uart_tx_fifop[id], (uint8_t*)uart_tx_buf, uart_pa[id].tx_buf_len);
		}
		#endif
	}
	return ret;
}

bool api_uart_deinit(uint8_t id)
{
	bool ret;
	ret = hal_uart_deinit(id);
	if(ret){
		emf_free(s_uart_rx_fifop[id],sizeof(app_fifo_t*));
		emf_free(uart_rx_buf[id],uart_pa[id].rx_buf_len);
		
		#if UART_TX_FIFO_ENABLED
		emf_free(s_uart_tx_fifop[id],sizeof(app_fifo_t*));
		emf_free(uart_tx_buf[id],uart_pa[id].tx_buf_len);
		#endif
	}
	return ret;
}

void api_uarts_init(void)
{
	uint8_t id;
	for(id=0; id<m_uart_num; id++){
		api_uart_init(id);
	}   
}

void api_uarts_deinits(void)
{
	uint8_t id;
	for(id=0; id<m_uart_num; id++){
		api_uart_deinit(id);
	}   
}

void api_uart_handler(void)
{
	#if UART_TX_FIFO_ENABLED
	uint8_t id;
	uint8_t buf[UART_TX_FIFO_MTU];
	uint16_t len=UART_TX_FIFO_MTU;
	for(id=0; id<m_uart_num; id++){
		if(ERROR_SUCCESS == app_fifo_read(s_uart_tx_fifop[id], (uint8_t*)&buf, &len)){
			api_uart_tx(id,buf,len);
		}
	}
	#endif
}
#endif

