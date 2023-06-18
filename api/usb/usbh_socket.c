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
#if USBH_SOCKET_ENABLED
#include "api/usb/usbh_socket.h"
#include "api/usb/host/usbh.h"
#include "api/usb/host/usbh.h"
#include "apP/app_command.h"
#if USBD_SOCKET_ENABLED		//共享内存方式通讯
#include "api/usb/usbd_socket.h"
#endif
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
trp_handle_t usbh_socket_trp;			//TODO
bool usbh_socket_configured=false;
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
bool usbh_socket_art_cmd(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	if(NULL == phandle){
		#if USBD_SOCKET_ENABLED		//共享内存方式通讯
		ret = usbd_socket_arg_decode(phandle,cmd,dev_type,buf,len);
		#endif
	}else{
		ret = api_command_arg_tx(phandle,cmd,dev_type,buf,len);
	}
	return ret;
}


bool usbh_socket_arg_decode(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len)
{
	error_t err = ERROR_STALL;
	uint8_t id;
    usbh_class_t* pcalss = NULL;
	usb_control_request_t *preq;
	uint8_t *req_buf = NULL;
	uint16_t req_len = 0;


	id = usbh_class_find_by_type_all(dev_type>>8,dev_type&0XFF, &pcalss);
    if (id == USBH_NULL){
		return err;
	}

	switch(cmd){
		case CMD_SOCKET_CONFIGURED:
			usbh_socket_configured = buf[0];
			break;
		case CMD_SOCKET_SETUP:
			preq = (usb_control_request_t*)buf;
			if(USB_DIR_IN == preq->bmRequestType.bits.direction){
				req_buf = emf_malloc(preq->wLength);
				if(NULL != req_buf){
					err = usbh_ctrl_transfer( id, preq, req_buf, &req_len);
				}
			}else{
				err = usbh_ctrl_transfer( id, preq, buf+8, NULL);
			}
			
			if ( err == ERROR_SUCCESS ){
				usbh_socket_art_cmd(phandle,CMD_SOCKET_SETUP_ACK,0,req_buf,req_len);
		    }else if(ERROR_STALL == err){
				usbh_socket_art_cmd(phandle,CMD_SOCKET_SETUP_STALL,0,NULL,0);
			}
			break;

		case CMD_SOCKET_OUT:			
			err = usbh_out(id, &pcalss->endpout ,buf, len);
			break;
		
	}
	return (err == ERROR_SUCCESS);;
}


void usbh_socket_init(trp_handle_t* phandle, uint16_t dev_type)
{
	usbh_socket_configured = false;
	usbh_socket_trp = *phandle;
	usbh_socket_art_cmd(phandle,CMD_SOCKET_SYNC,dev_type,NULL,0);
}
#endif

