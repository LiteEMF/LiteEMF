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


#ifndef _gamepad_remap_h
#define _gamepad_remap_h
#include "emf_typedef.h" 
#include "km_typedef.h"
#include  "api/api_storage.h"
#include "app/imu/imu_driver.h"
#include "app/app_joystick.h"
#include "app/gamepad/app_gamepad_key.h"

#include "utils/emf_utils.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef GP_MACRG_DT		 //in ms
#define GP_MACRG_DT  8
#endif

//宏录制
#ifndef GAMEAPD_MACRO_ENABLE
#define GAMEAPD_MACRO_ENABLE	1
#endif


//手柄拓展功能在ram中运行
#ifndef GAMEAPD_REMAP_RAM_CODE
#define GAMEAPD_REMAP_RAM_CODE()	AT_RAM_CODE()
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	axis2f_t lpf;
	kalman_axis2f_t kalman;
	firf_axis2_t fir;
}gamepad_filter_t;

typedef enum {
    KEY_REMAP_NULL = 0,
    GAMEPAD_REMAP_GP_TYEP,
    GAMEPAD_REMAP_KB_TYEP,
	GAMEPAD_REMAP_CONSUMER_TYEP
}gamepad_remap_type_t;

//改建
typedef union
{
	struct{
		uint8_t type;       // gamepad_remap_type_t
		uint8_t key_pos[3]; //支持手柄按键一对三, 支持键盘按键:byte[0]:控制按键; byte[1~2]:普通键值
	}gp;
	struct{
		uint8_t type;       // gamepad_remap_type_t
		uint8_t fn;
		uint8_t key[2];     	
	}kb;
	struct{
		uint8_t type;       // gamepad_remap_type_t
		uint8_t res;     	
		uint16_t key;
	}consumer;
}gamepad_remap_map_t;

//turbo auto连点功能
typedef struct{
	uint8_t en:1;
	uint8_t dir:1;
	timer_t timer;
	uint32_t key;
	uint32_t period;		//turbo周期 ms
}gamepad_auto_t;

//快速扳机
typedef struct{
	int16_t s_val;
	int16_t threshold;
	uint8_t active;
}fast_trigger_t;

//宏定义
typedef enum{
	GP_MACRO_STA_NONE,
    GP_MACRO_STA_RUN,       //宏录运行中
	GP_MACRO_STA_REC,		//宏录制中
}gamepad_macro_sta_t;

/**************************************************************
**	Parameters
att:bit0 	bit1
    0 		0     	//普通按住运行, 抬起结束
    1 		0 		//按下触发运行
    0 		1		//按下循环运行, 抬起结束运行
    1 		1		//点击开始循环, 点击结束循环
***************************************************************/
typedef union {
	struct{
		uint8_t cfg_index;      //宏定义序号
		uint8_t att;            //宏定义触发属性按位 bit0:0:保持按住运行,1:按下触发运行, bit1:1:宏定义循环
        uint16_t time;			//时间, 单位8ms(目前只表示循环间隔时间)
		uint32_t key;           //宏定义触发按键(HW_KEY_XXX)
		uint32_t res;			//预留
	}head;						//宏定义信息
	struct{
		uint8_t type;			//数据类型 gamepad_remap_type_t
		uint8_t res;
        uint16_t time;			//动作运行时间, 单位8ms
		uint32_t key;			//bit0~23按键定义是按照硬件HW_KEY_XX; bit24~27:左扳机模拟量0~15 bit28~31:右扳机模拟量0~15;
        int8_t 	rx;
		int8_t  ry;
		int8_t 	lx;
		int8_t  ly;
	}gp;
	struct{
		uint8_t type;			//数据类型 gamepad_remap_type_t
		uint8_t res;
        uint16_t time;			//动作运行时间, 单位8ms
		app_kb_t kb;			//8
	}kb;
}gamepad_macro_map_t;			//12

typedef struct {
	uint8_t cfg_index;
	uint16_t step;				//从1开始
	timer_t timer;
	timer_t macro_timer;
	api_storage_map_t *mapp;
}gp_macro_ctb_t;



//按键输出
typedef struct{
	app_gamepad_key_t gp;
	uint16_t consumer_key;
	app_kb_t kb;
}gamepad_remap_key_t;

extern gamepad_macro_sta_t m_macro_sta;
extern gp_macro_ctb_t  m_macro_ctb;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
//体感模拟摇杆
void gamepad_gyro_filter_init(gamepad_filter_t *filterp, float kalman_smoot, float* fir_buf_xy, uint8_t fir_imp_size);
void gamepad_gyro_filter(gamepad_filter_t *filterp, vector2f_t* vectorp);
void gamepad_mouse_to_stick(vector2f_t* vectorp, axis2i_t kconst, axis2f_t slopes, float exponent_f);
void gamepad_stick_to_mouse(vector2f_t* vectorp, axis2i_t kconst, axis2f_t slopes, float exponent_f);
void gamepad_imu_get_body_angle(angle_t* anglep, axis3f_t* accp, axis3f_t* gyrop, gyro_range_t gyro_range, float dt);
void gamepad_angle_to_stick(joystick_cfg_t *tick_cfgp, angle_t* anglep, vector2f_t* tick_vectorp);
//宏录制
bool gamepade_macro_start_record(gp_macro_ctb_t* macrop, gamepad_macro_map_t *headp, uint16_t max_step);
bool gamepade_macro_end_record(gp_macro_ctb_t* macrop, api_storage_map_t* out_mapp);
void gamepad_macro_task(app_gamepad_key_t* keyp, gamepad_remap_key_t *rekeyp);
//改建和连点
void gamepad_remap_key(uint32_t key_in, gamepad_remap_map_t* mapp, gamepad_remap_key_t *rekeyp);
void gamepad_auto_key(gamepad_auto_t *pauto, uint32_t *pkey);
int16_t trigger_fast_convert(fast_trigger_t *fastp,int16_t value);
//曲线转换
int32_t curve_shape_remap(int32_t value, int32_t max_val, uint8_t* pcurve_shape, uint8_t shape_cnt);
//快速扳机

#ifdef __cplusplus
}
#endif
#endif





