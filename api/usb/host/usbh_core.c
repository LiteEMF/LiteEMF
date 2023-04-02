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
#if APP_USBH_ENABLE
#include "api/usb/host/usbh_core.h"


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
extern error_t usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen);
/*******************************************************************
** Parameters:	id: usb id
                type: request desc type 
                index: request desc index
                language_id: request language id
                buf: out buf
                *plen: 输入请求描述符长度, 返回实际读取到的长度
** Returns:	    
** Description:		
*******************************************************************/
static error_t usbh_get_desc( uint8_t id , uint8_t type, uint8_t index, uint16_t language_id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len = *plen;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = USB_REQ_GET_DESCRIPTOR;

    req.wValue = SWAP16_L(U16(type,index));
    req.wIndex = language_id;
    req.wLength = SWAP16_L(tr_len);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len );                    // 执行控制传输
    *plen = tr_len;

    return err;
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	获取usb描述符
*******************************************************************/
error_t usbh_get_device_desc(uint8_t id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
    uint16_t tr_len = MIN(*plen, sizeof(usb_desc_device_t));
    err = usbh_get_desc(id,USB_DESC_DEVICE,0,0,buf,&tr_len);
    *plen = tr_len;
    return err;
}

error_t usbh_get_configuration_desc(uint8_t id, uint8_t index, uint8_t* buf, uint16_t *plen)  
{
    return usbh_get_desc(id,USB_DESC_CONFIGURATION,index,0,buf, plen);
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	获取usb string 描述符
*******************************************************************/
error_t usbh_get_string_desc(uint8_t id, uint8_t index, uint16_t language_id, uint8_t* buf, uint16_t *plen)  
{
  return usbh_get_desc(id,USB_DESC_STRING,index,language_id,buf,plen);
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description: usb set 
*******************************************************************/
error_t usbh_set_addr(uint8_t id, uint8_t addr)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = USB_REQ_SET_ADDRESS;

    req.wValue = SWAP16_L(addr);
    req.wIndex = 0;
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    //延时10ms?
    return err;
}

error_t usbh_get_status(uint8_t id, uint8_t* pstatus)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = USB_REQ_GET_STATUS;

    req.wValue = 0;
    req.wIndex = 0;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pstatus, &tr_len);
    return err;
}

error_t usbh_get_configuration(uint8_t id, uint8_t* pcfg)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = USB_REQ_GET_CONFIGURATION;

    req.wValue = 0;
    req.wIndex = 0;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pcfg, &tr_len);
    return err;
}
error_t usbh_set_configuration(uint8_t id, uint8_t cfg)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = USB_REQ_SET_CONFIGURATION;

    req.wValue = SWAP16_L(cfg);
    req.wIndex = 0;
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_get_intercace(uint8_t id, uint8_t inf, uint8_t *palt)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = USB_REQ_GET_INTERFACE;

    req.wValue = 0;
    req.wIndex = SWAP16_L(inf);;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, palt, &tr_len);
    return err;
}
error_t usbh_set_intercace(uint8_t id, uint8_t inf, uint8_t alt)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = USB_REQ_SET_INTERFACE;

    req.wValue = SWAP16_L(alt);
    req.wIndex = SWAP16_L(inf);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_endp_get_status(uint8_t id, uint8_t endp, uint8_t* pstatus)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = USB_REQ_GET_STATUS;

    req.wValue = 0;
    req.wIndex = SWAP16_L(endp);
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pstatus, &tr_len);
    return err;
}
error_t usbh_endp_clean_feature(uint8_t id, uint8_t endp)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = USB_REQ_CLEAR_FEATURE;

    req.wValue = 0;
    req.wIndex = SWAP16_L(endp);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_endp_set_feature(uint8_t id, uint8_t endp, usb_request_feature_selector_t feature)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = USB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bit.type = USB_REQ_TYPE_STANDARD;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = USB_REQ_SET_FEATURE;

    req.wValue = SWAP16_L(feature);
    req.wIndex = SWAP16_L(endp);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}


#endif






