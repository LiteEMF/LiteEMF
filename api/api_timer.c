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
#include "hw_board.h"
#if	defined HW_TIMER_MAP && API_TIMER_BIT_ENABLE
#include  "api/api_timer.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

const_t timer_map_t m_timer_map[] = HW_TIMER_MAP;          
uint8c_t m_timer_num = countof(m_timer_map);
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
#if WEAK_ENABLE
__WEAK void api_timer_hook(uint8_t id)
{
}
#endif
bool api_timer_init(uint8_t id)
{
	return hal_timer_init(id);
}
bool api_timer_deinit(uint8_t id)
{
	return hal_timer_deinit(id);
}

void api_timers_init(void)
{
	uint8_t id;
	for(id=0; id<m_timer_num; id++){
		api_timer_init(id);
	}   
}
void api_timers_deinit(void)
{
	uint8_t id;
	for(id=0; id<m_timer_num; id++){
		api_timer_deinit(id);
	}   
}

#endif







