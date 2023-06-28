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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID))
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
uint8c_t usbd_hid_descriptor_tab[9] = { USBD_HID_DESC  };


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t usbd_hid_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = 0;
    uint8_t hid_type;
	
	for(hid_type=0; hid_type<16; hid_type++){
		if(m_usbd_hid_types[id] & (1UL<<hid_type)){
			switch(hid_type){
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
                case HID_TYPE_VENDOR:
                    len += usbd_hid_vendor_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
                    break;
                #endif
                #if USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER) )
                case HID_TYPE_KB:
				case HID_TYPE_MOUSE:
				case HID_TYPE_CONSUMER:
                    len += usbd_hid_km_get_itf_desc(id, hid_type, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
				#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT)
				case HID_TYPE_MT:
                    len += usbd_hid_mt_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
				case HID_TYPE_GAMEPADE:
                    len += usbd_hid_gamepade_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
				case HID_TYPE_X360	:
                    len += usbd_hid_x360_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
				case HID_TYPE_XBOX	:
                   len += usbd_hid_xbox_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
				case HID_TYPE_SWITCH:
                    len += usbd_hid_switch_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
				case HID_TYPE_PS3	:
                    len += usbd_hid_ps_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
				case HID_TYPE_PS4	:
                    len += usbd_hid_ps_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
				case HID_TYPE_PS5	:
                    len += usbd_hid_ps_get_itf_desc(id, pindex, pdesc, desc_len, pdesc_index);
					break;
                #endif
				default:
					break;
            }
        }
    }
    return len;
}



error_t usbd_hid_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_SUCCESS;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
		if (preq->req.bRequest == USB_REQ_GET_DESCRIPTOR){
			uint8_t desc_type = (uint8_t)(preq->req.wValue >> 8);
			uint16_t desc_len;
			uint8_t *desc_buf;

			desc_len = get_hid_desc_map(TR_USBD, pclass->hid_type ,&desc_buf);

			if(HID_DESC_TYPE_REPORT == desc_type){
				preq->setup_len = MIN(preq->req.wLength, desc_len);
				memcpy((void*)preq->setup_buf,desc_buf,preq->setup_len);

                if(HID_TYPE_SWITCH != pclass->hid_type){        //switch ready 在 switch_controller 中设置
                    pdev->ready = true;
                    logd_g("usbd%d ready...\n",id);
                }
			}else if(HID_DESC_TYPE_HID == desc_type){
				preq->setup_len = MIN(preq->req.wLength, 9);
				memcpy((void*)preq->setup_buf,usbd_hid_descriptor_tab,preq->setup_len);
				preq->setup_buf[7] = desc_len&0xff;            //set hid report desc
				preq->setup_buf[8] = desc_len>>8;
			}else{
                err = ERROR_STALL;
            }
		}else{
            err = ERROR_STALL;
        }
	} else if (USB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		switch(preq->req.bRequest){
            case HID_REQ_CONTROL_SET_IDLE:
                break;
            case HID_REQ_CONTROL_GET_IDLE:
                preq->setup_buf[0] = 0;
                break;
			default:
				err = ERROR_STALL;
				break;
		}
    }
    
    if(ERROR_STALL ==  err){
        switch(pclass->hid_type){
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
        case HID_TYPE_VENDOR:
            err = usbd_hid_vendor_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER) )
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
        case HID_TYPE_CONSUMER:
            err = usbd_hid_km_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT)
        case HID_TYPE_MT:
            err = usbd_hid_mt_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            err = usbd_hid_gamepade_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
        case HID_TYPE_X360	:
            err = usbd_hid_x360_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
        case HID_TYPE_XBOX	:
            err = usbd_hid_xbox_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
        case HID_TYPE_SWITCH:
            err = usbd_hid_switch_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
        case HID_TYPE_PS3	:
            err = usbd_hid_ps_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
        case HID_TYPE_PS4	:
            err = usbd_hid_ps_control_request_process(id, pclass, preq);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
        case HID_TYPE_PS5	:
            err = usbd_hid_ps_control_request_process(id, pclass, preq);
            break;
        #endif
        default:
            break;
        }
    }
    return err;
}



/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_init(uint8_t id)
{
    uint8_t hid_type;

	for(hid_type=0; hid_type<16; hid_type++){
		if(m_usbd_hid_types[id] & (1UL<<hid_type)){
			switch(hid_type){
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
            case HID_TYPE_VENDOR:
                usbd_hid_vendor_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER) )
            case HID_TYPE_KB:
            case HID_TYPE_MOUSE:
            case HID_TYPE_CONSUMER:
                usbd_hid_km_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT)
            case HID_TYPE_MT:
                usbd_hid_mt_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
            case HID_TYPE_GAMEPADE:
                usbd_hid_gamepade_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
            case HID_TYPE_X360	:
                usbd_hid_x360_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
            case HID_TYPE_XBOX	:
                usbd_hid_xbox_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
            case HID_TYPE_SWITCH:
                usbd_hid_switch_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
            case HID_TYPE_PS3	:
                usbd_hid_ps_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
            case HID_TYPE_PS4	:
                usbd_hid_ps_init(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
            case HID_TYPE_PS5	:
                usbd_hid_ps_init(id);
                break;
            #endif
            default:
                break;
            }
        }
    }
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_hid_deinit(uint8_t id)
{
    uint8_t hid_type;

	for(hid_type=0; hid_type<16; hid_type++){
		if(m_usbd_hid_types[id] & (1UL<<hid_type)){
			switch(hid_type){
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
            case HID_TYPE_VENDOR:
                usbd_hid_vendor_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER) )
            case HID_TYPE_KB:
            case HID_TYPE_MOUSE:
            case HID_TYPE_CONSUMER:
                usbd_hid_km_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT)
            case HID_TYPE_MT:
                usbd_hid_mt_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
            case HID_TYPE_GAMEPADE:
                usbd_hid_gamepade_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
            case HID_TYPE_X360	:
                usbd_hid_x360_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
            case HID_TYPE_XBOX	:
                usbd_hid_xbox_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
            case HID_TYPE_SWITCH:
                usbd_hid_switch_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
            case HID_TYPE_PS3	:
                usbd_hid_ps_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
            case HID_TYPE_PS4	:
                usbd_hid_ps_deinit(id);
                break;
            #endif
            #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
            case HID_TYPE_PS5	:
                usbd_hid_ps_deinit(id);
                break;
            #endif
            default:
                break;
            }
        }
    }
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description: 
*******************************************************************/
void usbd_hid_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{

    switch(pclass->hid_type){
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
        case HID_TYPE_VENDOR:
            usbd_hid_vendor_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_CONSUMER) )
        case HID_TYPE_KB:
        case HID_TYPE_MOUSE:
        case HID_TYPE_CONSUMER:
            usbd_hid_km_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_MT)
        case HID_TYPE_MT:
            usbd_hid_mt_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            usbd_hid_gamepade_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
        case HID_TYPE_X360	:
            usbd_hid_x360_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
        case HID_TYPE_XBOX	:
            usbd_hid_xbox_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
        case HID_TYPE_SWITCH:
            usbd_hid_switch_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
        case HID_TYPE_PS3	:
            usbd_hid_ps_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
        case HID_TYPE_PS4	:
            usbd_hid_ps_process(id, pclass, evt, val);
            break;
        #endif
        #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
        case HID_TYPE_PS5	:
            usbd_hid_ps_process(id, pclass, evt, val);
            break;
        #endif
        default:
            break;
    }
}

#endif
