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

    //IAD Descriptor
    0x08,                       //bLength
    0x0b,                       //bDescriptorType
    0x00,                       //bFirstInterface
    0x02,                       //bInterfaceCount
    0x02,                       //bFunctionClass, Comunication and CDC control
    0x02,                       //bFunctionSubClass
    0x01,                       //bFunctionProtocol
    0x00,                       //iFunction

    /* Interface 0 (CDC) descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x00,                           // bInterfaceNumber 0
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints 1
    0x02,                           // bInterfaceClass
    0x02,                           // bInterfaceSubClass
    0x01,                           // bInterfaceProtocol
    0x00,                           // iInterface (String Index)

    /* Functional Descriptors */
    0x05,0x24,0x00, 0x10, 0x01, 

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    (TUSB_DIR_IN<<TUSB_DIR_POST),   // bEndpointAddress (IN/D2H)
    0x03,                           // bmAttributes (Interrupt)
    0x40, 0x00,                     // wMaxPacketSize 64
    0x01,                           // bInterval 1 (unit depends on device speed)

    /* Interface 1 (data interface) descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x01,                           // bInterfaceNumber 1
    0x00,                           // bAlternateSetting
    0x02,                           // bNumEndpoints 2
    0x0A,                           // bInterfaceClass
    0x00,                           // bInterfaceSubClass
    0x00,                           // bInterfaceProtocol
    0x00,                           // iInterface (String Index)

    /* Endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    (TUSB_DIR_OUT<<TUSB_DIR_POST),  // bEndpointAddress (OUT/H2D)
    0x02,                           // bmAttributes (Bulk)
    0x40, 0x00,                     // wMaxPacketSize 64
    0x00,                           // bInterval 0 (unit depends on device speed)

    /* Endpoint descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    (TUSB_DIR_IN<<TUSB_DIR_POST),   // bEndpointAddress (IN/D2H)
    0x02,                           // bmAttributes (Bulk)
    0x40, 0x00,                     // wMaxPacketSize 64
    0x00,                           // bInterval 0 (unit depends on device speed)
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
cdc_line_coding_t m_cdc_line_coding = {115200, 0, 0, 8};

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
    }
    *pdesc_index += len;

    return len;
}


error_t usbd_cdc_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(TUSB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bits.recipient) return err;

    if(TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;
		switch(preq->req.bRequest){
		case CDC_REQUEST_SET_LINE_CODING:{
            cdc_line_coding_t* line_codingp = (cdc_line_coding_t*)preq->setup_buf;
            memcpy(&m_cdc_line_coding, preq->setup_buf, sizeof(m_cdc_line_coding));
            
            logd("CDC_REQUEST_SET_LINE_CODING: bit_rate=%d,stop_bits=%d,parity=%d,data_bits=%d\n",
            m_cdc_line_coding.bit_rate,
            m_cdc_line_coding.stop_bits,
            m_cdc_line_coding.parity,
            m_cdc_line_coding.data_bits);
            err = ERROR_SUCCESS;
			break;
        }
        case CDC_REQUEST_GET_LINE_CODING:
            preq->setup_len = MIN(preq->req.wLength,sizeof(m_cdc_line_coding));
            memcpy(preq->setup_buf, &m_cdc_line_coding, preq->setup_len);
     
            logd("CDC_REQUEST_GET_LINE_CODING: bit_rate=%d,stop_bits=%d,parity=%d,data_bits=%d\n",
            m_cdc_line_coding.bit_rate,
            m_cdc_line_coding.stop_bits,
            m_cdc_line_coding.parity,
            m_cdc_line_coding.data_bits);
            err = ERROR_SUCCESS;
			break;
        case CDC_REQUEST_SET_CONTROL_LINE_STATE:
            err = ERROR_SUCCESS;
            break;
        }
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
        error_t err = usbd_class_in(id, DEV_TYPE_CDC, CDC_COMM_SUBCLASS_MOBILE_DIRECT_LINE_MODEL,usb_rxbuf,usb_rxlen);
        logd("cdc ep%d out%d:",pclass->endpout.addr, usb_rxlen);dumpd(usb_rxbuf,usb_rxlen);
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
    case USBD_EVENT_CONFIGURED:				//系统这configred后ready
		usbd_set_ready(id, true);
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
