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

#if API_STORAGE_ENABLE
#include  "api/api_flash.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
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
** Description:	must call api_flash_erase befor write
*******************************************************************/
bool api_flash_write(uint16_t offset,uint8_t *buf,uint16_t len)
{
	return hal_flash_write( offset, buf, len);;
}
bool api_flash_read(uint16_t offset,uint8_t *buf,uint16_t len)
{
	return hal_flash_read(offset, buf, len);
}
bool api_flash_erase(uint16_t offset)
{
	return hal_flash_erase(offset);
}
bool api_flash_init(void)
{
    return hal_flash_init();
}

#endif






