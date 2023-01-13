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
**	Description:	power manager
************************************************************************************************************/
#include "hw_config.h"

#include  "api/api_pm.h"
#include  "api/api_storage.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define SOFT_RESET_MASK		('r')

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
pm_reson_t m_reset_reson;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	used in SDK befor stop 
*******************************************************************/
void api_weakup_init(void)
{
   hal_weakup_init();
}

/*******************************************************************
** Parameters: index: boot index 0: default boot, 1 second boot
** Returns:	
** Description:		
*******************************************************************/
void api_boot(uint8_t index)
{
    hal_boot(index);
}

void api_reset(void)
{
	m_storage.reset_reson = SOFT_RESET_MASK;
    hal_reset();
}

void api_sleep(void)
{
    hal_sleep();
}

bool api_pm_init(void)
{
    m_reset_reson = hal_get_reset_reson();
	if(SOFT_RESET_MASK == m_storage.reset_reson){
		m_reset_reson = PM_RESON_SOFT;
	}
    return true;
}





