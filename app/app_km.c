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
#include  "hw_config.h"
#if APP_KM_ENABLE

#include  "api/api_tick.h"
#include  "app/app_km.h"
#include  "app/io_keyboard.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
kb_bit_t app_bit_kb;			//全键无冲突
kb_t app_std_kb;				//标准键盘

app_mouse_t app_mouse;
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if WEAK_ENABLE
__WEAK void app_kb_vendor_scan(kb_bit_t* keyp)
{
	
}
__WEAK void app_mouse_vendor_scan(app_mouse_t* pmouse)
{
	
}
#endif


void app_get_std_kb(kb_bit_t* pbit, kb_t* out)
{
	uint8_t i,v,k=0; 

	memset(out, 0, sizeof(kb_t));
	out->fn = pbit->fn;
	for(i= 0; i<sizeof(pbit->key); i++){
		for (v= 0; v<8; v++){
			if(pbit->key[i] & (0x01<<v)){
				if(k < sizeof(out->key)){
					out->key[k++] = i*8 + v;
				}
			}
		}
	}
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_init(void)
{
	io_keyboard_init();
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_deinit(void)
{
	io_keyboard_deinit();
	return true;
}



void app_kb_scan_task(void*pa)
{
    kb_bit_t key_bit;

	memset(&key_bit,0,sizeof(key_bit));
	
	io_keyboard_scan(&key_bit);
	app_kb_vendor_scan(&key_bit);
	app_bit_kb = key_bit;
	app_get_std_kb(&key_bit,&app_std_kb);		//获取标准键盘
	UNUSED_PARAMETER(pa);
}

void app_mouse_scan_task(void*pa)
{
    app_mouse_t mouse;

	memset(&mouse,0,sizeof(mouse));
	
	//add mouse driver

	app_mouse_vendor_scan(&mouse);
	app_mouse = mouse;
	UNUSED_PARAMETER(pa);
}



#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_km_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_kb_scan_task(NULL);
		app_mouse_scan_task(NULL);
	}
}
#endif


#endif





