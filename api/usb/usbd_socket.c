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
#if API_USBD_BIT_ENABLE && USBD_SOCKET_ENABLE
#include "api/usb/usbd_socket.h"
#include "api/usb/device/usbd.h"
#include "api/usb/device/usbd.h"

#if USBH_SOCKET_ENABLE		//共享内存方式通讯
#include "api/usb/usbh_socket.h"
#endif

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
trp_handle_t usbd_socket_trp={TR_NULL}; //记录当前通讯trp, 由主机发起通讯,自动记录通讯trp,project不需要修改!!
bool usbd_socket_configured=false;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/
static uint16_t usbd_socket_dev;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool usbd_socket_cmd(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	if(TR_NULL == phandle->trp){
		#if USBH_SOCKET_ENABLE		//共享内存方式通讯
		ret = usbh_socket_decode(phandle,cmd,buf,len);
		#endif
	}else{
		ret = api_command_tx(phandle,cmd,buf,len);
	}
	
	return ret;
}


bool usbd_socket_decode(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len)
{
	uint8_t err = ERROR_FAILE;
	uint8_t id = usbd_socket_trp.id;
	usbd_class_t *pclass;
	uint16_t dev_type;

	switch(cmd){
	case CMD_SOCKET_SYNC:		//支持半socket 手柄
		memcpy(&dev_type, buf, 2);
		usbd_socket_dev = SWAP16_L(dev_type);
		logd("usbd socket sync type=%x ...\n",usbd_socket_dev);
		usbd_socket_configured = true;
		usbd_socket_trp = *phandle;
		id = usbd_socket_trp.id;
		m_usbd_hid_types[id] = BIT(usbd_socket_dev&0XFF);
		m_usbd_types[id] |= BIT(usbd_socket_dev>>8);
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		m_usbd_types[id] |= BIT(DEV_TYPE_AUDIO); 
		#endif

		usbd_class_init(id);		//初始化usbd类
		usbd_reset(id);
		usbd_core_init(id);
		break;
	case CMD_SOCKET_SETUP_STALL:
		usbd_endp_stall(id, 0x00);
		break;
	case CMD_SOCKET_SETUP_ACK:
		err = usbd_in(id, 0x00, buf, len);
		break;
	case CMD_SOCKET_IN:
		pclass = usbd_class_find_by_type(id, usbd_socket_dev>>8, usbd_socket_dev&0XFF);
		if(pclass){
			err = usbd_in(id, pclass->endpin.addr, buf, len);
		}
		break;
	}
	return (err == ERROR_SUCCESS);
}

void usbd_socket_init(void)
{
	usbd_socket_configured = false;
}
#endif
