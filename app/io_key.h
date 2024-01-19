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


#ifndef _io_key_h
#define _io_key_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hw_board.h"
#include "api/pinmap.h"
#include "hal_gpio.h"


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef KEY_ACTIVE				//io key high level trigger , normal low level trigger
#define KEY_ACTIVE 			0  			
#endif
#ifndef MATRIX_KEY_ACTIVE		//matrix key
#define MATRIX_KEY_ACTIVE	0
#endif
#ifndef MATRIX_KEY_DELAY
#define MATRIX_KEY_DELAY()		delay_ns(0);
#endif


//power key
#ifndef POWER_KEY_ACTIVE
#define POWER_KEY_ACTIVE	0
#endif
#ifndef POWER_KEY_PULL							//pin_pull_t 
#define POWER_KEY_PULL		PIN_PULLUP		
#endif
#ifndef POWER_SWITCH_KEY						//used switch key
#define POWER_SWITCH_KEY	0					
#endif



#ifndef KEY_POWER_GPIO
#define KEY_POWER_GPIO 		PIN_NULL
#endif
#ifndef KEY_USB_DET_GPIO
#define KEY_USB_DET_GPIO	PIN_NULL
#endif
#ifndef KEY_CHARGER_GPIO
#define KEY_CHARGER_GPIO	PIN_NULL
#endif
#ifndef KEY_CHARGER_DONE_GPIO
#define KEY_CHARGER_DONE_GPIO	PIN_NULL
#endif
#ifndef KEY_A_GPIO
#define KEY_A_GPIO 			PIN_NULL
#endif
#ifndef KEY_B_GPIO
#define KEY_B_GPIO        	PIN_NULL
#endif	
#ifndef KEY_C_GPIO	
#define KEY_C_GPIO        	PIN_NULL
#endif	
#ifndef KEY_X_GPIO	
#define KEY_X_GPIO        	PIN_NULL
#endif	
#ifndef KEY_Y_GPIO	
#define KEY_Y_GPIO        	PIN_NULL
#endif	
#ifndef KEY_Z_GPIO	
#define KEY_Z_GPIO        	PIN_NULL
#endif
#ifndef KEY_L1_GPIO
#define KEY_L1_GPIO        	PIN_NULL
#endif	
#ifndef KEY_R1_GPIO	
#define KEY_R1_GPIO        	PIN_NULL
#endif	
#ifndef KEY_L2_GPIO	
#define KEY_L2_GPIO        	PIN_NULL
#endif	
#ifndef KEY_R2_GPIO	
#define KEY_R2_GPIO        	PIN_NULL
#endif
#ifndef KEY_SELECT_GPIO
#define KEY_SELECT_GPIO    	PIN_NULL
#endif
#ifndef KEY_START_GPIO
#define KEY_START_GPIO		PIN_NULL
#endif
#ifndef KEY_HOME_GPIO
#define KEY_HOME_GPIO		PIN_NULL
#endif
#ifndef KEY_L3_GPIO
#define KEY_L3_GPIO			PIN_NULL
#endif
#ifndef KEY_R3_GPIO
#define KEY_R3_GPIO			PIN_NULL
#endif
#ifndef KEY_CAPTURE_GPIO
#define KEY_CAPTURE_GPIO	PIN_NULL
#endif
#ifndef KEY_UP_GPIO
#define KEY_UP_GPIO			PIN_NULL
#endif
#ifndef KEY_DOWN_GPIO
#define KEY_DOWN_GPIO		PIN_NULL
#endif
#ifndef KEY_LEFT_GPIO
#define KEY_LEFT_GPIO		PIN_NULL
#endif
#ifndef KEY_RIGHT_GPIO
#define KEY_RIGHT_GPIO		PIN_NULL
#endif
#ifndef KEY_MENU_GPIO
#define KEY_MENU_GPIO		PIN_NULL
#endif
#ifndef KEY_MODE_GPIO
#define KEY_MODE_GPIO		PIN_NULL
#endif
#ifndef KEY_FN_GPIO
#define KEY_FN_GPIO			PIN_NULL
#endif
#ifndef KEY_M1_GPIO
#define KEY_M1_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M2_GPIO	
#define KEY_M2_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M3_GPIO	
#define KEY_M3_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M4_GPIO	
#define KEY_M4_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M5_GPIO	
#define KEY_M5_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M6_GPIO	
#define KEY_M6_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M7_GPIO	
#define KEY_M7_GPIO       	PIN_NULL
#endif	
#ifndef KEY_M8_GPIO	
#define KEY_M8_GPIO       	PIN_NULL
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

#if (PIN_NULL != KEY_POWER_GPIO)
	#if POWER_KEY_ACTIVE
		#define KEY_POWER	(api_gpio_in(KEY_POWER_GPIO))
	#else
		#define KEY_POWER	(!api_gpio_in(KEY_POWER_GPIO))
	#endif
#else
	#define KEY_POWER	false
#endif

#if PIN_NULL != KEY_USB_DET_GPIO
	#define KEY_USB_DET  	(!!api_gpio_in(KEY_USB_DET_GPIO))
#else
	#define KEY_USB_DET  	false
#endif
#if  PIN_NULL != KEY_CHARGER_GPIO
	#define KEY_CHARGER  	(!api_gpio_in(KEY_CHARGER_GPIO))
#else
	#define KEY_CHARGER		false
#endif

#if  PIN_NULL != KEY_CHARGER_DONE_GPIO
	#define KEY_CHARGER_DONE  	(!api_gpio_in(KEY_CHARGER_DONE_GPIO))
#else
	#define KEY_CHARGER_DONE	false
#endif


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint32_t io_key_scan(void);
bool io_key_init(void);
bool io_key_deinit(void);

#ifdef __cplusplus
}
#endif
#endif





