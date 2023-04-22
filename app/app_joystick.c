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
joystick:  stick  trigger
************************************************************************************************************/
#include  "hw_config.h"
#if APP_JOYSTICK_ENABLE

#include  "app/app_joystick.h"
#include  "api/api_tick.h"
#include  "api/api_log.h"
#include  "api/api_storage.h"
#ifdef HW_ADC_MAP
#include  "api/api_adc.h"
#endif
/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef APP_STICK_ACTIVE
#define APP_STICK_ACTIVE {{true, true},{true, true}}
#endif
#ifndef APP_TRIGGER_ACTIVE
#define APP_TRIGGER_ACTIVE {true, true}
#endif
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
joystick_t m_joystick;
joystick_cal_sta_t joystick_cal_sta;
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
const_t axis2_t stick_active[] = APP_STICK_ACTIVE;
const_t uint8_t trigger_active[] = APP_TRIGGER_ACTIVE;

joystick_cal_t *const joystick_calp = (joystick_cal_t*)m_storage.joystick_cal;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK bool app_joystick_get_adc(joystick_t* joystickp)
{
    #ifdef HW_ADC_MAP
	joystickp->tarigger[APP_TRIGGER_L_ID] = api_adc_value(ADC_L2_ID);
    joystickp->tarigger[APP_TRIGGER_R_ID] = api_adc_value(ADC_R2_ID);
	joystickp->stick[APP_STICK_L_ID].x = api_adc_value(ADC_LX_ID);
    joystickp->stick[APP_STICK_L_ID].y = api_adc_value(ADC_LY_ID);
	joystickp->stick[APP_STICK_R_ID].x = api_adc_value(ADC_RX_ID);
    joystickp->stick[APP_STICK_R_ID].y = api_adc_value(ADC_RY_ID);
    #endif
    return true;
}
//#!API
__WEAK void app_joystick_event(joystick_cal_sta_t event)
{
}
#endif

uint8_t get_stick_dir(axis2i_t* stickp)
{
    uint8_t dir=0;
 
    if(stickp->x > STICK_DIR_VAL){
        dir |= STICK_RIGHT;
    }else if(stickp->x < -STICK_DIR_VAL){
        dir |= STICK_LEFT;
    }
    if(stickp->y > STICK_DIR_VAL){
        dir |= STICK_UP;
    }else if(stickp->y < -STICK_DIR_VAL){
        dir |= STICK_DOWN;
    }
    return dir;
}

void get_stick_val(uint8_t dir, axis2i_t* stickp)
{   
    switch (dir & 0x0f){
        case STICK_UP:
            stickp->y = INT16_MAX;
            break;
        case STICK_DOWN:
            stickp->y = INT16_MIN;
            break;
        case STICK_RIGHT:
            stickp->x = INT16_MAX;
            break;
        case STICK_LEFT:
            stickp->x = INT16_MIN;
            break;
        case (STICK_UP | STICK_RIGHT):
            stickp->y = 23170;
            stickp->x = 23170;
            break;
        case (STICK_UP | STICK_LEFT):
            stickp->y = 23170;
            stickp->x = -23170;
            break;
        case (STICK_DOWN | STICK_RIGHT):
            stickp->y = -23170;
            stickp->x = 23170;
            break;
        case (STICK_DOWN | STICK_LEFT):
            stickp->y = -23170;
            stickp->x = -23170;
            break;
        default:
            stickp->y = 0;
            stickp->x = 0;
            break;
    }
}


void app_stick_deadzone(joystick_cfg_t* cfgp,axis2i_t* stickp)
{
	int32_t x,y;
    uint8_t centre_percent=0,side_percent=100;        //deadzone percent
    vector2f_t vector;
    float scale;

    if((0 == stickp->x) && (0 == stickp->y)) return;

    vector.x = stickp->x;
    vector.y = stickp->y;
    vector2f_normalization(&vector);
    vector.r = vector.r * (100 / 32767.0);

    if(NULL != cfgp){
        centre_percent = cfgp->centre_deadzone;
        side_percent = 100-cfgp->side_deadzone;
        if (cfgp->trunx) vector.x = -vector.x;
        if (cfgp->truny) vector.y = -vector.y;
    }
    
    if(vector.r > centre_percent){
        if(side_percent > centre_percent){
            scale = (vector.r - centre_percent)/(side_percent - centre_percent);
        }else{
            scale = 100;
        }

		//normalization
        if( scale > STICK_CIRCLE_LIMIT) {
            scale = STICK_CIRCLE_LIMIT;
        }
        
        x = vector.x * scale * 32768;
        y = vector.y * scale * 32768;
    }else{
        x = 0;
        y = 0;
    }
    
    stickp->x = constrain_int16(x);
    stickp->y = constrain_int16(y);
}

void app_trigger_deadzone(joystick_cfg_t* cfgp,uint16_t *valp)
{
	uint16_t val = *valp;
    uint16_t min, max;

	if(NULL != cfgp){
		if (cfgp->trunx) val = 0xffff - val;
		min = 0xffff * cfgp->centre_deadzone / 100;
		max = 0xffff * (100 - cfgp->side_deadzone) / 100;
		val = remap(val, min, max,0, 0xffff);
	}

	*valp = val;
}

uint16_t app_trigger_normalization(uint8_t id,joystick_t* adcp)
{
    int32_t adc,adc_min,adc_max;
    int32_t ret = 0;

    if(id >= APP_TRIGGER_NUMS) return ret;

    adc = adcp->tarigger[id];
    adc_min = joystick_calp->min.tarigger[id];
    adc_max = joystick_calp->max.tarigger[id];

	ret = remap(adc, adc_min, adc_max,0, 0xffff);
    if(!trigger_active[id]){ 
        ret = 0xffff - ret;
    }
	ret = constrain_int16(ret);
    return ret;
}

void app_stick_normalization(uint8_t id, axis2i_t* stickp,joystick_t* adcp)
{
    int32_t x,y;

    if(id >= APP_STICK_NUMS) return;

    //get joystick movement length
    x = 32768 * (adcp->stick[id].x - joystick_calp->mid.stick[id].x);
    y = 32768 * (adcp->stick[id].y - joystick_calp->mid.stick[id].y);
    if(x > 0){
        x /=  (joystick_calp->max.stick[id].x - joystick_calp->mid.stick[id].x);
    }else{
        x /=  (joystick_calp->mid.stick[id].x - joystick_calp->min.stick[id].x);
    }
    if(y > 0){
        y /=  (joystick_calp->max.stick[id].y - joystick_calp->mid.stick[id].y);
    }else{
        y /=  (joystick_calp->mid.stick[id].y - joystick_calp->min.stick[id].y);
    }
    if (!stick_active[id].x)    x = -x;
    if (!stick_active[id].y)    y = -y;
        
    stickp->x = constrain_int16(x);
    stickp->y = constrain_int16(y);
}





/*******************************************************************
** Parameters:		
** Returns:	    has avable r
** Description:		
*******************************************************************/
static bool joystick_get_cal_val(joystick_cal_t* calp, joystick_t* rp,joystick_t* adcp)
{
    bool ret = false;
    uint8_t i;
    for (i=0; i<APP_STICK_NUMS; i++){
        calp->min.stick[i].x = MIN(adcp->stick[i].x, calp->min.stick[i].x);
        calp->min.stick[i].y = MIN(adcp->stick[i].y, calp->min.stick[i].y);
        calp->max.stick[i].x = MAX(adcp->stick[i].x, calp->max.stick[i].x);
        calp->max.stick[i].y = MAX(adcp->stick[i].y, calp->max.stick[i].y);

        calp->min.tarigger[i] = MAX(adcp->tarigger[i], calp->min.tarigger[i]);
        calp->max.tarigger[i] = MAX(adcp->tarigger[i], calp->max.tarigger[i]);

        rp->stick[i].x = calp->max.stick[i].x - calp->min.stick[i].x;
        rp->stick[i].y = calp->max.stick[i].x - calp->min.stick[i].y;
        rp->tarigger[i] = calp->max.tarigger[i] - calp->min.tarigger[i];
        if((rp->stick[i].x > STICK_LIMIT_MIN_R) || (rp->stick[i].y > STICK_LIMIT_MIN_R)  || (rp->tarigger[i] > TRIGGER_LIMIT_MIN_R) ){
            ret = true;
        }
    }
    return ret;
}

    



    

static void joystick_do_cal(joystick_t* adcp)
{
    uint8_t id;
    axis2i_t axis;
    joystick_t r;
    static joystick_cal_t s_cal;
    static timer_t cal_timer;
    static joystick_cal_sta_t s_cal_sta = JOYSTICK_CAL_NONE;


    if(s_cal_sta != joystick_cal_sta){
        s_cal_sta = joystick_cal_sta;
        app_joystick_event(joystick_cal_sta);
    }

    switch (joystick_cal_sta){
    case JOYSTICK_CAL_START:
        s_cal.min = *adcp;
        s_cal.mid = *adcp;
        s_cal.max = *adcp;
        
        cal_timer = m_systick;
        joystick_cal_sta = JOYSTICK_CAL_MID;
        break;
    case JOYSTICK_CAL_MID:
        if(m_systick - cal_timer > 3*JOYSTICK_CAL_TIMEOUT){
            cal_timer = m_systick;
            joystick_cal_sta = JOYSTICK_CAL_FAILED;
        }else{
            if(joystick_get_cal_val(&s_cal,&r,adcp)){
                joystick_cal_sta = JOYSTICK_CAL_SID;
            }
        }
        break;
    case JOYSTICK_CAL_SID:
        joystick_get_cal_val(&s_cal,&r,adcp);

        if(m_systick - cal_timer > JOYSTICK_CAL_TIMEOUT){
            joystick_cal_sta = JOYSTICK_CAL_CHECK;
        }
        break;
    case JOYSTICK_CAL_CHECK:
        joystick_get_cal_val(&s_cal,&r,adcp);
        for (id = 0; id < APP_STICK_NUMS; id++){    //check
            if((r.stick[id].x < STICK_LIMIT_MIN_R) || (r.stick[id].y < STICK_LIMIT_MIN_R)){
                joystick_cal_sta = JOYSTICK_CAL_FAILED;
                logi("stick[%d] cal fail\n", id);
            }
            if(r.tarigger[id] < TRIGGER_LIMIT_MIN_R){
                joystick_cal_sta = JOYSTICK_CAL_FAILED;
                logi("tarigger[%d] cal fail\n", id);
            }
        }

        if(JOYSTICK_CAL_FAILED != joystick_cal_sta){    //cal fix
            for (id = 0; id < APP_STICK_NUMS; id++){
                axis = s_cal.mid.stick[id];
                axis2i_sub(&axis, &s_cal.min.stick[id]);
                AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADZONE/100.0);
                axis2i_add(&s_cal.min.stick[id], &axis);

                axis = s_cal.max.stick[id];
                axis2i_sub(&axis, &s_cal.mid.stick[id]);
                AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADZONE/100.0);
                axis2i_sub(&s_cal.max.stick[id], &axis);
            }
            for (id = 0; id < APP_TRIGGER_NUMS; id++){
                //适配扳机硬件死区
                s_cal.min.tarigger[id] += r.tarigger[id] * TRIGGER_CAL_DEADZONE / 100;
                s_cal.max.tarigger[id] -= r.tarigger[id] * TRIGGER_CAL_SIDE_DEADZONE / 100;
            }
            *joystick_calp = s_cal;
            joystick_cal_sta = JOYSTICK_CAL_SUCCEED;
            api_storage_auto_sync();
        }

        cal_timer = m_systick;
        break;
    case JOYSTICK_CAL_SUCCEED:
    case JOYSTICK_CAL_FAILED:
        if (m_systick - cal_timer > JOYSTICK_CAL_TIMEOUT){
            joystick_cal_sta = JOYSTICK_CAL_NONE;
        }
        break;
    default:
        break;
    }
}
void app_joystack_cal_start(void)
{
	joystick_cal_sta = JOYSTICK_CAL_START;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_joystick_init(void)
{
    joystick_cal_sta = JOYSTICK_CAL_NONE;
    memset(&m_joystick,0,sizeof(m_joystick));

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_joystick_deinit(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_joystick_task(void *pa)
{
    uint8_t id;
    joystick_t joystick_adc;

    app_joystick_get_adc(&joystick_adc);
    joystick_do_cal(&joystick_adc);
    for(id = 0; id < APP_STICK_NUMS; id++){
        m_joystick.tarigger[id] = app_trigger_normalization(id,&joystick_adc);
        app_stick_normalization(id, &m_joystick.stick[id],&joystick_adc);
    }

}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_joystick_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_joystick_task(NULL);
	}
}
#endif

#endif






