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
#include  "hw_config.h"
#if APP_RGB_ENABLE
#include "api/api_tick.h"
#include "app/rgb/app_rgb.h"
#include "app/rgb/rgb_driver.h"
#include "utils/emf_math.h"


#define DEBUG_LOG_ENABLE     	1
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
static uint32_t	rgb_tick=0;		// APP_RGB_SLICE

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
uint8_t m_pixels[APP_RGB_NUMS*3];
uint8_t m_brightness = 255;
rgb_cbt_t m_rgb_cbt[APP_RGB_NUMS];

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static bool app_rgb_static(uint8_t id, uint16_t step)
{
	bool ret = false;
	uint32_t c;
	uint8_t index;
	rgb_cbt_t *pcbt = &m_rgb_cbt[id];
	
	if(NULL != pcbt->palette){
		index = pcbt->offset + (step>>8) * pcbt->palette_step; 
		c = color_from_palette(index, 0XFF, NOBLEND, pcbt->palette, pcbt->palette_size);
	}else{
		c = pcbt->color;
	}
	
	ret = set_pixel_color(id, c);

	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	注意: period/2 必须是APP_RGB_SLICE的倍数		
*******************************************************************/
static bool app_rgb_blink(uint8_t id, uint16_t step)
{
	bool ret = false;
	uint8_t index;
	uint32_t c;
	rgb_cbt_t *pcbt = &m_rgb_cbt[id];

	if(0 == ((2 * step) & 0xff)){			// (2*step) & 0xff == APP_RGB_SLICE*rgb_tick%(pcbt->period/2);
		/*这里通过tikc的最后一位来判断闪烁开关,同时用于同步,但是亮灯时间不可控	*/
		if(0 == ((step>>7) & 0x01)){	// (step>>7) == APP_RGB_SLICE*rgb_tick/(pcbt->period/2) 

			if(NULL != pcbt->palette){
				index = pcbt->offset + (step>>8) * pcbt->palette_step;
				c = color_from_palette(index, 0XFF, NOBLEND, pcbt->palette, pcbt->palette_size);
			}else{
				c = pcbt->color;
			}
		}else{
			c = 0x00;
		}

		// logd("bl%d, step=%x, c=%x, %d,%d\n",id,step,c,((2 * step) & 0xff),((step>>7) & 0x01));
		ret = set_pixel_color(id, c);
	}
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static bool app_rgb_breath(uint8_t id, uint16_t step)
{
	bool ret = false;
	uint32_t c;
	uint8_t scale;
	uint8_t index;
	rgb_cbt_t *pcbt = &m_rgb_cbt[id];

	scale = LOWEST_BRIGHT + SCALE8( sin8( step - 64 ) + 128, 0XFF - LOWEST_BRIGHT);		//sin 同步颜色周期
	if(NULL != pcbt->palette){
		index = pcbt->offset + (step>>8) * pcbt->palette_step; 
		// index = pcbt->offset + scale;  //or used scale(如果palette 不是2的指数,会有略微的不同步)
		
		c = scale32(color_from_palette(index, 0XFF, NOBLEND, pcbt->palette, pcbt->palette_size), scale);
	}else{
		c = scale32(pcbt->color, scale);
	}
	ret = set_pixel_color(id, c);

	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	渐变模式如果没有设置 palette 和单色呼吸效果一致
*******************************************************************/
static bool app_rgb_gradient(uint8_t id, uint16_t step)
{
	bool ret = false;
	uint32_t c;
	uint8_t blend;
	rgb_cbt_t *pcbt = &m_rgb_cbt[id];

	blend = sin8( step - 64 ) + 128;
	if(NULL != pcbt->palette){
		c = color_from_palette(pcbt->offset + blend, 0XFF, LINEARBLEND, pcbt->palette, pcbt->palette_size);
	}else{
		c = blend32(Color_Black, pcbt->color, blend);
	}
	ret = set_pixel_color(id, c);
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static bool app_rgb_rainbow(uint8_t id, uint16_t step)
{
	rgb_t rgb;
	uint32_t c;
	hsv_t hsv;
	rgb_cbt_t *pcbt = &m_rgb_cbt[id];

	color_rgb(pcbt->color,(rgb_t*)&hsv);
				
	hsv.hsv.h +=  step;
	color_hsv_to_rgb_rainbow(&hsv, &rgb);
	c = color_rgbc(&rgb);

	return set_pixel_color(id, c);
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if WEAK_ENABLE
__WEAK void app_rgb_finished_cb(uint8_t id)
{

}
/*******************************************************************
** Parameters:	用户自定义模式
** Returns:	
** Description:		
*******************************************************************/
__WEAK bool app_rgb_vendor_mode(uint8_t id, uint16_t step)
{
	return false;
}
#endif

bool set_pixel_color(uint8_t id, uint32_t color) 
{
	bool ret = false;
	uint8_t *p;
	rgb_t rgb;

	if (id < APP_RGB_NUMS) {
		color_rgb(color, &rgb);

		if (255 != m_brightness) { 		// See notes in setBrightness()
			rgb.rgb.r = SCALE8(rgb.rgb.r , m_brightness);
			rgb.rgb.g = SCALE8(rgb.rgb.g , m_brightness);
			rgb.rgb.b = SCALE8(rgb.rgb.b , m_brightness);
		}

		p = &m_pixels[id * 3];     		// 3 bytes per pixel

		if(memcmp(p, rgb.val, 3)){
			p[0] = rgb.rgb.r;		   // R,G,B always stored
			p[1] = rgb.rgb.g;
			p[2] = rgb.rgb.b;
			// logd("f:");dumpd(m_pixels,sizeof(m_pixels));
			ret = true;
		}
	}

	return ret;
}

void app_rgb_set_brightness(uint8_t brightness)
{
	m_brightness = brightness;
}
uint8_t app_rgb_get_brightness(void)
{
	return m_brightness;
}

/*******************************************************************
** Parameters:	period: 周期单位ms, times:次数 0~126
			RGB_STATIC_MODE 模式下 period*times 相当于亮灯延时时间
** Returns:	
** Description: 注意: period/2 必须是APP_RGB_SLICE的倍数		
*******************************************************************/
bool app_rgb_set_mode(uint8_t id, rgb_mode_t mode, uint32_t color, uint16_t period, uint8_t times)
{
	rgb_cbt_t *pcbt;

	if(id >= APP_RGB_NUMS) return false;

	pcbt = &m_rgb_cbt[id];
	memset(pcbt, 0, sizeof(rgb_cbt_t));

	period = (period / (APP_RGB_SLICE*2)) * (APP_RGB_SLICE*2);	//period/2 必须是APP_RGB_SLICE的倍数

	pcbt->mode = mode;
    pcbt->brightness = 0xff;
    pcbt->period = period; 				//period unit ms
	if(times){
		pcbt->times = times+1;
		pcbt->tick_synk = 1;
	}

	pcbt->color = color;

	return true;
}
/*******************************************************************
** Parameters:	period: 周期单位ms, times:次数 0~126
			palette,palette_size: 调色板, offset: 偏移0~255
** Returns:	
** Description:		
*******************************************************************/
bool app_rgb_set_palette_mode(uint8_t id, rgb_mode_t mode, uint8_t offset, uint32_t *palette, uint8_t palette_size, uint16_t period, uint8_t times)
{
	rgb_cbt_t *pcbt;

	if(id >= APP_RGB_NUMS) return false;

	pcbt = &m_rgb_cbt[id];
	memset(pcbt, 0, sizeof(rgb_cbt_t));

	pcbt->mode = mode;
    pcbt->brightness = 0xff;
    pcbt->period = period; 				//period unit ms
	if(times){
		pcbt->times = times+1;
		pcbt->tick_synk = 1;
	}

	pcbt->offset = offset;
	pcbt->palette_step = 256 / pcbt->palette_size;
	pcbt->palette_size = palette_size;
	pcbt->palette = palette;

	return true;
}





/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_rgb_init(void)
{
	rgb_driver_init();

	memset(m_pixels, 0, sizeof(m_pixels[APP_RGB_NUMS*3]));
	memset(m_rgb_cbt, 0, sizeof(m_rgb_cbt[APP_RGB_NUMS]));
	m_brightness = 255;

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_rgb_deinit(void)
{
	app_rgb_init();
	rgb_driver_show(m_pixels,sizeof(m_pixels));

	rgb_driver_deinit();
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_rgb_task(void *pa)
{	
	bool ret = false;
	rgb_cbt_t *pcbt;
	int8_t i;
	uint16_t step;						//rgb period step * 256 (放大256倍,低8位为小数)

	static bool rgb_show = false;		//if rgb hw show led faile retry show

	rgb_tick++;
	for(i=0; i< APP_RGB_NUMS; i++){
		pcbt = &m_rgb_cbt[i];
		step = 256 * APP_RGB_SLICE * rgb_tick / pcbt->period;
		
		// logd("rgb%d, mode=%d,step=%x\n", i, pcbt->mode, step);
		if(pcbt->times){						//定时显示次数
			if(0 == (step & 0XFF)){				//切换一个完整周期
				if(pcbt->tick_synk){			//强制同步, 第一个周期不亮灯
					pcbt->tick_synk = 0;
				}else{
					pcbt->times--;
					if(0 == pcbt->times) {
						pcbt->color = Color_Black;
						pcbt->mode = RGB_STATIC_MODE;
						app_rgb_finished_cb(i);
					}
				}
			}
		}
		
		if(pcbt->tick_synk){
			ret = set_pixel_color(i, 0x00);
		}else{
			switch(pcbt->mode){
			case RGB_STATIC_MODE:	//常亮
				ret |= app_rgb_static(i, step);
				break;
			case RGB_BLINK_MODE:	//闪烁
				ret |= app_rgb_blink(i, step);
				break;		
			case RGB_BREATH_MODE:	//呼吸
				ret |= app_rgb_breath(i, step);
				break;	
			case RGB_GRADIENT_MODE:	//渐变
				ret |= app_rgb_gradient(i, step);
				break; 	
			case RGB_RAINBOW_MODE:	//彩红
				ret |= app_rgb_rainbow(i,step);
				break;	
			default:				//自定义
				ret |= app_rgb_vendor_mode(i,step);
				break;
			}
		}
	}

	if(ret || rgb_show){
		rgb_show = !rgb_driver_show(m_pixels,sizeof(m_pixels));
	}
	
	UNUSED_PARAMETER(pa);
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_rgb_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_rgb_task(NULL);
	}
}
#endif

#endif






