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
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
#include "api/usb/device/usbd.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

static uint8c_t iap2_itf_desc_tab[] = {

	0x09,0x04,0x00,0x00,0x02,0xFF,0xF0,0x00,0x04,                       // usb_desc_interface_t
	0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x02,0X40,0x00,0x01,			//端点描述符
    0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x02,0X40,0x00,0x01,			//端点描述符

	0x09,0x04,0x01,0x00,0x01,0x03,0x00,0x00,0x00,                       //接口描述符
	0x09,0x21,0x10,0x01,0x00,0x01,0x22,0x00,0x00,
	0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x02,0X40,0x00,0x01,
	
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t usbd_iap_hid_types[USBD_NUM];
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_iap2_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_iap2_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

uint16_t usbd_iap2_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(iap2_itf_desc_tab),rep_desc_len;
    uint8_t hid_type;

    if (desc_len <= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, iap2_itf_desc_tab, len);
        usbd_reset_configuration_desc(id, DEV_TYPE_IAP2, 0, pindex, pdesc + *pdesc_index, len);
        
        for(hid_type=0; hid_type<16; hid_type++){
            if (usbd_iap_hid_types[id] & (0X01UL<<hid_type)){
                rep_desc_len = get_hid_desc_map(TR_USBD, hid_type, NULL);
                pdesc[*pdesc_index + 9+7+7+16] = rep_desc_len&0xff;            //set hid report desc
                pdesc[*pdesc_index + 9+7+7+17] = rep_desc_len>>8;
                break;
            }
        }
    }
    *pdesc_index += len;

    return len;
}

error_t usbd_iap2_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(USB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bit.recipient) return err;

    if(USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bit.type){
        if(USB_REQ_SET_INTERFACE == preq->req.bRequest) {
           err = ERROR_SUCCESS;
		}
	}else if(USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bit.type) {

    }
    return err;
}

error_t usbd_iap2_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
    logd("iap2 hid ep%d in%d:",pclass->endpout.addr, len);dumpd(buf,len);
    return ERROR_SUCCESS;
}



/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_iap2_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_iap2_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_iap2_task(uint8_t id)
{
    UNUSED_PARAMETER(id);
}

#endif
