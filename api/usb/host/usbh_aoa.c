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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA))
#include "api/usb/host/usbh.h"
#include "api/hid/hid_dev_desc.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t aoa_hid_types;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8c_t *credetials[] = {	"LiteEMF",				//manufacturer name 
                                	"demo",					//mode
                                	"demo",					//string description
                                	"1.0",					//version
                                	"LiteEMF-test.com.cn",		//url
                                	"01234456789",			//serial number
                                };
/*****************************************************************************************************
**	static Function
******************************************************************************************************/
static error_t accessory_get_version(uint8_t id, uint16_t *pversion)
{
	error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_VENDOR;
	req.bmRequestType.bits.direction = TUSB_DIR_IN;
    req.bRequest = ACCESSORY_GET_PROTOCOL_VERSION;
	req.wValue = 0;
    req.wIndex = 0;
    req.wLength = SWAP16_L(2);

    err = usbh_ctrl_transfer(id, &req, (uint8_t*)pversion, NULL);
	*pversion = SWAP16_L(*pversion);

    return err;
}

static error_t accessory_send_string(uint8_t id,uint8_t string_id, char *string, uint16_t len)
{
	error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_VENDOR;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = ACCESSORY_SEND_STRING;
	req.wValue = 0;
    req.wIndex = SWAP16_L(string_id);
    req.wLength = SWAP16_L(len);

    err = usbh_ctrl_transfer(id, &req, (uint8_t*)string, NULL);

    return err;
}

static error_t accessory_set_request(uint8_t id, uint8_t request,  uint8_t index, uint8_t *buf, uint16_t len)
{
	error_t err;
	usb_control_request_t req;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_DEVICE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_VENDOR;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = request;
	req.wValue = SWAP16_L(index);
    req.wIndex = 0;
    req.wLength = SWAP16_L(len);

    err = usbh_ctrl_transfer(id, &req, buf, NULL);

    return err;
}

static error_t usbh_aoa_register_hid(uint8_t id, uint8_t index, uint8_t *desc, uint16_t len)		//id form 1
{
	error_t err;
	err = accessory_set_request(id, ACCESSORY_REGISTER_HID, index, NULL, 0);
	if(err) return err;

	err = accessory_set_request(id, ACCESSORY_SET_HID_REDPORT_DESC, index, desc, len);
    return err;
}
/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t usbh_aoa_switch(uint8_t id)
{
    error_t err;
    uint8_t i;
	uint16_t ver;
	char mcu_ver[] = {'1','.','0'};

	err = accessory_get_version(id,&ver);
	if(err) return err;
	logd("aoa ver=%x\n",ver);

	for(i = 0; i < 6; i++){
		err = accessory_send_string(id,i, (uint8_t*)credetials[i], strlen(credetials[i]));
		if(err) return err;
	}

	err = accessory_set_request(id, ACCESSORY_START, 0, NULL,0);
    return err;
	
	logd("aoa switch success\n");

    return err;
}

error_t usbh_aoa_hid_out(uint8_t index, uint8_t *buf, uint8_t len)
{
	error_t err = ERROR_NOT_FOUND;
	uint8_t id;
	usbh_class_t *pcalss = NULL;
	id = usbh_class_find_by_type_all(DEV_TYPE_AOA,0,&pcalss);
	if(id != USBH_NULL){
		if(0 == index){
			err = usbh_out(id, &pcalss->endpout, buf, len);
		}else{
			err = accessory_set_request(id, ACCESSORY_SEND_HID_EVENT, index, buf, len);
	   }
	}
	
   	return( err );
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_aoa_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("aoa endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_aoa( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if ((pdev->vid  == GOOGLE_VID) && ((pdev->pid&0xff00) == GOOGLE_PID_ACCESSORY)){         // apple
		if(pclass->endpin.addr && pclass->endpout.addr){
			err = ERROR_SUCCESS;
		}
	}else if((AOA_CLASS == pclass->itf.if_cls) && (pclass->itf.if_pro == AOA_PROTOCOL)){
		if(pclass->endpin.addr && pclass->endpout.addr){
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
error_t usbh_aoa_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNKNOW;
	usbh_dev_t* pdev = get_usbh_dev(id);
	uint8_t *pdesc;
	uint16_t len;

	if ((pdev->vid  == GOOGLE_VID) && ((pdev->pid&0xff00) == GOOGLE_PID_ACCESSORY)) {
		
		#if AOA_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
		if(aoa_hid_types & HID_TYPE_GAMEPADE){
			len = get_hid_desc_map(TR_USBH, HID_TYPE_GAMEPADE,&pdesc);
			err = usbh_aoa_register_hid(id, GAMEPAD_REPORT_ID, pdesc, len);
			if(err) return err;
		}
		#endif
		#if AOA_HID_SUPPORT & BIT_ENUM(HID_TYPE_KB)
		if(aoa_hid_types & HID_TYPE_KB){
			len = get_hid_desc_map(TR_USBH, HID_TYPE_KB,&pdesc);
			err = usbh_aoa_register_hid(id, KB_REPORT_ID, pdesc, len);
			if(err) return err;
		}
		#endif
		#if AOA_HID_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
		if(aoa_hid_types & HID_TYPE_MOUSE){
			len = get_hid_desc_map(TR_USBH, HID_TYPE_MOUSE,&pdesc);
			err = usbh_aoa_register_hid(id, MOUSE_REPORT_ID, pdesc, len);
			if(err) return err;
		}
		#endif

		err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    	if(ERROR_SUCCESS == err) pdev->class_ready = true;
	}else{
		pdev->class_ready = false;
		usbh_aoa_switch(id);			//usb will reset
		err = ERROR_UNSUPPORT;			
	}

    return err;
}

error_t usbh_aoa_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	UNUSED_PARAMETER(pdesc);
	UNUSED_PARAMETER(len);
    return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_aoa_deinit( uint8_t id, usbh_class_t *pclass) 
{
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_aoa_task( uint8_t id, usbh_class_t *pclass)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}


#endif






