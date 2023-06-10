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
#if USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA)) 

#include "api/usb/host/usbh.h"

#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad.h"
#endif


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
app_gamepad_key_t usbh_gamepad_key;	   				//手柄按键信息


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
hid_type_t usbh_hid_vendor_gamepad(uint16_t vid, uint16_t pid)
{
	hid_type_t dev = DEV_TYPE_NONE;

    switch(vid){
        case SWITCH_VID:
            dev = HID_TYPE_SWITCH;
            break;
        case XBOX_VID:
			if(X360_PID == pid){
				dev = HID_TYPE_X360;
			}else if(XBOXX_PID == pid){
				dev = HID_TYPE_XBOX;
        	}else{
	            dev = HID_TYPE_XBOX;
            }
            break;
        case PS_VID:
        	if(PS3_PID == pid){
				dev = HID_TYPE_PS3;
			}else if(PS5_PID == pid){
				dev = HID_TYPE_PS5;
            }else if(PS4_PID == pid){
				dev = HID_TYPE_PS4;
        	}else if(pid == PS4_S1_PID){
				dev = HID_TYPE_PS4;
			}
            break;
        case COOVE_VID:
            if(COOVELITE_PID == pid){  
				dev = HID_TYPE_PS4;
			}
            break;
        default:
            dev = HID_TYPE_NONE;
            break;
    }
    return dev;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdata used storage hub port numbers	
*******************************************************************/
void usbh_hid_gamepad_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("hid endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);

    trp_handle_t trp_handle = {TR_USBH, id, U16(pclass->dev_type,pclass->hid_type) };
    app_gamepad_in_process(&trp_handle,&usbh_gamepad_key,buf,len);
}

error_t usbh_hid_gamepad_open( uint8_t id, usbh_class_t *pclass)
{
    error_t err = ERROR_SUCCESS;
    uint8_t buf[0X40];
    uint16_t len;
    trp_handle_t trp_handle = {TR_USBH, id, U16(pclass->dev_type,pclass->hid_type) };
    
    switch(pclass->hid_type){
        #if USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            gamepad_controller_init(&trp_handle);
            break;
        #endif
        #if (HIDD_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            if(pclass->itf.if_num){
                if(pclass->endpin.type == USB_ENDP_TYPE_INTER){             //gamepad
                    xbox_controller_init(&trp_handle);
                }else if(pclass->endpin.type == USB_ENDP_TYPE_ISOCH){       //audio
                    #if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)        //如果支持打开audio接口,xbox默认一直打开
                    usbh_req_set_itf(id, pclass->itf.if_num, 1);
                    #endif
                }
            }
            break;
        #endif
        #if USBH_HID_SUPPORT & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            switch_controller_init(&trp_handle);
            break;
        #endif
        #if USBH_HID_SUPPORT & HID_PS_MASK
        case HID_TYPE_PS3	:
            ps_controller_init(&trp_handle);
            break;
        case HID_TYPE_PS4	:
            ps_controller_init(&trp_handle);

            len = 0x30;
            err = usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0X03, buf, &len);
            if(ERROR_SUCCESS != err){
                len = 0X25;
                err = usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0x02, buf, &len);
                len = 0X31;
                usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0xA3, buf, &len);
                len = 0X10;
                usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0x12, buf, &len);
            }
            break;
        case HID_TYPE_PS5	:
            ps_controller_init(&trp_handle);

            len = 0X29;
            err = usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0X03, buf, &len);
            len = 0X40;
            usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0x20, buf, &len);
            len = 0X10;
            usbh_hid_get_report(id, pclass->itf.if_num,HID_REPORT_TYPE_FEATURE, 0x09, buf, &len);

            len = 0X11;
            memset(buf,0,0x11);
            buf[0] = 0x08;
            buf[1] = 0x01;
            usbh_hid_set_report(id, pclass->itf.if_num, HID_REPORT_TYPE_FEATURE,0x08, buf, len);
            break;
        #endif
        default:
            break;
    }
    
    return err;
}

error_t usbh_hid_gamepad_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo)
{
    error_t err = ERROR_NOT_FOUND;
    usbh_dev_t* pdev = get_usbh_dev(id);
    hid_type_t 	hid_type;

    hid_type = usbh_hid_vendor_gamepad(pdev->vid, pdev->pid);       //先通过vid 获取设备类型

    if(HID_TYPE_NONE != hid_type){
        pclass->hid_type = hid_type;
        err = ERROR_SUCCESS;
        logd("usbh gamepad type=%d\n",pclass->hid_type);
        return err;
    }

    if(USB_CLASS_VENDOR == pclass->itf.if_cls){     //特殊识别xbox
        if(XBOX_SUBCLASS == pclass->itf.if_sub_cls){
            pclass->hid_type = HID_TYPE_XBOX;
            if(XBOXX_PID == pdev->pid){
                logd("usbh xbox seriesx\n");
				// xbox_series = XBOX_SERIESX;
                err = ERROR_SUCCESS;
            }else{
                logd("usbh xbox one\n");
                err = ERROR_SUCCESS;
            }
        }else if (X360_SUBCLASS == pclass->itf.if_sub_cls){
            pclass->hid_type = HID_TYPE_X360;
            logd("usbh x360\n");
            err = ERROR_SUCCESS;
        }
    }else{          //USB_CLASS_HID识别ps4 switch dinput
        uint8_t i;
        hid_items_t hat_switch,vendor;

        memset(&hat_switch, 0, sizeof(hat_switch));
        memset(&vendor, 0, sizeof(vendor));

        for(i=0; i<pinfo->reportItems; i++){
            if(0 == hat_switch.report_length){
                hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X01, 0x39,&hat_switch);
            }
            if(0 == vendor.report_length){
                if(hid_find_items(pinfo,i,HID_REPORT_TYPE_FEATURE,0xFFF0, 0x47,&vendor)){   //usb
                    if((0x3F != vendor.bit_count) || (0x08 != vendor.bit_size)){
                        memset(&vendor, 0, sizeof(vendor));
                    }
                }
            }
        }

        if((1 == vendor.report_id) && (1 == hat_switch.report_id)){
            pclass->hid_type = HID_TYPE_PS4;
            err = ERROR_SUCCESS;
        }
    }

    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_gamepad_deinit( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNKNOW;
    trp_handle_t trp_handle = {TR_USBH, id, U16(pclass->dev_type,pclass->hid_type) };

    app_gamepad_deinit(&trp_handle);
    
    UNUSED_PARAMETER(id);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hid_gamepad_task(uint8_t id, usbh_class_t *pclass)
{
    trp_handle_t trp_handle = {TR_USBH, id, U16(pclass->dev_type,pclass->hid_type) };
    app_gamepad_task(&trp_handle);
    UNUSED_PARAMETER(id);
}

#endif






