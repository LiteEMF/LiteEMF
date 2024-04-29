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


#ifndef _api_os_timer_h
#define _api_os_timer_h
#include "emf_typedef.h" 
#include "utils/list.h"
#include "api/api_tick.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef API_OS_SOFT_TIMER_ENABLE
#define API_OS_SOFT_TIMER_ENABLE    1
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	TIMER_DEACTIVATED 	= BIT(0),			  /**< 1: timer is deactive 0:not*/
	TIMER_ACTIVE   		= BIT(1),             /**< 1: timer is active 0: not*/
	TIMER_IS_DYNAMIC 	= BIT(2),             /**< 1:is dynamic timer 0: static*/
	TIMER_ONE_SHOT 		= BIT(3),             /**< 0: periodic timer 1: one shot timer */
}api_os_timer_ctrl_t;

typedef void(*timer_cb_t)(void* pa);

typedef struct {
	timer_t timer;
	uint32_t timeout;			//ms
	void* pa;
	timer_cb_t cb;
	api_os_timer_ctrl_t ctrl;
	#if API_OS_SOFT_TIMER_ENABLE
	void* os_time_id;
	#endif
	list_head_t list;
}api_os_timer_t;

static list_head_t timer_head = LIST_HEAD_INIT(timer_head);


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t api_os_timer_register(api_os_timer_t *ptimer,timer_cb_t cb,void *pa,timer_t timeout,api_os_timer_ctrl_t ctrl);
api_os_timer_t* api_os_timer_create(timer_cb_t cb,void *pa,timer_t timeout,api_os_timer_ctrl_t ctrl);
error_t api_os_timer_start(api_os_timer_t *timerp);
error_t soft_timer_modify(api_os_timer_t *ptimer, uint32_t timeout);
error_t api_os_timer_stop(api_os_timer_t *timerp);
error_t api_os_timer_delete(api_os_timer_t *timerp);
void api_os_timer_delete_all(void);
bool api_os_timer_init(void);
bool api_os_timer_deinit(void);
void api_os_timer_task(void *pa);
void api_os_timer_handler(uint32_t period_10us);




//hal
error_t hal_os_timer_register(api_os_timer_t *ptimer);
error_t hal_os_timer_create(api_os_timer_t *ptimer);
error_t hal_os_timer_start(api_os_timer_t *timerp);
error_t hal_os_timer_modify(api_os_timer_t *ptimer);
error_t hal_os_timer_stop(api_os_timer_t *timerp);
error_t hal_os_timer_delete(api_os_timer_t *timerp);

#ifdef __cplusplus
}
#endif
#endif





