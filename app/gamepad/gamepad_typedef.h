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
#ifndef	_gamepad_typedef_H
#define	_gamepad_typedef_H

#include "emf_typedef.h" 

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


//android   通用  Gamepad
#define GAMEPAD_A_POS 			(0)
#define GAMEPAD_B_POS 			(1)
#define GAMEPAD_C_POS 			(2)
#define GAMEPAD_X_POS 			(3)
#define GAMEPAD_Y_POS 			(4)
#define GAMEPAD_Z_POS 			(5)
#define GAMEPAD_L1_POS 			(6)
#define GAMEPAD_R1_POS 			(7)
#define GAMEPAD_L2_POS 			(8)
#define GAMEPAD_R2_POS 			(9)
#define GAMEPAD_SELECT_POS 		(10)		/*XBOX_BT_MENU_POS */
#define GAMEPAD_START_POS 		(11)		/*XBOX_BT_MAP_POS */
#define GAMEPAD_HOME_POS 		(12)
#define GAMEPAD_L3_POS 			(13)
#define GAMEPAD_R3_POS 			(14)
#define GAMEPAD_RES_POS 		(15)
#define GAMEPAD_BACK_POS		(16) 		/*only xbox back or share */


#define GAMEPAD_UP_POS 			(20)
#define GAMEPAD_DOWN_POS 		(21)
#define GAMEPAD_LEFT_POS 		(22)
#define GAMEPAD_RIGHT_POS 		(23)
#define GAMEPAD_L_UP_POS		(24)
#define GAMEPAD_L_DOWN_POS		(25)
#define GAMEPAD_L_LEFT_POS		(26)
#define GAMEPAD_L_RIGHT_POS		(27)
#define GAMEPAD_R_UP_POS		(28)
#define GAMEPAD_R_DOWN_POS		(29)
#define GAMEPAD_R_LEFT_POS		(30)
#define GAMEPAD_R_RIGHT_POS		(31)


#define GAMEPAD_A 			( 1UL<<GAMEPAD_A_POS 	 )
#define GAMEPAD_B 			( 1UL<<GAMEPAD_B_POS 	)
#define GAMEPAD_C 			( 1UL<<GAMEPAD_C_POS 	)
#define GAMEPAD_X 			( 1UL<<GAMEPAD_X_POS 	)
#define GAMEPAD_Y 			( 1UL<<GAMEPAD_Y_POS 	)
#define GAMEPAD_Z 			( 1UL<<GAMEPAD_Z_POS 	)
#define GAMEPAD_L1 			( 1UL<<GAMEPAD_L1_POS 	)
#define GAMEPAD_R1 			( 1UL<<GAMEPAD_R1_POS 	)
#define GAMEPAD_L2 			( 1UL<<GAMEPAD_L2_POS 	)
#define GAMEPAD_R2 			( 1UL<<GAMEPAD_R2_POS 	)
#define GAMEPAD_SELECT 		( 1UL<<GAMEPAD_SELECT_POS)
#define GAMEPAD_START 		( 1UL<<GAMEPAD_START_POS )
#define GAMEPAD_HOME 		( 1UL<<GAMEPAD_HOME_POS  )
#define GAMEPAD_L3 			( 1UL<<GAMEPAD_L3_POS)
#define GAMEPAD_R3 			( 1UL<<GAMEPAD_R3_POS)
#define GAMEPAD_RES 		( 1UL<<GAMEPAD_RES_POS 	 )
#define GAMEPAD_BACK	 	( 1UL<<GAMEPAD_BACK_POS )

#define GAMEPAD_UP 			( 1UL<<GAMEPAD_UP_POS 	)
#define GAMEPAD_DOWN 		( 1UL<<GAMEPAD_DOWN_POS  )
#define GAMEPAD_LEFT 		( 1UL<<GAMEPAD_LEFT_POS  )
#define GAMEPAD_RIGHT 		( 1UL<<GAMEPAD_RIGHT_POS )


//PC Dinput mode  Gamepad
#define DINPUT_Y_POS		    (0)
#define DINPUT_B_POS			(1)
#define DINPUT_A_POS			(2)
#define DINPUT_X_POS			(3)
#define DINPUT_L1_POS			(4)
#define DINPUT_R1_POS			(5)
#define DINPUT_L2_POS			(6)
#define DINPUT_R2_POS			(7)
#define DINPUT_SELECT_POS		(8)
#define DINPUT_START_POS		(9)
#define DINPUT_L3_POS	    	(10)
#define DINPUT_R3_POS	    	(11)

#define DINPUT_HOME_POS			(19)
#define DINPUT_UP_POS			(20)
#define DINPUT_DOWN_POS			(21)
#define DINPUT_LEFT_POS			(22)
#define DINPUT_RIGHT_POS		(23)
#define DINPUT_L_UP_POS			(24)
#define DINPUT_L_DOWN_POS		(25)
#define DINPUT_L_LEFT_POS		(26)
#define DINPUT_L_RIGHT_POS		(27)
#define DINPUT_R_UP_POS			(28)
#define DINPUT_R_DOWN_POS		(29)
#define DINPUT_R_LEFT_POS		(30)
#define DINPUT_R_RIGHT_POS		(31)


#define DINPUT_Y		    (0x01UL<<DINPUT_Y_POS		)
#define DINPUT_B			(0x01UL<<DINPUT_B_POS		)
#define DINPUT_A			(0x01UL<<DINPUT_A_POS		)
#define DINPUT_X			(0x01UL<<DINPUT_X_POS		)
#define DINPUT_L1			(0x01UL<<DINPUT_L1_POS		)
#define DINPUT_R1			(0x01UL<<DINPUT_R1_POS		)
#define DINPUT_L2			(0x01UL<<DINPUT_L2_POS		)
#define DINPUT_R2			(0x01UL<<DINPUT_R2_POS		)
#define DINPUT_SELECT		(0x01UL<<DINPUT_SELECT_POS	)
#define DINPUT_START		(0x01UL<<DINPUT_START_POS	)
#define DINPUT_L3	    	(0x01UL<<DINPUT_L3_POS		)
#define DINPUT_R3	    	(0x01UL<<DINPUT_R3_POS		)

#define DINPUT_HOME			(0x01UL<<DINPUT_HOME_POS	)
#define DINPUT_UP			(0x01UL<<DINPUT_UP_POS		)
#define DINPUT_DOWN			(0x01UL<<DINPUT_DOWN_POS	)
#define DINPUT_LEFT			(0x01UL<<DINPUT_LEFT_POS	)
#define DINPUT_RIGHT		(0x01UL<<DINPUT_RIGHT_POS	)
#define DINPUT_L_UP			(0x01UL<<DINPUT_L_UP_POS	)
#define DINPUT_L_DOWN		(0x01UL<<DINPUT_L_DOWN_POS	)
#define DINPUT_L_LEFT		(0x01UL<<DINPUT_L_LEFT_POS	)
#define DINPUT_L_RIGHT		(0x01UL<<DINPUT_L_RIGHT_POS)
#define DINPUT_R_UP			(0x01UL<<DINPUT_R_UP_POS	)
#define DINPUT_R_DOWN		(0x01UL<<DINPUT_R_DOWN_POS	)
#define DINPUT_R_LEFT		(0x01UL<<DINPUT_R_LEFT_POS	)
#define DINPUT_R_RIGHT		(0x01UL<<DINPUT_R_RIGHT_POS)
/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/

#ifndef pack
#pragma pack(1)
#endif

typedef struct {
	uint8_t id;
	uint8_t lx;
	uint8_t ly;
	uint8_t rx;
	uint8_t ry;
	uint8_t hat_switch;
	uint16_t button;
	uint8_t r2;
	uint8_t l2;
}gamepad_retport_t;




#ifndef pack
#pragma pack()
#endif


#endif

