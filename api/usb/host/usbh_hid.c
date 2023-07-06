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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA)))

#include "api/usb/host/usbh.h"
#include  "api/hid/hid_desc_parser.h"


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
** Description: usb hid 
*******************************************************************/
error_t usbh_hid_set_idle(uint8_t id, uint8_t itf)  
{
    error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = HID_REQ_CONTROL_SET_IDLE;

    req.wValue = 0;     //Idle rate = 0 mean only report when there is changes
    req.wIndex = SWAP16_L(itf);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}
    
error_t usbh_hid_set_report(uint8_t id, uint8_t itf, hid_report_type_t type, uint8_t rep_id, uint8_t* buf, uint16_t len)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = HID_REQ_CONTROL_SET_REPORT;

    req.wValue = SWAP16_L(U16(type,rep_id));
    req.wIndex = SWAP16_L(itf);
    req.wLength = SWAP16_L(len);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len);

    return err;
}

error_t usbh_hid_get_report(uint8_t id, uint8_t itf, hid_report_type_t type, uint8_t rep_id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = HID_REQ_CONTROL_GET_REPORT;

    req.wValue = SWAP16_L(U16(type,rep_id));
    req.wIndex = SWAP16_L(itf);
    req.wLength = SWAP16_L(*plen);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len);
	*plen = tr_len;
    return err;
}

error_t usbh_hid_get_report_desc( uint8_t id, uint8_t index, uint16_t itf, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_STANDARD;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = TUSB_REQ_GET_DESCRIPTOR;

    req.wValue = SWAP16_L(U16(HID_DESC_TYPE_REPORT,index));
    req.wIndex = SWAP16_L(itf);
    req.wLength = SWAP16_L(*plen);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len );                    // 执行控制传输
    *plen = tr_len;
    return err;
}


/*******************************************************************
** Parameters:	buf,len: desc buf and len
                dir: endp dir
** Returns:	    hid report desc len
** Description:  
*******************************************************************/
uint16_t usbh_get_hid_desc_len(uint8_t* buf ,uint16_t len) 
{  
    uint16_t i, l;
    uint8_t inf_num=0;
    usb_hid_descriptor_hid_t *pdesc;
    
    for ( i = 0; i < len; i += l ){
        l = buf[i];
        if(0 == l) break;

    	if( buf[i+1] == TUSB_DESC_INTERFACE){
    		inf_num++;
    		if(inf_num > 1) break;
    	}
        pdesc = (usb_hid_descriptor_hid_t*)(buf+i);
        if(pdesc->bDescriptorType == HID_DESC_TYPE_HID){
            
            return SWAP16_L(pdesc->wReportLength);
        }
    }
    return 0;
}







/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_hid_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	// logd("hid endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);

    switch(pclass->hid_type){
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
            usbh_hid_km_in_process(id, pclass, buf, len);
            break;
        case HID_TYPE_GAMEPADE:
            #if (HIDH_SUPPORT & HID_GAMEPAD_MASK)
            usbh_hid_gamepad_in_process(id, pclass, buf, len);
            #endif
            break;
        default:
            break;
    }
    
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_hid( uint8_t id, usbh_class_t *pclass)
{
    error_t err = ERROR_NOT_FOUND;
    if(TUSB_CLASS_HID == pclass->itf.if_cls){
        err = ERROR_SUCCESS;
    }else if(TUSB_CLASS_VENDOR == pclass->itf.if_cls){
        if((XBOX_SUBCLASS == pclass->itf.if_sub_cls) || (X360_SUBCLASS == pclass->itf.if_sub_cls)){
            if(pclass->endpin.addr && (TUSB_ENDP_TYPE_INTER == pclass->endpin.type)){
                err = ERROR_SUCCESS;
            }
        }
    }
	return err;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNKNOW;
	usbh_dev_t* pdev = get_usbh_dev(id);

    switch(pclass->hid_type){
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
            err = usbh_hid_km_open(id, pclass);
        logd("usbh_hid_open km %d\n",err);
            break;
        case HID_TYPE_GAMEPADE:
            #if (HIDH_SUPPORT & HID_GAMEPAD_MASK)
            err = usbh_hid_gamepad_open(id, pclass);
            #endif
            break;
        default:
            break;
    }
    
    if(err) return err;
    err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    if(err) return err;
logd("usbh_hid_open err= %d\n",err);
    if(ERROR_SUCCESS == err) pdev->class_ready = true;
    return err;
}

error_t usbh_hid_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
    error_t err = ERROR_SUCCESS;
    uint16_t desc_len; 
    uint8_t *desc_buf;
    hid_desc_info_t hid_info;

    if(TUSB_CLASS_HID == pclass->itf.if_cls){            //except xbox
        desc_len = usbh_get_hid_desc_len(pdesc, len);
        if(0 == desc_len) return ERROR_UNSUPPORT;

        err = usbh_hid_set_idle(id, pclass->itf.if_num);
        // if(err) return err;          //note:  dev can STALL

        desc_buf = emf_malloc(desc_len);
        if(NULL == desc_buf) return ERROR_NO_MEM;
        
        err = usbh_hid_get_report_desc(id, 0, pclass->itf.if_num, desc_buf, &desc_len);
        if(err) return err;

        //pase report desc
        err = hid_desc_parse_report(&hid_info, desc_buf, desc_len);
        emf_free(desc_buf);

        if(ERROR_SUCCESS == err){
            hid_desc_dump(&hid_info);
            err = usbh_hid_km_init(id, pclass, &hid_info);
            if(err){
                #if (HIDH_SUPPORT & HID_GAMEPAD_MASK)
                err = usbh_hid_gamepad_init(id, pclass, &hid_info);
                #endif
            }
            hid_desc_info_free(&hid_info);
        }
    }

    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_deinit( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNKNOW;
    switch(pclass->hid_type){
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
            err = usbh_hid_km_deinit(id, pclass);
            break;
        case HID_TYPE_GAMEPADE:
            #if (HIDH_SUPPORT & HID_GAMEPAD_MASK)
            err = usbh_hid_gamepad_deinit(id, pclass);
            #endif
            break;
        default:
            break;
    }
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hid_task(uint8_t id, usbh_class_t *pclass)
{
    switch(pclass->hid_type){
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
            usbh_hid_km_task(id, pclass);
            break;
        case HID_TYPE_GAMEPADE:
            #if (HIDH_SUPPORT & HID_GAMEPAD_MASK)
            usbh_hid_gamepad_task(id, pclass);
            #endif
            break;
        default:
            break;
    }
}

#endif






