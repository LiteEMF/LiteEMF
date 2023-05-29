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


#ifndef _key_typedef_h
#define _key_typedef_h
#include "utils/emf_typedef.h" 
#include "hw_config.h"
#include "api/api_gpio.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define HW_KEY_A_POS 			(0)
#define HW_KEY_B_POS 			(1)
#define HW_KEY_C_POS 			(2)
#define HW_KEY_X_POS 			(3)
#define HW_KEY_Y_POS 			(4)
#define HW_KEY_Z_POS 			(5)
#define HW_KEY_L1_POS 			(6)
#define HW_KEY_R1_POS 			(7)
#define HW_KEY_L2_POS 			(8)
#define HW_KEY_R2_POS 			(9)
#define HW_KEY_SELECT_POS 		(10)
#define HW_KEY_START_POS 		(11)
#define HW_KEY_HOME_POS 		(12)
#define HW_KEY_L3_POS 			(13)			
#define HW_KEY_R3_POS 			(14)
#define HW_KEY_RES_POS 			(15)

#define HW_KEY_UP_POS 			(16)		
#define HW_KEY_DOWN_POS 		(17)
#define HW_KEY_LEFT_POS 		(18)
#define HW_KEY_RIGHT_POS 		(19)
#define HW_KEY_MENU_POS 		(20)	
#define HW_KEY_MODE_POS 		(21)
#define HW_KEY_FN_POS 			(22)
#define HW_KEY_M1_POS 		    (23)
#define HW_KEY_M2_POS		    (24)
#define HW_KEY_M3_POS 			(25)
#define HW_KEY_M4_POS 		    (26)
#define HW_KEY_M5_POS 		    (27)
#define HW_KEY_M6_POS 		    (28)
#define HW_KEY_M7_POS 		    (29)
#define HW_KEY_M8_POS 		    (30)
#define HW_KEY_POWER_POS 		(31)		//POWER
#define HW_KEY_MAX_POS	 		(32)

#define HW_KEY_NULL 		(0)  
#define HW_KEY_A 			BIT(0)
#define HW_KEY_B 			BIT(1)
#define HW_KEY_C 			BIT(2)
#define HW_KEY_X 			BIT(3)
#define HW_KEY_Y 			BIT(4)
#define HW_KEY_Z 			BIT(5)
#define HW_KEY_L1 			BIT(6)
#define HW_KEY_R1 			BIT(7)
#define HW_KEY_L2 			BIT(8)
#define HW_KEY_R2 			BIT(9)
#define HW_KEY_SELECT 		BIT(10)
#define HW_KEY_START 		BIT(11)
#define HW_KEY_HOME 		BIT(12)
#define HW_KEY_L3 			BIT(13)			
#define HW_KEY_R3 			BIT(14)
#define HW_KEY_CAPTURE 		BIT(15)		/*截图/返回/touchkey*/

#define HW_KEY_UP 			BIT(16)		
#define HW_KEY_DOWN 		BIT(17)
#define HW_KEY_LEFT 		BIT(18)
#define HW_KEY_RIGHT 		BIT(19)
#define HW_KEY_MENU 		BIT(20)		/*fun key*/
#define HW_KEY_MODE 		BIT(21)		/*fun key*/
#define HW_KEY_FN	 		BIT(22)		/*fun key*/
#define HW_KEY_M1 		    BIT(23)
#define HW_KEY_M2		    BIT(24)
#define HW_KEY_M3 			BIT(25)
#define HW_KEY_M4 		    BIT(26)
#define HW_KEY_M5 		    BIT(27)
#define HW_KEY_M6 		    BIT(28)
#define HW_KEY_M7 		    BIT(29)
#define HW_KEY_M8 		    BIT(30)
#define HW_KEY_POWER 		BIT(31)

// 摇杆
#define APP_STICK_L_ID	  	0
#define APP_STICK_R_ID	  	1
#define APP_STICK_NUMS		2
#define APP_TRIGGER_L_ID	0
#define APP_TRIGGER_R_ID	1
#define APP_TRIGGER_NUMS	2

#define  STICK_UP			( 1UL<<0)
#define  STICK_DOWN			( 1UL<<1)
#define  STICK_LEFT			( 1UL<<2)
#define  STICK_RIGHT		( 1UL<<3)
#define  STICK_L_UP			STICK_UP
#define  STICK_L_DOWN		STICK_DOWN
#define  STICK_L_LEFT		STICK_LEFT
#define  STICK_L_RIGHT		STICK_RIGHT
#define  STICK_R_UP			(STICK_UP<<4)
#define  STICK_R_DOWN		(STICK_DOWN<<4)
#define  STICK_R_LEFT		(STICK_LEFT<<4)
#define  STICK_R_RIGHT		(STICK_RIGHT<<4)


#ifdef KEY_POWER_GPIO
	#if POWER_KEY_ACTIVE
		#define KEY_POWER	(api_gpio_in(KEY_POWER_GPIO))
	#else
		#define KEY_POWER	(!api_gpio_in(KEY_POWER_GPIO))
	#endif
#else
	#define KEY_POWER	false
#endif

#ifdef KEY_USB_DET_GPIO
	#define KEY_USB_DET  	(!!api_gpio_in(KEY_USB_DET_GPIO))
#else
	#define KEY_USB_DET  	false
#endif
#ifdef KEY_CHARGER_GPIO
	#define KEY_CHARGER  	(!api_gpio_in(KEY_CHARGER_GPIO))
#else
	#define KEY_CHARGER		false
#endif



#ifdef __cplusplus
}
#endif
#endif





