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
#include "app/multitouch/app_map_setting.h"
#include "app/multitouch/macro_map.h"
#include "api/api_tick.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef union{

	struct {
		uint16_t time_out;			//超时ms
		uint16_t res:14;
		uint16_t run:1;				//是否运行
		uint16_t event:1;			//事件
	} tri;


	struct {
		uint16_t step;				//滑动步长
		uint16_t res:14;
		uint16_t run:1;				//是否运行
		uint16_t event:1;			//事件
	} slide;
	struct {
		uint16_t step;				//滑动步长
		
		int16_t dirx:2;
		int16_t diry:2;
		uint16_t res:10;
		uint16_t run:1;				//是否运行
		uint16_t event:1;			//事件
	} slide8;

	struct {
		uint16_t times;			 	//连点次数
		uint16_t res:14;
		uint16_t run:1;				//是否运行
		uint16_t event:1;			//事件
	} autos;

	struct {
		uint16_t time_out;			//超时ms

		uint16_t res:13;
		uint16_t step_run:1;		//1:宏步骤当前正在执行,0:宏步骤运行完正在等待间隔时间
		uint16_t run:1;				//是否运行
		uint16_t event:1;			//事件
	} macro;

	uint32_t value;
}mt_msg_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool mt_auto_run(uint16_t id, mt_msg_t* pmsg, mt_map_t* mapp);
bool mt_slide_run(uint16_t id, uint16_t min_step, uint16_t max_step, mt_msg_t* pmsg, mt_map_t* mapp);
bool mt_function_init(const api_storage_map_t* mapp, const api_storage_map_t* macro_mapp,mt_msg_t* msgp);
bool mt_function_deinit(void);
void mt_function_task(void* pa);
void mt_function_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





