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
#ifndef m_tick
volatile timerd_t m_tick;			//1ms
#endif
/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
#ifndef delay_ns
void delay_ns(uint32_t ns)
{
	hal_delay_ns(ns);
}
#endif
#ifndef delay_us
void delay_us(uint32_t us)
{
	hal_delay_us(us);
}
#endif
#ifndef delay_ms
void delay_ms(uint32_t ms)
{
	hal_delay_ms(ms);
}
#endif

void api_tick_init(void)
{
	m_task_tick10us = 0;	//10us
	m_tick = 0;				//1ms
    hal_tick_init();
}






