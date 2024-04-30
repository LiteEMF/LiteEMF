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


#ifndef _mt_function_h
#define _mt_function_h
#include "emf_typedef.h" 

#include "api/api_transport.h"
#include "km_typedef.h"
#include "utils/emf_utils.h"
#include "app/multitouch/multitouch.h"
#include "api/api_tick.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define MT_TRI_DT		8
#define MT_AUTO_DT		8
#define MT_SLIDE_DT		30

//key attribute
typedef  enum {	
	MT_NONE = 0,
	MT_KEY_TRI,				//trigger
	MT_KEY_AUTO,			//连点
	MT_SLIDE,				//滑动模式
	MT_SLIDE8,				//wasd滑动模式, 8方向滑动

	MT_TICK_WASD = 10,		//摇杆走位模式
	MT_TICK_VISUAL,			//摇杆视角模式

	MT_MACRO = 15,			//max 15
}mt_mode_t;

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef  struct {	
	uint8_t mode:4;				//按键模式
	uint8_t	att:3;				//属性
	uint8_t macro:1;			//宏模式
	uint8_t	d;					//参数d
	uint16_t pa;				//参数pa
	uint32_t key;				//km key[4] or gamepad key
	int16_t x;					//0~TOUCH_X_LOGICAL_MAX 
	int16_t y;					//0~TOUCH_Y_LOGICAL_MAX 
}mt_map_t;		//12


typedef union{
	struct {
		uint8_t step;
		int8_t dirx:2;
		int8_t diry:2;
		int8_t res:3;
		uint8_t event:1;			//事件
	} slide8;

	struct {
		uint16_t pa:15;				//参数
		uint16_t event:1;			//事件
	} mode;

	uint16_t value;
}mt_msg_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void mt_map_swapl(mt_map_t* mapp, uint16_t size);
void mt_map_dump(mt_map_t* mapp, uint16_t size);
bool mt_auto_run(uint16_t id, mt_map_t* mapp);
bool mt_slide_run(uint16_t id, mt_map_t* mapp, uint16_t min_step, uint16_t max_step);
bool mt_function_init(const mt_map_t* mpp, mt_msg_t* msgp, uint16_t size);
bool mt_function_deinit(void);
void mt_function_task(void* pa);
void mt_function_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





