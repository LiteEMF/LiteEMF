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
#if API_WDT_ENABLE

#include  "api/api_wdt.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
//hal

void hal_wdt_feed(void)
{
}

bool hal_wdt_init(uint32_t ms)
{
	return false;
}
bool hal_wdt_deinit(void)
{
	return false;
}


#endif







