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
#if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK)
#include  "xbox_controller.h"
#include  "api/api_tick.h"


#include  "api/api_log.h"



/*******************************************************************
** Function: 	
** Parameters:		
** Returns:			
*******************************************************************/
void xbox_controller_init(trp_handle_t *phandle)
{
	#if (HIDD_SUPPORT & HID_XBOX_MASK)
	xbox_device_init(phandle);
	#endif
	#if (HIDH_SUPPORT & HID_XBOX_MASK)
	xbox_host_init(phandle);
	#endif
}

void xbox_controller_deinit(trp_handle_t *phandle)
{
	#if (HIDD_SUPPORT & HID_XBOX_MASK)
	xbox_device_deinit(phandle);
	#endif
	#if (HIDH_SUPPORT & HID_XBOX_MASK)
	xbox_host_deinit(phandle);
	#endif
}

void xbox_controller_task(trp_handle_t *phandle)
{
	#if (HIDD_SUPPORT & HID_XBOX_MASK)
	xbox_device_task(phandle);
	#endif
	#if (HIDH_SUPPORT & HID_XBOX_MASK)
	xbox_host_task(phandle);
	#endif
}

#endif






