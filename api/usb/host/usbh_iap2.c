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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2))
#include "api/usb/host/usbh.h"
#include "app/iap2/app_iap2.h"

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
bool usbh_iap2_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	if(0 == len) return;
	app_iap2_in_process(buf,len);
	logd("iap2 endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_iap2( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev) return err;
	if ((pdev->vid == 0x05AC) && ((pdev->pid&0xff00) == 0x1200)){         // apple
		if(pclass->endpin.addr ){
			err = ERROR_SUCCESS;
		}
	}

	return err;
}

error_t usbh_iap2_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);

	err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
    if(ERROR_SUCCESS == err){
		pdev->class_ready = true;
		app_iap2_init();
	}
    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_iap2_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
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
error_t usbh_iap2_deinit( uint8_t id, usbh_class_t *pclass) 
{
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_iap2_task(uint8_t id, usbh_class_t *pclass)
{
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev) return err;
	if(TUSB_STA_CONFIGURED == pdev->state){
		app_iap2_task(NULL);
	}
	UNUSED_PARAMETER(pclass);
}


#endif






