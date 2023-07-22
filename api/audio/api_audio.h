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


#ifndef _api_audio_h
#define _api_audio_h
#include "emf_typedef.h" 
#include "hal_audio.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef API_AUDIO_SPK_VOL_PERCENT		 //0~100%
#define API_AUDIO_SPK_VOL_PERCENT         100
#endif
#ifndef API_AUDIO_MIC_VOL_PERCENT		 //0~100%
#define API_AUDIO_MIC_VOL_PERCENT         100
#endif



#ifndef API_AUDIO_SPK_RATE
#define API_AUDIO_SPK_RATE				48000
#endif
#ifndef API_AUDIO_SPK_RESOLUTION
#define API_AUDIO_SPK_RESOLUTION		16
#endif
#ifndef API_AUDIO_SPK_CHANNEL
#define API_AUDIO_SPK_CHANNEL			2
#endif
#ifndef API_AUDIO_MIC_RATE
#define API_AUDIO_MIC_RATE				48000
#endif
#ifndef API_AUDIO_MIC_RESOLUTION
#define API_AUDIO_MIC_RESOLUTION		16
#endif
#ifndef API_AUDIO_MIC_CHANNEL
#define API_AUDIO_MIC_CHANNEL			1
#endif

//   Calculate wMaxPacketSize of Endpoints
#define API_AUDIO_SIZE(_maxFrequency, _SampleResolution, _nChannels) \
    ( ((((_maxFrequency + 999)) / 1000) + 1) * (_SampleResolution/8) * _nChannels )

#define API_AUDIO_SPK_SIZE(paudio)   \
    API_AUDIO_SIZE((paudio)->spk_sampel.rate, (paudio)->spk_sampel.resolution, (paudio)->spk_sampel.channel)

#define API_AUDIO_MIC_SIZE(paudio)   \
    API_AUDIO_SIZE((paudio)->mic_sampel.rate, (paudio)->mic_sampel.resolution, (paudio)->mic_sampel.channel)
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
    uint8_t mute;
    uint16_t max_vol;
    uint16_t min_vol;
    uint16_t def_vol;
    uint16_t vol_res;
    uint16_t cur_vol_l;
    uint16_t cur_vol_r;
}audio_vol_t;

typedef struct {
    uint32_t rate;          //采样率
    uint8_t resolution;     //位
    uint8_t channel;
}api_audio_sampel_t;

typedef struct {
    audio_vol_t spk_vol;
    audio_vol_t mic_vol;
    api_audio_sampel_t spk_sampel;
    api_audio_sampel_t mic_sampel;

    uint8_t bAGC: 1;
    uint8_t res: 7;
}api_audio_t;


typedef struct {
    uint8_t input_terminal;
    uint8_t feature_unit;
    uint8_t output_terminal;
}api_audio_id_t;

extern volatile bool api_audio_earphone_in;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_audio_spk_get_vol_percent(uint8_t id,api_audio_t *paudio, uint16_t *l_vol, uint16_t *r_vol);
uint16_t api_audio_mic_get_vol_percent(uint8_t id,api_audio_t *paudio);
void api_audio_spk_set_vol(uint8_t id,api_audio_t *paudio,uint16_t l_vol, uint16_t r_vol);
void api_audio_mic_set_vol(uint8_t id,api_audio_t *paudio,uint16_t mic_vol);
void api_audio_spk_set_mute(uint8_t id,api_audio_t *paudio,uint8_t mute);
void api_audio_mic_set_mute(uint8_t id,api_audio_t *paudio,uint8_t mute);
void api_audio_open_spk(uint8_t id,api_audio_t *paudio,uint32_t rate,uint8_t resolution,uint8_t channel);
void api_audio_close_spk(uint8_t id,api_audio_t *paudio);
void api_audio_open_mic(uint8_t id,api_audio_t *paudio,uint32_t rate,uint8_t resolution,uint8_t channel);
void api_audio_close_mic(uint8_t id,api_audio_t *paudio);
bool api_audio_init(api_audio_t *paudio);
bool api_audio_deinit(api_audio_t *paudio);
void api_audio_task(void* pa);
void api_audio_handler(uint32_t period_10us);


//HAL
void hal_audio_spk_set_vol(uint8_t id,uint16_t l_vol, uint16_t r_vol);
void hal_audio_mic_set_vol(uint8_t id,uint16_t mic_vol);
void hal_audio_open_spk(uint8_t id,api_audio_t *paudio);
void hal_audio_close_spk(uint8_t id);
void hal_audio_open_mic(uint8_t id,api_audio_t *paudio);
void hal_audio_close_mic(uint8_t id);
bool hal_audio_init(uint8_t id,api_audio_t *paudio);

#ifdef __cplusplus
}
#endif
#endif





