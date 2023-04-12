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


#ifndef _nfc_driver_h
#define _nfc_driver_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define TAG_SIZE_MAX		540		//NTAG215
#define TAG_PAGE_SIZE		(60)	//NTAG215 page, 根据不同芯片不一样



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool nfc_driver_auth(uint8_t *ptPwdData,uint8_t* ptPack);
bool nfc_driver_polling(uint8_t *psn, uint8_t*psn_len, uint8_t *pversion, uint8_t*pversion_len);
bool nfc_driver_read(uint16_t addr, uint8_t *buf, uint16_t len);
bool nfc_driver_write(uint16_t addr, uint8_t *buf, uint16_t len);
bool nfc_driver_init(void);
bool nfc_driver_deinit(void);
void nfc_driver_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





