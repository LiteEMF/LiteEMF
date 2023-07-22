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


#ifndef _api_commander_h
#define _api_commander_h
#include "emf_typedef.h" 
#include  "api/api_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef CMD_SHEAD
#define CMD_SHEAD		0XA5
#endif
#ifndef CMD_HEAD
#define CMD_HEAD		0XA4
#endif
#define COM_HEAD_LEN	4

//cmd 指令定义
#define CMD_POS				(0)					//固定8位指令
#define CMD_MASK			0X000000FF
#define CMD_ARG_EN_POS		(8)					//1:带arg参数,0:不带参数
#define CMD_ARG_EN_MASK		0X0000FF00
#define CMD_ARG_POS			(16)				//参数
#define CMD_ARG_MASK		0XFFFF0000


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint8_t head;
	uint8_t len;
	uint8_t pack_index;
	uint8_t cmd;
}command_head_t;

typedef struct{
	uint8_t* pcmd;				// 动态分配内存, command_head_t + data + sum , pack_index:0, len:mtu
	uint16_t len;				// 接收指令长度, 注意 和 pcmd 指针长度是不一样的
}command_rx_t;


typedef struct {
	trp_handle_t *phandle;
	void * ptimer;			//长包数据定时发送定时器
	uint8_t *buf;			//定时发送初始化保证为NULL
	uint16_t len;
	uint16_t index;
	uint32_t cmd;
}command_tx_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t api_command_pack_size(uint8_t mtu,uint16_t len);
bool api_command_timer_tx(command_tx_t *txp, trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len, uint32_t ms);
bool api_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len);
bool api_command_arg_tx(trp_handle_t* phandle,uint8_t cmd, uint16_t arg, uint8_t *buf,uint16_t len);
bool command_rx_free(command_rx_t *rxp);			// free  api_command_rx or api_command_rx_byte 
bool api_command_rx(command_rx_t* rxp,uint8_t* buf,uint8_t len);
bool api_command_rx_byte(command_rx_t *rxp, uint8_t mtu, uint8_t c, uint8_t *s_buf, uint8_t *s_plen);

#ifdef __cplusplus
}
#endif
#endif





