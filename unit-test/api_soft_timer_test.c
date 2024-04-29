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
#if API_OS_TIMER_ENABLE
#include "api/api_os_timer.h"
#include "utils/emf_utils.h"

#include "api/api_log.h"

void timer1_cb(void* pa)
{
	logd("time1...\n");
}
void timer2_cb(void* pa)
{
	static uint32_t times = 0;
	times++;
	logd("time2 %d...\n",times);

	if(times >= 5){
		api_os_timer_delete((api_os_timer_t*)pa);
	}
	if(times >= 10){
		api_os_timer_delete_all();
	}
}

void api_os_timer_test(void)
{
	api_os_timer_t *timer1;
	api_os_timer_t *timer2;

	timer1 = api_os_timer_create(&timer1_cb,NULL,  200,TIMER_ACTIVE);
	timer2 = api_os_timer_create(&timer2_cb,timer1,400,TIMER_ACTIVE);
}


#endif






