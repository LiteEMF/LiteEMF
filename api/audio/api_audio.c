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
#if API_AUDIO_ENABLE
#include "api/audio/api_audio.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
volatile bool api_audio_earphone_in;
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static api_audio_t m_api_audio;


/*****************************************************************************************************
**	static Function
******************************************************************************************************/
/*******************************************************************
** Parameters:
** Returns:	
** Description: 注意这是一个双向转换函数
*******************************************************************/
static uint32_t vol_convert(uint16_t v)
{
	uint8_t i;
    uint16c_t vol_table[] = {
        0xe3a0, 0xe461, 0xe519, 0xe5c8, 0xe670, 0xe711, 0xe7ac, 0xe840, 0xe8cf, 0xe959,
        0xe9df, 0xea60, 0xeadc, 0xeb55, 0xebca, 0xec3c, 0xecab, 0xed16, 0xed7f, 0xede5,
        0xee48, 0xeea9, 0xef08, 0xef64, 0xefbe, 0xf016, 0xf06c, 0xf0c0, 0xf113, 0xf164,
        0xf1b3, 0xf200, 0xf24c, 0xf297, 0xf2e0, 0xf328, 0xf36e, 0xf3b4, 0xf3f8, 0xf43a,
        0xf47c, 0xf4bd, 0xf4fc, 0xf53b, 0xf579, 0xf5b5, 0xf5f1, 0xf62c, 0xf666, 0xf69f,
        0xf6d7, 0xf70e, 0xf745, 0xf77b, 0xf7b0, 0xf7e5, 0xf818, 0xf84b, 0xf87e, 0xf8b0,
        0xf8e1, 0xf911, 0xf941, 0xf970, 0xf99f, 0xf9cd, 0xf9fb, 0xfa28, 0xfa55, 0xfa81,
        0xfaad, 0xfad8, 0xfb03, 0xfb2d, 0xfb56, 0xfb80, 0xfba9, 0xfbd1, 0xfbf9, 0xfc21,
        0xfc48, 0xfc6f, 0xfc95, 0xfcbb, 0xfce1, 0xfd06, 0xfd2b, 0xfd50, 0xfd74, 0xfd98,
        0xfdbc, 0xfddf, 0xfe02, 0xfe25, 0xfe47, 0xfe69, 0xfe8b, 0xfead, 0xfece, 0xfeef,
        0xff0f,
    };

    if (v <= 100) {
        return vol_table[v];
    }

    for (i = 0; i < countof(vol_table); i++) {
        if (v <= vol_table[i]) {
            return i;
        }
    }

    return 0;
}
/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:	l_vol,r_vol: 百分百0~100
** Returns:			
** Description: 获取百分比音量
*******************************************************************/
void api_audio_spk_get_vol_percent(uint8_t id,api_audio_t *paudio, uint16_t *l_vol, uint16_t *r_vol)
{
    if (l_vol) {
        *l_vol = 0;
    }
    if (r_vol) {
        *r_vol = 0;
    }

	if (!paudio->spk_vol.mute) {
		if (l_vol) {
			*l_vol = vol_convert(paudio->spk_vol.cur_vol_l);
		}
		if (r_vol) {
			*r_vol = vol_convert(paudio->spk_vol.cur_vol_r);
		}
	}
}
/*******************************************************************
** Parameters:
** Returns:		百分百0~100
** Description:获取百分比音量
*******************************************************************/
uint16_t api_audio_mic_get_vol_percent(uint8_t id,api_audio_t *paudio)
{
	if (!paudio->mic_vol.mute) {
		return vol_convert(paudio->mic_vol.cur_vol_l);
	}
    return 0;
}

void api_audio_spk_set_vol(uint8_t id,api_audio_t *paudio,uint16_t l_vol, uint16_t r_vol)
{
	paudio->spk_vol.cur_vol_l = vol_convert(l_vol);
	paudio->spk_vol.cur_vol_r = vol_convert(r_vol);
	if (!paudio->spk_vol.mute) {
		#if API_AUDIO_ENABLE
		hal_audio_spk_set_vol(id,l_vol,r_vol);
		#endif
	}
}


void api_audio_mic_set_vol(uint8_t id,api_audio_t *paudio,uint16_t mic_vol)
{
	paudio->mic_vol.cur_vol_l = vol_convert(mic_vol);
	if (!paudio->mic_vol.mute) {
		#if API_AUDIO_ENABLE
		hal_audio_mic_set_vol(id, mic_vol );
		#endif
	}
}

void api_audio_spk_set_mute(uint8_t id,api_audio_t *paudio,uint8_t mute)
{
    paudio->spk_vol.mute = mute;

    #if API_AUDIO_ENABLE
	if (paudio->spk_vol.mute) {
		hal_audio_spk_set_vol(id,0,0);
	}else{
        uint16_t l_vol = vol_convert(paudio->spk_vol.cur_vol_l);
	    uint16_t r_vol = vol_convert(paudio->spk_vol.cur_vol_r);
        hal_audio_spk_set_vol(id,l_vol,r_vol);
    }
    #endif
}
void api_audio_mic_set_mute(uint8_t id,api_audio_t *paudio,uint8_t mute)
{
    paudio->mic_vol.mute = mute;
    
    #if API_AUDIO_ENABLE
	if (paudio->mic_vol.mute) {
		hal_audio_mic_set_vol(id,0);
	}else{
        uint16_t mic_vol = vol_convert(paudio->mic_vol.cur_vol_l);
        hal_audio_mic_set_vol(id,mic_vol);
    }
    #endif
}

void api_audio_open_spk(uint8_t id,api_audio_t *paudio,uint32_t rate,uint8_t resolution,uint8_t channel)
{
	logd("%s,rate=%d,res=%d,ch=%d", __FUNCTION__,rate,resolution,channel);

    paudio->spk_sampel.rate = rate;
    paudio->spk_sampel.resolution = resolution;
    paudio->spk_sampel.channel = channel;
    #if API_AUDIO_ENABLE
    hal_audio_open_spk(id,paudio);
    #endif
}
void api_audio_close_spk(uint8_t id,api_audio_t *paudio)
{
	logd("%s", __FUNCTION__);
    #if API_AUDIO_ENABLE
    hal_audio_close_spk(id);
    #endif
}
void api_audio_open_mic(uint8_t id,api_audio_t *paudio,uint32_t rate,uint8_t resolution,uint8_t channel)
{
    logd("%s,rate=%d,res=%d,ch=%d", __FUNCTION__,rate,resolution,channel);
    paudio->mic_sampel.rate = rate;
    paudio->mic_sampel.resolution = resolution;
    paudio->mic_sampel.channel = channel;
    #if API_AUDIO_ENABLE
    hal_audio_open_mic(id,paudio);
    #endif
}
void api_audio_close_mic(uint8_t id,api_audio_t *paudio)
{
	logd("%s", __FUNCTION__);
    #if API_AUDIO_ENABLE
    hal_audio_close_mic(id);
    #endif
}
void api_audio_spk_stream_write(uint8_t id,uint8_t *buf, uint16_t len)
{
    uac_speaker_stream_write(buf,len);
}
uint16_t api_audio_mic_stream_read(uint8_t id,uint8_t *buf, uint16_t len)
{
    return uac_mic_stream_read(buf,len);
}


bool api_audio_init(api_audio_t *paudio)
{
    memset(paudio, 0 , sizeof(api_audio_t));

    paudio->spk_sampel.rate =  API_AUDIO_SPK_RATE;
    paudio->spk_sampel.resolution =  API_AUDIO_SPK_RESOLUTION;
    paudio->spk_sampel.channel =  API_AUDIO_SPK_CHANNEL;
    paudio->mic_sampel.rate =  API_AUDIO_MIC_RATE;
    paudio->mic_sampel.resolution =  API_AUDIO_MIC_RESOLUTION;
    paudio->mic_sampel.channel =  API_AUDIO_MIC_CHANNEL;

    paudio->spk_vol.def_vol = vol_convert(API_AUDIO_SPK_VOL_PERCENT);
    paudio->spk_vol.cur_vol_l = paudio->spk_vol.def_vol;
    paudio->spk_vol.cur_vol_r = paudio->spk_vol.def_vol;
    paudio->spk_vol.max_vol = vol_convert(100);
    paudio->spk_vol.min_vol = vol_convert(0);
    paudio->spk_vol.vol_res = 0x30;

    paudio->mic_vol.max_vol = vol_convert(100);
    paudio->mic_vol.min_vol = vol_convert(0);
    paudio->mic_vol.vol_res = 0x30;
    paudio->mic_vol.def_vol = vol_convert(API_AUDIO_MIC_VOL_PERCENT);
    paudio->mic_vol.cur_vol_l = paudio->mic_vol.def_vol;
	
	return hal_audio_init(0,paudio);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_audio_deinit(api_audio_t *paudio)
{
	api_audio_close_spk(0,paudio);
	api_audio_close_mic(0,paudio);
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_audio_task(void* pa)
{

}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_audio_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_audio_task(NULL);
	}
}
#endif

#endif






