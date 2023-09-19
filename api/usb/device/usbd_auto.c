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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO))
#include "api/usb/device/usbd.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
static uint8c_t auto_desc_gamepad_map[] =
{
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x05,        // Usage (Game Pad)
0xA1, 0x01,        // Collection (Application)
	0x85, 0x01,        //   Report ID (1)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x01,        //   Usage (Pointer)
	0xA1, 0x00,        //   Collection (Physical)
		0x09, 0x30,        //     Usage (X)
		0x09, 0x31,        //     Usage (Y)
		0x09, 0x32,        //     Usage (Z)
		0x09, 0x35,        //     Usage (Rz)
		0x15, 0x00,        //     Logical Minimum (0)
		0x26, 0xFF, 0x00,  //     Logical Maximum (255)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x04,        //     Report Count (4)
		0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0x09, 0x39,        //   Usage (Hat switch)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x07,        //   Logical Maximum (7)
	0x35, 0x00,        //   Physical Minimum (0)
	0x46, 0x3B, 0x01,  //   Physical Maximum (315)
	0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x10,        //   Usage Maximum (0x10)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x10,        //   Report Count (16)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x02,        //   Usage Page (Sim Ctrls)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x09, 0xC4,        //   Usage (Accelerator)
	0x09, 0xC5,        //   Usage (Brake)
	0x95, 0x02,        //   Report Count (2)
	0x75, 0x08,        //   Report Size (8)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection
};
static uint8c_t auto_itf_desc_tab[] = {
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             			//usb_desc_interface_t
    0x09,0x21,0X11,0X01,0x00,0x01,0x22,0x00,0x00,
    0x07,0x05,(TUSB_DIR_IN<<TUSB_DIR_POST), 0x03,0x40,0x00,0x02,			//usb_desc_endpoint_t
	0x07,0x05,(TUSB_DIR_OUT<<TUSB_DIR_POST),0x03,0x40,0x00,0x04,			//usb_desc_endpoint_t
};


usbd_det_t m_usbd_auto_type = USBD_DET_NULL;				//自动识别到的主机


static uint8_t usbd_setup_devdesc_len;			//第一次获取设备描述符长度值
static uint8_t usbd_steup_buf[2][8];
static uint8_t usbd_steup_setp;				//从0开始, 包括TUSB_GET_DESCRIPTOR 和特殊定义config, step3,step4用于判断主机类型, setp6~step7单独用于判断PC和XBOX
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
	//PC
0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00,				//需要在step7继续判断XBOX
	//ANDROID
0X80,0x06,0x00,0x06,0x00,0x00,0x0A,0x00,
	//SWITCH
0x80,0x06,0x00,0x02,0x00,0x00,0x09,0x00,				//跑到第五步:switch, 只跑到底4步, 判断第一步是否是R90
	//X360
0x80,0x06,0x00,0x02,0x00,0x00,0x50,0x01,
	//XBOX
0x80,0x06,0xEE,0x03,0x00,0x00,0x12,0x00,				//开机第一次XBOX 会读取该XBOX特殊指令
	//XBOXX
0x80,0x06,0x03,0x03,0x09,0x04,0xFF,0x00,
	//PS3
0x80,0x06,0x00,0x02,0x00,0x00,0x08,0x00,
	//PS4
0x80,0x06,0x00,0x03,0x00,0x00,0x02,0x00,
	//PS5
0x80,0x06,0x00,0x03,0x00,0x00,0x04,0x00,
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
    usbd_steup_setp = 0;
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

    if (desc_len >= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, auto_itf_desc_tab, len);
        usbd_assign_configuration_desc(id, DEV_TYPE_AUTO, 0, pindex, pdesc + *pdesc_index, len);

        rep_desc_len = sizeof(auto_desc_gamepad_map);
		pdesc[*pdesc_index + 16] = rep_desc_len&0xff;            //set hid report desc
		pdesc[*pdesc_index + 17] = rep_desc_len>>8;
    }
    *pdesc_index += len;

    return len;
}


/*******************************************************************
** Parameters:  pclass unused
** Returns:
** Description:
*******************************************************************/
error_t usbd_auto_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
    uint8_t i;
    
    logd("usbd_auto_setp=%d:",usbd_steup_setp);dumpd(&preq->req, 8);

    if (TUSB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
		if (TUSB_REQ_GET_DESCRIPTOR == preq->req.bRequest) {
            uint8_t desc_type = (uint8_t)(preq->req.wValue >> 8);
	        uint8_t desc_index = (uint8_t)(preq->req.wValue & 0xFF);

		    if (TUSB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bits.recipient) {
                if(TUSB_DESC_DEVICE == desc_type){
                    if(1== usbd_setup_devdesc_len){
                        usbd_setup_devdesc_len = preq->req.wLength;  //第一次获取设备描述符长度
                    }
                }
            }else if(TUSB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bits.recipient) {
                if(HID_DESC_TYPE_REPORT == desc_type){
                    preq->setup_len = MIN(preq->req.wLength, sizeof(auto_desc_gamepad_map));
                    memcpy(preq->setup_buf,auto_desc_gamepad_map,preq->setup_len);
                }
            }
        }
	}

    if( (3 == usbd_steup_setp) ||  (4 == usbd_steup_setp) ){
		usbd_steup_timer = m_systick;
		memcpy(usbd_steup_buf[usbd_steup_setp-3],&preq->req,8);			//只保存step3 和step4
	}else if(6 == usbd_steup_setp){									    //step6 开始计时
		usbd_steup_timer = m_systick;
	}

	if(4 == usbd_steup_setp){					//区分pc boot模式, boot模式下只能枚举4步
		for(i = 0; i < USBD_DET_MAX; i++){
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
		logd("usbd auto %d setup:",i);	dumpd(usbd_steup_buf[0], sizeof(usbd_steup_buf));
	}

    usbd_steup_setp++;
    return err;
}

error_t usbd_auto_out_process(uint8_t id, usbd_class_t* pclass)
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
    usbd_steup_setp = 0;
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
void usbd_auto_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
        usbd_auto_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_auto_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_auto_out_process(id, pclass);
        break;
    case  USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}

#if WEAK_ENABLE
__WEAK void ex_usbd_auto_det(uint8_t id, usbd_det_t dev)
{
	logd("usbd auto det=%d\n",(uint16_t)dev);
    m_usbd_types[id] = USBD_TYPE_SUPPORT & ~BIT(DEV_TYPE_AUTO);
    switch(dev){
        #if (USBD_HID_SUPPORT & HID_SWITCH_MASK)
        case USBD_DET_SWITCH:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_SWITCH);
            break;		
        #endif	
        #if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360))	
        case USBD_DET_X360:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_X360);
            break;
        #endif	
        #if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
        case USBD_DET_XBOX:
        case USBD_DET_XBOXX:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_XBOX);
            break;
        #endif	
        #if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3))
        case USBD_DET_PS3:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_PS3);
            break;
        #endif	
        #if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4))
        case USBD_DET_PS4:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_PS4);
            break;
        #endif
        #if (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5))
        case USBD_DET_PS5:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT & ~HID_GAMEPAD_MASK | BIT(HID_TYPE_PS5);
            break;
        #endif

        case USBD_DET_PC:
        case USBD_DET_ANDROID:
        default:
            m_usbd_hid_types[id] = USBD_HID_SUPPORT;
            break;
    }

    usbd_class_init(id);		//初始化usbd 类
    usbd_reset(id);
    usbd_core_init(id);
}
#endif

void usbd_auto_task(uint8_t id)
{	
	if(USBD_DET_NULL != m_usbd_auto_type){
        usbd_dev_t *pdev = usbd_get_dev(id);

		if(USBD_DET_PC == m_usbd_auto_type){				//PC/xbox特殊判断, Xbox 有时候不会set configuration
			if(usbd_steup_setp > 9){						//判断为PC
				ex_usbd_auto_det(id, m_usbd_auto_type);
			}else if(6 <= usbd_steup_setp){					//大于6 小于 9
				if((m_systick - usbd_steup_timer) > 170){		//超时没有获取到判断为xbox, xbox 要到第8步骤才停止
					m_usbd_auto_type = USBD_DET_XBOX;
					ex_usbd_auto_det(id, m_usbd_auto_type);
				}
			}
		}else if(USBD_DET_SWITCH == m_usbd_auto_type){		//switch 特殊判断
			if(TUSB_STA_CONFIGURED != pdev->state) return;  //需等configuration
			
			if(usbd_steup_setp >= 5){						//能跑到第五步才是SWITCH
				ex_usbd_auto_det(id, m_usbd_auto_type);
			}else{				//此处可能是PC boot 等待reset
			}
		}else{												//其他正常识别到
			ex_usbd_auto_det(id, m_usbd_auto_type);
		}
	}
}	



#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:	10ms周期	
** Returns:	
** Description:		
*******************************************************************/
void usbd_auto_handler(uint8_t id, uint32_t period_10us)
{
	static timer_t s_timer;

    if(m_usbd_types[id] & BIT_ENUM(DEV_TYPE_AUTO)){
        if((m_task_tick10us - s_timer) >= period_10us){
            s_timer = m_task_tick10us;
            usbd_auto_task(id);
        }
    }
}
#endif


#endif
