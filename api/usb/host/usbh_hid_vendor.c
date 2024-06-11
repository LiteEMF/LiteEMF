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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID)) && (USBH_HID_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)))
#include "api/api_transport.h"
#include "api/usb/host/usbh.h"

#include "app/gamepad/app_gamepad.h"
#if APP_RUMBLE_ENABLE
#include  "app/app_rumble.h"
#endif
#if APP_CMD_ENABLE
#include "app/app_command.h"
#endif

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
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_hid_vendor_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
//	logd("hid endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);

    #if APP_CMD_ENABLE
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    trp_handle.index = U16(pclass->dev_type, pclass->hid_type);

    app_command_rx(&trp_handle,buf,len);
    #endif
	
}

error_t usbh_hid_vendor_open( uint8_t id, usbh_class_t *pclass)
{
    error_t err = ERROR_SUCCESS;
    trp_handle_t trp_handle;
		
    trp_handle.trp = TR_USBH;
    trp_handle.id = id;
    trp_handle.index = U16(pclass->dev_type, pclass->hid_type);
    

    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	这里只做识别
*******************************************************************/
error_t usbh_hid_vendor_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo,uint8_t collection_index)
{
    uint8_t i;
    error_t err = ERROR_NOT_FOUND;

    if(hid_match_collection(pinfo, collection_index, 0XFF80, 0X01)){ //0x05,0x01  0x09,0X02 fix VENDOR
        pclass->hid_type = HID_TYPE_VENDOR;
        err = ERROR_SUCCESS;
    }

    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_vendor_deinit( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNKNOW;
    UNUSED_PARAMETER(id);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hid_vendor_task(uint8_t id, usbh_class_t *pclass)
{
    UNUSED_PARAMETER(id);
}

#endif






