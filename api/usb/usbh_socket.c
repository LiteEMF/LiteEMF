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
#if API_USBH_BIT_ENABLE && USBH_SOCKET_ENABLE
#include "api/usb/usbh_socket.h"
#include "api/usb/host/usbh.h"
#include "app/app_commander.h"
#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad.h"
#endif
#if USBD_SOCKET_ENABLE		//共享内存方式通讯
#include "api/usb/usbd_socket.h"
#endif
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
/*记录当前通讯trp,注意: project通过修改这个值来实现不同通讯方式的socket!!!*/
#if USBD_SOCKET_ENABLE 					//通常的默认值,可以在工程修改
trp_handle_t usbh_socket_trp={TR_NULL};
#else
trp_handle_t usbh_socket_trp={TR_UART,0, 0};
#endif

bool usbh_socket_configured=false;
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint16_t usbh_socket_dev = U16(DEV_TYPE_NONE,HID_TYPE_NONE);


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
bool usbh_socket_cmd(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	if(TR_NULL == phandle->trp){
		#if USBD_SOCKET_ENABLE		//共享内存方式通讯
		ret = usbd_socket_decode(phandle,cmd,buf,len);
		#endif
	}else{
		ret = app_command_tx(phandle,cmd,buf,len);
	}
	return ret;
}


bool usbh_socket_decode(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len)
{
	error_t err = ERROR_STALL;
	uint8_t id;
    usbh_class_t* pcalss = NULL;
	usb_control_request_t *preq;
	uint8_t *req_buf = NULL;
	uint16_t req_len = 0;

	if(memcmp(phandle, &usbh_socket_trp, sizeof(usbh_socket_trp))){
		logd( "usbh socket trp err!\n");
		return err;
	}

	id = usbh_class_find_by_type_all(usbh_socket_dev>>8,usbh_socket_dev&0XFF, &pcalss);
    if (id == USBH_NULL){
		if(CMD_SOCKET_SETUP == cmd){		//防止未接手柄时指令一直不回复
			usbh_socket_cmd(phandle,CMD_SOCKET_SETUP_STALL,NULL,0);
		}
		logd( "usbh socket=%X not find!\n",usbh_socket_dev);
		return err;
	}

	switch(cmd){
		case CMD_SOCKET_READY:
			usbh_socket_configured = buf[0];
			break;
		case CMD_SOCKET_SETUP:
			preq = (usb_control_request_t*)buf;

			//简单处理替换接口号 //TODO 在对应设备中处理
			if((TUSB_REQ_RCPT_INTERFACE == preq->bmRequestType.bits.recipient)){
				uint8_t itf = preq->wIndex & 0XFF;	

				if(TUSB_REQ_TYPE_STANDARD == preq->bmRequestType.bits.type){
					preq->wIndex = (preq->wIndex & 0XFF00) | pcalss->itf.if_num;
				}else{	//不是所有itf recpt 请求的windexL都是接口号, 这里特殊处理
					if(usbh_socket_dev == U16(DEV_TYPE_HID, HID_TYPE_X360)){	//x360 特殊处理	
						#if HIDH_SUPPORT & HID_XBOX_MASK
						if(0X81 <= preq->bRequest && 0X87 >= preq->bRequest){
							preq->wIndex = (preq->wIndex & 0XFF00) | m_x360_identify_itf;
						}
						#endif
					}else{		//ps4走这里
						preq->wIndex = (preq->wIndex & 0XFF00) | pcalss->itf.if_num;
					}
				}
			}

			if(TUSB_DIR_IN == preq->bmRequestType.bits.direction){
				req_buf = emf_malloc(preq->wLength);
				if(NULL != req_buf){
					err = usbh_ctrl_transfer( id, preq, req_buf, &req_len);
				}
				logd("usbh socket setup=%x:",(uint16_t)err); dumpd(preq,8);
				dumpd(req_buf,req_len);
			}else{
				err = usbh_ctrl_transfer( id, preq, buf+8, NULL);
				logd("usbh socket setup=%x:",(uint16_t)err); dumpd(preq,8);
				dumpd(buf+8,preq->wLength);
			}
			
			if ( err == ERROR_SUCCESS ){
				usbh_socket_cmd(phandle,CMD_SOCKET_SETUP_ACK,req_buf,req_len);
		    }else if(ERROR_NACK != err){
				usbh_socket_cmd(phandle,CMD_SOCKET_SETUP_STALL,NULL,0);
			}
			emf_free(req_buf);
			break;
		case CMD_SOCKET_OUT:	
			// logd("usbh CMD_SOCKET_OUT");dumpd(buf, len);	
			err = usbh_out(id, &pcalss->endpout ,buf, len);
			break;
		
	}
	return (err == ERROR_SUCCESS);
}


void usbh_socket_init(trp_handle_t* phandle, uint16_t trp_index)
{
	usbh_socket_configured = false;
	usbh_socket_trp = *phandle;
	usbh_socket_dev = trp_index;
	logd_r("usbh socket init=%x",usbh_socket_dev);

	trp_index = SWAP16_L(usbh_socket_dev);
	usbh_socket_cmd(phandle,CMD_SOCKET_SYNC,&trp_index,2);
}
#endif

