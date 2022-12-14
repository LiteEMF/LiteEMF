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


#ifndef _hw_board_h
#define _hw_board_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#if PROJECT_KM

#elif PROJECT_DEMO
	#if DEMO1
		//uart
		#define HW_UART_MAP {\
			{PA_00,0,0},	\
			{PA_01,0,0}	\
			}

		//adc
		#define HW_ADC_MAP {\
			{PA_00,0,0},	\
			{PA_01,0,0}	\
			}
		//IIC
		#define HW_IIC_MAP {\
			{PA_00,0,0,0,0},	\
			{PA_01,0,0,0,0}	\
			}
		//spi
		#define HW_SPI_MAP {\
			{PA_00,0,0,0,0,0},	\
			{PA_01,0,0,0,0,0}	\
			}
		//pwm
		#define HW_PWM_MAP {\
			{PA_00,0,0},	\
			{PA_01,0,0}	\
			}

	#else

	#endif
#endif




#ifdef __cplusplus
}
#endif
#endif





