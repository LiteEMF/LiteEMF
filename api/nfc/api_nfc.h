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


#ifndef _api_nfc_h
#define _api_nfc_h
#include "utils/emf_typedef.h" 
#include "nfc_driver.h"
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************************************************
**	Defined
********************************************************************************************************************/

/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/
typedef enum {
	NFC_CLOSE = 0,
	NFC_IDLE,						//初始化完成,等待指令
	NFC_POLLING,					//扫描NTAG卡
	NFC_GET_TAG_SN,					//扫描到卡,等待读取,注意这个顺序不能被改变
	NFC_NTAG_READ,
	NFC_NTAG_READ_FINLSH,
	NFC_NTAG_WRITE,
	NFC_NTAG_WRITE_FINLSH,
	NFC_NTAG_FINLSH,				//操作完成
}api_nfc_state_t;

extern api_nfc_state_t  m_api_nfc_sta;
extern uint8_t m_api_nfc_addr;
extern uint8_t m_nfc_sn[8];
extern uint8_t m_nfc_buf[TAG_SIZE_MAX];

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_nfc_start_polling(void);
bool api_nfc_stop_polling(void);    
bool api_nfc_start_read(void);
bool api_nfc_start_write(void);
bool api_nfc_start_finlsh(void);
bool api_nfc_polling(void);
bool api_nfc_read(uint16_t address,uint8_t* nfc_buf, uint16_t len);
bool api_nfc_write(uint16_t address,uint8_t* nfc_buf, uint16_t len);
bool api_nfc_close(void); 

bool api_nfc_init(void);
bool api_nfc_deinit(void);
void api_nfc_task(void* pa);
void api_nfc_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





