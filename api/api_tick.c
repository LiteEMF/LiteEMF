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

#include  "api/api_tick.h"


/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#ifndef m_task_tick10us
volatile timerd_t m_task_tick10us;	//used for task tick 10us
#endif
#ifndef m_systick
volatile timerd_t m_systick;			//1ms
#endif
/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
#ifndef api_delay_ns
void api_delay_ns(uint32_t ns)
{
	hal_delay_ns(ns);
}
#endif
#ifndef api_delay_us
void api_delay_us(uint32_t us)
{
	hal_delay_us(us);
}
#endif
#ifndef api_delay_ms
void api_delay_ms(uint32_t ms)
{
	hal_delay_ms(ms);
}
#endif


void api_os_delay_ms(uint32_t ms)
{
	hal_os_delay_ms(ms);
}

void api_tick_init(void)
{
	#ifndef m_task_tick10us
	m_task_tick10us = 0;	//10us
	#endif
	#ifndef m_systick
	m_systick = 0;				//1ms
	#endif
    hal_tick_init();
}






