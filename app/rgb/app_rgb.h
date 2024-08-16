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


#ifndef _app_rgb_h
#define _app_rgb_h
#include "emf_typedef.h" 
#include "app/rgb/color_utils.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef APP_RGB_NUMS
#define APP_RGB_NUMS (1)
#endif
#ifndef APP_RGB_SLICE
#define APP_RGB_SLICE (20)
#endif
#ifndef LOWEST_BRIGHT
#define LOWEST_BRIGHT   	2
#endif
#ifndef DEFAULT_BRIGHT
#define DEFAULT_BRIGHT   	255
#endif

#ifndef BLINK_FAST		//单位 ms
#define BLINK_FAST (200) 
#endif
#ifndef BLINK_SLOW
#define BLINK_SLOW (1200)
#endif
#ifndef BREATH_FAST
#define BREATH_FAST (1500)
#endif
#ifndef BREATH_SLOW
#define BREATH_SLOW (3000)
#endif




#define RGB_FOREVER (0x00)

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct
{
    uint8_t mode;
    uint8_t brightness;			//unused now
    uint16_t period; 			//period unit ms
    uint8_t times;

	uint8_t palette_offset;
	uint8_t palette_step;	
	uint8_t palette_size;
	uint32_t *palette;			//调色板
	uint32_t color;

    uint8_t times_tick;
	uint8_t times_synk;		//make sure sync start brink is off , 默认为0, times有效时根据设置时间定义
} rgb_cbt_t;

typedef enum{
    RGB_STATIC_MODE = 0,
    RGB_BLINK_MODE,		//闪烁
    RGB_BREATH_MODE,	//呼吸
    RGB_GRADIENT_MODE, 	//渐变
    RGB_RAINBOW_MODE,	//彩红
} rgb_mode_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_rgb_finished_cb(uint8_t id);					//__WEAK 
bool app_rgb_vendor_mode(uint8_t id, uint16_t step);	//__WEAK 
bool set_pixel_color(uint8_t id, uint32_t color);

void app_rgb_set_brightness(uint8_t brightness);
uint8_t app_rgb_get_brightness(void);
bool app_rgb_set_mode(uint8_t id, rgb_mode_t mode, uint32_t color, uint16_t period, uint8_t times);
bool app_rgb_set_palette_mode(uint8_t id, rgb_mode_t mode, uint8_t offset, uint32_t *palette, uint8_t palette_size, uint16_t period, uint8_t times);
#define app_rgb_set_static(id,color,period) 	app_rgb_set_mode(id,RGB_STATIC_MODE,color,period,0)
#define app_rgb_set_blink(id,color,period)  	app_rgb_set_mode(id,RGB_BLINK_MODE, color,period,0)
#define app_rgb_set_blink_times(id,color,period,times)  	app_rgb_set_mode(id,RGB_BLINK_MODE, color,period,times)
#define app_rgb_set_breath(id,color,period) 	app_rgb_set_mode(id,RGB_BREATH_MODE,color,period,0)
#define app_rgb_set_rainbow(id,color,period) 	app_rgb_set_mode(id,RGB_RAINBOW_MODE,color,period,0)
#define app_rgb_set_pal_static(id,offset,palette,palette_size,period) 	app_rgb_set_palette_mode(id,RGB_STATIC_MODE,offset,palette,palette_size,period0)
#define app_rgb_set_pal_blink(id,offset,palette,palette_size,period)  	app_rgb_set_palette_mode(id,RGB_BLINK_MODE,offset,palette,palette_size,period,times)
#define app_rgb_set_pal_breath(id,offset,palette,palette_size,period) 	app_rgb_set_palette_mode(id,RGB_BREATH_MODE,offset,palette,palette_size,period,0)
#define app_rgb_set_pal_gradient(id,offset,palette,palette_size,period) app_rgb_set_palette_mode(id,RGB_GRADIENT_MODE,offset,palette,palette_size,period,0)
#define app_rgb_set_pal_rainbow(id,offset,palette,palette_size,period) 	app_rgb_set_palette_mode(id,RGB_RAINBOW_MODE,offset,palette,palette_size,period,0)

bool app_rgb_init(void);
bool app_rgb_deinit(void);
void app_rgb_task(void* pa);
void app_rgb_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





