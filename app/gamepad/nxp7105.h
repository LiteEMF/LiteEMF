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

#ifndef	_nxp7105_H
#define	_nxp7105_H
#include "hw_config.h" 
#include "emf_typedef.h" 

/*******************************************************************************************************************
**	Hardware  Defined	一级定义
********************************************************************************************************************/
#define NXP7105_MTU		0x80		
#ifndef NXP7105_IIC_ID
#define NXP7105_IIC_ID	0
#endif	
//*********************************************************************************//
//                              二级定义				                           //
//*********************************************************************************//
//读取加密步骤:
//发读取指令更新地址
//写入cmd=0x46,index,cmd_len=80,指令设置读取地址
//发读取指令读取数据,同时更新地址
//如此循环,最后一个读取不用发写

//写加密步骤:
//先写cmd=0X48cmd 开始写(注意上电的时候不能读)
//发读取指令更新地址
//写cmd=0X44,index=0,cmd_len=80,data
//发读取指令更新地址
//写cmd=0X44,index=1,cmd_len=80,data

/*注意:可以多次读取, 不能多次写入否则会有问题, 1.7105的IIC读取要必须延时280us*/

//发送任何命令前都要读取07判断是否空闲
typedef struct {			//add: 0x07
	uint8_t len;
	uint8_t stu;			//0x07 idle, 0x17:busy
}nxp7105_stu_read_t;


typedef struct {			//reg: 0x82
	uint8_t len;			//不包含自己的长度
	uint8_t last_reg_add;	//reg_add|0x02 上一次炒作的reg_add
	uint8_t buf[128];		//最大128
	uint8_t end_l;			//小端,固定是0x0009 
	uint8_t end_h;			//小端,固定是0x0009 
}nxp7105_encrypt_read_t;




typedef struct {			//reg_add: 0x00 0x10 0x20 0x30 0x40 0x50 0x60 0x70,每次指令+0x10如此循环
	uint8_t len;			//不包含自己的长度
	uint8_t res;			// 默认0x80
	uint8_t cmd;			//0x48:开始,  0x44:写加密, 0x46:读加密
	uint8_t res2;			// 默认0x80
	uint8_t index;			//from 0,	cmd48:默认00,  cmd44/cmd46:默认80|index,

	uint8_t cmd_len;		//buf长度, cmd48:默认00,	cmd44:写加密长度,cmd48:读加密长度 
	uint8_t buf[128];		//如果有的话最大128
}nxp7105_write_t;

typedef struct {			//reg_add: 0x00 0x10 0x20 0x30 0x40 0x50 0x60 0x70,每次指令+0x10如此循环
	uint8_t len;			//不包含自己的长度
	uint8_t res;			// 默认0x80
	uint8_t cmd;			//0x48:开始,  0x44:写加密, 0x46:读加密
	uint8_t res2;			// 默认0x80
	uint8_t index;			//from 0,	cmd48:默认00,  cmd44/cmd46:默认80|index,

	uint8_t cmd_len;		//buf长度, cmd48:默认00,	cmd44:写加密长度, cmd48:读加密长度 
}nxp7105_write_head_t;
/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/







/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/
bool nxp7105_write_encrypt(uint8_t index, uint8_t* buf, uint8_t len);
bool nxp7105_read_encrypt(uint8_t index,uint8_t* buf, uint8_t len);
void nxp7105_init(void);



#endif

