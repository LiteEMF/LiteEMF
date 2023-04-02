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


#ifndef _app_nfc_h
#define _app_nfc_h
#include "emf_typedef.h"
#include "nfc_driver.h"
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************************************************
**	Defined
********************************************************************************************************************/
#define TAG_SIZE_MAX		540		//NTAG215
#define TAG_PAGE_SIZE		(60)		//NTAG215 page, 根据不同芯片不一样

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
}app_nfc_state_t;

extern app_nfc_state_t  m_app_nfc_sta;
extern uint8_t m_app_nfc_addr;
extern uint8_t m_nfc_sn[8];
extern uint8_t m_nfc_buf[TAG_SIZE_MAX];

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_nfc_start_polling(void);
bool app_nfc_stop_polling(void);    
bool app_nfc_start_read(void);
bool app_nfc_start_write(void);
bool app_nfc_start_finlsh(void);
bool app_nfc_polling(void);
bool app_nfc_read(uint16_t address,uint8_t* nfc_buf, uint16_t len);
bool app_nfc_write(uint16_t address,uint8_t* nfc_buf, uint16_t len);
bool app_nfc_close(void); 

bool app_nfc_init(void);
bool app_nfc_deinit(void);
void app_nfc_handler(void);

#ifdef __cplusplus
}
#endif
#endif





