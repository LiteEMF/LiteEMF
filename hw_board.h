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
			{PA_01,0,0}		\
			}

		//adc
		#define HW_ADC_MAP {	\
			{PA_00,0,0},		\
			{PA_01,0,0}			\
			}
		//IIC
		#define HW_IIC_MAP {	\
			{PA_00,0,0,0,0},	\
			{PA_01,0,0,0,0}		\
			}
		//spi
		#define HW_SPI_MAP {\
			{PA_00,0,0,0,0,0},	\
			{PA_01,0,0,0,0,0}	\
			}
		//pwm
		#define HW_PWM_MAP {	\
			{PA_00,0,0},		\
			{PA_01,0,0}			\
			}

		//martix key
		#define MATRIX_KEY_IN_GPIO	{PA_00, PA_01, PA_02}		//OUT
		#define MATRIX_KEY_OUT_GPIO	{PA_00, PA_01}				//IN	
		#define MATRIX_KEY	{		\
			{HW_KEY_A, HW_KEY_B, HW_KEY_C},\
			{HW_KEY_X, HW_KEY_Y, HW_KEY_Z},\
		}


		#define MATRIX_KB_IN_GPIO	{PA_00, PA_01, PA_02}		//OUT
		#define MATRIX_KB_OUT_GPIO	{PA_00, PA_01}				//IN	
		#define MATRIX_KB	{	\
			{KB_A, KB_B, KB_C},\
			{KB_X, KB_Y, KB_Z},\
		}

		#define IO_KB_GPIO 			{PA_00,PA_01,PA_02}
		#define IO_KB 				{KB_A,KB_B,KB_C}

		#define HW_LED_MAP 			{PA_00,PA_01,PA_02,PA_03}
		#define HW_LED_ACTIVE_MAP 	{true,true,true,true}

		#define HW_STORAGE			\
			uint8_t vendor;			\
			uint8_t led_sta[5]
	#elif DEMO_WIN32
		
		//IIC
		#define HW_IIC_MAP {	\
			{PIN_NULL,0,0,0,0},	\
			{PIN_NULL,0,0,0,0}		\
			}

	#else

	#endif
#endif




#ifdef __cplusplus
}
#endif
#endif





