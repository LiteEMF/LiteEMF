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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))) && (USBH_HID_SUPPORT & HID_GAMEPAD_MASK)
#include "api/api_transport.h"
#include "api/usb/host/usbh.h"

#include "app/gamepad/app_gamepad.h"
#if APP_RUMBLE_ENABLE
#include  "app/app_rumble.h"
#endif

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
app_gamepad_key_t usbh_gamepad_key;	   				//手柄按键信息
hid_type_t usbh_gamepad_type = HID_TYPE_NONE;

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
        	}else{
	            dev = HID_TYPE_XBOX;
            }
            break;
        case PS_VID:
        	if(PS3_PID == pid){
				dev = HID_TYPE_PS3;
			}else if((PS5_PID == pid) || (PS5_EDGE_PID == pid)){
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
** Description:		
*******************************************************************/
bool usbh_hid_gamepad_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	// logd("hid endp%d in%d:",(uint16_t)pclass->endpin.addr,len);dumpd(buf,len);
    bool ret;
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    hid_type_t hid_type = app_gamepad_get_hidtype(pclass->hid_types);
    trp_handle.index = U16(pclass->dev_type, hid_type);

   ret = app_gamepad_host_process(&trp_handle,&usbh_gamepad_key,buf,len);

    #if USBH_SOCKET_ENABLE
	if(!ret){
        usbh_socket_cmd(&usbh_socket_trp,CMD_SOCKET_IN,buf,len);
        ret = true;
	}
	#endif
    return ret;
}

error_t usbh_hid_gamepad_open( uint8_t id, usbh_class_t *pclass)
{
    error_t err = ERROR_SUCCESS;
    uint8_t buf[0X40];
    uint16_t len;
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    hid_type_t hid_type = app_gamepad_get_hidtype(pclass->hid_types);		//注意不会有多个手柄描述符,这里只处理一个手柄设备
    trp_handle.index = U16(pclass->dev_type, hid_type);
    
    switch(hid_type){
        #if USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_GAMEPADE) | BIT_ENUM(HID_TYPE_DINPUT))
        case HID_TYPE_GAMEPADE:
        case DEF_HID_TYPE_DINPUT:
            gamepad_controller_init(&trp_handle);
            break;
        #endif
        #if (HIDD_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            if(pclass->endpin.type == TUSB_ENDP_TYPE_INTER){             //gamepad
                // if(HID_TYPE_X360 == hid_type){
                //     uint8_t x360_ift_str[0XB2];
                //     uint16_t str_len = sizeof(x360_ift_str);
                //     usbh_req_get_string_desc(id, 4, 0X409, x360_ift_str, &str_len);
                // }
                xbox_controller_init(&trp_handle);
            }else if(pclass->endpin.type == TUSB_ENDP_TYPE_ISOCH){       //audio //TODO 这里进不来
                #if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)        //如果支持打开audio接口,xbox默认一直打开
                usbh_req_set_itf(id, pclass->itf.if_num, 1);
                #endif
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
            len = 4;
            memset(buf,0,0x11);
    
            buf[0] = PS3_CMD_ON >> 4;
            buf[1] = PS3_CMD_ON & 0XFF;
            usbh_hid_set_report(id, pclass->itf.if_num, HID_REPORT_TYPE_FEATURE,0XF4, buf, len);
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

            #if APP_RUMBLE_ENABLE
            app_rumble_set_duty(RUMBLE_L,0,1000);      //马达震动提示, 同时设置ps4灯光   
            #endif
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
    
    #if USBH_SOCKET_ENABLE      //socket从这里开始触发引导
    if(ERROR_SUCCESS == err){
        usbh_socket_init(&usbh_socket_trp, U16(DEV_TYPE_HID, hid_type));       //TODO 选择socket通讯协议
    }
	#endif

    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	这里只做识别
*******************************************************************/
error_t usbh_hid_gamepad_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo ,uint8_t collection_index)
{
    error_t err = ERROR_NOT_FOUND;
    usbh_dev_t* pdev = get_usbh_dev(id);
    hid_type_t 	hid_type;
    
    if(NULL == pdev) return err;
    hid_type = usbh_hid_vendor_gamepad(pdev->vid, pdev->pid);   //先通过vid 获取设备类型

    if(HID_TYPE_NONE == hid_type){
        if(XBOX_CLASS == pclass->itf.if_cls){          //特殊识别xbox
            if(XBOX_SUBCLASS == pclass->itf.if_sub_cls){
                hid_type = HID_TYPE_XBOX;
            }else if (X360_SUBCLASS == pclass->itf.if_sub_cls 
                || X360_IDENTIFY_SUBCLASS == pclass->itf.if_sub_cls){
                hid_type = HID_TYPE_X360;
            }
        }else if(NULL != pinfo){          //TUSB_CLASS_HID识别ps4 switch dinput
            uint8_t i;
            hid_collection_t* pcollection;
            hid_items_t hat_switch,vendor;

            memset(&hat_switch, 0, sizeof(hat_switch));
            memset(&vendor, 0, sizeof(vendor));

            
            for(i=0; i<pinfo->collections; i++){
                if(hid_match_collection(pinfo, i, 0X01, 0X05)){ //0x05,0x01  0x09,0X05 fix gamepad
                    pcollection = &pinfo->item_list.collectionList[i];
                    if(0 == hat_switch.report_length){
                        hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X01, 0x39,&hat_switch);
                    }
                    if(0 == vendor.report_length){
                        if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_FEATURE,0xFFF0, 0x47,&vendor)){   //usb
                            if((0x3F != vendor.bit_count) || (0x08 != vendor.bit_size)){
                                memset(&vendor, 0, sizeof(vendor));
                            }
                        }
                    }
                }
            }
            if((1 == vendor.report_id) && (1 == hat_switch.report_id)){
                hid_type = HID_TYPE_PS4;
            }
        }
    }

    if(HID_TYPE_NONE != hid_type){
        #if USBH_HID_SUPPORT & HID_XBOX_MASK
        if(HID_TYPE_X360 == hid_type){      //x360 特殊处理
            if(X360_IDENTIFY_SUBCLASS == pclass->itf.if_sub_cls){
                m_x360_identify_itf = pclass->itf.if_num;
                logd("m_x360_identify_itf=%d\n",m_x360_identify_itf);
            }else if(pclass->endpin.addr && (X360_PROTOCOL == pclass->itf.if_pro)){
                err = ERROR_SUCCESS;
            }
        #endif
        }else if(pclass->endpin.addr && (TUSB_ENDP_TYPE_INTER == pclass->endpin.type)){
            err = ERROR_SUCCESS;
        }   

        if(ERROR_SUCCESS == err){
            pclass->hid_types |= BIT(hid_type);
            usbh_gamepad_type = hid_type;
            // logd("usbh gamepad init type=%d\n",usbh_gamepad_type);
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
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    hid_type_t hid_type = app_gamepad_get_hidtype(pclass->hid_types);	//注意不会有多个手柄描述符,这里只处理一个手柄设备
    trp_handle.index = U16(pclass->dev_type, hid_type);

    app_gamepad_deinit(&trp_handle);
    usbh_gamepad_type = HID_TYPE_NONE;
    
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
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    hid_type_t hid_type = app_gamepad_get_hidtype(pclass->hid_types);
    trp_handle.index = U16(pclass->dev_type, hid_type);
    app_gamepad_task(&trp_handle);
    UNUSED_PARAMETER(id);
}

#endif






