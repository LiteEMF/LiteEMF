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
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
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
** Description:	
*******************************************************************/
bool usbh_hid_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	// logd("hid endp%d in%d:",(uint16_t)pclass->endpin.addr,len);dumpd(buf,len);
    #if (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
    if((pclass->hid_types) & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE))){
        if(usbh_hid_km_in_process(id, pclass, buf, len)){
            return true;
        }
    }
    #endif
    #if (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
    if((pclass->hid_types) & BIT(HID_TYPE_VENDOR)){      //hid vendor没有ID,不会和其他设备复合
        if(usbh_hid_vendor_in_process(id, pclass, buf, len)){
            return true
        }
    }
    #endif
    #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)           //gamepad比较复杂放到最后
    if((pclass->hid_types) & HID_GAMEPAD_MASK){
        if(usbh_hid_gamepad_in_process(id, pclass, buf, len)){
            return true;
        }
    }
    #endif

    return false;
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:	xbox模式只有中断端点才会被识别	
*******************************************************************/
error_t usbh_match_hid( uint8_t id, usbh_class_t *pclass)
{
    error_t err = ERROR_NOT_FOUND;
    if(TUSB_CLASS_HID == pclass->itf.if_cls){
        err = ERROR_SUCCESS;
    }else if(TUSB_CLASS_VENDOR == pclass->itf.if_cls){          //xbox 特殊处理
        if((XBOX_SUBCLASS == pclass->itf.if_sub_cls) 
            || (X360_SUBCLASS == pclass->itf.if_sub_cls)
            || (X360_IDENTIFY_SUBCLASS == pclass->itf.if_sub_cls)){
            err = ERROR_SUCCESS;
        }
    }
	UNUSED_PARAMETER(id);
	return err;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);

    if(NULL == pdev) return err;

    #if (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
    if((pclass->hid_types) & (HID_TYPE_KB | HID_TYPE_MOUSE)){
        err = usbh_hid_km_open(id, pclass);
        if(err) return err;
    }
    #endif
    #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
    if((pclass->hid_types) & HID_GAMEPAD_MASK){
        err = usbh_hid_gamepad_open(id, pclass);
        if(err) return err;
    }
    #endif
    #if (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
    if((pclass->hid_types) & BIT(HID_TYPE_VENDOR)){
        err = usbh_hid_vendor_open(id, pclass);
        if(err) return err;
    }
    #endif

    err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    if(err) return err;

    if(ERROR_SUCCESS == err) pdev->class_ready = true;
    return err;
}

error_t usbh_hid_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
    error_t err = ERROR_UNKNOW;
    uint16_t desc_len; 
    uint8_t *desc_buf;
    hid_desc_info_t hid_info;

    if(TUSB_CLASS_HID == pclass->itf.if_cls){                   //except xbox
        if(0 == pclass->itf.num_endp) return ERROR_NOT_FOUND;   //排除无端点接口

        desc_len = usbh_get_hid_desc_len(pdesc, len);
        if(0 == desc_len) return ERROR_UNSUPPORT;

        err = usbh_hid_set_idle(id, pclass->itf.if_num);
        // if(err) return err;          //note:  dev can STALL

        desc_buf = emf_malloc(desc_len);
        if(NULL == desc_buf){
            logd_r("malloc len=%d err!\n",desc_len);
            return ERROR_NO_MEM;
        }
        
        err = usbh_hid_get_report_desc(id, 0, pclass->itf.if_num, desc_buf, &desc_len);
        if(err) return err;
        logd("usbh get report desc:");dumpd(desc_buf, desc_len);
        //pase report desc
        err = hid_desc_parse_report(&hid_info, desc_buf, desc_len);
        emf_free(desc_buf);

        if(ERROR_SUCCESS == err){
            uint8_t i;
            hid_collection_t* pcollection;

            err = ERROR_NOT_FOUND;
            hid_desc_dump(&hid_info);
            
            for(i=0; i<hid_info.collections; i++){
                pcollection = &hid_info.item_list.collectionList[i];
                if(0 == pcollection->parent){     //顶层集合确定设备类型
                    #if (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
                    if(ERROR_SUCCESS == usbh_hid_km_init(id, pclass, &hid_info,i)){
                        err = ERROR_SUCCESS;
                    }
                    #endif

                    #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
                    if(ERROR_SUCCESS == usbh_hid_gamepad_init(id, pclass, &hid_info,i)){
                        err = ERROR_SUCCESS;
                    }
                    #endif

                    #if (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
                    if(ERROR_SUCCESS == usbh_hid_vendor_init(id, pclass, &hid_info,i)){
                        err = ERROR_SUCCESS;
                    }
                    #endif
                }
            }
            hid_desc_info_free(&hid_info);
        }
    }else if(XBOX_CLASS == pclass->itf.if_cls){          //xbox 特殊处理, x360 if_pro 必须是X360_PROTOCOL
        if((XBOX_SUBCLASS == pclass->itf.if_sub_cls) 
            || (X360_SUBCLASS == pclass->itf.if_sub_cls)
            || (X360_IDENTIFY_SUBCLASS == pclass->itf.if_sub_cls)){
            #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
            err = usbh_hid_gamepad_init(id, pclass, NULL, 0);
            #endif
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

    #if (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
    if((pclass->hid_types) & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE))){
        err = usbh_hid_km_deinit(id, pclass);
    }
    #endif
    #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
    if((pclass->hid_types) & HID_GAMEPAD_MASK){
        err = usbh_hid_gamepad_deinit(id, pclass);
    }
    #endif
    #if (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
    if((pclass->hid_types) & BIT(HID_TYPE_VENDOR)){
        err = usbh_hid_vendor_deinit(id, pclass);
    }
    #endif

	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hid_task(uint8_t id, usbh_class_t *pclass)
{
    #if (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
    if((pclass->hid_types) & (HID_TYPE_KB | HID_TYPE_MOUSE)){
        usbh_hid_km_task(id, pclass);
    }
    #endif
    #if (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
    if((pclass->hid_types) & HID_GAMEPAD_MASK){
        usbh_hid_gamepad_task(id, pclass);
    }
    #endif
    #if (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
    if((pclass->hid_types) & BIT(HID_TYPE_VENDOR)){
        usbh_hid_vendor_task(id, pclass);
    }
    #endif
}

#endif






