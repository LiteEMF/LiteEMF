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


#ifndef _usbd_audio_h
#define _usbd_audio_h
#include "emf_typedef.h" 
#include "api/usb/device/usbd_core.h"
#include "api/usb/usb_audio_typedef.h"
#include "api/audio/api_audio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
// Speaker path
#define USBD_SPK_INPUT_TERMINAL  0x01
#define USBD_SPK_FEATURE_UNIT    0x02
#define USBD_SPK_OUTPUT_TERMINAL 0x03		
// Microphone path
#define USBD_MIC_INPUT_TERMINAL  0X04
#define USBD_MIC_FEATURE_UNIT    0X05
#define USBD_MIC_OUTPUT_TERMINAL 0X06
#define USBD_MIC_SELECTOR_UNIT   0X07

#ifndef USBD_AUDIO_SPK_RATE
#define USBD_AUDIO_SPK_RATE				48000
#endif
#ifndef USBD_AUDIO_SPK_RESOLUTION
#define USBD_AUDIO_SPK_RESOLUTION		16
#endif
#ifndef USBD_AUDIO_SPK_CHANNEL
#define USBD_AUDIO_SPK_CHANNEL			2
#endif

#ifndef USBD_AUDIO_MIC_RATE_NUM
#define USBD_AUDIO_MIC_RATE_NUM			1
#endif
#ifndef USBD_AUDIO_MIC_RATE
#define USBD_AUDIO_MIC_RATE				48000
#endif
#ifndef USBD_AUDIO_MIC_RESOLUTION
#define USBD_AUDIO_MIC_RESOLUTION		16
#endif
#ifndef USBD_AUDIO_MIC_CHANNEL
#define USBD_AUDIO_MIC_CHANNEL			1
#endif




#ifndef USBD_AUDIO_ID
#define USBD_AUDIO_ID				0
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern api_audio_t usbd_audio_info;





/*****************************************************************************************************
**  Function
******************************************************************************************************/
void usbd_audio_spk_transfer(uint8_t id, uint8_t ep, uint8_t len);
void usbd_audio_spk_sof_transfer(uint8_t id, uint8_t ep);

error_t usbd_audio_reset(uint8_t id);
error_t usbd_audio_suspend(uint8_t id);
uint16_t usbd_audio_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_audio_control_request_process(uint8_t id, usbd_class_t *pclass,  usbd_req_t* const preq);
error_t usbd_audio_out_process(uint8_t id, usbd_class_t *pclass);
error_t usbd_audio_spk_en(uint8_t id,uint8_t en);
error_t usbd_audio_mic_en(uint8_t id,uint8_t en);
error_t usbd_audio_init(uint8_t id);
error_t usbd_audio_deinit(uint8_t id);
void usbd_audio_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);

#ifdef __cplusplus
}
#endif
#endif





