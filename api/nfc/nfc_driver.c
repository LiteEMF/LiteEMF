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
#include  "nfc_driver.h"

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
** Description:	如果密码验证成功，返回2字节PACK+2字节的CRC，合计32bit
	如果验证成功，返回的pack和传进来的pack应该是一样的，否则出错
	ptPack设置NULL不检查pack
** Parameters:		
** Returns:			
*******************************************************************/
bool nfc_driver_auth(uint8_t *ptPwdData,uint8_t* ptPack)
{
	bool ret = false;

	#ifdef MS52X_NFC
	ret = !HXW_Ntag21x_PwdAuth(ptPwdData,ptPack);
	#endif

	return ret;
}

bool nfc_driver_polling(uint8_t *psn, uint8_t*psn_len, uint8_t *pversion, uint8_t*pversion_len)
{
	bool ret = false;

	#ifdef MS52X_NFC
	ret = !HXW_APP_Ntag21xGetVersion(psn,psn_len,pversion,pversion_len);
	#endif

	return ret;
}

bool nfc_driver_read(uint16_t addr, uint8_t *buf, uint16_t len)
{
	bool ret = false;

	uint8_t i,read_len=0;
	uint8_t read_buf[TAG_PAGE_SIZE];
	uint8_t addrhl[2] = {0,TAG_PAGE_SIZE/4 -1};

	#ifdef MS52X_NFC
	// api_nfc_auth(key,NULL);		//激活
    for(i=0;i<((len+TAG_PAGE_SIZE-1)/TAG_PAGE_SIZE);i++){
		ret = !HXW_APP_Ntag21xNoAuthFastRead(addrhl,buf, &read_len);			
		addrhl[0] += TAG_PAGE_SIZE/4;
		addrhl[1] += TAG_PAGE_SIZE/4;
		if(!ret){
			logd("read pag%d,len=%d err!\n",i,read_len);dumpd(read_buf,16);
			break;
		}
		if(len-i*TAG_PAGE_SIZE < read_len)  read_len = len-i*TAG_PAGE_SIZE; //防止溢出拷贝
		memcpy(&buf[i*TAG_PAGE_SIZE],read_buf, read_len);
    }
	#endif

	return ret;	
}

bool nfc_driver_write(uint16_t addr, uint8_t *buf, uint16_t len)
{
	bool ret = false;
	uint8_t i,write_len;
	uint8_t write_buf[TAG_PAGE_SIZE];

   
    for(i=0;i<((len+15)/TAG_PAGE_SIZE);i++){
		memset(write_buf,0,sizeof(write_buf));
		write_len = TAG_PAGE_SIZE;
		if(len-i*TAG_PAGE_SIZE < TAG_PAGE_SIZE)  write_len = len-i*TAG_PAGE_SIZE; //防止溢出拷贝
		memcpy(write_buf,&buf[i*TAG_PAGE_SIZE],write_len);
		
		// do write
		if(!ret){
			logd("write pag%d,len=%d err!\n",i,write_len);
			ret = false;
			break;
		}
    }

	return ret;
		
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool nfc_driver_init(void)
{
	#ifdef MS52X_NFC
	HXW_Ms52x_Init();
	#endif
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool nfc_driver_deinit(void)
{
	#ifdef MS52X_NFC
	HXW_CloseAntena();
	#endif
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void nfc_driver_task(void* pa)
{
	UNUSED_PARAMETER(pa);
}








