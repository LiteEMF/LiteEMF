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
#if APP_INFR_ENABLE

#include  "app/app_infr.h"
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
infr_t	m_infr;


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
infr_ctb_t infr_ctb[INFR_MAX];
uint8_t m_infr_sync;       
uint8_t m_infr_mode;

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if WEAK_ENABLE
__WEAK bool app_infr_show(void)
{
	bool ret = true;
	uint8_t id;

    for (id = 0; id < INFR_MAX; id++){
        #ifdef HW_PWM_MAP
        if(m_infr.mode[id] != -1)
        {
            api_pwm_set_duty(m_infr.mode[id],  m_infr.duty[id]);
        }
        #endif
    }

    return ret;
}
#endif


void app_infr_set_pa(uint8_t id, uint8_t min,uint8_t percent)
{
	if(id < INFR_MAX){
		infr_ctb[id].min = min;
		infr_ctb[id].percent = percent;
	}
}


void app_infr_set_duty(uint8_t id,uint8_t duty,uint32_t timeout_ms)
{
    uint8_t infr_duty;

	if(id >= INFR_MAX) return;

	infr_ctb[id].timeout = timeout_ms;

    if(0 == duty || 0 == infr_ctb[id].percent){         
        infr_duty = 0;
    }else{
		infr_duty = (uint32_t)duty * infr_ctb[id].percent * (255-infr_ctb[id].min) / (255 * 100);
        infr_duty += infr_ctb[id].min;
    }

    if(m_infr.duty[id] || infr_duty){
        m_infr_sync =  APP_INFR_SYNC_TIMES;
        m_infr.duty[id] = infr_duty;
        logi("infr[%d]=%d\n",id,infr_duty);
    }
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_infr_init(uint8_t pwm_id1,uint8_t pwm_id2)
{
	uint8_t id;

    m_infr_sync = 0;
    m_infr_mode = 0;
    memset(&m_infr,0,sizeof(m_infr));
    memset(&infr_ctb,0,sizeof(infr_ctb));
    m_infr.mode[0] = pwm_id1;
    m_infr.mode[1] = pwm_id2;
    for (id = 0; id < INFR_MAX; id++){
       infr_ctb[id].percent = 100;
       infr_ctb[id].min = INFR_MIN_DEFAULT;
    }
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_infr_deinit(void)
{
	app_infr_init(-1,-1);
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_infr_task(uint32_t dt_ms)
{
    uint8_t id;

    for (id = 0; id < INFR_MAX; id++){
        if (infr_ctb[id].timeout >= dt_ms){
            infr_ctb[id].timeout -= dt_ms;

            if(infr_ctb[id].timeout < dt_ms){
                infr_ctb[id].timeout = 0;
                m_infr.duty[id] = 0;
                m_infr_sync = APP_INFR_SYNC_TIMES;
            }
        }
    }

    if(m_infr_sync){
        if( app_infr_show() ) m_infr_sync--;
    }

}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_infr_handler(uint32_t period_10us)
{
	static timer_t s_timer;
    uint32_t ms = period_10us/100;

	if((m_systick - s_timer) >= ms){
		s_timer = m_systick;
		app_infr_task(ms);
	}
}
#endif

#endif




