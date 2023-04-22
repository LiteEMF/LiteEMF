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
#if (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR))
#include "api/usb/device/usbd.h"

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
uint8c_t vendor_itf_desc_tab[] = {
	0x09,0x04,0x00,0x00,0x01,0x03,0x00,0x00,0x00,             			//接口描述符,键盘
    USBD_HID_DESC,           			//HID类描述符
    0x07,0x05,(USB_DIR_IN<<USB_DIR_POST),0x03,0x40,0x00,0x04,			//端点描述符
};

/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_vendor_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_vendor_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_vendor_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len,rep_desc_len;

	len = sizeof(vendor_itf_desc_tab);
	if (desc_len <= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, vendor_itf_desc_tab, len);
		usbd_reset_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_VENDOR, pindex, pdesc + *pdesc_index, len);

		rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_VENDOR, NULL);
		pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
		pdesc[*pdesc_index + 17] = rep_desc_len>>8;
	}
	*pdesc_index += len;

    return len;
}

error_t usbd_hid_vendor_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_SUCCESS;
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	UNUSED_PARAMETER(preq);
    return err;
}

error_t usbd_hid_vendor_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
	logd("vendor hid ep%d in%d:",pclass->endpout.addr, len);dumpd(buf,len);
    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_vendor_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_vendor_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_vendor_task(uint8_t id)
{
	UNUSED_PARAMETER(id);
}

#endif
