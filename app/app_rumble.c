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
#if APP_RUMBLE_ENABLE

#include  "app/app_rumble.h"
#ifdef HW_PWM_MAP
#include  "api/api_pwm.h"
#endif
#include  "api/api_tick.h"
#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
rumble_t	m_rumble;
bool m_rumble_enable = true;            //rumble default enable true, controler in project

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
rumble_ctb_t rumble_ctb[RUMBLE_MAX];
uint8_t m_rumble_sync;       

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if WEAK_ENABLE
__WEAK bool app_rumble_show(void)
{
	bool ret = true;
	uint8_t id;

    for (id = 0; id < RUMBLE_MAX; id++){
        #ifdef HW_PWM_MAP
        api_pwm_set_duty(id,  m_rumble.duty[id]);
        #endif
    }

    return ret;
}
#endif


void app_rumble_set_pa(uint8_t id, uint8_t min,uint8_t percent)
{
	if(id < RUMBLE_MAX){
		rumble_ctb[id].min = min;
		rumble_ctb[id].percent = percent;
	}
}


void app_rumble_set_duty(uint8_t id,uint8_t duty,uint32_t timeout_ms)
{
	uint8_t rumble_duty;

	if(id >= RUMBLE_MAX) return;
    if(!m_rumble_enable) duty = 0;      //rumble disable duty设置为0

	rumble_ctb[id].timeout = timeout_ms;

    if(0 == duty || 0 == rumble_ctb[id].percent){         
        rumble_duty = 0;
    }else{
		rumble_duty = duty * rumble_ctb[id].percent / 100;
        if(rumble_duty < rumble_ctb[id].min){
            rumble_duty = rumble_ctb[id].min;
        }
    }

    if(m_rumble.duty[id] || rumble_duty){
        m_rumble_sync =  APP_RUMBLE_SYNC_TIMES;
        m_rumble.duty[id] = rumble_duty;
        logi("rumble[%d]=%d\n",id,rumble_duty);
    }
}

void app_set_rumble(rumble_t *rumblep,uint32_t timeout_ms)
{
    uint8_t id;

    for (id = 0; id < RUMBLE_MAX; id++){
        app_rumble_set_duty(id,rumblep->duty[id],timeout_ms);
    }
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_rumble_init(void)
{
	uint8_t id;

    m_rumble_sync = 0;
    m_rumble_enable = true;
    memset(&m_rumble,0,sizeof(m_rumble));
    memset(&rumble_ctb,0,sizeof(rumble_ctb));
    for (id = 0; id < RUMBLE_MAX; id++){
       rumble_ctb[id].percent = 100;
       rumble_ctb[id].min = RUMBLE_MIN_DEFAULT;
    }
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_rumble_deinit(void)
{
	app_rumble_init();
    app_rumble_show();
    m_rumble_enable = false;		//关闭马达设置,防止关机马达震动
    
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_rumble_task(uint32_t dt_ms)
{
    uint8_t id;

    for (id = 0; id < RUMBLE_MAX; id++){
        if (rumble_ctb[id].timeout > dt_ms){
            rumble_ctb[id].timeout -= dt_ms;

            if(rumble_ctb[id].timeout < dt_ms){
                rumble_ctb[id].timeout = 0;
                m_rumble.duty[id] = 0;
                m_rumble_sync = APP_RUMBLE_SYNC_TIMES;
            }
        }
    }

    if(m_rumble_sync){
        if( app_rumble_show() ) m_rumble_sync--;
    }

}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_rumble_handler(uint32_t period_10us)
{
	static timer_t s_timer;
    uint32_t ms = period_10us/100;

	if((m_systick - s_timer) >= ms){
		s_timer = m_systick;
		app_rumble_task(ms);
	}
}
#endif

#endif




