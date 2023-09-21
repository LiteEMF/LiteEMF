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
	&& ( USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_PS3) | BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5)) )

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
static uint8c_t ps_gamepade_itf_desc_tab[] = {
	0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             			//usb_desc_interface_t
    USBD_HID_DESC,          			//usb_hid_descriptor_hid_t
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0x40,0x00,0x02,			//usb_desc_endpoint_t
	0x07,0x05,(TUSB_DIR_OUT<<TUSB_DIR_POST),0x03,0x40,0x00,0x04,			//usb_desc_endpoint_t
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
	if (desc_len >= *pdesc_index + len) {
		memcpy(pdesc + *pdesc_index, ps_gamepade_itf_desc_tab, len);
		usbd_assign_configuration_desc(id, DEV_TYPE_HID, HID_TYPE_PS4, pindex, pdesc + *pdesc_index, len);

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
	ps4_cmd_t* cmdp = (ps4_cmd_t*)preq->setup_buf;

	if (TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t report_type = preq->req.wValue>>8;
		uint8_t report_id   = preq->req.wValue & 0XFF;

		switch(preq->req.bRequest){
		case HID_REQ_CONTROL_GET_REPORT:
			if ((report_type == HID_REPORT_TYPE_FEATURE) && preq->req.wLength){
				memset(cmdp, 0, sizeof(ps4_cmd_t));
				switch (report_id){
                case 0X03:		//第三方手柄主机会发03,解决PS4体感键盘无法使用的问题
					preq->setup_len = preq->req.wLength;
					memcpy(preq->setup_buf,class_reuqes_in_03,preq->setup_len);
					err = ERROR_SUCCESS;
                    break; 
				case 0XF3:		//第三方手柄主机会发F3
					preq->setup_len = preq->req.wLength;
					memcpy(preq->setup_buf,class_reuqes_in_f3,preq->setup_len);
					err = ERROR_SUCCESS;
					break;
                case 0X02:		//原装手柄主机会发02
					preq->setup_len = preq->req.wLength;
					memcpy(preq->setup_buf,class_reuqes_in_02,preq->setup_len);
					err = ERROR_SUCCESS;
                    break;
				case 0XA3:
					preq->setup_len = preq->req.wLength;
					memcpy(preq->setup_buf,class_reques_in_a3,preq->setup_len);
					err = ERROR_SUCCESS;
					break;
                case 0X12:{
					ps_bt_mac_t ps_mac;
					ps_get_bt_mac(&ps_mac);
					preq->setup_len = preq->req.wLength;
					memcpy(preq->setup_buf,&ps_mac,preq->setup_len);
					err = ERROR_SUCCESS;
                    break;
				}
				case 0XF1:	//responce key
					#if  defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED
					cmdp->cmd = PS_ANSWE_CMD;
					cmdp->index = ps_encrypt.cmd_index;
					cmdp->data_index = ps_encrypt.index;
					memcpy(cmdp->buf, &ps_encrypt_buf[ps_encrypt.index*sizeof(cmdp->buf)],sizeof(cmdp->buf));
					preq->setup_len = preq->req.wLength;
					ps_encrypt.index++;
					if(ps_encrypt.index > 18){
						ps_encrypt_stop();
					}
					err = ERROR_SUCCESS;
					#endif
					break;
				case 0XF2:	//caculate key
					#if  defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED
					cmdp->cmd = PS_CALCULATE_CHECK_CMD;
					cmdp->index = ps_encrypt.cmd_index;
					if(PS_ANSWER == ps_encrypt.step){
						cmdp->data_index = 0;
					}else{
						cmdp->data_index = 0x10;
					}
					preq->setup_len = preq->req.wLength;
					err = ERROR_SUCCESS;
					#endif
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
				case 0X13:{
					ps_set_bt_link((ps_bt_link_t*)preq->setup_buf);
					break;
				}
				case 0X14:
					err = ERROR_SUCCESS;
					break;
				case 0xF0: 	//set tocken key
					#if  defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED
					memcpy(&ps_encrypt_buf[cmdp->data_index*sizeof(cmdp->buf)], cmdp->buf,sizeof(cmdp->buf));
					if(cmdp->data_index == 4){
						ps_encrypt_start(cmdp->index);
					}
					err = ERROR_SUCCESS;
					#endif
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

error_t usbd_hid_ps_out_process(uint8_t id, usbd_class_t* pclass)
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
error_t usbd_hid_ps_init(uint8_t id)
{
	trp_handle_t trp_handle;
		
	trp_handle.trp = TR_USBD;
	trp_handle.id = id;
	trp_handle.index = U16(DEV_TYPE_HID, HID_TYPE_PS4);
	app_gamepad_init( &trp_handle ); 
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
	trp_handle_t trp_handle;
		
	trp_handle.trp = TR_USBD;
	trp_handle.id = id;
	trp_handle.index = U16(DEV_TYPE_HID, HID_TYPE_PS4);
	app_gamepad_deinit( &trp_handle ); 
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_hid_ps_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
		usbd_hid_ps_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_hid_ps_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_hid_ps_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#endif
