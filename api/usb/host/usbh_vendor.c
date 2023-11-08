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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR))
#include "api/usb/host/usbh.h"


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
void usbh_vendor_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("vendor endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_vendor( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;

	if ((TUSB_CLASS_VENDOR == pclass->itf.if_cls)){
		if( pclass->endpin.addr || pclass->endpout.addr ){
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
error_t usbh_vendor_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);

	err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    if(ERROR_SUCCESS == err) pdev->class_ready = true;
    return err;
}

error_t usbh_vendor_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
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
error_t usbh_vendor_deinit( uint8_t id, usbh_class_t *pclass) 
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
void usbh_vendor_task(uint8_t id, usbh_class_t *pclass)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}


#endif






