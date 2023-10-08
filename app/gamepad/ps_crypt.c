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
#if (defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED)
#include  "app/gamepad/ps_controller.h"
#include  "app/gamepad/ps_crypt.h"
#include  "api/api_tick.h"
#include "api/api_iic_host.h"

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

#ifdef PS_P2_ENCRYPT_ENABLED
bool p2_write_encrypt(uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret = false;
	uint16_t addr;
	uint8_t cmd_buf[17];
	if(0 == index){
		addr = 0x109;
		cmd_buf[0] = 0;
		memcpy(cmd_buf+1,&buf,len);
		len =  17;
	}else {
		if(15 == index){
			addr = 0;
		}else{
			addr = 0x01;
		}
		len =  16;
		memcpy(cmd_buf,buf,len);
	}
	#ifdef HW_IIC_MAP
	ret = api_iic_host_write(P2_IIC_ID,0x60,addr,cmd_buf,len);
	#endif
	return ret;
}
bool p2_read_encrypt(uint8_t index, uint8_t* buf, uint8_t len)
{
	bool ret = false;
	uint16_t addr;
	if(15 == index){
		addr = 0x82;
	}else{
		addr = 0x83;
	}
	#ifdef HW_IIC_MAP
	ret = api_iic_host_read(P2_IIC_ID,0x60,addr,buf,len);
	#endif
	return ret;
}
#endif



__WEAK bool os_ps_task_en(bool en)		//如果使用os,用于开始和停止任务
{
	return true;
}

bool ps_encrypt_start(uint8_t cmd_index)
{
	bool ret = true;

	ps_encrypt.step = PS_CHALLENGE;
	ps_encrypt.cmd_index = cmd_index;

	ret = os_ps_task_en(true);

	logd("ps_encrypt_start\n");
	return ret;
}

bool ps_encrypt_stop(void)
{
	bool ret = true;
	ps_encrypt.step = PS_IDLE;
	ret = os_ps_task_en(false);

	logd("ps_encrypt_stop\n");
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	p2 PS_CALCULATE_PHASE2过程运算需要占用大量时间
	建议创建低优先级任务进行处理
*******************************************************************/
void ps_encrypt_task(void *pa)
{
	uint16_t addr;

	//logd("p2 setp=%d\n",ps_encrypt.step);
	switch(ps_encrypt.step){
	case PS_CHALLENGE:

		ps_encrypt.len = 0;
		ps_encrypt.index = 0;

		#if defined PS_P2_ENCRYPT_ENABLED
		ps_encrypt.step = PS_WRITE_SECURITY;
		logd("set PS_WRITE_SECURITY\n");
		#elif defined PS_7105_ENCRYPT_ENABLED
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
			#if defined PS_P2_ENCRYPT_ENABLED
			if(p2_write_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],16)){
				ps_encrypt.len += 16;
				ps_encrypt.index++;
			}else{
				logd("iic write %d err!\n",ps_encrypt.index);
			}
			#elif defined PS_7105_ENCRYPT_ENABLED
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
			#if defined PS_P2_ENCRYPT_ENABLED
			if(p2_read_encrypt(ps_encrypt.index,&ps_encrypt_buf[ps_encrypt.len],16)){
				ps_encrypt.len += 16;
				ps_encrypt.index++;
			}else{
				logd("iic read %d err!\n",ps_encrypt.index);
			}
			#elif defined PS_7105_ENCRYPT_ENABLED
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
		#ifdef PS_P2_ENCRYPT_ENABLED
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


bool ps_encrypt_init(void)
{
	#if defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED
	ps_encrypt.step	= PS_IDLE;
	#endif

	#if defined PS_7105_ENCRYPT_ENABLED
	nxp7105_init();
	#endif

	return true;
}

#endif



