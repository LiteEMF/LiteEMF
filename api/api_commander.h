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
#include "utils/emf_utils.h"
#include  "api/api_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef CMD_HEAD
#define CMD_HEAD		0XA4
#endif

#ifndef CMD_SHEAD		//CMD short HEAD
#define CMD_SHEAD		0XA5	
#endif

#define CMD_SHEAD_LEN	3
#define CMD_HEAD_LEN	4
#define CMD_PACK_LEN	5


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint8_t head;
	uint8_t len;
	uint8_t pack_index:7;
	uint8_t pack_start:1;	//注意短包底层发送数据时候没有pack_start和pack_index
	uint8_t cmd;
	uint8_t buf[1];			//占位
}command_head_t;


typedef struct{
	bytes_t sbuf;				//单字节接收缓存字节,大小为MTU大小			
	bytes_t cmds;				// 动态分配内存, command_head_t + data + sum , pack_index:0, len:mtu, 注意, 数据和指针长度不是一样的
}command_rx_t;


typedef struct {
	trp_handle_t handle;
	void * ptimer;			//长包数据定时发送定时器
	uint8_t *buf;			//定时发送初始化保证为NULL
	uint16_t len;
	uint16_t index;
	uint8_t cmd;
}command_tx_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t api_command_pack_size(uint8_t mtu,uint16_t len);
bool command_frame_tx(command_tx_t *txp);
bool command_frame_rx(bytes_t *cmdp, uint8_t mtu, uint8_t c);
bool api_command_rx(bytes_t* rxp,uint8_t* buf,uint8_t len);

#ifdef __cplusplus
}
#endif
#endif





