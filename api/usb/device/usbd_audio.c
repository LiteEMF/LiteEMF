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
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
#include "api/usb/device/usbd.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

static uint8c_t uac_itf_desc_tab[] = {
    // uac_ac_standard_interface_desc  len = 191  bNumInterfaces num = 3
    0x09, /*bLength: Interface Descriptor size*/
    0x04, /*bDescriptorType: Interface descriptor type*/
    0x00, /*bInterfaceNumber: Number of Interface*/
    0x00, /*bAlternateSetting: Alternate setting*/
    0x00, /*bNumEndpoints*/
    TUSB_CLASS_AUDIO, // InterfaceClass:audio
    AUDIO_SUBCLASS_CONTROL, // InterfaceSubClass:audio ctl
    AUDIO_FUNC_PROTOCOL_CODE_UNDEF, // PC_PROTOCOL_UNDEFINED InterfaceProtocol
    0, // Interface String

    // audio_desc_cs_ac_interface_t ?
    0x0a, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_HEADER, // DescriptorSubType:audio control header
    0x00, 0x01, // bcdADC:audio Device Class v1.00
    0x47,
    0x00, 0x02, // TotalLength of class-specific AC interface descriptors
    0x01, // InterfaceNr(1) - AS #2 id AudioStreaming interface 1 belongs to this AudioControl interface
    0x02, // InterfaceNr(1) - AS #2 id AudioStreaming interface 1 belongs to this AudioControl interface

    // audio_desc_input_terminal_t
    0x0c, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, // DescriptorSubType:Input Terminal
    USBD_SPK_INPUT_TERMINAL, // TerminalID
    (AUDIO_TERM_TYPE_USB_STREAMING & 0xff), (AUDIO_TERM_TYPE_USB_STREAMING >> 8) & 0xff, // TerminalType:USB Streaming
    0x00, // AssocTerminal
    USBD_AUDIO_SPK_CHANNEL, // NrChannels:2 channel
#if USBD_AUDIO_SPK_CHANNEL == 1
    0x04, 0x00, // Mono center front channel
#else
    0x03, 0x00, // ChannelConfig:Left Front,Right Front,
#endif
    0x00, // ChannelName String
    0x00, // Terminal String

    // uac_mic_input_terminal_desc[]
    0x0c, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_INPUT_TERMINAL, // DescriptorSubType:Input Terminal
    USBD_MIC_INPUT_TERMINAL, // TerminalID
    (AUDIO_TERM_TYPE_IN_GENERIC_MIC & 0xff), (AUDIO_TERM_TYPE_IN_GENERIC_MIC >> 8) & 0xff, // TerminalType:Microphone
    0, // AssocTerminal
    USBD_AUDIO_MIC_CHANNEL, // NrChannels
#if USBD_AUDIO_MIC_CHANNEL == 2
    0x03, 0x00, // wChannelConfig:Left Front right Front
#else
    0x00, 0x00, // wChannelConfig:
#endif
    0x00, // ChannelName String
    0x00, // Terminal String

    /// uac_spk_output_terminal_desc USB Speaker OT
    0x09, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, // DescriptorSubTYpe:Output Terminal
    USBD_SPK_OUTPUT_TERMINAL, // TerminalID
    (AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER & 0xff), (AUDIO_TERM_TYPE_OUT_GENERIC_SPEAKER >> 8) & 0xff, // TerminalType:Speaker
    0, // AssocTerminal:
    USBD_SPK_FEATURE_UNIT, // SourceID: Feature UNIT
    0x00, // Terminal String

    /// uac_mic_output_terminal_desc USB Microphone OT
    0x09, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_OUTPUT_TERMINAL, // DescriptorSubTYpe:Output Terminal
    USBD_MIC_OUTPUT_TERMINAL, // TerminalID
    (AUDIO_TERM_TYPE_USB_STREAMING & 0xff), (AUDIO_TERM_TYPE_USB_STREAMING >> 8) & 0xff, // TerminalType:USB Sreaming
    2, // AssocTerminal:
    USBD_MIC_FEATURE_UNIT, // SourceID:A #Feature UNIT
    0x00, // Terminal String

    // uac_mic_selector_uint_desc SELECTOR_UNIT
    0x7, //  bLength	7
    TUSB_DESC_CS_INTERFACE, //  bDescriptorType	CS_INTERFACE (0x24)
    AUDIO_CS_AC_INTERFACE_SELECTOR_UNIT, //    bDescriptorSubtype	SELECTOR_UNIT (0x05)
    USBD_MIC_SELECTOR_UNIT, // bUnitID	33
    1, // bNrInPins	1
    USBD_MIC_FEATURE_UNIT, // baSourceID(1)	50
    0, // iSelector	None (0)

    // uac_spk_feature_desc
    0x07 + (USBD_AUDIO_SPK_CHANNEL + 1) * 1, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, // DescriptorSubType:Audio Feature Unit
    USBD_SPK_FEATURE_UNIT, // UnitID
    USBD_SPK_INPUT_TERMINAL, // SourceID:1 #USB Streaming IT
    0x01, // ControlSize:1 byte
    0x01, // Controls:BIT[0……7]:Mute,Volume Bass Mid Treble......
    0x02, // left ch
#if USBD_AUDIO_SPK_CHANNEL == 2
    0x02, // right ch
#endif
    0x00, // Feature String

    // uac_mic_feature_desc
    /// Audio Feature Unit Descriptor(Microphone)
    7 + (USBD_AUDIO_MIC_CHANNEL + 1) * 1, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AC_INTERFACE_FEATURE_UNIT, // DescriptorSubType:Audio Feature Unit
    USBD_MIC_FEATURE_UNIT, // UnitID
    USBD_MIC_INPUT_TERMINAL, // SourceID: #Microphone
    0x01, // ControlSize:1 byte
    0x43, 0x00, //   bmaControls[0] (Mute,Volume,Automatic)
#if USBD_AUDIO_MIC_CHANNEL == 2
    0x00,
#endif
    0x00, // Feature String

    //-------------------Speaker  interface------------------------------------------------------//
    /// standard interface AS descriptor(Interface 1, Alternate Setting 0)
    0x09, // Length
    0x04, // DescriptorType:Interface
    0x01, // InterfaceNum
    0x00, // AlternateSetting:0
    0x00, // NumEndpoint:0
    TUSB_CLASS_AUDIO, // InterfaceClass:audio
    AUDIO_SUBCLASS_STREAMING, // InterfaceSubClass:audio streaming
    0x00, // InterfaceProtocol
    0x00, // Interface String

    /// standard interface AS descriptor(Interface 2, Alternate Setting 1)
    0x09, // Length
    0x04, // DescriptorType:Interface
    0x01, // InterfaceNum
    0x01, // AlternateSetting
    0x01, // NumEndpoint
    TUSB_CLASS_AUDIO, // InterfaceClass:audio
    AUDIO_SUBCLASS_STREAMING, // InterfaceSubClass:audio streaming
    0x00, // InterfaceProtocol
    0x00, // Interface String

    /// Audio Streaming Interface Descriptor:AS_GENERAL(0x01),
    0x07, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AS_INTERFACE_AS_GENERAL, // DescriptorSubType:AS_GENERAL
    USBD_SPK_INPUT_TERMINAL, // TerminalLink:#1 USB Streaming IT
    0x01, // Delay:1
    (AUDIO_DATA_FORMAT_TYPE_I_PCM & 0xFF), (AUDIO_DATA_FORMAT_TYPE_I_PCM >> 8), // FormatTag:PCM

    /// Type 1 Format type descriptor
    0x0b, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, // DescriptorSubType:Format_type
    AUDIO_DATA_FORMAT_TYPE_I_PCM, // FormatType:Format type 1
    USBD_AUDIO_SPK_CHANNEL, // NumberOfChannel
    USBD_AUDIO_SPK_RESOLUTION / 8, // SubframeSize:2byte
    USBD_AUDIO_SPK_RESOLUTION, // BitsResolution:16bit
    0x01, // SampleFreqType:One sampling frequency.
    USBD_AUDIO_SPK_RATE & 0xff, (USBD_AUDIO_SPK_RATE >> 8) & 0xff, (USBD_AUDIO_SPK_RATE >> 16) & 0xff,

    // Isochronous,Synchronization Type(Asynchronous)
    0x09, // Length
    0x05, // DescriptorType:endpoint descriptor
    (TUSB_DIR_OUT << TUSB_DIR_POST), // EndpointAddress:Output endpoint 1
    0x09, // 0x09,iso sync
    (API_AUDIO_SIZE(USBD_AUDIO_SPK_RATE, USBD_AUDIO_SPK_RESOLUTION, USBD_AUDIO_SPK_CHANNEL) & 0xff),
    (API_AUDIO_SIZE(USBD_AUDIO_SPK_RATE, USBD_AUDIO_SPK_RESOLUTION, USBD_AUDIO_SPK_CHANNEL) >> 8), // MaxPacketSize
    0x01, // one packet per frame 0x00,  //Interval    //Asynchronous
    0x00, // unused
    0x00, // unused

    // Audio Endpoint descriptor,General
    0x07, // Length
    0x25, // DescriptorType:audio endpoint descriptor
    AUDIO_CS_AS_INTERFACE_AS_GENERAL, // DescriptorSubType:audio endpiont general
    0x00, // Attributes
    0x00, // LockDelayUnits
    0x00, 0x00, // LockDelay

    /*-----------------------------------------MIC--------------------------------------------------*/
    /// standard interface AS descriptor(Interface 1, Alternate Setting 0)	uac_mic_as_interface_desc
    0x09, // Length
    0x04, // DescriptorType:Interface
    0x02, // InterfaceNum
    0x00, // AlternateSetting
    0x00, // NumEndpoint
    TUSB_CLASS_AUDIO, // InterfaceClass:audio
    AUDIO_SUBCLASS_STREAMING, // InterfaceSubClass:audio streaming
    0x00, // InterfaceProtocol
    0x00, // Interface String

    /// standard interface AS descriptor(Interface 2, Alternate Setting 1)
    0x09, // Length
    0x04, // DescriptorType:Interface
    0x02, // InterfaceNum
    0x01, // AlternateSetting
    0x01, // NumEndpoint
    TUSB_CLASS_AUDIO, // InterfaceClass:audio
    AUDIO_SUBCLASS_STREAMING, // InterfaceSubClass:audio streaming
    0x00, // InterfaceProtocol
    0x00, // Interface String

    /// Audio Streaming Interface Descriptor:AS_GENERAL
    0x07, // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AS_INTERFACE_AS_GENERAL, // DescriptorSubType:AS_GENERAL
    USBD_MIC_OUTPUT_TERMINAL, // TerminalLink:#1 USB Streaming OT
    0x01, // Delay:1
    (AUDIO_DATA_FORMAT_TYPE_I_PCM & 0xFF), (AUDIO_DATA_FORMAT_TYPE_I_PCM >> 8), // FormatTag:PCM

    /// Type 1 Format type descriptor
    0x08 + 3 * (USBD_AUDIO_MIC_RATE_NUM), // Length
    TUSB_DESC_CS_INTERFACE, // DescriptorType:audio interface descriptor
    AUDIO_CS_AS_INTERFACE_FORMAT_TYPE, // DescriptorSubType:Format_type
    AUDIO_DATA_FORMAT_TYPE_I_PCM, // FormatType:Format type 1
    USBD_AUDIO_MIC_CHANNEL, // NumberOfChannel:1
    USBD_AUDIO_MIC_RESOLUTION / 8, // SubframeSize:2byte
    USBD_AUDIO_MIC_RESOLUTION, // BitsResolution:16bit
    USBD_AUDIO_MIC_RATE_NUM, // SampleFreqType:One sampling frequency.
#if USBD_AUDIO_MIC_RATE_NUM > 1
    (USBD_AUDIO_MIC_RATE_1)&0xff, (USBD_AUDIO_MIC_RATE_1 >> 8) & 0xff, (USBD_AUDIO_MIC_RATE_1 >> 16) & 0xff,
    (USBD_AUDIO_MIC_RATE_2)&0xff, (USBD_AUDIO_MIC_RATE_2 >> 8) & 0xff, (USBD_AUDIO_MIC_RATE_2 >> 16) & 0xff,
    (USBD_AUDIO_MIC_RATE)&0xff, (USBD_AUDIO_MIC_RATE >> 8) & 0xff, (USBD_AUDIO_MIC_RATE >> 16) & 0xff,
    (USBD_AUDIO_MIC_RATE_4)&0xff, (USBD_AUDIO_MIC_RATE_4 >> 8) & 0xff, (USBD_AUDIO_MIC_RATE_4 >> 16) & 0xff,
#else
    (USBD_AUDIO_MIC_RATE)&0xff, (USBD_AUDIO_MIC_RATE >> 8) & 0xff, (USBD_AUDIO_MIC_RATE >> 16) & 0xff,
#endif

    /// Endpoint 1 - Standard Descriptor:Output Endpoint1
    // Isochronous,Synchronization Type(Asynchronous)
    0x09, // Length
    0x05, // DescriptorType:endpoint descriptor
    (TUSB_DIR_IN << TUSB_DIR_POST), // EndpointAddress
    0x05,
    (API_AUDIO_SIZE(USBD_AUDIO_MIC_RATE, USBD_AUDIO_MIC_RESOLUTION, USBD_AUDIO_MIC_CHANNEL) & 0xff),
    (API_AUDIO_SIZE(USBD_AUDIO_MIC_RATE, USBD_AUDIO_MIC_RESOLUTION, USBD_AUDIO_MIC_CHANNEL) >> 8), // MaxPacketSize
    0x01, // one packet per frame   //Interval
    0x00, // unused
    0x00, // unused

    /// Endpoint - Audio Streaming Descriptor
    // Audio Endpoint descriptor,General
    0x07, // Length
    0x25, // DescriptorType:audio endpoint descriptor
    AUDIO_CS_AS_INTERFACE_AS_GENERAL, // DescriptorSubType:audio endpiont general
#if USBD_AUDIO_MIC_RATE_NUM > 1
    0x01, //   bmAttributes (Sampling Freq Control)
#else
    0x00, // Attributes
#endif
    0x00, // LockDelayUnits
    0x00, 0x00, // LockDelay
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
api_audio_t usbd_audio_info;


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_audio_reset(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}

error_t usbd_audio_suspend(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}

uint16_t usbd_audio_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t* pdesc, uint16_t desc_len, uint16_t* pdesc_index)
{
    uint16_t len = sizeof(uac_itf_desc_tab);

    if (desc_len >= *pdesc_index + len) {
        memcpy(pdesc + *pdesc_index, uac_itf_desc_tab, len);
        usbd_assign_configuration_desc(id, DEV_TYPE_AUDIO, 0, pindex, pdesc + *pdesc_index, len);
    }
    *pdesc_index += len;

    return len;
}


static error_t audio_ac_set_cur(uint8_t id, usbd_req_t* const preq)
{
	error_t err = ERROR_SUCCESS;
    uint8_t mute;
    uint32_t volume = 0;
	audio_feature_unit_control_selector_t ctrl_sel = preq->req.wValue>>8;
	uint8_t audio_id = preq->req.wIndex >> 8;
	api_audio_t* paudio = &usbd_audio_info;

    switch (ctrl_sel) {
        case AUDIO_FU_CTRL_MUTE:
			mute = preq->setup_buf[0];
            if (audio_id == USBD_SPK_FEATURE_UNIT) {
                #if API_AUDIO_ENABLE
                api_audio_spk_set_mute(USBD_AUDIO_ID,&usbd_audio_info,mute);
                #endif
            } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                #if API_AUDIO_ENABLE
                api_audio_mic_set_mute(USBD_AUDIO_ID,&usbd_audio_info,mute);
                #endif
            }else{
                err = ERROR_STALL;
            }
            break;

        case AUDIO_FU_CTRL_VOLUME:
			memcpy((uint8_t *)&volume,preq->setup_buf, MIN(4,preq->req.wLength));
			volume =  SWAP16_L(volume);
            if (audio_id == USBD_SPK_FEATURE_UNIT) {
                if (preq->req.wValue & 0xff == 1) {
                    paudio->spk_vol.cur_vol_l = volume;
                } else if (preq->req.wValue & 0xff == 2) {
                    paudio->spk_vol.cur_vol_r = volume;
                }else{
                    paudio->spk_vol.cur_vol_l = volume;
                    paudio->spk_vol.cur_vol_r = volume;
                }
                #if API_AUDIO_ENABLE
                api_audio_spk_set_mute(USBD_AUDIO_ID,&usbd_audio_info,paudio->spk_vol.mute);
                #endif
            } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                paudio->mic_vol.cur_vol_l = volume;
                #if API_AUDIO_ENABLE
                api_audio_mic_set_mute(USBD_AUDIO_ID,&usbd_audio_info,paudio->mic_vol.mute);
                #endif
            }else{
                err = ERROR_STALL;
            }
            break;

        case AUDIO_FU_CTRL_AGC:
            paudio->bAGC = preq->setup_buf[0];
            break;
		
        default :
            err = ERROR_STALL;
            break;
    }
    return err;
}

static error_t audio_ac_get_cur(uint8_t id, usbd_req_t* const preq)
{
    error_t err = ERROR_SUCCESS;
    uint8_t mute;
	audio_feature_unit_control_selector_t ctrl_sel = preq->req.wValue>>8;
	uint8_t audio_id = preq->req.wIndex >> 8;
	api_audio_t* paudio = &usbd_audio_info;

	switch (ctrl_sel) {
	case AUDIO_FU_CTRL_MUTE:
		if (audio_id == USBD_SPK_FEATURE_UNIT) {
			preq->setup_buf[0] = paudio->spk_vol.mute;
		} else if (audio_id == USBD_MIC_FEATURE_UNIT) {
			preq->setup_buf[0] = paudio->mic_vol.mute;
		}else{
			preq->setup_buf[0] = 0;
		}
		break;
	case AUDIO_FU_CTRL_VOLUME:
		if (audio_id == USBD_SPK_FEATURE_UNIT) {
			if (preq->req.wValue & 0xff == 2) {
				preq->setup_buf[0] = paudio->spk_vol.cur_vol_r;
				preq->setup_buf[1] = paudio->spk_vol.cur_vol_r>>8;
			}else{						//1
				preq->setup_buf[0] = paudio->spk_vol.cur_vol_l;
				preq->setup_buf[1] = paudio->spk_vol.cur_vol_l>>8;
			}
		} else if (audio_id == USBD_MIC_FEATURE_UNIT) {
			preq->setup_buf[0] = paudio->mic_vol.cur_vol_l;
			preq->setup_buf[1] = paudio->mic_vol.cur_vol_l>>8;
		}
		break;
	case AUDIO_FU_CTRL_AGC:
		preq->setup_buf[0] = paudio->bAGC;
		break;
	default:
		err = ERROR_STALL;
		break;
	}

    return err;
}


error_t usbd_audio_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq)
{
    error_t err = ERROR_STALL;
    uint8_t itf = preq->req.wIndex & 0XFF;
	
    if(TUSB_REQ_RCPT_INTERFACE != preq->req.bmRequestType.bits.recipient) return err;

    if (TUSB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
        if(TUSB_REQ_SET_INTERFACE == preq->req.bRequest) {
            if(AUDIO_SUBCLASS_STREAMING == pclass->itf.if_sub_cls){
                #if API_AUDIO_ENABLE
                if(pclass->endpout.addr){                   //out endp
                    if (0 == pclass->itf.if_alt) {
                        api_audio_close_spk(USBD_AUDIO_ID,&usbd_audio_info);
                    }else{
                        api_audio_open_spk(USBD_AUDIO_ID,&usbd_audio_info,USBD_AUDIO_SPK_RATE,USBD_AUDIO_SPK_RESOLUTION,USBD_AUDIO_SPK_CHANNEL);
                    }
                }
                if(pclass->endpin.addr){
                    if (0 == pclass->itf.if_alt) {
                        api_audio_close_mic(USBD_AUDIO_ID,&usbd_audio_info);
                    }else {
                        api_audio_open_mic(USBD_AUDIO_ID,&usbd_audio_info,USBD_AUDIO_MIC_RATE,USBD_AUDIO_MIC_RESOLUTION,USBD_AUDIO_MIC_CHANNEL);
                        usbd_in(id, pclass->endpin.addr, NULL , 0);				   //usb 启动发送
                    }
                }
                #endif
                err = ERROR_SUCCESS;
            }
		}
	} else if (TUSB_REQ_TYPE_CLASS == preq->req.bmRequestType.bits.type) {
		uint8_t audio_id = preq->req.wIndex >> 8;
		api_audio_t* paudio = &usbd_audio_info;

		if(AUDIO_SUBCLASS_CONTROL == pclass->itf.if_sub_cls){
            err = ERROR_SUCCESS;
            switch(preq->req.bRequest){
                case VIDEO_REQUEST_SET_CUR:
                    err = audio_ac_set_cur(audio_id,preq);
                    break;
                case VIDEO_REQUEST_GET_CUR:
                    err = audio_ac_get_cur(audio_id,preq);
                    break;
                case VIDEO_REQUEST_GET_DEF:
                    if (audio_id == USBD_SPK_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->spk_vol.def_vol;
                        preq->setup_buf[1] = paudio->spk_vol.def_vol>>8;
                    } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->mic_vol.def_vol;
                        preq->setup_buf[1] = paudio->mic_vol.def_vol>>8;
                    }
                    break;
                case VIDEO_REQUEST_GET_MAX:
                    if (audio_id == USBD_SPK_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->spk_vol.max_vol;
                        preq->setup_buf[1] = paudio->spk_vol.max_vol>>8;
                    } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->mic_vol.max_vol;
                        preq->setup_buf[1] = paudio->mic_vol.max_vol>>8;
                    }
                    break;
                case VIDEO_REQUEST_GET_MIN:
                    if (audio_id== USBD_SPK_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->spk_vol.min_vol;
                        preq->setup_buf[1] = paudio->spk_vol.min_vol>>8;
                    } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->mic_vol.min_vol;
                        preq->setup_buf[1] = paudio->mic_vol.min_vol>>8;
                    }
                    break;
                case VIDEO_REQUEST_GET_RES:
                    if (audio_id == USBD_SPK_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->spk_vol.vol_res;
                        preq->setup_buf[1] = paudio->spk_vol.vol_res>>8;
                    } else if (audio_id == USBD_MIC_FEATURE_UNIT) {
                        preq->setup_buf[0] = paudio->mic_vol.vol_res;
                        preq->setup_buf[1] = paudio->mic_vol.vol_res>>8;
                    }
                    break;
                case VIDEO_REQUEST_GET_LEN:
                case VIDEO_REQUEST_GET_INFO:
                    break;
                default:
                    err = ERROR_STALL;
                    break;
            }
        }
    }
    return err;
}

error_t usbd_audio_out_process(uint8_t id, usbd_class_t* pclass)
{
    uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
    error_t err;

    err = usbd_out(id,pclass->endpout.addr,usb_rxbuf,&usb_rxlen);
    if((ERROR_SUCCESS == err) && usb_rxlen){
        logd("audio ep%d in%d:",pclass->endpout.addr, usb_rxlen);dumpd(usb_rxbuf,usb_rxlen);
    }

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:	
** Description:		
*******************************************************************/
usbd_class_t *usbd_audio_mic_find(uint8_t id)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if((pclass->dev_type == DEV_TYPE_AUDIO) && (AUDIO_SUBCLASS_STREAMING == pclass->itf.if_sub_cls)){
			if(pclass->endpin.addr) continue;
            if(pclass->endpin.type != TUSB_ENDP_TYPE_ISOCH) continue;
            
			if(pdev->itf_alt[pclass->itf.if_num] == pclass->itf.if_alt){
				return pclass;
			}
		}
	}
	
	return NULL;
}


void usbd_audio_mic_transfer(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t frame_len)
{
	uint8_t  mic_buffer[64];
	uint32_t mic_frame_len = API_AUDIO_MIC_SIZE(&usbd_audio_info);

	memset(mic_buffer,0,sizeof(mic_buffer));

	if(mic_frame_len == frame_len){
		memcpy(mic_buffer,buf,mic_frame_len);
	}
	usbd_in(id, ep, mic_buffer,sizeof(mic_buffer));
}

/*******************************************************************
** Parameters:
** Returns: 
** Description:
*******************************************************************/
error_t usbd_audio_spk_en(uint8_t id,uint8_t en)
{
    usbd_class_t* pclass;

    pclass = usbd_class_find_by_type(id, DEV_TYPE_AUDIO,0);
    if(NULL == pclass) return ERROR_DISCON;

    #if API_AUDIO_ENABLE
    if(en){
        api_audio_open_spk(USBD_AUDIO_ID,&usbd_audio_info,USBD_AUDIO_SPK_RATE,USBD_AUDIO_SPK_RESOLUTION,USBD_AUDIO_SPK_CHANNEL);
    }else{
	    api_audio_close_spk(USBD_AUDIO_ID,&usbd_audio_info);
    }
    #endif

    return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns: 
** Description:
*******************************************************************/
error_t usbd_audio_mic_en(uint8_t id,uint8_t en)
{
    usbd_class_t* pclass;

    pclass = usbd_audio_mic_find(id);
    if(NULL == pclass) return ERROR_DISCON;

    #if API_AUDIO_ENABLE
    if(en){
        api_audio_open_mic(USBD_AUDIO_ID,&usbd_audio_info,USBD_AUDIO_MIC_RATE,USBD_AUDIO_MIC_RESOLUTION,USBD_AUDIO_MIC_CHANNEL);
        usbd_audio_mic_transfer(id, pclass->endpin.addr, NULL, 0);
    }else{
	    api_audio_close_mic(USBD_AUDIO_ID,&usbd_audio_info);
    }
    #endif
    return ERROR_SUCCESS;
}
/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_audio_init(uint8_t id)
{
    memset(&usbd_audio_info, 0, sizeof(usbd_audio_info));

    #if API_AUDIO_ENABLE
	api_audio_init(&usbd_audio_info);
    #endif
	UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_audio_deinit(uint8_t id)
{
    #if API_AUDIO_ENABLE
	api_audio_close_spk(USBD_AUDIO_ID,&usbd_audio_info);
	api_audio_close_mic(USBD_AUDIO_ID,&usbd_audio_info);
    #endif
	UNUSED_PARAMETER(id);
    return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_audio_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
    switch(evt){
    case  USBD_EVENT_RESET:
        usbd_audio_reset(id);
        break;
    case  USBD_EVENT_SUSPEND:
        usbd_audio_suspend(id);
        break;
    case  USBD_EVENT_EP_OUT:
        usbd_audio_out_process(id, pclass);
        break;
    case USBD_EVENT_EP_IN:
        break;
    default:
        break;
    }
}



#endif
