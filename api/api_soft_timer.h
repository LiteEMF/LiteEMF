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


#ifndef _api_soft_timer_h
#define _api_soft_timer_h
#include "emf_typedef.h" 
#include "utils/list.h"
#include "api/api_tick.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	TIMER_INACTIVE		= 0X00,               /**< timer is inactive */
	TIMER_DEACTIVATED 	= BIT(0),			  /**< timer is deactive */
	TIMER_ACTIVE   		= BIT(1),             /**< timer is active */
	TIMER_IS_DYNAMIC 	= BIT(2),             /**< is dynamic timer */
	TIMER_ONE_SHOT 		= BIT(3),             /**< one shot timer */
	TIMER_PERIODIC 		= BIT(4),             /**< periodic timer */
}soft_timer_ctrl_t;

typedef void(*timer_cb_t)(void* pa);

typedef struct {
	timer_t timer;
	uint32_t timeout;
	void* pa;
	timer_cb_t cb;
	soft_timer_ctrl_t ctrl;
	list_head_t list;
}soft_timer_t;

static list_head_t timer_head = LIST_HEAD_INIT(timer_head);


/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t soft_timer_register(soft_timer_t *ptimer,timer_cb_t cb,void *pa,timer_t timeout,soft_timer_ctrl_t ctrl);
soft_timer_t* soft_timer_create(timer_cb_t cb,void *pa,timer_t timeout,soft_timer_ctrl_t ctrl);
error_t soft_timer_start(soft_timer_t *timerp);
error_t soft_timer_stop(soft_timer_t *timerp);
error_t soft_timer_delete(soft_timer_t *timerp);
void soft_timer_delete_all(void);
bool soft_timer_init(void);
bool soft_timer_deinit(void);
void soft_timer_task(void *pa);
void soft_timer_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





