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


#ifndef _api_system_h
#define _api_system_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "hal/hal_system.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef HAL_SYS_FREQ						//系统时钟
#define HAL_SYS_FREQ			(48 * 1000000L)			
#endif

#ifndef API_ENTER_CRITICAL
#define API_ENTER_CRITICAL()
#endif
#ifndef API_EXIT_CRITICAL
#define API_EXIT_CRITICAL()
#endif


typedef enum{
	SYSCLK = 0,
	UARTCLK,
	IICCLK,
	SPICLK,
	PWMCLK
}emf_clk_t;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint32_t api_get_sysclk(emf_clk_t clk);
bool api_get_uuid(uint8_t *uuid, uint8_t len);
//hal
uint32_t hal_get_sysclk(emf_clk_t clk);
bool hal_get_uuid(uint8_t *uuid, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif





