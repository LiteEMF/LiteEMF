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
#if (HIDD_SUPPORT & HID_PS_MASK) && (PS_P2_ENCRYPT_ENABLED || PS_7105_ENCRYPT_ENABLED)
#include  "ps_controller.h"
#include  "api/api_tick.h"
#include "api/api_iic.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
ps_encrypt_t	ps_encrypt;
uint8_t ps_encrypt_buf[1040];

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**	static Function
******************************************************************************************************/




/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if PS_P2_ENCRYPT_ENABLED
bool p2_write_encrypt(uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret;
	uint16_t addr;
	uint8_t buf[17];
	if(0 == index){
		addr = 0x109;
		buf[0] = 0;
		memcpy(buf+1,&buf,len);
		len =  17;
	}else {
		if(15 == index){
			addr = 0;
		}else{
			addr = 0x01;
		}
		len =  16;
		memcpy(buf,buf,len);
	}
	ret = hw_iic_write(0x60,addr,buf,len);
	return ret;
}
bool p2_read_encrypt(uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret;
	uint16_t addr;
	if(15 == index){
		addr = 0x82;
	}else{
		addr = 0x83;
	}
	ret = hw_iic_read(0x60,addr,buf,len);
	return ret;
}
#endif


__WEAK bool os_ps_post_msg(uint32_t msg)		//如果使用os,并且需要发送消息通知任务开启时使用
{
	return true;
}

bool ps_encrypt_start(uint8_t cmd_index)
{
	bool ret = true;
	//hw_iic_scan();
	ps_encrypt.step = PS_CHALLENGE;
	ps_encrypt.cmd_index = cmd_index;

	ret = os_ps_post_msg(PS_CHALLENGE);

	logd("PS_CHALLENGE_start=%d\n",ps_encrypt.step);
	return ret;
}

void ps_encrypt_task(void *pa)
{
	uint16_t addr;

	//logd("p2 setp=%d\n",ps_encrypt.step);
	switch(ps_encrypt.step){
	case PS_CHALLENGE:

		ps_encrypt.len = 0;
		ps_encrypt.index = 0;

		#if PS_P2_ENCRYPT_ENABLED
		ps_encrypt.step = PS_WRITE_SECURITY;
		logd("set PS_WRITE_SECURITY\n");
		#elif PS_7105_ENCRYPT_ENABLED
		if(nxp7105_write_encrypt(ps_encrypt.index,NULL,0)){
			ps_encrypt.step = PS_WRITE_SECURITY;
			logd("set PS_WRITE_SECURITY\n");
		}
		#endif
		break;
	case PS_WRITE_SECURITY:
		if(ps_encrypt.len >= 256){
			ps_encrypt.step = PS_READ_SECURITY;
			ps_encrypt.index = 0;
			ps_encrypt.len = 0;
			logd("set PS_READ_SECURITY\n");
		}else{
			#if PS_P2_ENCRYPT_ENABLED
			if(p2_write_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],16)){
				ps_encrypt.len += 16;
				ps_encrypt.index++;
			}else{
				logd("iic write %d err!\n",ps_encrypt.index);
			}
			#elif PS_7105_ENCRYPT_ENABLED
			if(nxp7105_write_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],NXP7105_MTU)){
				ps_encrypt.len += NXP7105_MTU;
				ps_encrypt.index++;
			}
			#endif
		}
		break;
	case PS_READ_SECURITY:
		if(ps_encrypt.len >= PS_READ_SECURITY_LEN){
			ps_encrypt.step = PS_CALCULATE_PHASE2;
			ps_encrypt.index = 0;
			ps_encrypt.len = 0;
			logd("set PS_CALCULATE_PHASE2\n");
		}else{
			#if PS_P2_ENCRYPT_ENABLED
			if(p2_read_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],16)){
				ps_encrypt.len += 16;
				ps_encrypt.index++;
			}else{
				logd("iic read %d err!\n",ps_encrypt.index);
			}
			#elif PS_7105_ENCRYPT_ENABLED
			uint8_t len=0;
			if(ps_encrypt.index){		//第一条指令是获取地址不读取数据
				len = MIN(NXP7105_MTU, PS_READ_SECURITY_LEN - ps_encrypt.len);
			}
			if(nxp7105_read_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],len)){
				ps_encrypt.len += len;
				ps_encrypt.index++;
			}
			#endif
		}
		break;
	case PS_CALCULATE_PHASE2:
	{
		#if PS_P2_ENCRYPT_ENABLED
		extern int p2_test(void);
		timer_t p2_time = m_systick;
		logd("p2 start...");
		//int ret = p2_test();
		p2_doit(ps_encrypt_buf,ps_encrypt_buf);
		logd("p2 t=%d...\n",m_systick-p2_time);
		#endif

		ps_encrypt.step = PS_ANSWER;
		ps_encrypt.index = 0;
		logd("set PS_ANSWER\n");
		break;
	}
	default:
		break;
	}
}


#endif



