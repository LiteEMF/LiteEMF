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
#include "hw_config.h"
#if TURBO_ENABLE
#include "turbo.h"
#include "api/api_tick.h"

#include "api/api_log.h"

//TODO 驱动功能待定
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool turbo_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool turbo_deinit(void)
{
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void turbo_task(turbo_t *pturbo, uint32_t *pkey)
{
	uint32_t turboing_key;

    if(pturbo->turbo_en) {
        turboing_key = *pkey & pturbo->turbo_key;
        if (turboing_key){
            pturbo->turbo_dir = !pturbo->turbo_dir;
            
            if (pturbo->turbo_dir){
                *pkey |= turboing_key;
            }else{
                *pkey &= ~turboing_key;
            }
        }else{
            pturbo->turbo_dir = false;
        }
    }
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void turbo_handler(uint32_t period_10us,turbo_t *pturbo, uint32_t *pkey)
{
	static timer_t s_timer;
	if (m_task_tick10us - s_timer >= (period_10us*100)>>1){
		s_timer = m_task_tick10us;
		turbo_task(pturbo, pkey);
	}
}
#endif


#endif






