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
bool usbd_socket_en = true;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
trp_handle_t usbd_socket_trp={TR_NULL, 0, 0}; //记录当前通讯trp, 由主机发起通讯,自动记录通讯trp,project不需要修改!!
/*****************************************************************************************************
**	static Function
******************************************************************************************************/
static uint16_t usbd_socket_dev = U16(DEV_TYPE_NONE, DEF_HID_TYPE_NONE);

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



error_t usbd_socket_control_request_process(uint8_t id, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;
	uint8_t* p;
	uint8_t out_len = 0;

	if(TUSB_DIR_OUT == preq->req.bmRequestType.bits.direction){
		out_len = preq->req.wLength;
	}
	p = emf_malloc(out_len + 8);
	if(NULL != p){
		memcpy(p, &preq->req, 8);
		memcpy(p+8, preq->setup_buf, out_len);
		if(usbd_socket_cmd(&usbd_socket_trp, CMD_SOCKET_SETUP, p, out_len + 8)){
			err = ERROR_NACK;
		}
		emf_free(p);
	}
	return err;
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
		// logd("CMD_SOCKET_IN");dumpd(buf, len);
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
	usbd_socket_trp.trp = TR_NULL;
	usbd_socket_dev = U16(DEV_TYPE_NONE, DEF_HID_TYPE_NONE);
}
#endif
