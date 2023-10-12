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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
#include "api/usb/host/usbh.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
api_audio_t usbh_audio_info;
api_audio_id_t spk_id;
api_audio_id_t mic_id;


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
** Parameters:	lr: 1:left, 2:right, 0: left + right	
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_audio_set_request(uint8_t id, video_control_request_t reqest, uint8_t itf, audio_feature_unit_control_selector_t type, uint8_t lr, uint8_t audio_id,uint8_t* buf, uint16_t len)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = reqest;

    req.wValue = SWAP16_L(U16(type,lr));
    req.wIndex = SWAP16_L(U16(audio_id,itf));
    req.wLength = SWAP16_L(len);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len);

    return err;
}

error_t usbh_audio_get_request(uint8_t id, video_control_request_t reqest, uint8_t itf, audio_feature_unit_control_selector_t type, uint8_t lr, uint8_t audio_id, uint8_t* buf, uint16_t *plen)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = reqest;

    req.wValue = SWAP16_L(U16(type,lr));
    req.wIndex = SWAP16_L(U16(audio_id,itf));
    req.wLength = SWAP16_L(*plen);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len);
	*plen = tr_len;
    return err;
}


error_t usbh_audio_endp_set_feature(uint8_t id, uint8_t endp, audio_clock_src_control_selector_t ctrl ,uint8_t* buf, uint16_t len)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_ENDPOINT;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = TUSB_REQ_SET_FEATURE;

    req.wValue = SWAP16_L(ctrl);
    req.wIndex = SWAP16_L(endp);
    req.wLength = SWAP16_L(len);

    err = usbh_ctrl_transfer(id, &req, buf, &tr_len);
    return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_audio_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("audio endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_audio( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;

	if ((TUSB_CLASS_AUDIO == pclass->itf.if_cls)){
		if( (pclass->endpin.addr && (TUSB_ENDP_TYPE_ISOCH == pclass->endpin.type)) 
			|| (pclass->endpout.addr && (TUSB_ENDP_TYPE_ISOCH == pclass->endpout.type)) ){
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
error_t usbh_audio_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(pclass->itf.if_sub_cls == AUDIO_SUBCLASS_CONTROL){
        uint16_t vol;
        uint16_t len = 2;

        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_MIN, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, spk_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.min_vol = SWAP16_L(vol);
        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_MAX, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, spk_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.max_vol = SWAP16_L(vol);
        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_RES, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, spk_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.vol_res = SWAP16_L(vol);

        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_MIN, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, mic_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.min_vol = SWAP16_L(vol);
        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_MAX, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, mic_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.max_vol = SWAP16_L(vol);
        err = usbh_audio_get_request(id, VIDEO_REQUEST_GET_RES, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, mic_id.feature_unit, (uint8_t*)&vol,&len);
        if(ERROR_SUCCESS == err) usbh_audio_info.spk_vol.vol_res = SWAP16_L(vol);

        vol = SWAP16_L(usbh_audio_info.spk_vol.cur_vol_l);
        err = usbh_audio_set_request(id, VIDEO_REQUEST_SET_CUR, pclass->itf.if_num, AUDIO_FU_CTRL_VOLUME, 0, spk_id.feature_unit,(uint8_t*)&vol, 2);
    }else if(pclass->itf.if_sub_cls == AUDIO_SUBCLASS_STREAMING){
        uint8_t rate[3];

        if(pclass->endpout.addr){       //spk
            err = usbh_req_set_itf(id, pclass->itf.if_num, 1);  

            rate[0] = usbh_audio_info.spk_sampel.rate & 0XFF;
            rate[1] = (usbh_audio_info.spk_sampel.rate >> 8) & 0XFF;
            rate[2] = (usbh_audio_info.spk_sampel.rate >> 16) & 0XFF;
            err = usbh_audio_endp_set_feature(id, pclass->endpout.addr, AUDIO_CS_CTRL_SAM_FREQ, rate, 3);
        }

        if(pclass->endpin.addr){       //mic
            // err = usbh_req_set_itf(id, pclass->itf.if_num, 1);   //mic not open immediately

            rate[0] = usbh_audio_info.mic_sampel.rate & 0XFF;
            rate[1] = (usbh_audio_info.mic_sampel.rate >> 8) & 0XFF;
            rate[2] = (usbh_audio_info.mic_sampel.rate >> 16) & 0XFF; 
            err = usbh_audio_endp_set_feature(id, pclass->endpin.addr | TUSB_DIR_IN_MASK, AUDIO_CS_CTRL_SAM_FREQ, rate, 3);
        }  
        if(ERROR_SUCCESS == err){
            err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
            if(ERROR_SUCCESS == err) pdev->class_ready = true;
        }
    }

    return err;
}

error_t usbh_audio_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
    error_t err = ERROR_UNKNOW;
	usbh_dev_t* pdev = get_usbh_dev(id);
    uint8_t inf_num = 0;
    uint16_t i, l;

    for ( i = 0; i < len; i += l ){
        l = pdesc[i];
        if(0 == l) break;

    	if( pdesc[i+1] == TUSB_DESC_INTERFACE){
    		if(++inf_num > 1) break;
    	}

        if(pclass->itf.if_sub_cls == AUDIO_SUBCLASS_CONTROL){
            if( pdesc[i+1] != TUSB_DESC_CS_INTERFACE) continue;

            switch(pdesc[i+2]){
            case AUDIO_CS_AC_INTERFACE_HEADER:
                memset(&usbh_audio_info, 0, sizeof(usbh_audio_info));
                memset(&spk_id, 0, sizeof(spk_id));
                memset(&mic_id, 0, sizeof(mic_id));

                // usbh_audio_info.spk_vol.cur_vol_l = vol_convert(API_AUDIO_SPK_VOL_PERCENT);     //TODO fix by user
                // usbh_audio_info.spk_vol.cur_vol_l = vol_convert(API_AUDIO_SPK_VOL_PERCENT);
                // usbh_audio_info.mic_vol.cur_vol_l = vol_convert(API_AUDIO_SPK_VOL_PERCENT);
                usbh_audio_info.spk_vol.cur_vol_l = 0xff0f;     //TODO fix by user
                usbh_audio_info.spk_vol.cur_vol_l = 0xff0f;
                usbh_audio_info.mic_vol.cur_vol_l = 0xff0f;
                break;
            case AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL:{
                audio_desc_input_terminal_t *pterminal = (audio_desc_input_terminal_t*)(pdesc+i);

                if(AUDIO_TERM_TYPE_USB_UNDEFINED != pterminal->wTerminalType & 0XFF00){    //mic
                    mic_id.input_terminal = pterminal->bTerminalID;
                }else{
                    spk_id.input_terminal = pterminal->bTerminalID;
                }
                break;
            }
            case AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL:{
                audio_desc_output_terminal_t *pterminal = (audio_desc_output_terminal_t*)(pdesc+i);

                if(AUDIO_TERM_TYPE_USB_UNDEFINED != pterminal->wTerminalType & 0XFF00){    //spk
                    spk_id.output_terminal = pterminal->bTerminalID;
                    spk_id.feature_unit = pterminal->bCSourceID;
                }else{
                    mic_id.output_terminal = pterminal->bTerminalID;
                    mic_id.feature_unit = pterminal->bCSourceID;
                }
                break;
            }
            default:
                break;
            }
        }else if(pclass->itf.if_sub_cls == AUDIO_SUBCLASS_STREAMING){
            if(pdesc[i+1] != TUSB_DESC_CS_INTERFACE) continue;

            if(AUDIO_CS_AS_INTERFACE_FORMAT_TYPE == pdesc[i+2]){
                audio_desc_type_I_format_t *pterminal = (audio_desc_type_I_format_t*)(pdesc+i);
                
                if(pclass->endpout.addr){           //spk
                    usbh_audio_info.spk_sampel.resolution = pterminal->bBitResolution;
                    usbh_audio_info.spk_sampel.channel = pterminal->bNrChannels;
                    usbh_audio_info.spk_sampel.rate = U32(0,pterminal->bMsbSamFreq,pterminal->bMidSamFreq,pterminal->bLsbSamFreq);
                }else if(pclass->endpout.addr){       //mic
                    usbh_audio_info.mic_sampel.resolution = pterminal->bBitResolution;
                    usbh_audio_info.mic_sampel.channel = pterminal->bNrChannels;
                    usbh_audio_info.mic_sampel.rate = U32(0,pterminal->bMsbSamFreq,pterminal->bMidSamFreq,pterminal->bLsbSamFreq);
                }
            }
        }
    }
    return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_audio_deinit( uint8_t id, usbh_class_t *pclass) 
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_audio_task(uint8_t id, usbh_class_t *pclass)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}


#endif






