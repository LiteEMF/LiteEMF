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
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
#include "api/usb/device/usbd.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

static uint8c_t printer_itf_desc_tab[] = {
   
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
error_t usbd_printer_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_printer_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_printer_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(printer_itf_desc_tab);

    if (desc_len <= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, printer_itf_desc_tab, len);
        usbd_assign_configuration_desc(id, DEV_TYPE_PRINTER, 0, pindex, pdesc + *pdesc_index, len);
    }
    *pdesc_index += len;

    return len;
}

error_t usbd_printer_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(USB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bit.recipient) return err;

    if(USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bit.type) {

    }
    return err;
}

error_t usbd_printer_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_printer_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_printer_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_printer_task(uint8_t id)
{
    UNUSED_PARAMETER(id);
}

#endif
