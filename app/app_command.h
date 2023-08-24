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
#include  "api/api_commander.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef EMF_CMD_VERSION		//emf command版本号
#define EMF_CMD_VERSION		0x32					
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
	CMD_GET_MODE,
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
	CMD_TEST_MODE,
	CMD_DEV_CTRL,
	//res
	CMD_STORAGE_SYNC=0X1D,
	CMD_HEART_BEAT=0X1E,
	CMD_RECOVER_FACTORY=0X1F,

	/*---- 0x20	设备数据和控制 ----*/
	CMD_KM_KEY=0X20,
	CMD_GAMEPAD_KEY,
	CMD_JOYSTICK_CAL,
	CMD_SET_RUMBLE,
	CMD_RGB_MODE,
	CMD_RGB_BRIGHT,
	//res
	
	/*---- 0x30	设备扩展数据和控制 ----*/
	CMD_MUSIC_VOL=0X30,				//设备音量
	CMD_USB_STA,				//设备USB状态

	/*---- 0x50	设备配置 ----*/
	

	/*---- 0xE0	socket ----*/
	CMD_SOCKET_SETUP=0XE0,
	CMD_SOCKET_OUT,
	CMD_SOCKET_IN,
	CMD_SOCKET_SETUP_ACK,
	CMD_SOCKET_SETUP_STALL,
	CMD_SOCKET_SYNC,
	CMD_SOCKET_CONFIGURED,

	/*---- 0xF0	自定义 ----*/
	CMD_VENDOR_PACK=0XEF,
	CMD_VENDOR=0XF0,
	CMD_UNSUPPORT,
}app_cmd_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool template_init(void);
bool template_deinit(void);
void template_task(void* pa);
void template_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





