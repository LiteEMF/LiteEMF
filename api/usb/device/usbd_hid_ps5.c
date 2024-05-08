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
#if API_USBD_BIT_ENABLE && \
	(USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) \
	&& ( USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5) )

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
static uint8c_t ps5_gamepade_itf_desc_tab[] = {
	0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             			//usb_desc_interface_t
    USBD_HID_DESC,          			//usb_hid_descriptor_hid_t
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0x40,0x00,0x01,			//usb_desc_endpoint_t
	0x07,0x05,(TUSB_DIR_OUT<<TUSB_DIR_POST),0x03,0x40,0x00,0x01,			//usb_desc_endpoint_t
};

static uint8c_t class_reuqes_in_20[] = {
0x20, 0x41, 0x70, 0x72, 0x20, 0x20, 0x37, 0x20, 0x32, 0x30, 0x32, 0x31, 0x30, 0x32, 
0x3A, 0x32, 0x38, 0x3A, 0x34, 0x35, 0x02, 0x00, 0x04, 0x00, 0x13, 0x03, 0x00, 0x00, 
0x14, 0x00, 0x02, 0x01, 0x40, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x67, 0x02, 0x00, 0x00, 0x2A, 0x00, 0x01, 0x00, 0x09, 0x00, 0x01, 0x00, 
0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8c_t class_reuqes_in_05[] = {
0x05, 0xFA, 0xFF, 0xF4, 0xFF, 0x00, 0x00, 0x9A, 0x22, 0x55, 0xDD, 0x83, 0x22, 0x64, 0xDD, 0xC2, 0x22, 0x42,
0xDD, 0x1C, 0x02, 0x1C, 0x02, 0x07, 0x20, 0x17, 0xE0, 0x02, 0x1F, 0x21, 0xDF, 0xED, 0x1F, 0xE2, 0xDF, 0x04, 0x00,
0x00, 0x00, 0x00, 0x00,
};

static uint8c_t class_reuqes_in_09[] = {
0x09, 0xBC, 0xD6, 0x59, 0x9B, 0xB9, 0x4C, 0x08, 0x25, 0x00, 0x8A, 0x81, 0x75, 0x7D,
0x49, 0x64, 0x00, 0x00, 0x00, 0x00
};
/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_ps5_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_ps5_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_ps5_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len,rep_desc_len;

	len = sizeof(ps5_gamepade_itf_desc_tab);
	if (desc_len >= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, ps5_gamepade_itf_desc_tab, len);
		usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_PS5, pindex, pdesc + *pdesc_index, len);

		rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_PS5, NULL);
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
	ps4原装:	in:	0X02	
				in:	0XA3	
				in:	0X12	
				out: 0X13(如果蓝牙连接地址有改变)
				out: 0X14
				out: 0X14
	第三方手柄:	in:	0X03	
				in:	0XF3	
*******************************************************************/
error_t usbd_hid_ps5_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;
	usbd_dev_t *pdev = usbd_get_dev(id);
	ps4_cmd_t* cmdp = (ps4_cmd_t*)preq->setup_buf;

	if (TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;

		switch(preq->req.bRequest){
		case HID_REQ_CONTROL_GET_REPORT:
			if ((report_type == HID_REPORT_TYPE_FEATURE) && preq->req.wLength){
				preq->setup_len = preq->req.wLength;
				memset(cmdp, 0, MIN(preq->req.wLength,sizeof(ps4_cmd_t)));		//这里注意防止内存溢出

				switch (report_id){
                case 0X20:
					memcpy(preq->setup_buf,class_reuqes_in_20,MIN(preq->setup_len,sizeof(class_reuqes_in_20)));
					err = ERROR_SUCCESS;
                    break; 
				case 0X09:
					memcpy(preq->setup_buf,class_reuqes_in_09,MIN(preq->setup_len,sizeof(class_reuqes_in_09)));
					err = ERROR_SUCCESS;
					break;
                case 0X05:
					memcpy(preq->setup_buf,class_reuqes_in_05,MIN(preq->setup_len,sizeof(class_reuqes_in_05)));
					err = ERROR_SUCCESS;
                    break;
                default:
                    break;
                }
			}
			break;
		case  HID_REQ_CONTROL_SET_REPORT:
			if ((report_type == HID_REPORT_TYPE_FEATURE) && preq->req.wLength){
				logd("usbd ps set report id=%x:",report_id);dumpd(preq->setup_buf,preq->req.wLength);
				switch(report_id){
					default:
					break;
				}
			}
			break;
		default:
			break;
		}
    }
    return err;
}

error_t usbd_hid_ps5_out_process(uint8_t id, usbd_class_t* pclass)
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
error_t usbd_hid_ps5_init(uint8_t id)
{
	trp_handle_t trp_handle={TR_USBD,0,U16(DEV_TYPE_HID, HID_TYPE_PS5)};
		
	trp_handle.id = id;
	app_gamepad_init( &trp_handle ); 
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_ps5_deinit(uint8_t id)
{
	trp_handle_t trp_handle={TR_USBD,0,U16(DEV_TYPE_HID, HID_TYPE_PS5)};
		
	trp_handle.id = id;
	app_gamepad_deinit( &trp_handle ); 
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_ps5_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
		usbd_hid_ps5_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_hid_ps5_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_hid_ps5_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
