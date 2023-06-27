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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC))
#include "api/usb/device/usbd.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/


static uint8c_t cdc_itf_desc_tab[] = {
   0x09
};

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
error_t usbd_cdc_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_cdc_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

uint16_t usbd_cdc_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(cdc_itf_desc_tab);
    usbd_dev_t *pdev = usbd_get_dev(id);

    if (desc_len >= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, cdc_itf_desc_tab, len);
        usbd_assign_configuration_desc(id, DEV_TYPE_CDC, 0, pindex, pdesc + *pdesc_index, len);
        
        if(USB_STA_CONFIGURED == pdev->state){
            pdev->ready = true;       //枚举完
            logd_g("usbd%d ready...\n",id);
        }
    }
    *pdesc_index += len;

    return len;
}

error_t usbd_cdc_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
        error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(USB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bits.recipient) return err;

    if(USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {

    }
    return err;
}

error_t usbd_cdc_out_process(uint8_t id, usbd_class_t* pclass)
{
    uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
    error_t err;

    err = usbd_out(id,pclass->endpout.addr,usb_rxbuf,&usb_rxlen);
    if((ERROR_SUCCESS == err) && usb_rxlen){
        logd("cdc hid ep%d in%d:",pclass->endpout.addr, usb_rxlen);dumpd(usb_rxbuf,usb_rxlen);
    }

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_cdc_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_cdc_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_cdc_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
        usbd_cdc_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_cdc_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_cdc_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
