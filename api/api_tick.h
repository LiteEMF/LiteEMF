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


#ifndef _api_tick_h
#define _api_tick_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "api/api_system.h"
#include "hal/hal_tick.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

#ifndef	timerd_t
#define timerd_t	uint32d_t
#endif
#ifndef	timer_t
#define timer_t	uint32_t
#endif

#ifndef m_task_tick10us
extern volatile timerd_t m_task_tick10us;	//used for task tick 10us
#endif
#ifndef m_systick
extern  volatile timerd_t m_systick;			//1ms
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
void delay_ns(uint32_t ns);
#endif
#ifndef delay_us
void delay_us(uint32_t us);
#endif
#ifndef delay_ms
void delay_ms(uint32_t ms);
#endif
void api_tick_init(void);


//hal
void hal_delay_ns(uint32_t ns);
void hal_delay_us(uint32_t us);
void hal_delay_ms(uint32_t ms);
void hal_tick_init(void);

#ifdef __cplusplus
}
#endif
#endif





