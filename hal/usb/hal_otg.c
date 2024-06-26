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
#if API_OTG_BIT_ENABLE
#include "api/usb/usb_typedef.h"
#include "api/usb/api_otg.h"

#include "api/api_log.h"

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
error_t hal_otg_init(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}
error_t hal_otg_deinit(uint8_t id)
{
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}

#endif



