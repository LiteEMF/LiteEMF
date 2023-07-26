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
	otg 目前只负责切换和初始化, usbd/usbh任务还是单独调用适配
************************************************************************************************************/
#include "hw_config.h"
#if API_OTG_BIT_ENABLE
#include "api/api_tick.h"
#include "api/usb/api_otg.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
// api_otg_t m_otg_dev[OTG_NUM];

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
bool api_otg_init(uint8_t id, api_otg_t mode)
{
	switch(mode){
	case API_OTG_IDEL:
		#if API_USBD_BIT_ENABLE
		{
			usbd_dev_t *pdev = usbd_get_dev(id);
			usbd_req_t *preq = usbd_get_req(id);

			memset(pdev,0, sizeof(usbd_dev_t));
			memset(preq,0, sizeof(usbd_req_t));
		}
		#endif

		#if API_USBH_BIT_ENABLE
		{
			uint8_t i;
			usbh_dev_t* pdev = get_usbh_dev(id<<4);
			for(i = 0; i < (HUB_MAX_PORTS+1); i++,pdev++){
				memset(pdev, 0 ,sizeof(usbh_dev_t));
				INIT_LIST_HEAD(&pdev->class_list);
			}

			usbh_class_buf_init();
			#if USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))
			usbh_hid_km_pa_init();
			#endif
		}
		#endif

		hal_otg_init(id);
		break;
	case API_OTG_DEV:
		usbd_init(id);
		break;
	case API_OTG_HOST:
		usbh_init(id<<4);
		break;
	}

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_otg_deinit(uint8_t id, api_otg_t mode)
{
	switch(mode){
	case API_OTG_IDEL:
		api_otg_init(id,API_OTG_IDEL);
		hal_otg_deinit(id);
		break;
	case API_OTG_DEV:
		usbd_deinit(id);
		break;
	case API_OTG_HOST:
		usbh_deinit(id<<4);
		break;
	}

	return true;
}


void api_otgs_init(void)
{
	uint8_t id;

	for(id=0; id<OTG_NUM; id++){
		if(API_OTG_BIT_ENABLE & BIT(id)){
			api_otg_init(id, API_OTG_IDEL);
		}else if(API_USBD_BIT_ENABLE & BIT(id)){
			api_otg_init(id, API_OTG_DEV);
		}else if(API_USBH_BIT_ENABLE & BIT(id)){
			api_otg_init(id, API_OTG_HOST);
		}
	}
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_otg_task(void* pa)
{
	UNUSED_PARAMETER(pa);
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_otg_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_otg_task(NULL);
	}
}
#endif

#endif