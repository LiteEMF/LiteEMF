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
#if (HIDD_SUPPORT & HID_XBOX_MASK)
#include  "xbox_controller.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"
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
/*
回复ACK指令
cmd: 需要回复的指令
index: 当前通讯的index
recive_len: 已经接收到的实际数据长度
total_len: 一共需要接收的数据长度
*/
bool xbox_ack(trp_handle_t *phandle,uint8_t cmd,xbox_ctrl_t bctrl, uint8_t index, uint8_t recive_len,uint8_t total_len)
{
	bool ret = false;
	xbox_ack_t pack;
	memset(&pack,0,sizeof(pack));

	pack.cmd = XBOX_ACK_CMD;
	pack.bctrl = XBOX_CTRL_HSH;
	if(bctrl & XBOX_CTRL_UAC){
		pack.bctrl |= XBOX_CTRL_UAC;
	}
	pack.index = index;
	pack.len = 0x09;

	pack.ack_cmd = cmd;
	pack.ack_bctrl = pack.bctrl;
	pack.rec_len = SWAP16_L(recive_len);
	pack.residue_len = SWAP16_L(total_len - recive_len);
	ret = api_transport_tx(phandle,(uint8_t*)&pack,pack.len+4);
	return ret;
}

/*
回复ACK指令
cmdp: 需要回复的指令
*/
bool xbox_cmd_ack(trp_handle_t *phandle,xbox_command_t* rx_cmdp)
{
	return xbox_ack(phandle,rx_cmdp->cmd,rx_cmdp->bctrl,rx_cmdp->index,rx_cmdp->offset,rx_cmdp->len);
}


bool xbox_command_fill(trp_handle_t* phandle,xbox_command_t*cmdp,xbox_cmd_index_t *pindex, uint8_t uac_en, uint8_t cmd,uint8_t* buf,uint8_t len)
{
	bool ret = false;
	uint8_t index;

	if((XBOX_KEY_REP_CMD != cmd) && (XBOX_RUMBLE_CMD != cmd)){
    	cmdp->bctrl = XBOX_CTRL_HSH;
	}
    if(len > 0x3A){										//长包发送
		cmdp->bctrl |= XBOX_CTRL_LPACKET;
	}
	if(XBOX_SECURITY_CMD == cmd){
		index = ++pindex->security_index;
	}else if(uac_en && ((XBOX_STATUS_CMD != cmd) || (0 == buf[0])) ){	//XBOX_STATUS_CMD特殊处理
		index = ++pindex->uac_index;
		cmdp->bctrl |= XBOX_CTRL_UAC;
	}else{
		index = ++pindex->cmd_index;
	}

	cmdp->cmd = cmd;
	cmdp->index = index;
	cmdp->retry = 3;
	cmdp->offset = 0;
	cmdp->len = len;
	cmdp->trp_handle = *phandle;
	cmdp->timer = m_systick-8;			//立即发送

	emf_free_and_clear(cmdp->pbuf);		//防止指令异常
	cmdp->pbuf = emf_malloc(len);
	if(cmdp->pbuf != NULL){
		memcpy(cmdp->pbuf, buf,len);
		ret = true;
	}
	return ret;
}


//return: 当前包发送成功(如果长包要多次发送)
bool xbox_command_send(xbox_command_t*tx_cmdp)
{
	bool ret = false;
	xbox_usb_pack_t pack;
	uint16_t lpack_len;

	if(NULL == tx_cmdp->pbuf) return ret;

	memset(&pack,0,sizeof(pack));
	pack.cmd = tx_cmdp->cmd;
	pack.index = tx_cmdp->index;
	pack.len = MIN(0X3A, tx_cmdp->len - tx_cmdp->offset);

	pack.bctrl = tx_cmdp->bctrl & ~XBOX_CTRL_HEAD & ~XBOX_CTRL_NEED_ACK;
	if((XBOX_SECURITY_CMD == tx_cmdp->cmd) || (XBOX_SERIAL_CMD == tx_cmdp->cmd)){		//这两个指令必须ACK
		pack.bctrl |= XBOX_CTRL_NEED_ACK;
	}
	if(tx_cmdp->len <= 0x3A){									//短包发送	
		memcpy(pack.p.buf, tx_cmdp->pbuf,pack.len);	
		tx_cmdp->cmd = XBOX_NONE_CMD;							//短包指令直接发送完成
		emf_free_and_clear(tx_cmdp->pbuf);
		ret = true;
	}else{														//长包发送, 长包发送完成是在接收ACK中清除发送busy状态的
		pack.bctrl |= XBOX_CTRL_LPACKET;
		if(0 == tx_cmdp->offset){
			pack.bctrl |= XBOX_CTRL_NEED_ACK | XBOX_CTRL_HEAD;
		}else if (tx_cmdp->offset >= 0x3A*5){						//每4个包需要一次ACK
			pack.bctrl |= XBOX_CTRL_NEED_ACK;
		}else if ((pack.len < 0x3A) && (0 != pack.len)){		//最后一个包有效数据需要ACK
			pack.bctrl |= XBOX_CTRL_NEED_ACK;
		}														//注意长包需要发送一次剩余长度为0的空包

		if(0 == tx_cmdp->offset){									//发送开始包,发的是数据总长度
			lpack_len = tx_cmdp->len;
		}else if(tx_cmdp->offset){									//已发送的数据,包括结束包	
			lpack_len = tx_cmdp->offset;
		}
		if(lpack_len < 0x80){									//长包数据长度的格式
			pack.flag = 1;
			pack.p.lpack.len0 = 0;
			pack.p.lpack.len1 = lpack_len;
		}else{
			pack.flag = 0;
			pack.p.lpack.len0 = (lpack_len & 0x7f) | 0x80;
			pack.p.lpack.len1 = lpack_len / 0x80;
		}
		memcpy(pack.p.lpack.buf, tx_cmdp->pbuf + tx_cmdp->offset, pack.len);
	}

	if(pack.bctrl){
		tx_cmdp->timer = m_systick;
		if(pack.bctrl & XBOX_CTRL_LPACKET){
			ret = api_transport_tx(&tx_cmdp->trp_handle,(uint8_t*)&pack,pack.len+6);
		}else{
			ret = api_transport_tx(&tx_cmdp->trp_handle,(uint8_t*)&pack,pack.len+4);
		}
		if(ret){
			tx_cmdp->offset += MIN(pack.len, 0X3A);						//第一包数据发的是总长度
			tx_cmdp->bctrl = pack.bctrl;
		}
	}
	return ret;
}




bool xbox_get_ack(trp_handle_t *phandle, xbox_command_t *tx_cmdp, uint8_t* buf,uint8_t len)
{
	if(XBOX_ACK_CMD == buf[0]){							//发送收到的ack包
		xbox_ack_t* ackp = (xbox_ack_t*)buf;
		if(ackp->ack_cmd == tx_cmdp->cmd){
			if(tx_cmdp->bctrl & XBOX_CTRL_LPACKET){		//ack后长包继续发送,短包不发送!!
				xbox_command_send(tx_cmdp);
				if(0 == ackp->residue_len){
					tx_cmdp->cmd = XBOX_NONE_CMD;
					emf_free_and_clear(tx_cmdp->pbuf);
				}
			}
		}
	}
}
/*******************************************************************************
* 描述    : 主要用于握手阶段手柄从机处理主机发过来的数据
* 输入    : buf  指向输出数据   len 数据长度
* 输出    : cmdp 返回获取到的指令
* 返回值  : 是否成功
*******************************************************************************/
bool xbox_get_cmd(trp_handle_t *phandle, xbox_command_t *rx_cmdp, uint8_t* buf,uint8_t len)
{
	bool ret = false;
	xbox_usb_pack_t* p = (xbox_usb_pack_t*)buf;


	if(4 > len) return false;
	
	if(p->bctrl & XBOX_CTRL_LPACKET){				//长包
		uint16_t pack_len;
		if(0 == p->p.lpack.len0){					//解析包长度
			pack_len = p->p.lpack.len1;
		}else{
			pack_len = (p->p.lpack.len0 & 0x7f) + p->p.lpack.len1*0x80;
		}

		if(p->bctrl & XBOX_CTRL_HEAD){											//是第一包
			rx_cmdp->cmd = p->cmd;
			rx_cmdp->bctrl = p->bctrl;
			rx_cmdp->index = p->index;
			rx_cmdp->retry = 0;
			rx_cmdp->offset = p->len;
			rx_cmdp->len = pack_len;
			rx_cmdp->trp_handle = *phandle;

			emf_free_and_clear(rx_cmdp->pbuf);									//防止指令异常
			rx_cmdp->pbuf = emf_malloc(rx_cmdp->len);
			if(NULL != rx_cmdp->pbuf){
				memcpy(rx_cmdp->pbuf, p->p.lpack.buf, p->len);
			}
		}else if((rx_cmdp->cmd == p->cmd) && (rx_cmdp->index == p->index) ){	//接收完整数据包	
			if(rx_cmdp->offset == pack_len){									//匹配包序号
				rx_cmdp->bctrl = p->bctrl;
				if(NULL != rx_cmdp->pbuf && rx_cmdp->len >= p->len + rx_cmdp->offset){
					memcpy(rx_cmdp->pbuf + rx_cmdp->offset, p->p.lpack.buf, p->len);
				}
				rx_cmdp->offset += p->len;

				if((rx_cmdp->offset == rx_cmdp->len) && (0==p->len)){			//接收完成
					ret = true;
				}
			}else{
				emf_free_and_clear(rx_cmdp->pbuf);
				loge("xbox get index err:%x but in %x\n",pack_len,rx_cmdp->offset);
			}
		}else{
			emf_free_and_clear(rx_cmdp->pbuf);
			loge("xbox get cmd err:%x but in %x\n",p->cmd, rx_cmdp->cmd);
		}
	}else{										//短包
		rx_cmdp->cmd = p->cmd;
		rx_cmdp->bctrl = p->bctrl;
		rx_cmdp->index = p->index;
		rx_cmdp->retry = 0;
		rx_cmdp->offset = p->len;
		rx_cmdp->len = p->len;
		rx_cmdp->trp_handle = *phandle;
		rx_cmdp->pbuf = emf_malloc(rx_cmdp->len);
		if(NULL != rx_cmdp->pbuf){
			memcpy(rx_cmdp->pbuf,p->p.buf,p->len);
		}
		ret = true;
	}

	#if !USBD_SOCKET_ENABLE && !USBH_SOCKET_ENABLE
	if( p->bctrl & XBOX_CTRL_NEED_ACK ){		//回复接收ACK
		xbox_cmd_ack(phandle,rx_cmdp);
	}
	#endif
	
	return ret;
}

bool xbox_command_free(xbox_command_t* cmdp)
{
	emf_free_and_clear(cmdp->pbuf);
}

void xbox_command_tx_process(xbox_command_t* tx_cmdp)	//发送 handler
{
	if(tx_cmdp->bctrl & XBOX_CTRL_NEED_ACK){	//需要ACK
		if(m_systick - tx_cmdp->timer >= 8*8){	//超时未接受到ack重新开始, 如果接收到ACK在ack中处理
			tx_cmdp->timer = m_systick;
			if(tx_cmdp->retry){
				tx_cmdp->retry--;
				tx_cmdp->offset = 0;
				xbox_command_send(tx_cmdp);
			}else{
				tx_cmdp->cmd = XBOX_NONE_CMD;		//发送失败
				emf_free_and_clear(tx_cmdp->pbuf);
			}
		}
	}else{											//不需要ACK 直接发送
		if(m_systick - tx_cmdp->timer >= 8){
			tx_cmdp->timer = m_systick;
			xbox_command_send(tx_cmdp);
		}
	}
}


#endif






