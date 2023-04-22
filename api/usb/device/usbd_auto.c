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
#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
#include "api/usb/device/usbd.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
static uint8c_t auto_desc_gamepad_map[] =
{
    #include "api/hid/hid_desc_gamepad.h"
};
static uint8c_t auto_itf_desc_tab[] = {
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             			//usb_desc_interface_t
    USBD_HID_DESC,									          			//usb_hid_descriptor_hid_t
    0x07,0x05,(USB_DIR_IN<<USB_DIR_POST), 0x03,0x40,0x00,0x02,			//usb_desc_endpoint_t
	0x07,0x05,(USB_DIR_OUT<<USB_DIR_POST),0x03,0x40,0x00,0x04,			//usb_desc_endpoint_t
};


usbd_det_t m_usbd_auto_type = USBD_DET_NULL;				//自动识别到的主机


static uint8_t usbd_setup_devdesc_len;			//第一次获取设备描述符长度值
static uint8_t usbd_steup_buf[2][8];
static uint8_t usbd_steup_setp;				//从1开始, 包括USB_GET_DESCRIPTOR 和特殊定义config, 不包含setaddr,setconfig等, step3,step4用于判断主机类型, setp6~step7单独用于判断PC和XBOX
static timer_t usbd_steup_timer;			//用于第7步计时

// static uint8c_t usb_host_enum_list[USBD_DET_MAX][8*3]={
// 	//SWITCH 会跑到第五步		//PC boot只能跑4步后会重启
// 0x80,0x06,0x00,0x01,0x00,0x00,0x08,0x00,	0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00,
// 	//PS4
// 0x80,0x06,0x00,0x01,0x00,0x00,0x08,0x00,	0X80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0x80,0x06,0x00,0x03,0x00,0x00,0x02,0x00,
// 	//XBOX 第一次 后面和PC一样
// 0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0X80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00,	0x80,0x06,0xEE,0x03,0x00,0x00,0x12,0x00,
// 	//XBOXX PC 需要进一步验证第4步
// 0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0X80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00,	0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00, 0xC0,0x90,0x00,0x00,0x04,0x00,0x10,0x00
// 	//PS5
// 0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0X80,0x06,0x00,0x03,0x00,0x00,0x02,0x00,	0x80,0x06,0x00,0x03,0x00,0x00,0x04,0x00,
// 	//PS3
// 0x80,0x06,0x00,0x01,0x00,0x00,0x40,0x00,	0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0x80,0x06,0x00,0x02,0x00,0x00,0x08,0x00,
// 	//X360
// 0x80,0x06,0x00,0x01,0x00,0x00,0x08,0x00,	0X80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0x80,0x06,0x00,0x02,0x00,0x00,0x50,0x01,
// 	//ANDROID
// 0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0X80,0x06,0x00,0x06,0x00,0x00,0x0A,0x00,	0X80,0x06,0x00,0x06,0x00,0x00,0x0A,0x00,
// 	//PC	XBOXX
// 0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0X80,0x06,0x00,0x02,0x00,0x00,0xFF,0x00,	0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00,
//	//R90 dongle只能跑4步, 不会重启
// 0x80,0x06,0x00,0x01,0x00,0x00,0x40,0x00,	0x80,0x06,0x00,0x01,0x00,0x00,0x12,0x00,	0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00,
// };



static uint8c_t xboxx_enum4[8]={0xC0 ,0x90 ,0x00 ,0x00 ,0x04 ,0x00 ,0x10 ,0x00};			//xboxx
static uint8c_t usb_host_enum3[USBD_DET_MAX][8]={
	//SWITCH
0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00,				//跑到第五步:switch, 只跑到底4步, 判断第一步是否是R90
	//PS4
0x80,0x06,0x00,0x03,0x00,0x00,0x02,0x00,
	//XBOX
0x80,0x06,0xEE,0x03,0x00,0x00,0x12,0x00,				//开机第一次XBOX 会读取该XBOX特殊指令
	//XBOXX
0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00,
	//PS5
0x80,0x06,0x00,0x03,0x00,0x00,0x04,0x00,
	//PS3
0x80,0x06,0x00,0x02,0x00,0x00,0x08,0x00,
	//X360
0x80,0x06,0x00,0x02,0x00,0x00,0x50,0x01,
	//ANDROID
0X80,0x06,0x00,0x06,0x00,0x00,0x0A,0x00,
	//PC
0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00,				//需要在step7继续判断XBOX
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
error_t usbd_auto_reset(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

error_t usbd_auto_suspend(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}


uint16_t usbd_auto_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(auto_itf_desc_tab),rep_desc_len;

    if (desc_len <= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, auto_itf_desc_tab, len);
        usbd_reset_configuration_desc(id, DEV_TYPE_AUTO, 0, pindex, pdesc + *pdesc_index, len);

        rep_desc_len = sizeof(auto_desc_gamepad_map);
		pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
		pdesc[*pdesc_index + 17] = rep_desc_len>>8;
    }
    *pdesc_index += len;

    return len;
}



error_t usbd_auto_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
    uint8_t i;
	
    if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bit.type){
		if (USB_REQ_GET_DESCRIPTOR == preq->req.bRequest) {
            uint8_t desc_type = (uint8_t)(preq->req.wValue >> 8);
	        uint8_t desc_index = (uint8_t)(preq->req.wValue & 0xFF);

		    if (USB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bit.recipient) {
                usbd_steup_setp++;
                if(USB_DESC_DEVICE == desc_type){
                    if(1== usbd_setup_devdesc_len){
                        usbd_setup_devdesc_len = preq->req.wLength;  //第一次获取设备描述符长度
                    }
                }
            }else if(USB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bit.recipient) {
                if(HID_DESC_TYPE_REPORT == desc_type){
                    preq->setup_len = MIN(preq->setup_len, sizeof(auto_desc_gamepad_map));
                    memcpy(preq->setup_buf,auto_desc_gamepad_map,preq->setup_len);
                }
            }
        }
	}else{
        usbd_steup_setp++;
    }

    if( (3 == usbd_steup_setp) ||  (4 == usbd_steup_setp) ){
		usbd_steup_timer = m_systick;
		memcpy(usbd_steup_buf[usbd_steup_setp-3],&preq->req,8);			//只保存step3 和step4
	}else if(6 == usbd_steup_setp){									    //step6 开始计时
		usbd_steup_timer = m_systick;
	}

	if(4 == usbd_steup_setp){					//区分pc boot模式, boot模式下只能枚举4步
		for(i=0; i< USBD_DET_MAX; i++){
			if(0 == memcmp(usbd_steup_buf[0], usb_host_enum3[i], 8)){
				if(i == USBD_DET_XBOXX){
					if(0 == memcmp(usbd_steup_buf[1], xboxx_enum4, 8)){
						break;
					}
				}else{
					break;
				}
			}
		}
		m_usbd_auto_type = (usbd_det_t)i;
		//logd("usbd auto %d setup:",i);	dumpd(usbd_steup_buf[0], sizeof(usbd_steup_buf));
	}

    return err;
}

error_t usbd_auto_out_process(uint8_t id, usbd_class_t* pclass, uint8_t* buf, uint16_t len)
{
    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_auto_init(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_auto_deinit(uint8_t id)
{
    UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_auto_task(uint8_t id)
{
    UNUSED_PARAMETER(id);
}

#endif
