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


#ifndef _api_otg_h
#define _api_otg_h
#include "utils/emf_typedef.h" 

#include "api/usb/device/usbd.h"
#include "api/usb/host/usbh.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#if USBD_NUM > USBH_NUM
#define OTG_NUM			USBD_NUM
#else
#define OTG_NUM			USBH_NUM
#endif	


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	API_OTG_IDEL = 0,
	API_OTG_DEV,
	API_OTG_HOST
}api_otg_t;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_otg_init(uint8_t id, api_otg_t mode);
bool api_otg_deinit(uint8_t id, api_otg_t mode);
void api_otgs_init(void);
void api_otg_task(void* pa);
void api_otg_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





