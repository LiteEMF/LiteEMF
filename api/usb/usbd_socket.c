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
#if USBD_SOCKET_ENABLE
#include "usbd_socket.h"
#include "api/api_transport.h"
#include "api/usb/device/usbd.h"
#include "apP/app_command.h"

#include "api/api_log.h"
// usb socket 用于手柄引导


/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
trp_handle_t usbd_socket_trp;			//TODO
bool usbd_socket_configured=false;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool usbd_socket_art_cmd(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	if(NULL == phandle){
		#if USBH_SOCKET_ENABLED		//共享内存方式通讯
		ret = usbh_socket_arg_decode(phandle,cmd,dev_type,buf,len);
		#endif
	}else{
		ret = app_send_arg_command(phandle,cmd,dev_type,buf,len);
	}
	return ret;
}


bool usbd_socket_arg_decode(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len)
{
	uint8_t err;
	uint8_t id = 0;			//TODO
	usbd_class_t *pclass;
	switch(cmd){
	case CMD_SOCKET_SYNC:		//支持半socket 手柄
		logd("usbd socket sync type=%x ...\n",dev_type);
		usbd_socket_configured = true;
		usbd_socket_trp = *phandle;
		m_usbd_hid_types[id] = BIT(dev_type&0XFF);
		m_usbd_types[id] |= BIT(dev_type>>8); 
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		m_usbd_types[id] |= BIT(DEV_TYPE_AUDIO); 
		#endif

		usbd_class_init(id);		//初始化usbd 类
		usbd_class_reset(id);		//usbd_core_reset 会调用 usbd_core_init 
		break;
	case CMD_SOCKET_SETUP_STALL:
		usbd_endp_stall(id, 0x00);
		break;
	case CMD_SOCKET_SETUP_ACK:
		err = usbd_in(id, 0x00, buf, len);
		break;
	case CMD_SOCKET_IN:
		pclass = usbd_class_find_by_type(id, dev_type>>8, dev_type&0XFF);
		if(pclass){
			err = usbd_in(id, pclass->endpin.addr, buf, len);
		}
		break;
	}
	return (err == ERROR_SUCCESS);
}

void usbd_socket_init(trp_handle_t* phandle, uint16_t dev_type)
{
	usbd_socket_configured = false;
}
#endif
