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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#ifndef	_xbox_command_h
#define	_xbox_command_h

#include "emf_typedef.h"
#include "api/api_transport.h"
#include "api/api_tick.h"

#include "app/gamepad/xbox_typedef.h"
/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


//通用指令, 马达和按键index不同单独处理
typedef struct {
	uint8_t cmd;
	uint8_t bctrl;		//XBOX_CTRL_RES
	uint8_t index;		//不同指令组index 不一样, 但是index 不为0
	uint8_t len:7;		//数据长度不包含自己, 如果是长包,不包括长包序号长度
	uint8_t flag:1;		//表示长包lpack中的长度的类型 1: 长度=len1(eg: 00 74 表示0X74),0: 长度=len0+0x80*len1(eg:96 03 表示(03-1)*0x80+0x96)
	union{
		xbox_start_t start;
		xbox_state_t state;
		xbox_security_pass_t security_pass;
		xbox_led_en_t led_en;
		xbox_info_t dev_info;
		serial_number_t serial_number;
		xbox_motor_t  rumble;
		xbox_lpacket_t lpack;
		uint8_t buf[60];	//最大58
	}p;
}xbox_usb_pack_t;			//xbox usb 指令格式

/*enum cmd ack */
typedef struct{
	uint8_t cmd;			//固定0x01
	xbox_ctrl_t bctrl;		//0x20(b_handshake=1)
	uint8_t index;			//注意是ack in 包中的index
	uint8_t len;			//0x09

	uint8_t res;			//0x00
	uint8_t ack_cmd;		//ack in cmd
	uint8_t ack_bctrl;		//固定0x20
	uint16_t rec_len;		//回复接收到的数据数据长度,如果长包,回复实际接收到的数据
	uint8_t res2[2];
	uint16_t residue_len;	//剩余数据长度
}xbox_ack_t;



//*********************************************************************************//
//                               app xbox			                          //
//*********************************************************************************//
typedef struct {
	uint8_t cmd_index;			//CTRL = 0x20使用, 从2开始
	uint8_t uac_index;			//CTRL = 0x21时使用,从1开始
	uint8_t security_index;		//单独给加密用,从1开始
}xbox_cmd_index_t;


typedef struct {
	uint8_t cmd;				//当前收发的指令
	uint8_t bctrl;				//当前bctrl
	uint8_t index;
	uint8_t retry;				//重试次数
	uint16_t offset;			//当前偏移量
	trp_handle_t  trp_handle;
	timer_t timer;				//定时器
	
	uint16_t len;				//接收/发送总长度
	uint8_t* pbuf;				//注意:使用过程注意释放资源
}xbox_command_t;				//长包

/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
//xbox_command
bool xbox_command_free(xbox_command_t* cmdp);
bool xbox_ack(trp_handle_t *phandle,uint8_t cmd,xbox_ctrl_t bctrl, uint8_t index, uint8_t recive_len,uint8_t total_len);
bool xbox_cmd_ack(trp_handle_t *phandle,xbox_command_t* rx_cmdp);
bool xbox_command_fill(trp_handle_t *phandle,xbox_command_t*tx_cmdp,xbox_cmd_index_t *pindex,uint8_t uac_en,uint8_t cmd,uint8_t* buf,uint8_t len);
bool xbox_command_send(xbox_command_t*tx_cmdp);
bool xbox_get_ack(trp_handle_t *phandle, xbox_command_t *tx_cmdp, uint8_t* buf,uint8_t len);
bool xbox_get_cmd(trp_handle_t *phandle, xbox_command_t* rx_cmdp, uint8_t* buf,uint8_t len);
void xbox_command_tx_process(xbox_command_t*cmdp);

#endif

