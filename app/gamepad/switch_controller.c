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
#if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK)
#include  "switch_controller.h"


#if APP_BATTERY_ENABLE
#include "app_battery.h"
#endif
#if API_NFC_ENABLE
#include "app/gamepad/switch_nfc.h"
#endif

#define DEBUG_LOG_ENABLE     	1
#include "api/api_log.h"


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
bool switch_controller_init(trp_handle_t *phandle)
{
    #if (HIDD_SUPPORT & HID_SWITCH_MASK)
    switch_device_init(phandle);
    #endif

    #if (HIDH_SUPPORT & HID_SWITCH_MASK)
    switch_host_init(phandle);
    #endif

    return true;
}

bool switch_controller_deinit(trp_handle_t *phandle)
{
    #if (HIDD_SUPPORT & HID_SWITCH_MASK)
    switch_device_deinit(phandle);
    #endif

    #if (HIDH_SUPPORT & HID_SWITCH_MASK)
    switch_host_deinit(phandle);
    #endif

    return true;
}

void switch_controller_handler(trp_handle_t *phandle)
{
    #if (HIDD_SUPPORT & HID_SWITCH_MASK)
    switch_device_handler(phandle);
    #endif

    #if (HIDH_SUPPORT & HID_SWITCH_MASK)
    switch_host_handler(phandle);
    #endif
}


#endif







