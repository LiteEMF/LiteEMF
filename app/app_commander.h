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


#ifndef _app_command_h
#define _app_command_h
#include "emf_typedef.h" 
#include "api/api_commander.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef EMF_CMD_VERSION		//emf 版本号
#define EMF_CMD_VERSION		0x01				
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	/*---- 0x00	设备信息 ----*/
	CMD_NULL = 0,
	CMD_GET_EMF_VERSION,
	CMD_GET_UUID,
	CMD_GET_DEV_VERSION,
	CMD_GET_MODEL,
	CMD_GET_MTU,
	//6,7
	CMD_DEV_PID_VID=8,
	CMD_DEV_SN,
	CMD_DEV_MAC,
	CMD_DEV_EID,
	CMD_DEV_NAME,
	//0X0D,0X0E,0X0F

	/*---- 0x10	设备信息 ----*/
	CMD_DEV_MODE=0X10,
	CMD_DEV_CTRL,
	CMD_TEST_MODE,
	//res
	CMD_STORAGE_SYNC=0X1D,
	CMD_HEART_BEAT=0X1E,
	CMD_RECOVER_FACTORY=0X1F,

	/*---- 0x20	设备数据和控制 ----*/
	CMD_KM_KEY=0X20,		//kb + mouse
	CMD_GAMEPAD_KEY,
	CMD_MT_KEY,
	CMD_JOYSTICK_CAL,
	CMD_SET_RUMBLE,
	CMD_LED_PERIOD,					//app_led period
	CMD_RGB_MODE,
	CMD_RGB_BRIGHT,
	//res
	
	/*---- 0x30	设备扩展数据和控制 ----*/
	CMD_MUSIC_VOL=0X30,				//设备音量
	CMD_USB_STA,					//设备USB状态

	/*---- 0x50	设备配置 ----*/
	/*---- 0xD0 ~ 0xDF	socket ----*/
	/*---- 0XDF 自定义封装包  ----*/
	/*---- 0xE0	~ 0XEF自定义 ----*/
}app_cmd_t;


typedef enum{
	/*---- 0xD0	socket ----*/
	CMD_SOCKET_SETUP=0XD0,
	CMD_SOCKET_OUT,
	CMD_SOCKET_IN,
	CMD_SOCKET_SETUP_ACK,
	CMD_SOCKET_SETUP_STALL,
	CMD_SOCKET_SYNC,
	CMD_SOCKET_READY,
}app_socket_cmd_t;

typedef enum{
	/*---- 自定义封装包 ----*/
	CMD_VENDOR_PACK=0XDF,
	/*---- 0xE0	自定义 ----*/
	CMD_VENDOR=0XE0,
	CMD_UNSUPPORT=0XFF,
}app_vendor_cmd_t;



#define CTRL_SUB_DEV	0X80	//控制从机单片机
typedef enum{
    CTRL_RESET=0,               //设备重新启动
    CTRL_STOP,                  //设备关机
	CTRL_BOOT,
}dev_ctrl_t;

extern trp_t test_trp;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_command_init(void);
bool app_command_vendor_decode(trp_handle_t *phandle,uint8_t* buf,uint16_t len);		//WEAK
bool app_command_std_decode(trp_handle_t *phandle,uint8_t* buf,uint16_t len);

bool app_command_timer_tx(command_tx_t *txp, trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len, uint32_t ms);
bool app_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len);
bool app_command_rx(trp_handle_t *phandle,uint8_t *buf, uint8_t len);
bool app_command_rx_byte(trp_handle_t *phandle, uint8_t c);		
bool app_command_rx_fifo(trp_handle_t *phandle, app_fifo_t* fifop);

#ifdef __cplusplus
}
#endif
#endif





