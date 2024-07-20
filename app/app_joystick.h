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


#ifndef _app_joystick_h
#define _app_joystick_h
#include "emf_typedef.h" 
#include "utils/emf_utils.h"
#include "key_typedef.h"
#include "api/api_adc.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef JOYSTCIK_DCAL_ENABLE         //DYNAMIC CAL 代码运行中自动校准是否打开
#define JOYSTCIK_DCAL_ENABLE     0
#endif
#ifndef APP_JOYSTICK_FIR_FILTER_ENABLE
#define APP_JOYSTICK_FIR_FILTER_ENABLE     0
#endif


#ifndef JOYSTICK_CAL_TIMEOUT
#define JOYSTICK_CAL_TIMEOUT            12000
#endif
//stick 上电未校准默认值
#ifndef STICK_CAL_DEFAULT_R
#define STICK_CAL_DEFAULT_R			(ADC_RES_MAX/4)
#endif

//circle limit in square
#ifndef STICK_CIRCLE_LIMIT				//摇杆画圆切边
#define STICK_CIRCLE_LIMIT  (1.03)
#endif
#ifndef STICK_LIMIT_MIN_R				//摇杆校准最小限制半径
#define STICK_LIMIT_MIN_R (ADC_RES_MAX/4)
#endif
#ifndef STICK_CAL_DEADBAND				//摇杆中心校准死区
#define STICK_CAL_DEADBAND       0
#endif
#ifndef STICK_CAL_SIDE_DEADBAND			//摇杆边界校准死区
#define STICK_CAL_SIDE_DEADBAND   3
#endif

//trigger 上电未校准默认值
#ifndef TRIGGER_DYNAMIC_ACTIVE_ENABLE           //扳机是否支持动态修改active
#define TRIGGER_DYNAMIC_ACTIVE_ENABLE   1
#endif

#ifndef TRIGGER_CAL_DEFAULT_R			//扳机未校准默认半径
#define TRIGGER_CAL_DEFAULT_R   (ADC_RES_MAX/4)
#endif

#ifndef TRIGGER_LIMIT_MIN_R             //校准判断
#define TRIGGER_LIMIT_MIN_R (ADC_RES_MAX/2)
#endif
#ifndef TRIGGER_CAL_DEADBAND            //扳机中心校准死区(开始按下的死区)
#define TRIGGER_CAL_DEADBAND      2
#endif
#ifndef TRIGGER_CAL_SIDE_DEADBAND       //扳机边界校准死区(按到底的死区)
#define TRIGGER_CAL_SIDE_DEADBAND 2
#endif




/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
    uint8_t trunx;
    uint8_t truny;
    uint8_t centre_deadband;
    uint8_t side_deadband;

    
} joystick_cfg_t;


typedef struct {
    axis2i_t stick[APP_STICK_NUMS];
	int16_t trigger[APP_TRIGGER_NUMS];
} joystick_t;           //12



typedef struct {    
    uint16_t cal_mask;
    joystick_t min;
    joystick_t mid;
    joystick_t max;
} joystick_cal_t;		//38

typedef enum{
    JOYSTICK_CAL_NONE = 0,
    JOYSTICK_CAL_START,
    JOYSTICK_CAL_MID,
    JOYSTICK_CAL_SID,
    JOYSTICK_CAL_CHECK,
    JOYSTICK_CAL_SUCCEED,
    JOYSTICK_CAL_FAILED
} joystick_cal_sta_t;



extern joystick_t m_joystick;
extern joystick_cal_sta_t joystick_cal_sta;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_joystick_get_adc(joystick_t* joystickp);       //__WEAK
void app_joystick_event(joystick_cal_sta_t event);   //__WEAK

void app_joystack_cal_start(void);
void app_joystack_cal_end(void);
void app_stick_deadband(joystick_cfg_t* cfgp,axis2i_t* stickp);
void app_trigger_deadband(joystick_cfg_t* cfgp,uint16_t *valp);
uint8_t is_dynamic_deadband_holding (int16_t value, uint8_t deadband, int16_t* pbuf, uint8_t size);
bool app_joystick_init(void);
bool app_joystick_deinit(void);
void app_joystick_task(void *pa);
void app_joystick_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





