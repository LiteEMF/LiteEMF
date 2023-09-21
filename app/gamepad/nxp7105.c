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


#include "hw_config.h"
#ifdef PS_7105_ENCRYPT_ENABLED
#include "app/gamepad/nxp7105.h"
#include  "api/api_iic_host.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t m_reg_addr=0;		//写指令的地址,


/*****************************************************************************************************
**  Function
******************************************************************************************************/

//if busy return ture
bool is_nxp7105_busy(void)
{
	bool ret = true;
	nxp7105_stu_read_t nxp7105_stu;
	#ifdef HW_IIC_MAP
	if(api_iic_host_read(NXP7105_IIC_ID,0x90,0x07,&nxp7105_stu,sizeof(nxp7105_stu))){
		if(0x07 == nxp7105_stu.stu){
			ret = false;
		}
		logd("R07:%x,%x",nxp7105_stu.len,nxp7105_stu.stu);
	}
	#endif
	return ret;
}

bool nxp7105_read(uint8_t* buf, uint8_t len)
{
	bool ret = false;
	nxp7105_encrypt_read_t encrypt_read;

	if(is_nxp7105_busy()) return false;
	#ifdef HW_IIC_MAP
	ret = api_iic_host_read(NXP7105_IIC_ID,0x90,0x82,&encrypt_read,len+4);
	#endif

	if(ret && (encrypt_read.len >= 3)){
		m_reg_addr = encrypt_read.last_reg_add + 0x10 - 0X02;
		m_reg_addr &= ~0x80;
		if(NULL != buf) memcpy(buf,encrypt_read.buf,len);
		ret = true;
		//logd("R:");dumpd(&encrypt_read,len + 6);
	}

	return ret;
}


bool nxp7105_write(uint8_t cmd,uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret = false;
	nxp7105_write_t encrypt_write;

	if(is_nxp7105_busy()) return false;

	m_reg_addr &= ~0x80;

	encrypt_write.len = 0x05 + len;
	encrypt_write.res = 0x80;
	if(0x48 == cmd){
		encrypt_write.cmd = 0x48;
		encrypt_write.res2 = 0x00;
		encrypt_write.index = 0x00;
		encrypt_write.cmd_len = 0x00;
	}else{		//0x44 0x48 
		encrypt_write.cmd = cmd;
		encrypt_write.res2 = NXP7105_MTU;
		encrypt_write.index = index;
		encrypt_write.cmd_len = NXP7105_MTU;		//这里雷蛇的是按需要读写的数据发
	}

	if(len) memcpy(encrypt_write.buf,buf,len);
	ret = api_iic_host_write(NXP7105_IIC_ID,0x90,m_reg_addr,&encrypt_write,len + 6);
	

	if(ret){
		//logd("W:");dumpd(&encrypt_write,len + 6);
	}

	return ret;
}


bool nxp7105_write_encrypt(uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret = false;
	uint8_t cmd;
	if(0 == len){
		cmd = 0x48;
	}else{
		if( !nxp7105_read(NULL,0) ) return false;
		cmd = 0x44;
	}
	ret =  nxp7105_write(cmd,index,buf, len);
	return ret;
}


//写准备读取加密
bool nxp7105_read_encrypt(uint8_t index,uint8_t* buf, uint8_t len)
{
	bool ret;
	if(0 == index){
		ret = nxp7105_read(NULL,0);
	}else{
		ret = nxp7105_read(buf,len);
	}
	if(!ret) return ret;

	if(index <= 0x08){
		ret =  nxp7105_write(0x46,index,NULL, 0);
	}

	return ret;
}

/*******************************************************************
** Function: 没有必要注释的函数可以不注释函数说明
** Parameters:
** Returns:
*******************************************************************/
void nxp7105_init(void)
{
	#ifdef HW_IIC_MAP
	uint32_t buf[0x1f];
	api_iic_host_write(NXP7105_IIC_ID,0x90,0X0F,NULL,0);

	delay_ms(4);						//4ms
	api_iic_host_read(NXP7105_IIC_ID,0x90,0x1f,buf,2);

	delay_ms(4);						//4ms
	api_iic_host_read(NXP7105_IIC_ID,0x90,0x2f,buf,0x1f);
	logd("7105 init:");dumpd(buf,0x1f);

	delay_ms(4);						//4ms
	api_iic_host_read(NXP7105_IIC_ID,0x90,0xff,buf,2);
	#endif
	m_reg_addr = 0;

}
#endif
