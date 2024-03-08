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
#include "hw_board.h"
#ifdef HW_UART_MAP

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_uart.h"
#include  "api/api_log.h"
#include  "api/api_tick.h"


#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t uart_map_t m_uart_map[] = HW_UART_MAP;
uint8c_t m_uart_num = countof(m_uart_map);

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

bool uart_fifo_init = false;		//fifo init only ones

app_fifo_t uart_rx_fifo[countof(m_uart_map)];

#if UART_TX_FIFO_ENABLED
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

bool api_uart_fifo_tx(uint8_t id, void * buf,uint16_t len)
{
	bool ret = false;
    uint16_t fifo_len = len;

	if(id >= m_uart_num) return false;

	#if UART_TX_FIFO_ENABLED
	if(ERROR_SUCCESS == app_fifo_write(&uart_rx_fifo[id], buf, &fifo_len)){
		ret = true;
	}
	#endif
	
	UNUSED_VARIABLE(buf);
	UNUSED_VARIABLE(fifo_len);
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	用于需要在uart中断处理中调用改函数	
*******************************************************************/
__WEAK void api_uart_rx_hook(uint8_t id,uint8_t* buf,uint16_t len)
{
	uint8_t err = ERROR_SUCCESS;
	uint16_t fifo_len = len;
	app_fifo_t* fifop;

	if(id >= m_uart_num) return;

	fifop = &uart_rx_fifo[id];
	
	// dumpd(buf,fifo_len);
	err = app_fifo_write(fifop,buf,&fifo_len);
	if( (ERROR_SUCCESS != err) || (fifo_len != len) ){
		logd("uo\n");
		if(ERROR_SUCCESS == err){
		    logd("write:%d %d,fifo:%d %d %d %d\n",fifo_len, len,
		        fifop->buf_size_max,
		        fifop->read_pos,
		        fifop->write_pos,
		        fifop->fifo_stu);
		}
	}
}

app_fifo_t* api_uart_get_rx_fifo(uint8_t id)
{
	if(id < m_uart_num){
		return &uart_rx_fifo[id];
	}
	return NULL;
}

bool api_uart_init(uint8_t id)
{
	bool ret = false;
	uint8_t *buf;
	uint32_t baud;

	if(id >= m_uart_num) return false;

	if(!uart_fifo_init){			//fifo init only ones
		uart_fifo_init = true;
		memset(uart_rx_fifo,0,sizeof(uart_rx_fifo));
		#if UART_TX_FIFO_ENABLED
		memset(uart_tx_fifo,0,sizeof(uart_tx_fifo));
		#endif
	}

	if(m_uart_map[id].rx_buf_len){							//rx fifo init									
		if(NULL == uart_rx_fifo[id].p_buf) {				//uart buf only malloc ones
			buf = emf_malloc(m_uart_map[id].rx_buf_len);	/*attention: 这里串口上电申请一次内存不释放*/
			ret = !app_fifo_init(&uart_rx_fifo[id], buf, m_uart_map[id].rx_buf_len);
			logd("init rx fifo%d,len=%d\n",id,m_uart_map[id].rx_buf_len);
			if(!ret){
				emf_free(buf);
			}
		}else{
			loge("uart malloc faile\n");
		}
	}

	#if UART_TX_FIFO_ENABLED
	if(m_uart_map[id].tx_buf_len){				//tx fifo init
		if(NULL == uart_tx_buf[id]) {			//uart buf only malloc ones
			buf = emf_malloc(m_uart_map[id].tx_buf_len);
			ret = !app_fifo_init(&uart_tx_fifo[id], buf, m_uart_map[id].tx_buf_len);
			if(!ret){
				emf_free(buf);
			}
		}else{
			loge("uart malloc faile\n");
		}
	}
	#endif

	baud = UART_BAUD_ATT(id);
	if(0 == baud) baud = UART_BADU_DEFAULT;

	ret &= hal_uart_init(id,baud);
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
		if((uint8_t)UART_DEBUG_ID != id){
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

#if UART_TX_FIFO_ENABLED
void api_uart_tx_task(void* pa)
{
	uint8_t id;
	uint8_t buf[UART_TX_FIFO_MTU];
	uint16_t len=UART_TX_FIFO_MTU;

	for(id=0; id<m_uart_num; id++){
		if(NULL != uart_tx_fifo){
			if(ERROR_SUCCESS == app_fifo_read(&uart_tx_fifo[id], (uint8_t*)&buf, &len)){
				api_uart_tx(id,buf,len);
			}
		}
	}
	UNUSED_PARAMETER(pa);
}
#endif



#if TASK_HANDLER_ENABLE && UART_TX_FIFO_ENABLED
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_uart_tx_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_uart_tx_task(NULL);
	}
}
#endif


#endif

