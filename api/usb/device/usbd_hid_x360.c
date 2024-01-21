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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360))

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
static uint8_t x360_itf_num = 0;

#if 0	//完整描述符用于参考
static uint8c_t x360_itf_desc_tab[] = {		//TODO 描述符内容不同?
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x00,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints 2
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x01,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x11,        // bLength
	0x21,        // bDescriptorType (HID)
	0x10, 0x01,  // bcdHID 1.10
	0x01,        // bCountryCode
	0x25, 0x81, 0x14,0x03,0x03,0x03, 0x04,
	0x13, 0x02, 0x08,0x03,0x03, 

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x81,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x04,        // bInterval 4 (unit depends on device speed)

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x02,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x08,        // bInterval 8 (unit depends on device speed)

	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x01,        // bInterfaceNumber 1
	0x00,        // bAlternateSetting
	0x04,        // bNumEndpoints 4
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x03,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x1B,        // bLength
	0x21,        // bDescriptorType (HID)
	0x00, 0x01,  // bcdHID 1.00
	0x01,        // bCountryCode
	0x01, 0x83, 0x40, 
	0x01, 0x04, 0x20, 
	0x16, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x16, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	
	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x83,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x02,        // bInterval 2 (unit depends on device speed)

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x04,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x04,        // bInterval 4 (unit depends on device speed)

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x85,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x40,        // bInterval 64 (unit depends on device speed)

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x05,        // bEndpointAddress (OUT/H2D)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x10,        // bInterval 16 (unit depends on device speed)

	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x02,        // bInterfaceNumber 2
	0x00,        // bAlternateSetting
	0x01,        // bNumEndpoints 1
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x02,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x09,        // bLength
	0x21,        // bDescriptorType (HID)
	0x00, 0x01,  // bcdHID 1.00
	0x01,        // bCountryCode
	0x22,0x86, 0x07, 0x00,

	0x07,        // bLength
	0x05,        // bDescriptorType (Endpoint)
	0x86,        // bEndpointAddress (IN/D2H)
	0x03,        // bmAttributes (Interrupt)
	0x20, 0x00,  // wMaxPacketSize 32
	0x10,        // bInterval 16 (unit depends on device speed)

	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x03,        // bInterfaceNumber 3
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	0xFF,        // bInterfaceClass
	0xFD,        // bInterfaceSubClass
	0x13,        // bInterfaceProtocol
	0x04,        // iInterface (String Index)

	0x06,        // bLength
	0x41,        // bDescriptorType (Unknown)
	0x00, 0x01, 0x01, 0x03, 
	// 144 bytes
};
#endif

static uint8c_t x360_itf_desc_tab[] = {
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x00,        // bInterfaceNumber 0
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints 2
	0xFF,        // bInterfaceClass
	0x5D,        // bInterfaceSubClass
	0x01,        // bInterfaceProtocol
	0x00,        // iInterface (String Index)

	0x11,        // bLength
	0x21,        // bDescriptorType (HID)
	0x10, 0x01,  // bcdHID 1.10
	0x01,        // bCountryCode
	0x25, (TUSB_DIR_IN<<TUSB_DIR_POST),  0x14,0x00,0x00,0x00,0x00,
	0x13, (TUSB_DIR_OUT<<TUSB_DIR_POST), 0x08,0x00,0x00, 

	/******************** Descriptor of Custom HID endpoints ***********/
	0x07,		  	/* bLength: Endpoint Descriptor size */
	0x05, 		  	/* bDescriptorType: */
	(TUSB_DIR_IN<<TUSB_DIR_POST),	/* bEndpointAddress: Endpoint Address (IN) */
	0x03,		  	/* bmAttributes: Interrupt endpoint */
	0x20,
	0x00,		  	/* wMaxPacketSize: 2 Bytes max */
	0x02,

	0x07,			/* bLength: Endpoint Descriptor size */
	0x05,			/* bDescriptorType: */
	(TUSB_DIR_OUT<<TUSB_DIR_POST),	/* bEndpointAddress:  Endpoint Address (OUT) */
	0x03, 			/* bmAttributes: Interrupt endpoint */
	0x20,
	0x00,			/* wMaxPacketSize: 2 Bytes max  */
	0x08, 

	// used for x360 host mode identify, 添加这个会导致xinput模式下驱动识别不正常
	0x09,        // bLength
	0x04,        // bDescriptorType (Interface)
	0x01,        // bInterfaceNumber 3
	0x00,        // bAlternateSetting
	0x00,        // bNumEndpoints 0
	0xFF,        // bInterfaceClass
	0xFD,        // bInterfaceSubClass
	0x13,        // bInterfaceProtocol
	0x04,        // iInterface (String Index)

	0x06,        // bLength
	0x41,        // bDescriptorType (Unknown)
	0x00, 0x01, 0x01, 0x03, 

};


char const_t* x360_itf_string = "Xbox Security Method 3, Version 1.00, \xA9 2005 Microsoft Corporation. All rights reserved.";

//qwSignature :MSFT100 必须是这个
//bMS_VendorCode:0X90
char const_t* x360_devdesc_string = "MSFT100\x90";

// Feature Descriptor	bMS_VendorCode:0X90
static uint8c_t x360_compat_id[] = {
	//head
	0x28,0x00,0x00,0x00,		//Length
	0x00,0x01,					//Version 1.0
	0x04,0x00,					//OS feature Extended: compat ID descriptor
	0x01,						//Number of function sections
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,//res

	//function	sections
	0x00,						//Interface index
	0x01,						//res
	0x58,0x55,0x53,0x42,0x31,0x30,0x00,0x00,		//compatibleID	:XUSB10
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,		//subCompatibleID
	0x00,0x00,0x00,0x00,0x00,0x00,	//res
};
static uint8c_t x360_produck_qualif[] = {
	0x0A, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00
};

static uint8c_t x360_null[] = {
	0x1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,
	0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,0X1E,
};


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_x360_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_x360_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_x360_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len;

	len = sizeof(x360_itf_desc_tab);
	if (desc_len >= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, x360_itf_desc_tab, len);
		usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_X360, pindex, pdesc + *pdesc_index, len);
		x360_itf_num = pdesc[*pdesc_index + 2];
		logd("set x360 itf num=%d\n",x360_itf_num);
	}
	*pdesc_index += len;

    return len;
}

error_t usbd_hid_x360_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if (TUSB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){

		if (TUSB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bits.recipient) {
			if(TUSB_REQ_GET_DESCRIPTOR == preq->req.bRequest){
				uint8_t desc_type = (uint8_t)(preq->req.wValue >> 8);
				uint8_t desc_index = (uint8_t)(preq->req.wValue & 0xFF);

				switch(desc_type){
				case TUSB_DESC_DEVICE_QUALIFIER:
					preq->setup_len = MIN(preq->req.wLength,sizeof(x360_produck_qualif));
					memcpy(preq->setup_buf, x360_produck_qualif, preq->setup_len);
					err = ERROR_SUCCESS;
					break;
				case TUSB_DESC_STRING:
					if(0XEE == desc_index){
						err = usbd_pack_unicode_string(x360_devdesc_string, preq->setup_buf, &preq->req.wLength);
					}else if(4 == desc_index){		//inf string
						err = usbd_pack_unicode_string(x360_itf_string, preq->setup_buf, &preq->req.wLength);
					}
					break;
				}
			}
		}
	}else if (TUSB_REQ_TYPE_VENDOR == preq->req.bmRequestType.bits.type){
		if (TUSB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bits.recipient) {
			if(0X90 == preq->req.bRequest){
				switch(preq->req.wIndex){
				case 0X04:		//compat ID
					preq->setup_len = MIN(preq->req.wLength,sizeof(x360_compat_id));
					memcpy(preq->setup_buf, x360_compat_id, preq->setup_len);
					preq->setup_buf[16] = x360_itf_num;		//fix x360 itf num
					logd("x360 itf num=%d\n",x360_itf_num);
					err = ERROR_SUCCESS;
					break;
				case 0X05:		//properties图标等信息
					break;
				case 0X01:		//Genre 不支持
					break;
				default:
					break;
				}
			}else if(0x01 == preq->req.bRequest){		//接PC会发送0X01请求
				if(0x00 == preq->req.wValue){
					preq->setup_len = MIN(preq->req.wLength,sizeof(x360_null));
					memcpy(preq->setup_buf, x360_null, preq->setup_len);
					err = usbd_set_ready(id, true);
				}
			// }else if(0xA1 == preq->req.bRequest){		//接主机会发送0xA1请求
				// usbd_set_ready(id, true);				//这里只判断ready
			}

		}else if(TUSB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bits.recipient) {
			uint8_t itf = preq->req.wIndex & 0XFF;

			if(0x01 == preq->req.bRequest){
				switch (preq->req.wValue){
				case 0X00:
					preq->setup_len = MIN(preq->req.wLength,sizeof(x360_null));
					memcpy(preq->setup_buf, x360_null, preq->setup_len);
					err = ERROR_SUCCESS;
					break;
				case 0X100:
					preq->setup_len = MIN(preq->req.wLength,sizeof(x360_null));
					memcpy(preq->setup_buf, x360_null, preq->setup_len);
					err = ERROR_SUCCESS;
					break;
				default:
					break;
				}
			}
		}	
	}
    return err;
}

error_t usbd_hid_x360_out_process(uint8_t id, usbd_class_t* pclass)
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
		if(!app_gamepad_dev_process(&trp_handle, usb_rxbuf,usb_rxlen)){
			#if USBD_SOCKET_ENABLE
			usbd_socket_cmd(&usbd_socket_trp, CMD_SOCKET_OUT, usb_rxbuf,usb_rxlen);
			#endif
		}
    }
    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_x360_init(uint8_t id)
{
trp_handle_t trp_handle;
		
	trp_handle.trp = TR_USBD;
	trp_handle.id = id;
	trp_handle.index = U16(DEV_TYPE_HID, HID_TYPE_X360);
	app_gamepad_init( &trp_handle );
	
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_x360_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_x360_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
		usbd_hid_x360_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_hid_x360_suspend(id);
        break;
	case USBD_EVENT_CONFIGURED:				//为了兼容linux系统这configred后ready
		usbd_set_ready(id, true);
		break;
    case  USBD_EVENT_EP_OUT:
        usbd_hid_x360_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
