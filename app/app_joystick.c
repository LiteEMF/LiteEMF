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


#if APP_JOYSTICK_FIR_FILTER_ENABLE
// @ref: https://github.dev/FreeJoy-Team/FreeJoy FILTER_LEVEL_1_COEF fir impulse_response
const float joystick_fir_imp[5] = {0.4, 0.3, 0.15, 0.1, 0.05};
float joystick_fir_buf[5];
firf_axis2_t m_tick_fir[APP_STICK_NUMS];
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
uint8_t trigger_active[] = APP_TRIGGER_ACTIVE;          //支持动态调整trigger方向


#if API_STORAGE_ENABLE
joystick_cal_t *const joystick_calp = (joystick_cal_t*)m_storage.joystick_cal;
#else
joystick_cal_t joystick_cal_default = {0X00, {{0,0},0} , {{ADC_RES_MAX/2,ADC_RES_MAX/2},ADC_RES_MAX/2} , {{ADC_RES_MAX,ADC_RES_MAX}, ADC_RES_MAX}};
joystick_cal_t *const joystick_calp = &joystick_cal_default;
#endif

static joystick_cal_t s_cal;
joystick_cal_t m_trim_cal;                     //动态校准后的校准数据, 动态校准不保存
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
	joystickp->trigger[APP_TRIGGER_L_ID] = api_adc_value(ADC_L2_ID);
    joystickp->trigger[APP_TRIGGER_R_ID] = api_adc_value(ADC_R2_ID);

    #if ID_NULL != ADC_LX_ID 
	    joystickp->stick[APP_STICK_L_ID].x = api_adc_value(ADC_LX_ID);
    #else
        joystickp->stick[APP_STICK_L_ID].x = m_trim_cal.mid.stick[APP_STICK_L_ID].x;
    #endif
    #if ID_NULL != ADC_LY_ID 
        joystickp->stick[APP_STICK_L_ID].y = api_adc_value(ADC_LY_ID);
    #else
        joystickp->stick[APP_STICK_L_ID].y = m_trim_cal.mid.stick[APP_STICK_L_ID].y;
    #endif
    #if ID_NULL != ADC_RX_ID 
	    joystickp->stick[APP_STICK_R_ID].x = api_adc_value(ADC_RX_ID);
    #else
        joystickp->stick[APP_STICK_R_ID].x = m_trim_cal.mid.stick[APP_STICK_R_ID].x;
    #endif
    #if ID_NULL != ADC_RY_ID 
        joystickp->stick[APP_STICK_R_ID].y = api_adc_value(ADC_RY_ID);
    #else
        joystickp->stick[APP_STICK_R_ID].y = m_trim_cal.mid.stick[APP_STICK_R_ID].y;
    #endif

    // logd("adc:%d %d, %d %d, %d %d\n",joystickp->stick[APP_STICK_L_ID].x,
    //                     joystickp->stick[APP_STICK_L_ID].y,
    //                     joystickp->stick[APP_STICK_R_ID].x,
    //                     joystickp->stick[APP_STICK_R_ID].y,
    //                     joystickp->trigger[APP_TRIGGER_L_ID],
    //                     joystickp->trigger[APP_TRIGGER_R_ID]
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
    logd("stick lx: %d %d %d\n", calp->min.stick[0].x,calp->mid.stick[0].x,calp->max.stick[0].x);
    logd("stick ly: %d %d %d\n", calp->min.stick[0].y,calp->mid.stick[0].y,calp->max.stick[0].y);
    logd("stick rx: %d %d %d\n", calp->min.stick[1].x,calp->mid.stick[1].x,calp->max.stick[1].x);
    logd("stick ry: %d %d %d\n", calp->min.stick[1].y,calp->mid.stick[1].y,calp->max.stick[1].y);
    logd("trigger active:%d %d\n", trigger_active[0], trigger_active[1]);
    logd("trigger l: %d %d %d\n", calp->min.trigger[0],calp->mid.trigger[0],calp->max.trigger[0]);
    logd("trigger r: %d %d %d\n", calp->min.trigger[1],calp->mid.trigger[1],calp->max.trigger[1]);
}

void update_trigger_active(joystick_cal_t *calp)
{
    uint8_t i;

    if(APP_JOYSTICK_CAL_MASK == calp->cal_mask){
        for(i=0; i<APP_TRIGGER_NUMS; i++){
            if((calp->max.trigger[i] - calp->mid.trigger[i]) 
                >= (calp->mid.trigger[i] - calp->min.trigger[i])){
                trigger_active[i] = true;
            }else{
                trigger_active[i] = false;
            }
        }
        logd_r("set trigger active:%d %d\n", trigger_active[0], trigger_active[1]);
    }
}


/*******************************************************************
** Parameters: deadband: 死区范围 //TODO带测试
** Returns:	
** Description:	Dynamic deadband implementation 动态死区保持
@ref: https://github.dev/FreeJoy-Team/FreeJoy IsDynamicDeadbandHolding	
*******************************************************************/
uint8_t is_dynamic_deadband_holding (int16_t value, uint8_t deadband, int16_t* pbuf, uint8_t size)
{
	uint8_t is_holding = 0;
	int32_t	disp = 0;
	int32_t treshold;

    if (abs(value - pbuf[0]) < 3*3*deadband){      // 3*3*deadband = 3 sigma
        disp = variance_calculate(value, pbuf, size);

        treshold = 3 * deadband;
        if (disp < treshold*treshold){
            is_holding = 1;
        }
    }

	return is_holding;
}

/*******************************************************************
** Parameters: pcurve_shape, shape_cnt: 曲线点
** Returns:	
** Description: 摇杆死区设置
@ref: https://github.dev/FreeJoy-Team/FreeJoy IsDynamicDeadbandHolding	
*******************************************************************/
void app_stick_deadband(joystick_cfg_t* cfgp, vector2f_t* vectorp)
{
	int32_t x,y;
    uint8_t centre_percent=0,side_percent=100;        //deadband percent
    float scale, percent_r;
    
    percent_r = vectorp->r * (100 / 32767.0);

    if(NULL != cfgp){
        centre_percent = cfgp->centre_deadband;
        side_percent = 100-cfgp->side_deadband;
        if (cfgp->trunx) vectorp->x = -vectorp->x;
        if (cfgp->truny) vectorp->y = -vectorp->y;
    }
    
    if(percent_r > centre_percent){
        if(side_percent > centre_percent){
            scale = (percent_r - centre_percent)/(side_percent - centre_percent);
        }else{
            scale = 1;
        }

		//normalization
        if( scale > STICK_CIRCLE_LIMIT) {
            scale = STICK_CIRCLE_LIMIT;
        }
        
        vectorp->r *= scale;
    }else{
        vectorp->r = 0;
    }
}

void app_trigger_deadband(joystick_cfg_t* cfgp,uint16_t *valp)
{
	uint16_t val = *valp;
    uint16_t min, max;

	if(NULL != cfgp){
		if (cfgp->trunx) val = 0x7fff - val;
		min = 0x7fff * cfgp->centre_deadband / 100;
		max = 0x7fff * (100 - cfgp->side_deadband) / 100;
		val = remap(val, min, max,0, 0x7fff);
	}

	*valp = val;
}

uint16_t app_trigger_normalization(uint8_t id,joystick_t* adcp)
{
    int32_t adc,adc_min,adc_max;
    int32_t ret = 0;

    if(id >= APP_TRIGGER_NUMS) return ret;

    adc = adcp->trigger[id];

    adc_min = m_trim_cal.min.trigger[id];
    adc_max = m_trim_cal.max.trigger[id];

	ret = remap(adc, adc_min, adc_max,0, 0x7fff);
    if(!trigger_active[id]){ 
        ret = 0x7fff - ret;
    }
	ret = constrain_int16(ret);

    // if(id==0) logd("%d %d",adcp->trigger[0], ret );
    return ret;
}

void app_stick_normalization(uint8_t id, axis2i_t* stickp,joystick_t* adcp)
{
    int32_t x,y,calx_r,caly_r;

    if(id >= APP_STICK_NUMS) return;

    //get joystick movement length
    x = 32768 * (adcp->stick[id].x - m_trim_cal.mid.stick[id].x);
    y = 32768 * (adcp->stick[id].y - m_trim_cal.mid.stick[id].y);
    if(x > 0){
        calx_r = m_trim_cal.max.stick[id].x - m_trim_cal.mid.stick[id].x;
    }else{
        calx_r = (m_trim_cal.mid.stick[id].x - m_trim_cal.min.stick[id].x);
    }
    if(y > 0){
        caly_r = (m_trim_cal.max.stick[id].y - m_trim_cal.mid.stick[id].y);
    }else{
        caly_r = (m_trim_cal.mid.stick[id].y - m_trim_cal.min.stick[id].y);
    }
    
    if(0 == calx_r) calx_r = 1;         //avoid div 0
    if(0 == caly_r) caly_r = 1;         //avoid div 0
    x /=  calx_r;
    y /=  caly_r;
    

    if (!stick_active[id].x)    x = -x;
    if (!stick_active[id].y)    y = -y;

    stickp->x = constrain_int16(x);
    stickp->y = constrain_int16(y);
    // if(id==0) logd("%d %d %d %d =%d",adcp->stick[0].y, (adcp->stick[id].y - m_trim_cal.mid.stick[id].y), caly_r,y,stickp->y );
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

        calp->min.trigger[i] = MIN(adcp->trigger[i], calp->min.trigger[i]);
        calp->max.trigger[i] = MAX(adcp->trigger[i], calp->max.trigger[i]);

        r.stick[i].x = calp->max.stick[i].x - calp->min.stick[i].x;
        r.stick[i].y = calp->max.stick[i].x - calp->min.stick[i].y;
        r.trigger[i] = calp->max.trigger[i] - calp->min.trigger[i];
        if((r.stick[i].x > STICK_LIMIT_MIN_R) || (r.stick[i].y > STICK_LIMIT_MIN_R)  || (r.trigger[i] > TRIGGER_LIMIT_MIN_R) ){
            ret = true;
        }
    }
    if(rp) *rp = r;
    return ret;
}

void joystick_set_cal_deadband(joystick_cal_t *calp)
{
    uint8_t id;
    axis2i_t axis;

    for (id = 0; id < APP_STICK_NUMS; id++){
        axis = calp->mid.stick[id];
        axis2i_sub(&axis, &calp->min.stick[id]);
        AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADBAND/100.0);
        axis2i_add(&calp->min.stick[id], &axis);

        axis = calp->max.stick[id];
        axis2i_sub(&axis, &calp->mid.stick[id]);
        AXIS2_MUL(&axis, STICK_CAL_SIDE_DEADBAND/100.0);
        axis2i_sub(&calp->max.stick[id], &axis);
    }
    for (id = 0; id < APP_TRIGGER_NUMS; id++){
        //适配扳机硬件死区
        int16_t r;
        r = calp->max.trigger[id] - calp->min.trigger[id];
        if(trigger_active[id]){
            calp->min.trigger[id] += r * TRIGGER_CAL_DEADBAND / 100;
            calp->max.trigger[id] -= r * TRIGGER_CAL_SIDE_DEADBAND / 100;
            calp->mid.trigger[id] = calp->min.trigger[id];
        }else{
            calp->min.trigger[id] += r * TRIGGER_CAL_SIDE_DEADBAND / 100;
            calp->max.trigger[id] -= r * TRIGGER_CAL_DEADBAND / 100;
            calp->mid.trigger[id] = calp->max.trigger[id];
        }
    }
}

void joystick_save(joystick_cal_t *calp)
{
    *joystick_calp = *calp;
    joystick_calp->cal_mask = APP_JOYSTICK_CAL_MASK;
    logd_b("joystick cal:");
    joystick_cal_dump(joystick_calp);

    m_trim_cal = *calp;
    joystick_set_cal_deadband(&m_trim_cal);
    logd_b("trim cal:");
    joystick_cal_dump(&m_trim_cal);
    
    #if API_STORAGE_ENABLE
    api_storage_auto_sync();
    #endif
}

static void joystick_dynamic_cal(joystick_t* adcp)
{
    uint8_t id;
    #if JOYSTCIK_DCAL_ENABLE
    static uint16_t dynamic_cal_num = 0;
    #endif
    joystick_t adc = *adcp;

    if(JOYSTICK_CAL_NONE != joystick_cal_sta) {
        return;
    }

    //上电默认校准
    if(APP_JOYSTICK_CAL_MASK != joystick_calp->cal_mask){
        logd_r("joystick load default val\n");

        for(id = 0; id < APP_STICK_NUMS; id++){
            if((adc.stick[id].x + STICK_CAL_DEFAULT_R > ADC_RES_MAX)
                || (adc.stick[id].x < STICK_CAL_DEFAULT_R)){
                adc.stick[id].x =  ADC_RES_MAX/2;
            }
            if((adc.stick[id].y + STICK_CAL_DEFAULT_R > ADC_RES_MAX)
                || (adc.stick[id].y < STICK_CAL_DEFAULT_R)){
                adc.stick[id].y =  ADC_RES_MAX/2;
            }
            s_cal.mid.stick[id] = adc.stick[id];
            s_cal.min.stick[id].x = adc.stick[id].x - STICK_CAL_DEFAULT_R;
            s_cal.min.stick[id].y = adc.stick[id].y - STICK_CAL_DEFAULT_R;
            s_cal.max.stick[id].x = adc.stick[id].x + STICK_CAL_DEFAULT_R;
            s_cal.max.stick[id].y = adc.stick[id].y + STICK_CAL_DEFAULT_R;

            if(trigger_active[id]){
                if(adc.trigger[id] + TRIGGER_CAL_DEFAULT_R > ADC_RES_MAX){
                    adc.trigger[id] = ADC_RES_MAX - TRIGGER_CAL_DEFAULT_R;
                }
                s_cal.min.trigger[id] = adc.trigger[id];
                s_cal.mid.trigger[id] = adc.trigger[id];
                s_cal.max.trigger[id] = adc.trigger[id] + TRIGGER_CAL_DEFAULT_R; 
            }else{
                if(adc.trigger[id] < TRIGGER_CAL_DEFAULT_R){
                    adc.trigger[id] = TRIGGER_CAL_DEFAULT_R;
                }
                s_cal.min.trigger[id] = adc.trigger[id] - TRIGGER_CAL_DEFAULT_R; 
                s_cal.mid.trigger[id] = adc.trigger[id];
                s_cal.max.trigger[id] = adc.trigger[id]; 
            }
            logd_g("trigger%d %d, %d %d %d",id, trigger_active[id],s_cal.min.trigger[id],s_cal.mid.trigger[id],s_cal.max.trigger[id]);
        }
        joystick_save(&s_cal);
        
    #if JOYSTCIK_DCAL_ENABLE
    }else{
        //使用实时动态校准
        bool is_trim = false;
        int16_t r;
        if(joystick_get_cal_val(&s_cal, NULL, adcp)){
            dynamic_cal_num++;
            if(dynamic_cal_num > 1000){
 
                dynamic_cal_num = 0;
                joystick_set_cal_deadband(&s_cal);
                for(id = 0; id < APP_STICK_NUMS; id++){
                    if(s_cal.min.stick[id].x < m_trim_cal.min.stick[id].x){
                        r = m_trim_cal.mid.stick[id].x - s_cal.min.stick[id].x;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            is_trim = true;
                        }
                        m_trim_cal.min.stick[id].x = m_trim_cal.mid.stick[id].x - r;
                    }
                    if(s_cal.max.stick[id].x > m_trim_cal.max.stick[id].x){
                        r = s_cal.max.stick[id].x - m_trim_cal.mid.stick[id].x;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            is_trim = true;
                        }
                        m_trim_cal.max.stick[id].x = m_trim_cal.mid.stick[id].x + r;
                    }

                    if(s_cal.min.stick[id].y < m_trim_cal.min.stick[id].y){
                        r = m_trim_cal.mid.stick[id].y - s_cal.min.stick[id].y;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            is_trim = true;
                        }
                        m_trim_cal.min.stick[id].y = m_trim_cal.mid.stick[id].y - r;
                    }
                    if(s_cal.max.stick[id].y > m_trim_cal.max.stick[id].y){
                        r = s_cal.max.stick[id].y - m_trim_cal.mid.stick[id].y;
                        if(r > dynamic_cal_max_r[id]){
                            r = dynamic_cal_max_r[id];
                        }else{
                            is_trim = true;
                        }
                        m_trim_cal.max.stick[id].y = m_trim_cal.mid.stick[id].y + r;
                    }

                    if(s_cal.min.trigger[id] < m_trim_cal.min.trigger[id]){
                        m_trim_cal.min.trigger[id] = s_cal.min.trigger[id];
                        is_trim = true;
                    }
                    if(s_cal.max.trigger[id] > m_trim_cal.max.trigger[id]){
                        m_trim_cal.max.trigger[id] = s_cal.max.trigger[id];
                        is_trim = true;
                    }
                }
                if(is_trim){
                    logd_b("trim cal:\n");
                    joystick_cal_dump(&m_trim_cal);
                }
            }
        }
    #endif
    }
 }

    

static void joystick_do_cal(joystick_t* adcp)
{
    uint8_t id;
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
            if(r.trigger[id] < TRIGGER_LIMIT_MIN_R){
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
                logi("trigger[%d] cal fail, %d %d %d\n", id, r.trigger[id], s_cal.min.trigger[id], s_cal.max.trigger[id]);
            }
        }

        if(JOYSTICK_CAL_FAILED != joystick_cal_sta){    //cal fix
            
            joystick_cal_sta = JOYSTICK_CAL_SUCCEED;
            #if TRIGGER_DYNAMIC_ACTIVE_ENABLE
            update_trigger_active(&s_cal);              //这里先动态更新扳机active
            #endif
            joystick_save(&s_cal);
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
            s_cal = *joystick_calp;
            joystick_cal_sta = JOYSTICK_CAL_NONE;
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
    s_cal = *joystick_calp;
    #if TRIGGER_DYNAMIC_ACTIVE_ENABLE
    update_trigger_active(joystick_calp);
    #endif
    logd_b("joystick cal:\n");
    joystick_cal_dump(joystick_calp);

    m_trim_cal = *joystick_calp;
    joystick_set_cal_deadband(&m_trim_cal);
    logd_b("trim cal:\n");
    joystick_cal_dump(&m_trim_cal);


    #if APP_JOYSTICK_FIR_FILTER_ENABLE
    fir_axis2_fiter_init(&m_tick_fir[0], joystick_fir_imp, joystick_fir_buf, countof(joystick_fir_buf));
    fir_axis2_fiter_init(&m_tick_fir[1], joystick_fir_imp, joystick_fir_buf, countof(joystick_fir_buf));
    #endif
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

    #if APP_JOYSTICK_FIR_FILTER_ENABLE
        #if (ID_NULL == ADC_LX_ID) || (ID_NULL == ADC_LY_ID)        //没有摇杆不计算,节约算力
        fir_axis2l_fiter(&m_tick_fir[APP_STICK_L_ID], &joystick_adc.stick[APP_STICK_L_ID]);
        joystick_adc.stick[APP_STICK_L_ID].x = m_tick_fir[APP_STICK_L_ID].x.out;
        joystick_adc.stick[APP_STICK_L_ID].y = m_tick_fir[APP_STICK_L_ID].y.out;
        #endif
        #if (ID_NULL == ADC_RX_ID) || (ID_NULL == ADC_LY_ID) 
        fir_axis2l_fiter(&m_tick_fir[APP_STICK_R_ID], &joystick_adc.stick[APP_STICK_R_ID]);
        joystick_adc.stick[APP_STICK_R_ID].x = m_tick_fir[APP_STICK_R_ID].x.out;
        joystick_adc.stick[APP_STICK_R_ID].y = m_tick_fir[APP_STICK_R_ID].y.out;
        #endif
    #endif        

    if(0 == s_ignore_num){
        joystick_dynamic_cal(&joystick_adc);    //校准
        joystick_do_cal(&joystick_adc);
        for(id = 0; id < APP_STICK_NUMS; id++){
            m_joystick.trigger[id] = app_trigger_normalization(id, &joystick_adc);
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






