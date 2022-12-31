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
#include  "api/api_gpio.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_gpio_mode(pin_t pin, uint8_t mode)
{
    EMF_ASSERT((pin_t)PIN_NC != pin);
	if((pin_t)PIN_NC != pin){
		hal_gpio_mode(pin,mode);
	}
}
void api_gpio_dir(pin_t pin, pin_dir_t dir, pin_pull_t pull)
{
    EMF_ASSERT((pin_t)PIN_NC != pin);
	if((pin_t)PIN_NC != pin){
		hal_gpio_dir(pin,dir,pull);
	}
}
uint32_t api_gpio_in(pin_t pin)
{
    EMF_ASSERT((pin_t)PIN_NC != pin);
	if((pin_t)PIN_NC != pin){
		return hal_gpio_in(pin);
	}
	return 0;
}
void api_gpio_out(pin_t pin, uint8_t value)
{
    EMF_ASSERT((pin_t)PIN_NC != pin);
	if((pin_t)PIN_NC != pin){
		hal_gpio_out(pin,value);
	}
}












