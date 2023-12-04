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

#define APP_JOYSTICK_CAL_MASK   0XAA55
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


#if API_STORAGE_ENABLE
joystick_cal_t *const joystick_calp = (joystick_cal_t*)m_storage.joystick_cal;
#else
joystick_cal_t joystick_cal_default = {0X00, {{0,0},0} , {{ADC_RES_MAX/2,ADC_RES_MAX/2},ADC_RES_MAX/2} , {{ADC_RES_MAX,ADC_RES_MAX},ADC_RES_MAX}};
joystick_cal_t *const joystick_calp = &joystick_cal_default;
#endif

static joystick_cal_t s_cal;
joystick_cal_t m_cal;                           //动态校准后的校准数据, 动态校准不保存
int16_t dynamic_cal_max_r[APP_STICK_NUMS] = {  //动态校准最大半径值, 默认3/2*r, 不修改情况下为ADC_RES_MAX/2
    STICK_CAL_DEFAULT_R*3/2, 
    STICK_CAL_DEFAULT_R*3/2
};
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

    #if ID_NULL != ADC_LX_ID 
	    joystickp->stick[APP_STICK_L_ID].x = api_adc_value(ADC_LX_ID);
    #else
        joystickp->stick[APP_STICK_L_ID].x = m_cal.mid.stick[APP_STICK_L_ID].x;
    #endif
    #if ID_NULL != ADC_LY_ID 
        joystickp->stick[APP_STICK_L_ID].y = api_adc_value(ADC_LY_ID);
    #else
        joystickp->stick[APP_STICK_L_ID].y = m_cal.mid.stick[APP_STICK_L_ID].y;
    #endif
    #if ID_NULL != ADC_RX_ID 
	    joystickp->stick[APP_STICK_R_ID].x = api_adc_value(ADC_RX_ID);
    #else
        joystickp->stick[APP_STICK_R_ID].x = m_cal.mid.stick[APP_STICK_R_ID].x;
    #endif
    #if ID_NULL != ADC_RY_ID 
        joystickp->stick[APP_STICK_R_ID].y = api_adc_value(ADC_RY_ID);
    #else
        joystickp->stick[APP_STICK_R_ID].y = m_cal.mid.stick[APP_STICK_R_ID].y;
    #endif

    // logd("adc:%d %d, %d %d\n",joystickp->stick[APP_STICK_L_ID].x,
    //                     joystickp->stick[APP_STICK_L_ID].y,
    //                     joystickp->stick[APP_STICK_R_ID].x,
    //                     joystickp->stick[APP_STICK_R_ID].y
    //                     );
    #endif
    return true;
}
//#!API
__WEAK void app_joystick_event(joystick_cal_sta_t event)
{
}
#endif


void joystick_cal_dump(joystick_cal_t *calp)
{
    logd("joystick cal:\n");
    logd("stick lx: %d %d %d\n", calp->min.stick[0].x,calp->mid.stick[0].x,calp->max.stick[0].x);
    logd("stick ly: %d %d %d\n", calp->min.stick[0].y,calp->mid.stick[0].y,calp->max.stick[0].y);
    logd("stick rx: %d %d %d\n", calp->min.stick[1].x,calp->mid.stick[1].x,calp->max.stick[1].x);
    logd("stick ry: %d %d %d\n", calp->min.stick[1].y,calp->mid.stick[1].y,calp->max.stick[1].y);
    logd("tarigger l: %d  %d\n", calp->min.tarigger[0],calp->max.tarigger[0]);
    logd("tarigger r: %d  %d\n", calp->min.tarigger[1],calp->max.tarigger[1]);
}

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

    adc_min = m_cal.min.tarigger[id];
    adc_max = m_cal.max.tarigger[id];

	ret = remap(adc, adc_min, adc_max,0, 0xffff);
    if(!trigger_active[id]){ 
        ret = 0xffff - ret;
    }
	ret = constrain_int16(ret);

    // if(id==0) logd("%d %d",adcp->tarigger[0], ret );
    return ret;
}

void app_stick_normalization(uint8_t id, axis2i_t* stickp,joystick_t* adcp)
{
    int32_t x,y,calx_r,caly_r;

    if(id >= APP_STICK_NUMS) return;

    //get joystick movement length
    x = 32768 * (adcp->stick[id].x - m_cal.mid.stick[id].x);
    y = 32768 * (adcp->stick[id].y - m_cal.mid.stick[id].y);
    if(x > 0){
        calx_r = m_cal.max.stick[id].x - m_cal.mid.stick[id].x;
    }else{
        calx_r = (m_cal.mid.stick[id].x - m_cal.min.stick[id].x);
    }
    if(y > 0){
        caly_r = (m_cal.max.stick[id].y - m_cal.mid.stick[id].y);
    }else{
        caly_r = (m_cal.mid.stick[id].y - m_cal.min.stick[id].y);
    }
    
    if(0 == calx_r) calx_r = 1;         //avoid div 0
    if(0 == caly_r) caly_r = 1;         //avoid div 0
    x /=  calx_r;
    y /=  caly_r;
    

    if (!stick_active[id].x)    x = -x;
    if (!stick_active[id].y)    y = -y;

    stickp->x = constrain_int16(x);
    stickp->y = constrain_int16(y);
    // if(id==0) logd("%d %d %d %d =%d",adcp->stick[0].y, 32768 * (adcp->stick[id].y - m_cal.mid.stick[id].y), caly_r,y,stickp->y );
}





/*******************************************************************
** Parameters:		
** Returns:	    has avable r
** Description:		
*******************************************************************/
static bool joystick_get_cal_val(joystick_cal_t* calp, joystick_t* rp, joystick_t* adcp)
{
    bool ret = false;
    uint8_t i;
    joystick_t r;
    for (i=0; i<APP_STICK_NUMS; i++){
        calp->min.stick[i].x = MIN(adcp->stick[i].x, calp->min.stick[i].x);
        calp->min.stick[i].y = MIN(adcp->stick[i].y, calp->min.stick[i].y);
        calp->max.stick[i].x = MAX(adcp->stick[i].x, calp->max.stick[i].x);
        calp->max.stick[i].y = MAX(adcp->stick[i].y, calp->max.stick[i].y);

        calp->min.tarigger[i] = MIN(adcp->tarigger[i], calp->min.tarigger[i]);
        calp->max.tarigger[i] = MAX(adcp->tarigger[i], calp->max.tarigger[i]);

        r.stick[i].x = calp->max.stick[i].x - calp->min.stick[i].x;
        r.stick[i].y = calp->max.stick[i].x - calp->min.stick[i].y;
        r.tarigger[i] = calp->max.tarigger[i] - calp->min.tarigger[i];
        if((r.stick[i].x > STICK_LIMIT_MIN_R) || (r.stick[i].y > STICK_LIMIT_MIN_R)  || (r.tarigger[i] > TRIGGER_LIMIT_MIN_R) ){
            ret = true;
        }
    }
    if(rp) *rp = r;
    return ret;
}

void joystick_set_cal_deadzone(joystick_cal_t *calp)
{
    uint8_t id;
    axis2i_t axis;

    for (id = 0; id < APP_STICK_NUMS; id++){
        axis = calp->mid.stick[id];
        axis2i_sub(&axis, &calp->min.stick[id]);
        AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADZONE/100.0);
        axis2i_add(&calp->min.stick[id], &axis);

        axis = calp->max.stick[id];
        axis2i_sub(&axis, &calp->mid.stick[id]);
        AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADZONE/100.0);
        axis2i_sub(&calp->max.stick[id], &axis);
    }
    for (id = 0; id < APP_TRIGGER_NUMS; id++){
        //适配扳机硬件死区
        int16_t r;
        r = calp->max.tarigger[id] - calp->min.tarigger[id];
        calp->min.tarigger[id] += r * TRIGGER_CAL_DEADZONE / 100;
        calp->max.tarigger[id] -= r * TRIGGER_CAL_SIDE_DEADZONE / 100;
    }
}

static void joystick_dynamic_cal(joystick_t* adcp)
{
    uint8_t id;
    static uint16_t dynamic_cal_num = 0;

    if(JOYSTICK_CAL_NONE != joystick_cal_sta) {
        return;
    }

    //上电默认校准
    if(APP_JOYSTICK_CAL_MASK != joystick_calp->cal_mask){
        logd_r("joystick load default val\n");

        for(id = 0; id < APP_STICK_NUMS; id++){
            if((adcp->stick[id].x + STICK_CAL_DEFAULT_R > ADC_RES_MAX)
                || (adcp->stick[id].x < STICK_CAL_DEFAULT_R)){
                adcp->stick[id].x =  ADC_RES_MAX/2;
            }
            if((adcp->stick[id].y + STICK_CAL_DEFAULT_R > ADC_RES_MAX)
                || (adcp->stick[id].y < STICK_CAL_DEFAULT_R)){
                adcp->stick[id].y =  ADC_RES_MAX/2;
            }
            if(adcp->tarigger[id] + TRIGGER_CAL_DEFAULT_R > ADC_RES_MAX){
                adcp->tarigger[id] = ADC_RES_MAX/2;
            }

            joystick_calp->mid.stick[id] = adcp->stick[id];
            joystick_calp->min.stick[id].x = adcp->stick[id].x - STICK_CAL_DEFAULT_R;
            joystick_calp->min.stick[id].y = adcp->stick[id].y - STICK_CAL_DEFAULT_R;
            joystick_calp->max.stick[id].x = adcp->stick[id].x + STICK_CAL_DEFAULT_R;
            joystick_calp->max.stick[id].y = adcp->stick[id].y + STICK_CAL_DEFAULT_R;

            joystick_calp->min.tarigger[id] = adcp->tarigger[id];
            joystick_calp->max.tarigger[id] = adcp->tarigger[id] + TRIGGER_CAL_DEFAULT_R; 
        }

        #if API_STORAGE_ENABLE
        joystick_calp->cal_mask = APP_JOYSTICK_CAL_MASK;
        api_storage_auto_sync();
        #endif

        m_cal = *joystick_calp;
        s_cal = *joystick_calp;
        joystick_cal_dump(joystick_calp);
    }else{
        //使用实时动态校准
        bool side_ret = false;
        int16_t r;
        if(joystick_get_cal_val(&s_cal, NULL, adcp)){
            dynamic_cal_num++;
            if(dynamic_cal_num > 1000){
 
                dynamic_cal_num = 0;
                joystick_set_cal_deadzone(&s_cal);
                for(id = 0; id < APP_STICK_NUMS; id++){
                    if(s_cal.min.stick[id].x < m_cal.min.stick[id].x){
                        r = m_cal.mid.stick[id].x - s_cal.min.stick[id].x;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            side_ret = true;
                        }
                        m_cal.min.stick[id].x = m_cal.mid.stick[id].x - r;
                    }
                    if(s_cal.max.stick[id].x > m_cal.max.stick[id].x){
                        r = s_cal.max.stick[id].x - m_cal.mid.stick[id].x;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            side_ret = true;
                        }
                        m_cal.max.stick[id].x = m_cal.mid.stick[id].x + r;
                    }

                    if(s_cal.min.stick[id].y < m_cal.min.stick[id].y){
                        r = m_cal.mid.stick[id].y - s_cal.min.stick[id].y;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            side_ret = true;
                        }
                        m_cal.min.stick[id].y = m_cal.mid.stick[id].y - r;
                    }
                    if(s_cal.max.stick[id].y > m_cal.max.stick[id].y){
                        r = s_cal.max.stick[id].y - m_cal.mid.stick[id].y;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            side_ret = true;
                        }
                        m_cal.max.stick[id].y = m_cal.mid.stick[id].y + r;
                    }

                    if(s_cal.min.tarigger[id] < m_cal.min.tarigger[id]){
                        m_cal.min.tarigger[id] = s_cal.min.tarigger[id];
                        side_ret = true;
                    }
                    if(s_cal.max.tarigger[id] > m_cal.max.tarigger[id]){
                        m_cal.max.tarigger[id] = s_cal.max.tarigger[id];
                        side_ret = true;
                    }
                }
                if(side_ret){
                    logd_b("joystick auto cal:");
                    joystick_cal_dump(&m_cal);
                }
                s_cal = m_cal;
            }
        }
    }
 }

    

static void joystick_do_cal(joystick_t* adcp)
{
    uint8_t id;
    axis2i_t axis;
    joystick_t r;
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
        if(m_systick - cal_timer > JOYSTICK_CAL_TIMEOUT){
            cal_timer = m_systick;
            joystick_cal_sta = JOYSTICK_CAL_FAILED;
        }else{
            if(joystick_get_cal_val(&s_cal, &r, adcp)){
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
                if(id == APP_STICK_L_ID){
                    #if (ID_NULL == ADC_LX_ID) || (ID_NULL == ADC_LY_ID)
                    continue;
                    #endif
                }else{
                    #if (ID_NULL == ADC_RX_ID) || (ID_NULL == ADC_LY_ID) 
                    continue;
                    #endif
                }
                joystick_cal_sta = JOYSTICK_CAL_FAILED;
                logi("stick[%d] cal fail, %d %d\n", id, r.stick[id].x, r.stick[id].y);
            }
            if(r.tarigger[id] < TRIGGER_LIMIT_MIN_R){
                if(id == APP_TRIGGER_L_ID){
                    #if (ID_NULL == ADC_L2_ID)
                    continue;
                    #endif
                }else{
                    #if (ID_NULL == ADC_R2_ID)
                    continue;
                    #endif
                }
                
                joystick_cal_sta = JOYSTICK_CAL_FAILED;
                logi("tarigger[%d] cal fail, %d %d %d\n", id, r.tarigger[id], s_cal.min.tarigger[id], s_cal.max.tarigger[id]);
            }
        }

        if(JOYSTICK_CAL_FAILED != joystick_cal_sta){    //cal fix

            joystick_set_cal_deadzone(&s_cal);
            joystick_cal_sta = JOYSTICK_CAL_SUCCEED;

            #if API_STORAGE_ENABLE
            joystick_calp->cal_mask = APP_JOYSTICK_CAL_MASK;
            *joystick_calp = s_cal;
            api_storage_auto_sync();
            #endif

            m_cal = *joystick_calp;
        }

        cal_timer = m_systick;
        break;
    case JOYSTICK_CAL_SUCCEED:
    case JOYSTICK_CAL_FAILED:
        if (m_systick - cal_timer > JOYSTICK_CAL_TIMEOUT/4){
            if(JOYSTICK_CAL_SUCCEED == joystick_cal_sta){
                logi("joystick cal successed!\n");
            }else{
                logi("joystick cal failed!\n");
            }

            s_cal = m_cal;
            joystick_cal_sta = JOYSTICK_CAL_NONE;
            joystick_cal_dump(joystick_calp);
        }
        break;
    default:
        break;
    }
}
void app_joystack_cal_start(void)
{
    if(JOYSTICK_CAL_NONE == joystick_cal_sta){
        logd("app_joystack_cal_start\n");
	    joystick_cal_sta = JOYSTICK_CAL_START;
    }
}

void app_joystack_cal_end(void)
{
    if((joystick_cal_sta == JOYSTICK_CAL_START) 
        || (joystick_cal_sta == JOYSTICK_CAL_MID) 
        || (joystick_cal_sta == JOYSTICK_CAL_SID)) {
        logd("app_joystack_cal_end\n");
        joystick_cal_sta = JOYSTICK_CAL_CHECK;
    }
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
    m_cal = *joystick_calp;
    s_cal = m_cal;
    joystick_cal_dump(joystick_calp);
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
    static uint8_t s_ignore_num = 20;          //忽略ADC前面20个不可靠数据

    app_joystick_get_adc(&joystick_adc);
    if(0 == s_ignore_num){
        joystick_dynamic_cal(&joystick_adc);
        joystick_do_cal(&joystick_adc);
        for(id = 0; id < APP_STICK_NUMS; id++){
            m_joystick.tarigger[id] = app_trigger_normalization(id, &joystick_adc);
            app_stick_normalization(id, &m_joystick.stick[id], &joystick_adc);
        }
    }else{
        s_ignore_num--;
    }
	UNUSED_PARAMETER(pa);
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






