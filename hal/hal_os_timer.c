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
注意: 使用到了函数指针, 该接口适用于51平台
************************************************************************************************************/
#include "hw_config.h"
#if API_OS_TIMER_ENABLE
#include "api/api_os_timer.h"

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
error_t hal_os_timer_register(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	动态注册一个定时器,删除后回自动释放内存
	由于使用到 emf_malloc 动态内存分配, 常驻定时器不要使用 api_os_timer_create 创建定时器
*******************************************************************/
error_t hal_os_timer_create(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}
error_t hal_os_timer_delete(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	调用soft_timer_start后等待timeout后才调用回调函数
				如果定时器已经启动, 再次调用相当于soft_timer_modify 重置定时器
*******************************************************************/
error_t hal_os_timer_start(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	更新定时器
*******************************************************************/
error_t hal_os_timer_modify(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}

error_t hal_os_timer_stop(api_os_timer_t *ptimer)
{
	error_t err = ERROR_SUCCESS;
	// if(TIMER_ONE_SHOT & ptimer->ctrl){
	// }else{
	// }
	UNUSED_PARAMETER(ptimer);
	return err;
}



#endif






