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


#ifndef _api_flash_h
#define _api_flash_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hal_flash.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef API_FLASH_PAGE_NUM
#define API_FLASH_PAGE_NUM		2
#endif
#ifndef API_FLASH_PAGE_SIZE
#define API_FLASH_PAGE_SIZE		0x100
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_flash_erase(uint16_t offset);
bool api_flash_write(uint16_t offset,uint8_t *buf,uint16_t len);
bool api_flash_read(uint16_t offset,uint8_t *buf,uint16_t len);
bool api_flash_init(void);

//hal
bool hal_flash_erase(uint16_t offset);
bool hal_flash_write(uint16_t offset,uint8_t *buf,uint16_t len);
bool hal_flash_read(uint16_t offset,uint8_t *buf,uint16_t len);
bool hal_flash_init(void);

#ifdef __cplusplus
}
#endif
#endif





