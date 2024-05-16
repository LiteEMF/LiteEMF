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


#ifndef _app_map_setting_h
#define _app_map_setting_h
#include "emf_typedef.h" 
#include "api/api_storage.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define MT_SLIDE_DT		30		/*滑屏最小单位时间*/
#define MT_AUTO_DT		8		/*连点间隔时间最小单位*/
//key attribute
typedef  enum {	
	MT_NONE = 0,
	MT_KEY_TRI,				//trigger
	MT_KEY_AUTO,			//连点
	MT_SLIDE,				//滑动模式
	MT_SLIDE8,				//wasd滑动模式, 8方向滑动

	MT_TICK_WASD = 10,		//摇杆走位模式
	MT_TICK_VISUAL,			//摇杆视角模式

	MT_MACRO_INOF = 15,		//宏定义map
	MT_MACRO = 15,			//max 15
}mt_mode_t;

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef  struct {	
	uint8_t mode:4;				//按键模式
	uint8_t	att:3;				//属性
	uint8_t	 r;					//参数r
	uint16_t t;					//参数t
	uint32_t k;					//参数k, km key[4] or gamepad key or 宏定义time
	int16_t x;					//0~TOUCH_X_LOGICAL_MAX 
	int16_t y;					//0~TOUCH_Y_LOGICAL_MAX 
}mt_map_t;		//12



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_map_swap_l(mt_map_t* mapp, uint16_t size);
void app_map_dump(mt_map_t* mapp, uint16_t size);

mt_map_t* find_map_by_id(uint16_t id, api_storage_map_t* app_map);
uint16_t find_map_by_key(uint32_t key, uint8_t att, api_storage_map_t* app_map);
bool app_map_add(mt_map_t* mapp, api_storage_map_t* app_map);
bool app_map_remove(mt_map_t* mapp, api_storage_map_t* app_map);
bool app_map_remove_by_id(uint16_t id, api_storage_map_t* app_map);
bool app_map_replace_by_id(uint16_t id, mt_map_t* mapp, api_storage_map_t* app_map);

#ifdef __cplusplus
}
#endif
#endif





