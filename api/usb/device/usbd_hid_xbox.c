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

//@ref: https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

#include "hw_config.h"
#if (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))

#include "api/usb/device/usbd.h"
#include "app/gamepad/app_gamepad.h"
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

static uint8c_t  xbox_itf_desc_tab[] =
{
	0x09,0x04,0x00,0x00,0x02,0xFF,0x47,0xD0,0x00,
	0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x03,0x40,0x00,0x02,
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x03,0x40,0x00,0x04,

	0x09,0x04,0x01,0x00,0x00,0xFF,0x47,0xD0,0x00,
	0x09,0x04,0x01,0x01,0x02,0xFF,0x47,0xD0,0x00,
	0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x01,0xE4,0x00,0x01,
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x01,0x40,0x00,0x01,

	0x09,0x04,0x02,0x00,0x00,0xFF,0x47,0xD0,0x00,
	0x09,0x04,0x02,0x01,0x02,0xFF,0x47,0xD0,0x00,
	0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x02,0x40,0x00,0x00,
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x02,0x40,0x00,0x00
};

// Feature Descriptor	bMS_VendorCode:0X90
static uint8c_t xbox_compat_id[] = {
	//head
	0x28,0x00,0x00,0x00,		//Length
	0x00,0x01,					//Version 1.0
	0x04,0x00,					//OS feature Extended: compat ID descriptor
	0x01,0x00,						//Number of function sections
	0x00,0x00,0x00,0x00,0x00,0x00,//res

	//function	sections
	0x00,						//Interface index
	0x01,						//res
	0x58,0x47,0x49,0x50,0x31,0x30,0x00,0x00,		//compatibleID	:XGIP10
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//subCompatibleID
	0x00,0x00,0x00,0x00,0x00,0x00,	//res
};




/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_xbox_reset(uint8_t id)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(DEV_TYPE_HID, HID_TYPE_XBOX)};
	xbox_controller_deinit(&trp_handle);
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_xbox_suspend(uint8_t id)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(DEV_TYPE_HID, HID_TYPE_XBOX)};
	xbox_controller_deinit(&trp_handle);
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_xbox_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len;

	len = sizeof(xbox_itf_desc_tab);
	if (desc_len <= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, xbox_itf_desc_tab, len);
		usbd_reset_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_GAMEPADE, pindex, pdesc + *pdesc_index, len);
	}
	*pdesc_index += len;

    return len;
}

error_t usbd_hid_xbox_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
	usbd_dev_t *pdev = usbd_get_dev(id);
	// uint8_t itf = preq->req.wIndex & 0XFF;

	if(USB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bit.recipient) return err;

    if (USB_REQ_TYPE_VENDOR == preq->req.bmRequestType.bit.type){
		if(0X90 == preq->req.bRequest){
			switch(preq->req.wValue){
			case 0X04:		//compat ID
				preq->setup_len = MIN(preq->setup_len,sizeof(xbox_compat_id));
				memcpy(preq->setup_buf, xbox_compat_id, preq->setup_len);
				err = ERROR_SUCCESS;
				break;
			case 0X05:		//properties图标等信息
				break;
			case 0X01:		//Genre 不支持
				break;
			default:
				break;
			}
		}
	}else if(USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bit.type){
        if(USB_REQ_SET_INTERFACE == preq->req.bRequest) {
			pdev->ready = true;			//xbox set interface 后打开端点
           	err = ERROR_SUCCESS;
		}
	}

    return err;
}

error_t usbd_hid_xbox_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(pclass->dev_type, pclass->hid_type)};

	app_gamepad_dev_process(&trp_handle, buf,len);

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:	
** Description:		
*******************************************************************/
usbd_class_t *usbd_xbox_audio_find(uint8_t id)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if((pclass->dev_type == DEV_TYPE_HID) && (HID_TYPE_XBOX == pclass->hid_type)){
			if(pclass->endpin.type != USB_ENDP_TYPE_ISOCH) continue;
			if(pclass->endpout.type != USB_ENDP_TYPE_ISOCH) continue;
			if(pdev->itf_alt[pclass->itf.if_num] == pclass->itf.if_alt){
				return pclass;
			}
		}
	}
	
	return NULL;
}



/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_xbox_init(uint8_t id)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(DEV_TYPE_HID, HID_TYPE_XBOX)};
	xbox_controller_init(&trp_handle);
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_xbox_deinit(uint8_t id)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(DEV_TYPE_HID, HID_TYPE_XBOX)};
	xbox_controller_deinit(&trp_handle);
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_xbox_task(uint8_t id)
{
	UNUSED_PARAMETER(id);
}

#endif
