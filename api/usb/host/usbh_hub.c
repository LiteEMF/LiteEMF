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
#if API_USBH_BIT_ENABLE && ((USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)))
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
  
    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
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

    req.bmRequestType.bits.recipient = (id & 0x0f)? TUSB_REQ_RCPT_OTHER : TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
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

    req.bmRequestType.bits.recipient = (id & 0x0f)? TUSB_REQ_RCPT_OTHER : TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
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

    req.bmRequestType.bits.recipient = (id & 0x0f)? TUSB_REQ_RCPT_OTHER : TUSB_REQ_RCPT_DEVICE;;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = HUB_REQUEST_GET_STATUS;

    req.wValue = 0;
    req.wIndex = SWAP16_L(id & 0x0f);;
    req.wLength = SWAP16_L(4);

    err = usbh_ctrl_transfer(id&0xF0, &req, pstatus, &tr_len);         //注意实际发送的对象是HUB
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
    uint8_t hub_stu[4];

    if(ERROR_SUCCESS == usbh_hub_port_get_status(id, hub_stu)){
        logd("hub%d_stu:",(uint16_t)id);dumpd(hub_stu,4);
    }

	return ERROR_SUCCESS;
}

error_t usbh_hub_port_reset(uint8_t id)  
{
    error_t err;
    uint8_t hub_stu[4];
    
    err = usbh_hub_port_get_status(id, hub_stu);
    if(err) return err;

    err = usbh_hub_port_set_feature(id, HUB_FEATURE_PORT_RESET); 
    return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_hub_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
    error_t err;
    uint8_t i, hub_id,port_num;
    uint8_t hub_stu[4];
    hub_status_response_t* phub_status = (hub_status_response_t*)hub_stu;
    hub_port_status_response_t* pport_status = (hub_port_status_response_t*)hub_stu;

    id &= 0xf0;
    port_num = (uint8_t)((uintptr_t)(pclass->pdat));
    if (buf[0] & BIT(0)){   // Hub bit 0 is for the hub device events
        err = usbh_hub_port_get_status(id, hub_stu);
        if(ERROR_SUCCESS == err){
            logd("hub%d_stu:",(uint16_t)id);dumpd(hub_stu,4);
            phub_status->status.value = SWAP16_L(phub_status->status.value);
            phub_status->change.value = SWAP16_L(phub_status->change.value);
            if(phub_status->change.bits.local_power_source){
                err = usbh_hub_port_clear_feature(id,HUB_FEATURE_HUB_LOCAL_POWER_CHANGE); 
            }else if(phub_status->change.bits.over_current){
                err = usbh_hub_port_clear_feature(id,HUB_FEATURE_HUB_OVER_CURRENT_CHANGE); 
            }
        }
    }else{                  // Hub bits 1 to n are hub port events
        for(i = 1; i <= port_num; i++){
            if(buf[0] & BIT(i)){
                hub_id = id | i;

                err = usbh_hub_port_get_status(hub_id, hub_stu);
                if(err) return;

                logd("hubport%d_stu:",(uint16_t)hub_id);dumpd(hub_stu,4);
                pport_status->status.value = SWAP16_L(pport_status->status.value);
                pport_status->change.value = SWAP16_L(pport_status->change.value);

                if(pport_status->change.bits.connection){         // Connection change
                    // Acknowledge Port Connection Change
                    err = usbh_hub_port_clear_feature(hub_id,HUB_FEATURE_PORT_CONNECTION_CHANGE);
                    if(err) return;

                    if(pport_status->status.bits.connection){         // Connection
                        usbh_det_event(hub_id, true); 
                    }else{
                        usbh_det_event(hub_id, false);
                    }
                }else{                  /// Clear other port status change interrupts. TODO Not currently handled - just cleared.
                    if(pport_status->change.bits.port_enable){
                        err = usbh_hub_port_clear_feature(hub_id,HUB_FEATURE_PORT_ENABLE_CHANGE);
                    }else if(pport_status->change.bits.suspend){
                        err = usbh_hub_port_clear_feature(hub_id,HUB_FEATURE_PORT_SUSPEND_CHANGE);
                    }else if(pport_status->change.bits.over_current){
                        err = usbh_hub_port_clear_feature(hub_id,HUB_FEATURE_PORT_OVER_CURRENT_CHANGE);
                    }else if(pport_status->change.bits.reset){
                        usbh_dev_t* pdev = get_usbh_dev(hub_id);
                        err = usbh_hub_port_clear_feature(hub_id, HUB_FEATURE_PORT_RESET_CHANGE);

                        if(TUSB_STA_POWERED == pdev->state){
                            usbh_set_status(hub_id, TUSB_STA_DEFAULT, 0);
                            
                            if(pport_status->status.bits.low_speed){
                                pdev->speed = TUSB_SPEED_LOW;
                            }else if(pport_status->status.bits.high_speed){
                                pdev->speed = TUSB_SPEED_HIGH;
                            }else{
                                pdev->speed = TUSB_SPEED_FULL;
                            }
                            logd("usbh%d speed=%d...\n", (uint16_t)hub_id, (uint16_t)pdev->speed);
                        }
                    }
                }
                usbh_hub_port_get_status(hub_id, hub_stu);
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

	if ((TUSB_CLASS_HUB == pclass->itf.if_cls) && (0 == (id & 0x0F)) ){  //不支持HUB级联
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
    uint8_t i,port_num;
    uint8_t buf[4];
    usbh_dev_t* pdev = get_usbh_dev(id);
    uint8_t hub_stu[4];

    id &= 0XF0;                             //not support hub connect hub
    port_num = (uint8_t)((uintptr_t)(pclass->pdat));
    // May need to GET_STATUS
    err = usbh_hub_port_get_status(id, buf);
    if(err) return err;
    logd("hub%d_stu=",(uint16_t)id);	  dumpd(buf,4);

    // Set Port Power to be able to detect connection, starting with port 1
    for(i = 1; i <= port_num; i++){
        err = usbh_hub_port_set_feature(id | i, HUB_FEATURE_PORT_POWER); 
        if(err) return err;
    }

    for(i = 1; i <= port_num; i++){
        usbh_hub_port_get_status(id | i, hub_stu);
    }

    usbh_hub_port_get_status(id, hub_stu);

    pdev->class_ready = true;
    usbh_set_status(id, TUSB_STA_CONFIGURED, 0);   

    return err;
}


error_t usbh_hub_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
    error_t err;
    descriptor_hub_desc_t desc;

    id &= 0XF0;                             //not support hub connect hub
    err = usbh_hub_get_desc(id, &desc);
    if(err) return err;

    if(pclass->endpin.interval > 10) pclass->endpin.interval = 10;     //most hub interval is 0xff
    pclass->pdat = (void*)((uintptr_t)desc.bNbrPorts);
    logd("hub port=%d\n",(uint16_t)desc.bNbrPorts);

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
void usbh_hub_task(uint8_t id, usbh_class_t *pclass)
{
    UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}

#endif






