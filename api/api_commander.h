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
	uint8_t* buf;				// cmd + data	
	uint16_t len;
	uint16_t max_len;

	uint8_t packet_num;
	uint8_t packet_index;
}command_rx_t;

typedef struct{
	uint8_t head;
	uint8_t len;
	uint8_t* frame;				//head + len + index + data + sum
	command_rx_t rx;
}command_stream_rx_t;				//数据流

/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t api_command_pack_size(uint8_t mtu,uint16_t len);
bool api_command_pack(uint8_t cmd, uint8_t mtu, uint8_t *buf,uint16_t len,uint8_t *ppacket, uint16_t packet_len);
bool api_command_arg_pack(uint8_t cmd, uint16_t arg, uint8_t mtu, uint8_t *buf,uint16_t len,uint8_t *ppacket, uint16_t packet_len);
bool api_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len);
bool api_command_arg_tx(trp_handle_t* phandle,uint8_t cmd, uint16_t arg, uint8_t *buf,uint16_t len);
bool api_command_unpack(command_rx_t* rxp,uint8_t* pcmd,uint8_t len);
bool api_command_byte_unpack(command_stream_rx_t* pstream, uint8_t mtu, uint8_t c);
bool command_rx_free(command_rx_t *rxp);
bool api_command_init(void);
bool api_command_deinit(void);
void api_command_handler(void);


#ifdef __cplusplus
}
#endif
#endif





