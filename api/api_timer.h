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


#ifndef _api_timer_h
#define _api_timer_h
#include "utils/emf_typedef.h" 
#include "hw_config.h"
#include "hal_timer.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define TIMER_FREQ_POS		(0)
#define TIMER_FREQ_MASK		0X000FFFFF		/* HZ  max 1M*/
#define TIMER_PRI_POS		(20)			/*Priority*/
#define TIMER_PRI_MASK		0X00F00000
#define TIMER_CH_POS		(24)
#define TIMER_CH_MASK		0X0F000000


#ifndef TIMER_FREQ_ATT
#define TIMER_FREQ_ATT(id)  		(FLD2VAL(TIMER_FREQ, m_timer_map[id].att))
#endif
#ifndef TIMER_CH_ATT
#define TIMER_CH_ATT(id)  			(FLD2VAL(TIMER_CH, m_timer_map[id].att))
#endif
#ifndef TIMER_PRI_ATT
#define TIMER_PRI_ATT(id)  			(FLD2VAL(TIMER_PRI, m_timer_map[id].att))
#endif




/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

typedef struct{
	uint32_t peripheral;
	uint32_t att;
} timer_map_t;

extern const_t timer_map_t m_timer_map[];          
extern uint8c_t m_timer_num;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_timer_hook(uint8_t id);		//__WEAK
bool api_timer_init(uint8_t id);
bool api_timer_deinit(uint8_t id);
void api_timers_init(void);

//hal
bool hal_timer_init(uint8_t id);
bool hal_timer_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





