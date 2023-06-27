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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD))
#include "api/usb/device/usbd.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

static uint8c_t msd_itf_desc_tab[] = {
    0x09,0x04,0x00,0x00,0x02,USB_CLASS_MSD,MSC_SUBCLASS_SCSI,MSC_PROTOCOL_BULK_ONLY,0x00,              //usb_desc_interface_t
    0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x02,0x40,0x00,0x00,			//usb_desc_endpoint_t
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x02,0x40,0x00,0x00,			//usb_desc_endpoint_t
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
msc_cfg_t usbd_msc_cfg;
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_msd_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    memset(&usbd_msc_cfg, 0, sizeof(usbd_msc_cfg));
    usbd_msc_cfg.stage = MSC_READ_CBW;
    usbd_msc_cfg.readonly = false;

    usbd_msc_cfg.scsi_blk_nbr = USBD_MSC_FLASH_BLOCK_NUM;
    usbd_msc_cfg.scsi_blk_size = USBD_MSC_FLASH_BLOCK_SIZE;

logd("usbd_msc_cfg.max_lun=%bd\n",usbd_msc_cfg.max_lun);
    return ERROR_SUCCESS;
}

error_t usbd_msd_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

uint16_t usbd_msd_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(msd_itf_desc_tab);

    if (desc_len >= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, msd_itf_desc_tab, len);
        usbd_assign_configuration_desc(id, DEV_TYPE_MSD, 0, pindex, pdesc + *pdesc_index, len);
    }
    *pdesc_index += len;

    return len;
}

error_t usbd_msd_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(USB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bits.recipient){
        if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
            if (USB_REQ_SET_CONFIGURATION == preq->req.bRequest){
                usbd_msd_reset(id);
            }
        }
    }else if(USB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bits.recipient){
        if(USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
            switch ( preq->req.bRequest ){
            case MSC_REQUEST_RESET:
                logd("MSC BOT Reset\n");
                // driver state reset
                usbd_msd_reset(id);
                err = ERROR_SUCCESS;
                break;

            case MSC_REQUEST_GET_MAX_LUN:
                logd("MSC Get Max Lun\n");
                preq->setup_buf[0] = usbd_msc_cfg.max_lun;  //default 0
                logd("usbd_msc_cfg.max_lun=%bd\n",usbd_msc_cfg.max_lun);
                preq->setup_len = 1;
                err = ERROR_SUCCESS;
                break;
            }
        }
    }


    return err;
}


error_t usbd_msd_out_process(uint8_t id, usbd_class_t* pclass)
{
    uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
    error_t err;

    err = usbd_out(id,pclass->endpout.addr,usb_rxbuf,&usb_rxlen);
    if((ERROR_SUCCESS == err) && usb_rxlen){
        mass_storage_bulk_out(id,usb_rxbuf,usb_rxlen);
    }

    return ERROR_SUCCESS;
}

error_t usbd_msd_in_process(uint8_t id,usbd_class_t* pclass)
{
    mass_storage_bulk_in(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_msd_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_msd_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_msd_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
        usbd_msd_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_msd_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_msd_out_process(id, pclass);
        break;
    case  USBD_EVENT_EP_IN:
        usbd_msd_in_process(id,pclass);
        break;
    default:
        break;
    }
}


#endif
