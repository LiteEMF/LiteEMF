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
#if API_USBH_BIT_ENABLE
#include "api/usb/host/usbh_core.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
usbh_dev_t m_usbh_dev[USBH_NUM][HUB_MAX_PORTS+1];


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
** Parameters:	id: usb id
                preq: request 请求
                buf: 发送/接收数据buf
                plen:   输入:发送数据长度/ 接收数据长度 
                        输出:实际发送/接收的数据长度
** Returns:	
** Description:	执行控制传输
*******************************************************************/
error_t usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen)
{
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    usbh_select_hub_port(id);
    return hal_usbh_ctrl_transfer(id, preq, buf, plen);
}

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
static error_t usbh_req_get_desc( uint8_t id , uint8_t type, uint8_t index, uint16_t language_id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len = *plen;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_DESCRIPTOR;

    req.wValue = SWAP16_L(U16(type,index));
    req.wIndex = language_id;
    req.wLength = SWAP16_L(tr_len);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len );                    // 执行控制传输
    *plen = tr_len;

    return err;
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	获取usb描述符
*******************************************************************/
error_t usbh_req_get_device_desc(uint8_t id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
    uint16_t tr_len = MIN(*plen, sizeof(usb_desc_device_t));
    err = usbh_req_get_desc(id,TUSB_DESC_DEVICE,0,0,buf,&tr_len);
    *plen = tr_len;
    return err;
}

error_t usbh_req_configuration_desc(uint8_t id, uint8_t index, uint8_t* buf, uint16_t *plen)  
{
    return usbh_req_get_desc(id,TUSB_DESC_CONFIGURATION,index,0,buf, plen);
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	获取usb string 描述符
*******************************************************************/
error_t usbh_req_get_string_desc(uint8_t id, uint8_t index, uint16_t language_id, uint8_t* buf, uint16_t *plen)  
{
  return usbh_req_get_desc(id,TUSB_DESC_STRING,index,language_id,buf,plen);
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description: usb set 
*******************************************************************/
error_t usbh_req_set_addr(uint8_t id, uint8_t addr)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_SET_ADDRESS;

    req.wValue = SWAP16_L(addr);
    req.wIndex = 0;
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    //延时10ms?
    return err;
}

error_t usbh_req_get_status(uint8_t id, uint8_t* pstatus)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_STATUS;

    req.wValue = 0;
    req.wIndex = 0;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pstatus, &tr_len);
    return err;
}

error_t usbh_req_get_configuration(uint8_t id, uint8_t* pcfg)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_CONFIGURATION;

    req.wValue = 0;
    req.wIndex = 0;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pcfg, &tr_len);
    return err;
}
error_t usbh_req_set_configuration(uint8_t id, uint8_t cfg)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_SET_CONFIGURATION;

    req.wValue = SWAP16_L(cfg);
    req.wIndex = 0;
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_req_get_itf(uint8_t id, uint8_t inf, uint8_t *palt)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_INTERFACE;

    req.wValue = 0;
    req.wIndex = SWAP16_L(inf);;
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, palt, &tr_len);
    return err;
}
error_t usbh_req_set_itf(uint8_t id, uint8_t inf, uint8_t alt)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_SET_INTERFACE;

    req.wValue = SWAP16_L(alt);
    req.wIndex = SWAP16_L(inf);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_req_get_endp_status(uint8_t id, uint8_t endp, uint8_t* pstatus)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_STATUS;

    req.wValue = 0;
    req.wIndex = SWAP16_L(endp);
    req.wLength = SWAP16_L(1);

    err = usbh_ctrl_transfer(id, &req, pstatus, &tr_len);
    return err;
}
error_t usbh_req_clean_endp_feature(uint8_t id, uint8_t endp)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_CLEAR_FEATURE;

    req.wValue = 0;
    req.wIndex = SWAP16_L(endp);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}

error_t usbh_req_set_endp_feature(uint8_t id, uint8_t endp, usb_request_feature_selector_t feature)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_SET_FEATURE;

    req.wValue = SWAP16_L(feature);
    req.wIndex = SWAP16_L(endp);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}




usbh_dev_t* get_usbh_dev(uint8_t id)   
{
    usbh_dev_t *pdev = NULL;

	if (USBH_NUM <= (id>>4)) return NULL;
	if (HUB_MAX_PORTS < (id&0x0f)) return NULL;

	pdev = &m_usbh_dev[id>>4][id&0x0f];

	return pdev;
}

error_t usbh_select_hub_port(uint8_t id)
{
    error_t err;
	usbh_dev_t* pdev = get_usbh_dev(id);

    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    err =  usbh_set_addr(id & 0xf0, pdev->addr );  // set root hub addr
    err =  usbh_set_speed(id, pdev->speed );       // set usb speed
    return err;
}


error_t usbh_port_reset(uint8_t id)
{   
    error_t err;
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;
    err = hal_usbh_port_reset(id);
    logd("usbh%x reset=%d\n",id,err);
    return err;
}
error_t usbh_set_speed(uint8_t id,usb_speed_t speed)
{   
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    return hal_usbh_set_speed(id,speed);
}
error_t usbh_set_addr(uint8_t id,uint8_t addr)
{   
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    return hal_usbh_set_addr(id, addr);
}
error_t usbh_port_en(uint8_t id, uint8_t en, usb_speed_t* pspeed)
{   
    error_t err;
    usbh_dev_t* pdev = get_usbh_dev(id);

    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    err = hal_usbh_port_en(id, en, pspeed);
    pdev->speed = *pspeed;
    logd("usbh%x en=%d,speed=%d,ret=%d\n",id,en,*pspeed,err);
    return err;
}
error_t usbh_endp_unregister(uint8_t id,usb_endp_t *endpp)
{
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    return hal_usbh_endp_unregister(id, endpp);
}
error_t usbh_endp_register(uint8_t id,usb_endp_t *endpp)
{
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    return hal_usbh_endp_register(id,endpp);
}
//timeout_ms: 0不等待
error_t usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen, uint16_t timeout_ms)
{
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    usbh_select_hub_port(id);
    return hal_usbh_in(id,endpp,buf,plen,timeout_ms);
}
error_t usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len)
{
    if((id>>4) >= USBH_NUM) return ERROR_FAILE;

    usbh_select_hub_port(id);
    return hal_usbh_out(id,endpp,buf,len);
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_core_init( uint8_t id )
{
	uint8_t i;
    error_t err = ERROR_UNSUPPORT;

	usbh_dev_t *pdev = (usbh_dev_t *)m_usbh_dev;
	
    memset(m_usbh_dev[id], 0 ,sizeof(m_usbh_dev[id]));

	for(i = 0; i < USBH_NUM * (HUB_MAX_PORTS+1); i++,pdev++){
		INIT_LIST_HEAD(&pdev->class_list);
	}

    if(API_USBH_BIT_ENABLE & BIT(id>>4)){
    	err = hal_usbh_driver_init(id);
	}
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_core_deinit( uint8_t id )
{
	return hal_usbh_driver_deinit(id);
}

#endif






