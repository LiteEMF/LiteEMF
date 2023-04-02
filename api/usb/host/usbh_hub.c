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

//@ref: https://github.com/hathach/tinyusb/blob/master/src/host/hub.c
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "hw_config.h"
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
#include "api/usb/host/usbh.h"

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
** Description: usb hub 
*******************************************************************/
error_t usbh_hub_get_desc(uint8_t id, descriptor_hub_desc_t* pdesc)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;
  
    req.bmRequestType.bit.recipient = USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_CLASS;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = HUB_REQUEST_GET_DESCRIPTOR;

    req.wValue = 0; 
    req.wIndex = 0;
    req.wLength = SWAP16_L(sizeof(descriptor_hub_desc_t));

    err = usbh_ctrl_transfer(id&0xF0, &req, (uint8_t*)pdesc, &tr_len);    //注意实际发送的对象是HUB
    return err;
}

error_t usbh_hub_port_clear_feature(uint8_t id, uint8_t feature)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = (id & 0x0f)? USB_REQ_RCPT_OTHER : USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_CLASS;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = HUB_REQUEST_CLEAR_FEATURE;

    req.wValue = SWAP16_L(feature);
    req.wIndex = SWAP16_L(id & 0x0f);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id&0xF0, &req, NULL, NULL);
    return err;
}

error_t usbh_hub_port_set_feature(uint8_t id, uint8_t feature)      //注意实际发送的对象是HUB
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bit.recipient = (id & 0x0f)? USB_REQ_RCPT_OTHER : USB_REQ_RCPT_DEVICE;
    req.bmRequestType.bit.type = USB_REQ_TYPE_CLASS;
	req.bmRequestType.bit.direction = USB_DIR_OUT;
    req.bRequest = HUB_REQUEST_SET_FEATURE;

    req.wValue = SWAP16_L(feature);     //Idle rate = 0 mean only report when there is changes
    req.wIndex = SWAP16_L(id & 0x0f);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id&0xF0, &req, NULL, NULL);    //注意实际发送的对象是HUB
    return err;
}

/*******************************************************************
** Parameters:	pstatus: hub_status_response_t, or hub_port_status_response_t
** Returns:	    
** Description:
*******************************************************************/
error_t usbh_hub_port_get_status(uint8_t id, uint8_t* pstatus)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bit.recipient = (id & 0x0f)? USB_REQ_RCPT_OTHER : USB_REQ_RCPT_DEVICE;;
    req.bmRequestType.bit.type = USB_REQ_TYPE_CLASS;
	req.bmRequestType.bit.direction = USB_DIR_IN;
    req.bRequest = HUB_REQUEST_GET_STATUS;

    req.wValue = 0;
    req.wIndex = SWAP16_L(id & 0x0f);;
    req.wLength = SWAP16_L(4);

    err = usbh_ctrl_transfer(id&0xF0, &req, NULL, &tr_len);         //注意实际发送的对象是HUB
    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description: 应该先读取状态,如果port 状态是enable 才会清除port
        HUB_FEATURE_PORT_ENABLE 是reset后自动enable的
*******************************************************************/
error_t usbh_hub_port_disable(uint8_t id)  
{
    error_t err;
    uint8_t buf[4];
    hub_port_status_response_t* pport_status = (hub_port_status_response_t*)buf;

    //如果枚举设备通讯出错可能导致usbh_hub_port_get_status返回失败,这里不清楚是HUB问题还是芯片问题
    //临时处理方法:如果失败直接清除HUB_PORT_ENABLE
    err = usbh_hub_port_get_status(id, buf);
    pport_status->change.value = SWAP16_L(pport_status->change.value);
    if(pport_status->status.bit.port_enable || err){
        err = usbh_hub_port_clear_feature(id, HUB_FEATURE_PORT_ENABLE); 

        if(ERROR_SUCCESS == usbh_hub_port_get_status(id, buf)){
            logd("hub%d_port_stu;",(uint16_t)id);
            dumpd(buf,4);
        }
    }
}

error_t usbh_hub_port_reset(uint8_t id)  
{
    error_t err;
    err = usbh_hub_port_clear_feature(id, HUB_FEATURE_PORT_RESET); 
    return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdata used storage hub port numbers	
*******************************************************************/
void usbh_hub_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
    error_t err;
    uint8_t i;
    uint8d_t hub_stu[4];
    hub_status_response_t* phub_status = (hub_status_response_t*)buf;
    hub_port_status_response_t* pport_status = (hub_port_status_response_t*)buf;

    id &= 0xf0;
    
    if (buf[0] & BIT(0)){   // Hub bit 0 is for the hub device events
        err = usbh_hub_port_get_status(id, hub_stu);
        if(ERROR_SUCCESS == err){
            logd("hub%d_stu:",(uint16_t)id);dumpd(hub_stu,4);
            if(phub_status->change.bit.local_power_source){
                err = usbh_hub_port_clear_feature(id,HUB_FEATURE_HUB_LOCAL_POWER_CHANGE); 
            }else if(phub_status->change.bit.over_current){
                err = usbh_hub_port_clear_feature(id,HUB_FEATURE_HUB_OVER_CURRENT_CHANGE); 
            }
        }
    }else{                  // Hub bits 1 to n are hub port events
        for(i = 1; i < (uint8_t)(pclass->pdata); i++){
            if(buf[0] & BIT(i)){
                err = usbh_hub_port_get_status(id | i, hub_stu);
                if(err) return;

                logd("hub%d_portstu:",(uint16_t)id);dumpd(hub_stu,4);

                if(pport_status->change.bit.connection){         // Connection change
                    // Acknowledge Port Connection Change
                    err = usbh_hub_port_clear_feature(id | i,HUB_FEATURE_PORT_CONNECTION_CHANGE);
                    if(err) return;

                    if(pport_status->status.bit.connection){         // Connection
                        err = usbh_hub_port_reset(id | i);
                        if(err) return;

                        usbh_det_event(id,true); 
                    }else{
                        usbh_det_event(id | i,false);
                    }
                }else{                  /// Clear other port status change interrupts. TODO Not currently handled - just cleared.
                    if(pport_status->change.bit.port_enable){
                        err = usbh_hub_port_clear_feature(id | i,HUB_FEATURE_PORT_ENABLE_CHANGE);
                    }else if(pport_status->change.bit.suspend){
                        err = usbh_hub_port_clear_feature(id | i,HUB_FEATURE_PORT_SUSPEND_CHANGE);
                    }else if(pport_status->change.bit.over_current){
                        err = usbh_hub_port_clear_feature(id | i,HUB_FEATURE_PORT_OVER_CURRENT_CHANGE);
                    }else if(pport_status->change.bit.reset){
                        err = usbh_hub_port_clear_feature(id | i,HUB_FEATURE_PORT_RESET_CHANGE);
                    }
                }
            }
        }
    }
}
    
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_hub( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;

	if ((USB_CLASS_HUB == pclass->itf.if_cls) && (0 == (id & 0x0F)) ){  //不支持HUB级联
		if(pclass->endpin.addr ){
			err = ERROR_SUCCESS;
		}
	}

	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hub_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err;
    uint8_t i;
    uint8_t buf[4];
    usbh_dev_t* pdev = get_usbh_dev(id);

    id &= 0XF0;                             //not support hub connect hub
    // May need to GET_STATUS
    err = usbh_hub_port_get_status(id, buf);
    if(err) return err;
    logd("hub%d_stu=%d\n",(uint16_t)id);
    dumpd(buf,4);

    // Set Port Power to be able to detect connection, starting with port 1
    for(i = 1; i <= (uint8_t)(pclass->pdata); i++){
        err = usbh_hub_port_set_feature(id | i, HUB_FEATURE_PORT_POWER); 
        if(err) return err;
    }

    pdev->class_ready = true;
    usbh_set_status(id, USB_STA_CONFIGURED, 0);   

    return err;
}


error_t usbh_hub_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
    error_t err;
    descriptor_hub_desc_t desc;

    id &= 0XF0;                             //not support hub connect hub
    err = usbh_hub_get_desc(id, &desc);
    if(err) return err;

    pclass->pdata = (void*)desc.bNbrPorts;
    logd("hub port=%d\n",desc.bNbrPorts);

    return err;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hub_deinit( uint8_t id, usbh_class_t *pclass) 
{
    UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hub_handler(uint8_t id, usbh_class_t *pclass)
{
    UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}

#endif






