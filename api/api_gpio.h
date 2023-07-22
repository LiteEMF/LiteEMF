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


#ifndef _api_gpio_h
#define _api_gpio_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hal_gpio.h"
#include "pinmap.h"
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef GPIO_OD_EN			//是否支持开漏输出
#define GPIO_OD_EN		1
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_gpio_mode(pin_t pin, uint8_t mode);
void api_gpio_dir(pin_t pin, pin_dir_t dir, pin_pull_t pull);
uint32_t api_gpio_in(pin_t pin);
void api_gpio_out(pin_t pin, uint8_t value);

//hal
void hal_gpio_mode(pin_t pin, uint8_t mode);
void hal_gpio_dir(pin_t pin, pin_dir_t dir, pin_pull_t pull);
uint32_t hal_gpio_in(pin_t pin);
void hal_gpio_out(pin_t pin, uint8_t value);

#ifdef __cplusplus
}
#endif
#endif





