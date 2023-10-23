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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) && (USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER)))
#include "api/usb/device/usbd.h"
#include "app/km_typedef.h"

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

#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_KB)
uint8c_t kb_itf_desc_tab[] = {
    0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,                       //接口描述符,键盘
    USBD_HID_DESC,                       //HID类描述符
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0X40,0x00,USBD_HID_KB_INTERVAL,			//端点描述符
    // 0x07,0x05,(TUSB_DIR_OUT<<TUSB_DIR_POST),0x03,0X40,0x00,0x08,			//键盘不需要out端点,通过控制传输设置
};
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
uint8c_t mouse_itf_desc_tab[] = {
    0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,						//接口描述符,鼠标
    USBD_HID_DESC,					//HID类描述符
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST),0x03,0X40,0x00,USBD_HID_MOUSE_INTERVAL,			//端点描述符
};
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_CONSUMER)
uint8c_t consumer_itf_desc_tab[] = {
   	0x09,0x04,0x02,0x00,0x01,0x03,0x00,0x00,0x00,						//接口描述符
    USBD_HID_DESC,						//HID类描述符
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0X40,0x00,0x04,			//端点描述符
};
#endif
/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_km_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_km_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_km_get_itf_desc(uint8_t id, uint8_t hid_type, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len,rep_desc_len,total_len = 0;

	#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_KB)
	if(HID_TYPE_KB == hid_type){
		len = sizeof(kb_itf_desc_tab);
		if (desc_len >= *pdesc_index + len) {
			memcpy(pdesc + *pdesc_index, kb_itf_desc_tab, len);
			usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_KB, pindex, pdesc + *pdesc_index, len);

			rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_KB, NULL);
			pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
			pdesc[*pdesc_index + 17] = rep_desc_len>>8;
		}
		*pdesc_index += len;
		total_len += len;
	}
	#endif

	#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
	if(HID_TYPE_MOUSE == hid_type){
		len = sizeof(mouse_itf_desc_tab);
		if (desc_len >= *pdesc_index + len) {
			memcpy(pdesc + *pdesc_index, mouse_itf_desc_tab, len);
			usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_MOUSE, pindex, pdesc + *pdesc_index, len);

			rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_MOUSE, NULL);
			pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
			pdesc[*pdesc_index + 17] = rep_desc_len>>8;
		}
		*pdesc_index += len;
		total_len += len;
	}
	#endif
	
	#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_CONSUMER)
	if(HID_TYPE_CONSUMER == hid_type){
		len = sizeof(consumer_itf_desc_tab);
		if (desc_len >= *pdesc_index + len) {
			memcpy(pdesc + *pdesc_index, consumer_itf_desc_tab, len);
			usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_CONSUMER, pindex, pdesc + *pdesc_index, len);

			rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_CONSUMER, NULL);
			pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
			pdesc[*pdesc_index + 17] = rep_desc_len>>8;
		}
		*pdesc_index += len;
		total_len += len;
	}
	#endif

    return total_len;
}

error_t usbd_hid_km_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;
	#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_KB)
	usbd_dev_t *pdev = usbd_get_dev(id);

	if (TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;
		switch(preq->req.bRequest){
		case HID_REQ_CONTROL_GET_REPORT:
			break;
		case  HID_REQ_CONTROL_SET_REPORT:
			if ((report_type == HID_REPORT_TYPE_OUTPUT) && preq->req.wLength){
				// Set keyboard LED e.g Capslock, Numlock etc...
				if (report_id == KB_REPORT_ID){
					kb_led_t kb_leds;
					kb_leds.val = preq->setup_buf[0];
					
					logd("kb led set report=%x ,%d\n",kb_leds.val, preq->setup_len);
					err = ERROR_SUCCESS;
				}
			}
			break;
		default:
			break;
		}
    }
	#endif
    return err;
}

error_t usbd_hid_km_out_process(uint8_t id, usbd_class_t* pclass)
{
	uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
    error_t err;

    err = usbd_out(id,pclass->endpout.addr,usb_rxbuf,&usb_rxlen);
    if((ERROR_SUCCESS == err) && usb_rxlen){
        logd("km hid ep%d in%d:",pclass->endpout.addr, usb_rxlen);dumpd(usb_rxbuf,usb_rxlen);
    }

    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_km_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_km_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_km_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
        usbd_hid_km_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_hid_km_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_hid_km_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
