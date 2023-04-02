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
#include  "io_key.h"
#include  "api/api_tick.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint32_t m_io_key;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if defined(MATRIX_KEY_IN_GPIO) && defined(MATRIX_KEY_OUT_GPIO)	&& defined(MATRIX_KEY)	
//[IN],[OUT]
static uint8c_t matrix_key_in_gpio[] = MATRIX_KEY_IN_GPIO; 
static uint8c_t matrix_key_out_gpio[] = MATRIX_KEY_OUT_GPIO; 
static const_t uint32_t matrix_key[countof(matrix_key_out_gpio)][countof(matrix_key_in_gpio)] = MATRIX_KEY;
 

static void matrix_key_init(void)
{
	uint8_t out;
	uint8_t in;

	for(out = 0; out < countof(matrix_key_out_gpio); out++){
		//out 
		api_gpio_dir(matrix_key_out_gpio[out], PIN_IN, PIN_PULLNONE);
		
		for(in = 0; in < countof(matrix_key_in_gpio); in++){
			#if MATRIX_KEY_ACTIVE
			api_gpio_dir(matrix_key_in_gpio[out], PIN_IN, PIN_PULLDOWN);
			#else
			api_gpio_dir(matrix_key_in_gpio[out], PIN_IN, PIN_PULLUP);
			#endif
		}
		
	}
}

static uint32_t matrix_key_scan(void)
{
	uint8_t out;
	uint8_t in;
	uint32_t key = 0;
	for(out = 0; out < countof(matrix_key_out_gpio); out++){
		//out 
		api_gpio_dir(matrix_key_out_gpio[out], PIN_OUT, 0);
		api_gpio_out(matrix_key_out_gpio[out], MATRIX_KEY_ACTIVE);
		MATRIX_KEY_DELAY();
		for(in = 0; in < countof(matrix_key_in_gpio); in++){
			if(MATRIX_KEY_ACTIVE == BOOL_SET(api_gpio_in(matrix_key_in_gpio[in])) ){
				key |= matrix_key[out][in];
			}
		}
		api_gpio_dir(matrix_key_out_gpio[out], PIN_IN, PIN_PULLNONE);
	}

	return key;
}
#endif


void iokey_init(void)
{
	pin_pull_t pull; 

	#if KEY_ACTIVE
	pull = PIN_PULLDOWN;
	#else
	pull = PIN_PULLUP;
	#endif

	#if PIN_NULL != KEY_POWER_GPIO
	api_gpio_dir(KEY_POWER_GPIO, PIN_IN,POWER_KEY_PULL);
	#endif
	#if PIN_NULL != KEY_USB_DET_GPIO
	api_gpio_dir(KEY_USB_DET_GPIO, PIN_IN, PIN_PULLNONE);		// no pull
	#endif
	#if PIN_NULL != KEY_CHARGER_GPIO
	api_gpio_dir(KEY_CHARGER_GPIO, PIN_IN, PIN_PULLUP);
	#endif

	#if PIN_NULL != KEY_A_GPIO
	api_gpio_dir(KEY_A_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_B_GPIO
	api_gpio_dir(KEY_B_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_C_GPIO
	api_gpio_dir(KEY_C_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_X_GPIO
	api_gpio_dir(KEY_X_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_Y_GPIO
	api_gpio_dir(KEY_Y_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_Z_GPIO
	api_gpio_dir(KEY_Z_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_L1_GPIO
	api_gpio_dir(KEY_L1_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_R1_GPIO
	api_gpio_dir(KEY_R1_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_L2_GPIO
	api_gpio_dir(KEY_L2_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_R2_GPIO
	api_gpio_dir(KEY_R2_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_SELECT_GPIO
	api_gpio_dir(KEY_SELECT_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_START_GPIO
	api_gpio_dir(KEY_START_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_HOME_GPIO
	api_gpio_dir(KEY_HOME_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_L3_GPIO
	api_gpio_dir(KEY_L3_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_R3_GPIO
	api_gpio_dir(KEY_R3_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_CAPTURE_GPIO
	api_gpio_dir(KEY_CAPTURE_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_UP_GPIO
	api_gpio_dir(KEY_UP_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_DOWN_GPIO
	api_gpio_dir(KEY_DOWN_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_LEFT_GPIO
	api_gpio_dir(KEY_LEFT_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_RIGHT_GPIO
	api_gpio_dir(KEY_RIGHT_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_MENU_GPIO
	api_gpio_dir(KEY_MENU_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_MODE_GPIO
	api_gpio_dir(KEY_MODE_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_FN_GPIO
	api_gpio_dir(KEY_FN_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M1_GPIO
	api_gpio_dir(KEY_M1_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M2_GPIO
	api_gpio_dir(KEY_M2_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M3_GPIO
	api_gpio_dir(KEY_M3_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M4_GPIO
	api_gpio_dir(KEY_M4_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M5_GPIO
	api_gpio_dir(KEY_M5_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M6_GPIO
	api_gpio_dir(KEY_M6_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M7_GPIO
	api_gpio_dir(KEY_M7_GPIO, PIN_IN, pull);
	#endif
	#if PIN_NULL != KEY_M8_GPIO
	api_gpio_dir(KEY_M8_GPIO, PIN_IN, pull);
	#endif
}



static uint32_t iokey_scan(void)
{
	uint32_t value=0;

	#if PIN_NULL != KEY_A_GPIO
	if(api_gpio_in(KEY_A_GPIO))	value |= HW_KEY_A;
	#endif
	#if PIN_NULL != KEY_B_GPIO
	if(api_gpio_in(KEY_B_GPIO))	value |= HW_KEY_B;
	#endif
	#if PIN_NULL != KEY_C_GPIO
	if(api_gpio_in(KEY_C_GPIO))	value |= HW_KEY_C;
	#endif
	#if PIN_NULL != KEY_X_GPIO
	if(api_gpio_in(KEY_X_GPIO))	value |= HW_KEY_X;
	#endif
	#if PIN_NULL != KEY_Y_GPIO
	if(api_gpio_in(KEY_Y_GPIO))	value |= HW_KEY_Y;
	#endif
	#if PIN_NULL != KEY_Z_GPIO
	if(api_gpio_in(KEY_Z_GPIO))	value |= HW_KEY_Z;
	#endif
	#if PIN_NULL != KEY_L1_GPIO
	if(api_gpio_in(KEY_L1_GPIO))	value |= HW_KEY_L1;
	#endif
	#if PIN_NULL != KEY_R1_GPIO
	if(api_gpio_in(KEY_R1_GPIO))	value |= HW_KEY_R1;
	#endif
	#if PIN_NULL != KEY_L2_GPIO
	if(api_gpio_in(KEY_L2_GPIO))	value |= HW_KEY_L2;
	#endif
	#if PIN_NULL != KEY_R2_GPIO
	if(api_gpio_in(KEY_R2_GPIO))	value |= HW_KEY_R2;
	#endif
	#if PIN_NULL != KEY_SELECT_GPIO
	if(api_gpio_in(KEY_SELECT_GPIO))	value |= HW_KEY_SELECT;
	#endif
	#if PIN_NULL != KEY_START_GPIO
	if(api_gpio_in(KEY_START_GPIO))	value |= HW_KEY_START;
	#endif
	#if PIN_NULL != KEY_HOME_GPIO
	if(api_gpio_in(KEY_HOME_GPIO))	value |= HW_KEY_HOME;
	#endif
	#if PIN_NULL != KEY_L3_GPIO
	if(api_gpio_in(KEY_L3_GPIO))	value |= HW_KEY_L3;
	#endif
	#if PIN_NULL != KEY_R3_GPIO
	if(api_gpio_in(KEY_R3_GPIO))	value |= HW_KEY_R3;
	#endif
	#if PIN_NULL != KEY_CAPTURE_GPIO
	if(api_gpio_in(KEY_CAPTURE_GPIO))	value |= HW_KEY_CAPTURE;
	#endif
	#if PIN_NULL != KEY_UP_GPIO
	if(api_gpio_in(KEY_UP_GPIO))	value |= HW_KEY_UP;
	#endif
	#if PIN_NULL != KEY_DOWN_GPIO
	if(api_gpio_in(KEY_DOWN_GPIO))	value |= HW_KEY_DOWN;
	#endif
	#if PIN_NULL != KEY_LEFT_GPIO
	if(api_gpio_in(KEY_LEFT_GPIO))	value |= HW_KEY_LEFT;
	#endif
	#if PIN_NULL != KEY_RIGHT_GPIO
	if(api_gpio_in(KEY_RIGHT_GPIO))	value |= HW_KEY_RIGHT;
	#endif
	#if PIN_NULL != KEY_MENU_GPIO
	if(api_gpio_in(KEY_MENU_GPIO))	value |= HW_KEY_MENU;
	#endif
	#if PIN_NULL != KEY_MODE_GPIO
	if(api_gpio_in(KEY_MODE_GPIO))	value |= HW_KEY_MODE;
	#endif
	#if PIN_NULL != KEY_FN_GPIO
	if(api_gpio_in(KEY_FN_GPIO))	value |= HW_KEY_FN;
	#endif
	#if PIN_NULL != KEY_M1_GPIO
	if(api_gpio_in(KEY_M1_GPIO))	value |= HW_KEY_M1;
	#endif
	#if PIN_NULL != KEY_M2_GPIO
	if(api_gpio_in(KEY_M2_GPIO))	value |= HW_KEY_M2;
	#endif
	#if PIN_NULL != KEY_M3_GPIO
	if(api_gpio_in(KEY_M3_GPIO))	value |= HW_KEY_M3;
	#endif
	#if PIN_NULL != KEY_M4_GPIO
	if(api_gpio_in(KEY_M4_GPIO))	value |= HW_KEY_M4;
	#endif
	#if PIN_NULL != KEY_M5_GPIO
	if(api_gpio_in(KEY_M5_GPIO))	value |= HW_KEY_M5;
	#endif
	#if PIN_NULL != KEY_M6_GPIO
	if(api_gpio_in(KEY_M6_GPIO))	value |= HW_KEY_M6;
	#endif
	#if PIN_NULL != KEY_M7_GPIO
	if(api_gpio_in(KEY_M7_GPIO))	value |= HW_KEY_M7;
	#endif
	#if PIN_NULL != KEY_M8_GPIO
	if(api_gpio_in(KEY_M8_GPIO))	value |= HW_KEY_M8;
	#endif

	return value;
}

uint32_t io_key_scan(void)
{
	static uint32_t s_key;
	static timer_t key_timer;

	uint32_t key;

	if((m_systick - key_timer) >= 1){
		key_timer = m_systick;

		key = iokey_scan();
		#if defined(MATRIX_KEY_IN_GPIO) && defined(MATRIX_KEY_OUT_GPIO)	&& defined(MATRIX_KEY)	
		key |= matrix_key_scan();
		#endif

		if(s_key != key){
			s_key = key;
		}else{
			m_io_key = s_key;
		}
	}

    return m_io_key;
 }
 


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool io_key_init(void)
{
	m_io_key = 0;
	iokey_init();

	#if defined(MATRIX_KEY_IN_GPIO) && defined(MATRIX_KEY_OUT_GPIO)	&& defined(MATRIX_KEY)	
	matrix_key_init();
	#endif

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool io_key_deinit(void)
{
	return io_key_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void io_key_handler(void)
{

}








