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
#include  "hw_config.h"
#if API_NFC_ENABLE
#include  "utils/emf_utils.h"
#include  "api/nfc/api_nfc.h"
#include  "api/nfc/nfc_driver.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
api_nfc_state_t  m_api_nfc_sta = NFC_CLOSE;
uint8_t m_api_nfc_addr;
uint8_t m_nfc_sn[8];
uint8_t m_nfc_buf[TAG_SIZE_MAX];

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
** Description:
** Parameters:		
** Returns:			
*******************************************************************/
__WEAK bool os_nfc_post_msg(uint32_t msg)		//如果使用os,并且需要发送消息通知任务开启时使用
{
	return true;
}


bool api_nfc_start_polling(void)     //任务可以被打断
{
	if(NFC_POLLING != m_api_nfc_sta){
		m_api_nfc_sta = NFC_POLLING;
		os_nfc_post_msg(m_api_nfc_sta);
	}
	
	return true;
}

bool api_nfc_stop_polling(void)     
{
	if(NFC_GET_TAG_SN < m_api_nfc_sta){
		m_api_nfc_sta = NFC_IDLE;
	}else{
		m_api_nfc_sta = NFC_GET_TAG_SN;
	}
	return true;
}

bool api_nfc_start_read(void)     
{
	if(NFC_NTAG_READ != m_api_nfc_sta){
		m_api_nfc_sta = NFC_NTAG_READ;
		os_nfc_post_msg(m_api_nfc_sta);
	}
	return true;
}

bool api_nfc_start_write(void)     
{
	if(NFC_NTAG_WRITE != m_api_nfc_sta){
		m_api_nfc_sta = NFC_NTAG_WRITE;
		os_nfc_post_msg(m_api_nfc_sta);
	}
	return true;
}

bool api_nfc_start_finlsh(void)     
{
	m_api_nfc_sta = NFC_NTAG_FINLSH;
	return true;
}


bool api_nfc_close(void)     
{
	if(NFC_CLOSE != m_api_nfc_sta){
		m_api_nfc_sta = NFC_CLOSE;
		// os_nfc_post_msg(m_api_nfc_sta);
	}
	return true;
}

/*******************************************************************
** Description:	如果密码验证成功，返回2字节PACK+2字节的CRC，合计32bit
	如果验证成功，返回的pack和传进来的pack应该是一样的，否则出错
	ptPack设置NULL不检查pack
** Parameters:		
** Returns:			
*******************************************************************/
bool api_nfc_auth(uint8_t *ptPwdData,uint8_t* ptPack)
{
	return nfc_driver_auth(ptPwdData,ptPack);
}

bool api_nfc_polling(void)
{
	bool ret = false;
	uint8_t sn[8],sn_len=8;
	uint8_t version[10],version_len=10;

	ret = nfc_driver_polling(sn,&sn_len,version,&version_len);

	if(ret){
		memcpy(m_nfc_sn,sn,8);
	}
	return ret;
}

bool api_nfc_read(uint16_t address,uint8_t* nfc_buf, uint16_t len)
{
	bool ret = false;
	// uint8_t key[]={0XDA,0XAC,0XA9,0X4D};	//SN:04 71 E2 01 1B 02 03 8C 	小丑卡
	uint8_t key[]={0XC8,0X2C,0XA8,0XE7};	//SN:04 63 C8 01 B1 03 03		猴子卡
    memset(nfc_buf,0,len);
	
	// api_nfc_auth(key,NULL);		//激活
	ret = nfc_driver_read(address,nfc_buf, len);
	api_nfc_deinit();

	if(ret){
		logd("nfc data %d crc=%x:",len,crc8(nfc_buf,len));dumpd(nfc_buf,len);
		logd("dm_lock:%x %x\n",nfc_buf[0x82*4],nfc_buf[0x82*4+1],nfc_buf[0x82*4+2]);
		logd("auth0:%x\n",nfc_buf[0x83*4+3]);
		logd("lock:%x %x\n",nfc_buf[10],nfc_buf[11]);
	}

	return ret;
}
bool api_nfc_write(uint16_t address,uint8_t* nfc_buf, uint16_t len)
{
	bool ret = false;

    ret = nfc_driver_write(address,nfc_buf,len);
	if(ret){
		logd("nfc write %d:",len);dumpd(nfc_buf,len);
	}
	api_nfc_deinit();

	return ret;
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_nfc_init(void)
{
	m_api_nfc_sta = NFC_CLOSE;
	m_api_nfc_addr = 0;
	return nfc_driver_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_nfc_deinit(void)
{
	nfc_driver_deinit();
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_nfc_task(void* pa)
{
	switch(m_api_nfc_sta){
	case NFC_IDLE:
		break;
	case NFC_POLLING:
		if(api_nfc_polling()){
			m_api_nfc_sta = NFC_GET_TAG_SN;
			logd("m_api_nfc_sta=%d\n",m_api_nfc_sta);
		}
		break;
	case NFC_NTAG_READ:
		if(api_nfc_read(0,m_nfc_buf, sizeof(m_nfc_buf))){
			m_api_nfc_sta = NFC_NTAG_READ_FINLSH;
		}
		break;
	case NFC_NTAG_WRITE:
		break;
	case NFC_CLOSE:
		/*关闭天线以及休眠卡*/
		api_nfc_deinit(); 
		m_api_nfc_sta = NFC_CLOSE;
		break;
	default:
		break;
	}
	UNUSED_PARAMETER(pa);
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_nfc_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		api_nfc_task(NULL);
	}
}
#endif


#endif






