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


#include "hw_config.h"
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) \
	&& (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3))

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
static uint8c_t ps3_gamepade_itf_desc_tab[] = {
	0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             		//usb_desc_interface_t
    USBD_HID_DESC,          										//usb_hid_descriptor_hid_t
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0x40,0x00,0x01,	//usb_desc_endpoint_t
	0x07,0x05,(TUSB_DIR_OUT<<TUSB_DIR_POST),0x03,0x40,0x00,0x01,	//usb_desc_endpoint_t
};


/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_ps3_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_ps3_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_ps3_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len,rep_desc_len;

	len = sizeof(ps3_gamepade_itf_desc_tab);
	if (desc_len >= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, ps3_gamepade_itf_desc_tab, len);
		usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_PS3, pindex, pdesc + *pdesc_index, len);

		rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_PS3, NULL);
		pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
		pdesc[*pdesc_index + 17] = rep_desc_len>>8;
	}
	*pdesc_index += len;

    return len;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:
*******************************************************************/
error_t usbd_hid_ps3_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;
	usbd_dev_t *pdev = usbd_get_dev(id);
	ps4_cmd_t* cmdp = (ps4_cmd_t*)preq->setup_buf;

	if (TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;

		switch(preq->req.bRequest){
		case  HID_REQ_CONTROL_SET_REPORT:
			if(0XF4 == report_id && HID_REPORT_TYPE_FEATURE == report_type){
				usbd_set_ready(id, true);		//接ps3主机这里设置ready
			}
			break;
		default:
			break;
		}
    }
    return err;
}

error_t usbd_hid_ps3_out_process(uint8_t id, usbd_class_t* pclass)
{
	uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
    error_t err;

    err = usbd_out(id,pclass->endpout.addr,usb_rxbuf,&usb_rxlen);
    if((ERROR_SUCCESS == err) && usb_rxlen){
        trp_handle_t trp_handle;
		
		trp_handle.trp = TR_USBD;
		trp_handle.id = id;
		trp_handle.index = U16(pclass->dev_type, pclass->hid_type);
		app_gamepad_dev_process(&trp_handle, usb_rxbuf,usb_rxlen);
    }

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_ps3_init(uint8_t id)
{
	trp_handle_t trp_handle={TR_USBD,0,U16(DEV_TYPE_HID, HID_TYPE_PS3)};

	trp_handle.id = id;
	app_gamepad_init( &trp_handle ); 
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_ps3_deinit(uint8_t id)
{
	trp_handle_t trp_handle={TR_USBD,0,U16(DEV_TYPE_HID, HID_TYPE_PS3)};

	trp_handle.id = id;
	app_gamepad_deinit( &trp_handle ); 
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_ps3_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
		usbd_hid_ps3_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_hid_ps3_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_hid_ps3_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
