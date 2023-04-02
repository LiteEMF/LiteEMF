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
#if APP_SOFT_TIMER_ENABLE
#include "app/app_soft_timer.h"
#include "utils/emf_utils.h"

#include "api/api_log.h"

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
** Description:	静态注册一个定时器,删除后资源不会释放
*******************************************************************/
error_t soft_timer_register(soft_timer_t *ptimer,timer_cb_t cb,void *pa,timer_t timeout,soft_timer_ctrl_t ctrl)
{
	if(NULL == ptimer) return ERROR_NULL;

	ptimer->ctrl = ctrl;
	ptimer->timeout = timeout;
	ptimer->pa = pa;
	ptimer->cb = cb;
	ptimer->timer = m_systick;
	
	list_add(&ptimer->list,&timer_head);
	return ERROR_SUCCESS;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	动态注册一个定时器,删除后回自动释放内存
*******************************************************************/
soft_timer_t* soft_timer_create(timer_cb_t cb,void *pa,timer_t timeout,soft_timer_ctrl_t ctrl)
{
	soft_timer_t * ptimer;
	
	ptimer = emf_malloc(sizeof(soft_timer_t));
	if(NULL == ptimer) return NULL;

	ptimer->ctrl = ctrl | TIMER_IS_DYNAMIC;
	ptimer->timeout = timeout;
	ptimer->pa = pa;
	ptimer->cb = cb;
	ptimer->timer = m_systick;
	
	list_add(&ptimer->list,&timer_head);
	return ptimer;
}
error_t soft_timer_delete(soft_timer_t *ptimer)
{
	error_t err = ERROR_NOT_FOUND;
	soft_timer_t *pos;

	if(NULL == ptimer) return ERROR_NULL;

	list_for_each_entry(pos, &timer_head, soft_timer_t, list){	//判断ptimer有效
		if(pos == ptimer){
			pos->ctrl &= ~TIMER_ACTIVE;
			pos->ctrl |= TIMER_DEACTIVATED;
			err = ERROR_SUCCESS;
			break;
		}
	}
	return err;
}

error_t soft_timer_start(soft_timer_t *ptimer)
{
	error_t err = ERROR_NOT_FOUND;
	soft_timer_t *pos;

	if(NULL == ptimer) return ERROR_NULL;
	list_for_each_entry(pos, &timer_head, soft_timer_t, list){	//判断ptimer有效
		if(pos == ptimer){
			ptimer->ctrl |= TIMER_ACTIVE;
			err = ERROR_SUCCESS;
			break;
		}
	}
	return err;
}

error_t soft_timer_stop(soft_timer_t *ptimer)
{
	error_t err = ERROR_NOT_FOUND;
	soft_timer_t *pos;

	if(NULL == ptimer) return ERROR_NULL;
	list_for_each_entry(pos, &timer_head, soft_timer_t, list){	//判断ptimer有效
		if(pos == ptimer){
			ptimer->ctrl &= ~TIMER_ACTIVE;
			err = ERROR_SUCCESS;
			break;
		}
	}
	return err;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	
*******************************************************************/
void soft_timer_delete_all(void)
{
	soft_timer_t *pos;

	list_for_each_entry(pos, &timer_head, soft_timer_t, list){
		pos->ctrl &= ~TIMER_ACTIVE;
		pos->ctrl |= TIMER_DEACTIVATED;
	}
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool soft_timer_init(void)
{
	INIT_LIST_HEAD(&timer_head);
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool soft_timer_deinit(void)
{
	soft_timer_delete_all();
	return true;
}


void soft_timer_handler(void)
{
	soft_timer_t *pos, *n;

	list_for_each_entry_safe(pos, n, &timer_head, soft_timer_t, list){
		if(pos->ctrl & TIMER_ACTIVE){
			if(m_systick - pos->timer >= pos->timeout){
				pos->timer = m_systick;

				if(pos->ctrl & TIMER_ONE_SHOT){
					pos->ctrl &= ~TIMER_ACTIVE;
				}
				if( (NULL != pos->cb) && !(pos->ctrl & TIMER_DEACTIVATED) ){
					pos->cb(pos->pa);
				}
			}
		}
		
		if(0 == (pos->ctrl & TIMER_ACTIVE)){							//用于单次运行在任务中重新开始判断
			if(pos->ctrl & (TIMER_ONE_SHOT | TIMER_DEACTIVATED)){		//单次运行,或者销毁
				API_DISABLE_IRQ();
				list_del(&pos->list);
				if(pos->ctrl & TIMER_IS_DYNAMIC){
					emf_free(pos);
				}
				API_ENABLE_IRQ();
				break;
			}
		}
	}
}


#endif






