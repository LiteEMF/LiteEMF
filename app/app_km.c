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
#include  "hw_config.h"
#if APP_KM_ENABLE


#include  "app/app_km.h"
#include  "app/io_keyboard.h"
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


#if WEAK_ENABLE
__WEAK void app_kb_vendor_scan(uint8_t* keyp, uint8_t len)
{
	
}
#endif

#if WEAK_ENABLE
__WEAK void app_km_event(void)
{
    
}
#endif

void app_km_scan(void)
{
	uint8_t i;
    uint8_t key[8];

	memset(&key,0,sizeof(key));
    i = io_keyboard_scan(key,sizeof(key));
    app_kb_vendor_scan(key,sizeof(key));
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_init(void)
{
	return io_keyboard_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_deinit(void)
{
	return io_keyboard_deinit();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_km_handler(void)
{

}


#endif





