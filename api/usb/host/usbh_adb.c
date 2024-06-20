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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB))
#include "api/usb/host/usbh.h"
#include "adb/app_adb.h"


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
bool usbh_adb_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("adb endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
	return true;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_adb( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;

	if((ADB_CLASS == pclass->itf.if_cls) && (pclass->itf.if_sub_cls == ADB_SUBCLASS)){
		if(pclass->endpin.addr && pclass->endpout.addr){
			err = ERROR_SUCCESS;
		}
	}
	
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_adb_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev) return err;
    err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    if(ERROR_SUCCESS == err){
		app_adb_run();
		pdev->class_ready = true;
	}
    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_adb_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	UNUSED_PARAMETER(pdesc);
	UNUSED_PARAMETER(len);
    return ERROR_UNKNOW;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_adb_deinit( uint8_t id, usbh_class_t *pclass) 
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_adb_task( uint8_t id, usbh_class_t *pclass)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}



#endif






