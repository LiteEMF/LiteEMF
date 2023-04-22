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
#include "hw_config.h"
#if APP_CMD_ENABLE
#include "api/api_transport.h"
#include "app/app_command.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

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
bool app_command_decode(trp_handle_t *phandle,uint8_t* buf,uint8_t len)
{
	bool ret = false;
	command_head_t *phead = (command_head_t*)buf;

	switch(phead->cmd){
	/*---- 0x00	设备信息 ----*/
	case CMD_GET_EMF_VERSION:
		break;
	case CMD_GET_UUID:
		break;
	case CMD_GET_DEV_VERSION:
		break;
	case CMD_GET_MODE:
		break;
	case CMD_GET_MTU:
		break;
	//case //6,7
	case CMD_DEV_PID_VID:
		break;
	case CMD_DEV_SN:
		break;
	case CMD_DEV_MAC:
		break;
	case CMD_DEV_EID:
		break;
	case CMD_DEV_NAME:
		break;

	/*---- 0x10	设备信息 ----*/
	case CMD_DEV_MODE:
		break;
	case CMD_TEST_MODE:
		break;
	case CMD_DEV_CTRL:
		break;
		
	case CMD_RECOVER_FACTORY:
		break;
	case CMD_HEART_BEAT:
		break;
	case CMD_STORAGE_SYNC:
		break;
	default:
		break;
	}
	return ret;
}


#endif






