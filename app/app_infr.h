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


#ifndef _app_infr_h
#define _app_infr_h
#include "emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef INFR_MIN_DEFAULT
#define INFR_MIN_DEFAULT			0
#endif

#ifndef APP_INFR_SYNC_TIMES
#define APP_INFR_SYNC_TIMES		2
#endif

#ifndef INFR_MAX
#define INFR_MAX      2
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct	
{
    uint8_t mode[INFR_MAX];
    uint8_t duty[INFR_MAX];
} infr_t;


typedef struct
{   
    uint32_t timeout;         
    uint8_t percent;
    uint8_t min;
} infr_ctb_t;

extern infr_t	m_infr;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_infr_show(void);                      //__WEAK
void app_infr_set_pa(uint8_t id, uint8_t min,uint8_t percent);
void app_infr_set_duty(uint8_t id,uint8_t duty,uint32_t timeout_ms);
bool app_infr_init(uint8_t pwm_id1,uint8_t pwm_id2);
bool app_infr_deinit(void);
void app_infr_task(uint32_t dt_ms);
void app_infr_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





