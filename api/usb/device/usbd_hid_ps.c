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
#if (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)) && ( USBD_HID_SUPPORT \
	& (BIT_ENUM(HID_TYPE_PS3) | BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5)) )

#include "api/usb/device/usbd.h"
#include "app/gamepad/ps_controller.h"


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
static uint8c_t ps_gamepade_itf_desc_tab[] = {
	0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             			//usb_desc_interface_t
    USBD_HID_DESC,          			//usb_hid_descriptor_hid_t
    0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x03,0x40,0x00,0x02,			//usb_desc_endpoint_t
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x03,0x40,0x00,0x04,			//usb_desc_endpoint_t
};


#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
//uint8c_t class_reuqes_in_03[20] ={0x03,0x21,0x27,0x04,0x41,0x00,0x2c,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0D};	//HORY 手柄
//uint8c_t class_reuqes_in_03[20] ={0x03,0x21,0x27,0x04,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0D};
static uint8c_t class_reuqes_in_03[] = {
	0x03, 0x21, 0x27, 0x04, 0xCF, 0x00, 0x2C, 0x56, 0x08, 0x00, 0x3D, 0x00, 0xE8, 0x03, 0x04, 0x00, 
	0xFF, 0x7F, 0x0D, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static uint8c_t class_reuqes_in_02[] = {
	0x02, 0x06, 0x00, 0xF7, 0xFF, 0xFA, 0xFF, 0x0B, 0x22, 0xED, 0xDD, 0x6C, 0x22, 
	0x8D, 0xDD, 0xA5, 0x23, 0x5B, 0xDC, 0x1C, 0x02, 0x1C, 0x02, 0xA6, 0x1F, 0x5A, 
	0xE0, 0x79, 0x20, 0x88, 0xDF, 0xCC, 0x1F, 0x34, 0xE0, 0x03, 0x00, 
};

static uint8c_t class_reques_in_a3[] = {
	0XA3,0X4A,0X75,0X6E,0X20,0X20,0X39,0X20,0X32,0X30,0X31,0X37,0X00,0X00,
	0X00,0X00,0X00,0X31,0X32,0X3A,0X33,0X36,0X3A,0X34,0X31,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X04,0XB4,0X01,0X00,0X00,0X00,0X07,
	0XA0,0X10,0X20,0X00,0XA0,0X02,0X00
};

uint8c_t class_reuqes_in_f3[4] ={0xF3,0x00,0x38,0x38};		//b8 不支持F2,改成38

#endif


/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_hid_ps_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_hid_ps_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_hid_ps_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len,rep_desc_len;

	len = sizeof(ps_gamepade_itf_desc_tab);
	if (desc_len <= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, ps_gamepade_itf_desc_tab, len);
		usbd_reset_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_PS4, pindex, pdesc + *pdesc_index, len);

		rep_desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_PS4, NULL);
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
error_t usbd_hid_ps_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if (USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bit.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;

		switch(preq->req.bRequest){
		case HID_REQ_CONTROL_GET_REPORT:
			if ((report_type == HID_REPORT_TYPE_FEATURE) && preq->setup_len){
				switch (report_id){
                case 0X03:		//第三方手柄主机会发03,解决PS4体感键盘无法使用的问题
					preq->setup_len = MIN(preq->setup_len, sizeof(class_reuqes_in_03));
					memcpy(preq->setup_buf,class_reuqes_in_03,preq->setup_len);
					err = ERROR_SUCCESS;
                    break; 
				case 0XF3:		//第三方手柄主机会发F3
					preq->setup_len = MIN(preq->setup_len, sizeof(class_reuqes_in_f3));
					memcpy(preq->setup_buf,class_reuqes_in_f3,preq->setup_len);
					err = ERROR_SUCCESS;
					break;
                case 0X02:		//原装手柄主机会发02
					preq->setup_len = MIN(preq->setup_len, sizeof(class_reuqes_in_02));
					memcpy(preq->setup_buf,class_reuqes_in_02,preq->setup_len);
					err = ERROR_SUCCESS;
                    break;
				case 0XA3:
					preq->setup_len = MIN(preq->setup_len, sizeof(class_reques_in_a3));
					memcpy(preq->setup_buf,class_reques_in_a3,preq->setup_len);
					err = ERROR_SUCCESS;
					break;
                case 0X12:{
					ps_bt_mac_t ps_mac;
					ps_get_bt_mac(&ps_mac);
					preq->setup_len = MIN(preq->setup_len, sizeof(ps_mac));
					memcpy(preq->setup_buf,&ps_mac,preq->setup_len);
					err = ERROR_SUCCESS;
                    break;
				}
				case 0XF1:	//responce key
					//do encryption
					break;
				case 0XF2:	//caculate key
					//do encryption
					break;
                default:
                    break;
                }
			}
			break;
		case  HID_REQ_CONTROL_SET_REPORT:
			if ((report_type == HID_REPORT_TYPE_FEATURE) && preq->setup_len){
				logd("usbd ps set report id=%d:",report_id);dumpd(preq->setup_buf,preq->setup_len);
				switch(report_id){
				case 0X13:{
					ps_set_bt_link((ps_bt_link_t*)preq->setup_buf);
					break;
				}
				case 0X14:
					err = ERROR_SUCCESS;
					break;
				case 0xF0: 	//set tocken key
					//do encryption
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

error_t usbd_hid_ps_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
	trp_handle_t trp_handle = {TR_USBD, id, U16(pclass->dev_type, pclass->hid_type)};

	ps_dev_process(&trp_handle, buf,len);

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_ps_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_ps_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_ps_handler(uint8_t id)
{
	UNUSED_PARAMETER(id);
}

#endif
