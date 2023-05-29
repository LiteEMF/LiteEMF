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
#include  "api/api_commander.h"
#include  "utils/emf_utils.h"
#if API_SOFT_TIMER_ENABLE
#include "api/api_soft_timer.h"
#endif

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
static bool command_frame_tx(command_tx_t *txp)
{
	bool ret = false;
	uint8_t mtu,data_mtu;
	uint8_t pack_num;
	uint8_t sub_cmd_len = ((txp->cmd & CMD_ARG_EN_MASK)? 2:0);

	uint8_t send_data_len;	
	uint8_t index = 0;
	uint8_t *cmd_buf;

	if(txp->len <= txp->index) return ret;
	if(NULL == txp->buf) return ret;

	mtu = api_transport_get_mtu(txp->phandle);
	data_mtu = mtu - 5;
	
	cmd_buf = emf_malloc(mtu);
	if(NULL == cmd_buf){
		loge_r("command_frame_tx ERROR_NO_MEM!\n");
		return false;
	}

	if(txp->index){
		cmd_buf[index++] = CMD_HEAD;
		pack_num = (txp->len - txp->index + (data_mtu - 1) ) / data_mtu;		//data_mtu-1 是为了保存余数
	}else{
		cmd_buf[index++] = CMD_SHEAD;
		pack_num = (txp->len + sub_cmd_len + (data_mtu - 1) ) / data_mtu;		//data_mtu-1 是为了保存余数
	}
	cmd_buf[index++] = 0;								//len 占位
	cmd_buf[index++] = pack_num;						//residue packet number
	cmd_buf[index++] = (uint8_t)txp->cmd;				//cmd

	if(sub_cmd_len && (0 == txp->index)){				//pack cmd arg
		cmd_buf[index++] = (uint8_t)(txp->cmd >> (CMD_ARG_POS+8));
		cmd_buf[index++] = (uint8_t)(txp->cmd >> CMD_ARG_POS);
	}

	send_data_len = MIN(mtu - index - 1, txp->len - txp->index);
	memcpy(&cmd_buf[index], txp->buf+txp->index, send_data_len);	//data
	index += send_data_len+1;
	cmd_buf[1] = index;									//len
	cmd_buf[index-1] = check_sum(cmd_buf,index-1);		//sum
	// logd("cmd:");dumpd(cmd_buf,index);

	ret = api_transport_tx(txp->phandle, cmd_buf,index);
	if(ret){
		txp->index += send_data_len;
	}
	
	emf_free(cmd_buf);
	return ret;
}

/*******************************************************************
** Parameters:	pcmd: 缓存接收到的指令帧, 必须是静态变量,size = mtu
				mtu: 输入流mtu, 用于判断数据有效性, 如果忽略可以设置最大值0XFF
				c : 输入的数据包字节数据
** Returns:	
** Description:	从输入流中通过字节包解包单帧
*******************************************************************/
static bool command_frame_rx(uint8_t mtu,uint8_t c, uint8_t *s_buf, uint8_t *s_plen)
{	
	bool ret = false;

	if(0 == *s_plen){
		if((CMD_HEAD == c) || (CMD_SHEAD == c)){
			s_buf[0] = c;
			*s_plen = 1;
		}
	}else if(1 == *s_plen){
		if(c > mtu){
			if((CMD_HEAD == c) || (CMD_SHEAD == c)){
				s_buf[0] = c;
				*s_plen = 1;
			}else{
				loge_r("cmd len err %x\n",(uint16_t)c);
				*s_plen = 0;
			}
		}else{
			s_buf[(*s_plen)++] = c;
		}
	}else{
		s_buf[(*s_plen)++] = c;

		if(*s_plen >= s_buf[1]){
			uint8_t sum = check_sum(s_buf, *s_plen-1);
			if(s_buf[*s_plen - 1] == sum){
				// logd("cmd in:");dumpd(s_buf, *s_plen);
				ret = true;
			}else{
				loge_r("cmd sum err except:%x but:%x\n",(uint16_t)s_buf[*s_plen - 1],(uint16_t)sum);
				dumpe(s_buf,*s_plen);
				emf_free_and_clear(s_buf);
				*s_plen = 0;
			}
		}
	}

	return ret;
}
/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:
** Returns:	
** Description:	用于计算打包后的数据长度	
*******************************************************************/
uint16_t api_command_pack_size(uint8_t mtu,uint16_t len)
{
	uint8_t data_mtu;
	uint8_t pack_num;
	uint16_t packet_len;

	data_mtu = mtu - 5;
	pack_num = (len + (data_mtu - 1) ) / data_mtu;		//data_mtu-1 是为了保存余数
	packet_len = pack_num * 5 + len;

	return packet_len;
}

static void api_command_tx_fill(command_tx_t *txp, trp_handle_t* phandle,uint32_t cmd, uint8_t *buf,uint16_t len)
{
	txp->cmd = cmd;
	txp->phandle = phandle;
	txp->buf = buf;
	txp->len = len;
	txp->index = 0;
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description:	用于类似蓝牙大数据发送, 需要定时拆分发送数据包
*******************************************************************/
#if API_SOFT_TIMER_ENABLE
static void api_command_timer_cb(command_tx_t *txp)
{
	if(NULL != txp->buf){
		command_frame_tx(txp);

		if(NULL == txp->buf || txp->index == txp->len){
			emf_free_and_clear(txp->buf);
			soft_timer_stop((soft_timer_t*)txp->ptimer);
		}
	}

}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	注意: 使用到了软件定时器, 该接口不适用于51平台
*******************************************************************/
bool api_command_timer_tx(command_tx_t *txp, trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len, uint32_t ms)
{
	bool ret = false;
	soft_timer_t *tx_timer;
	uint8_t *p;
	uint8_t mtu = api_transport_get_mtu(phandle);

	if(NULL != txp->buf) {
		loge("txp buf is not NULL\n");
		return ret;
	}
	
	if(mtu >= len+5){									//短包直接发送
		ret = api_command_tx(phandle,cmd, buf,len);
	}else{
		p = emf_malloc(len);
		if(NULL == p) return ret;

		memcpy(p, buf, len);
		api_command_tx_fill(txp, phandle, cmd, p, len);

		tx_timer = soft_timer_create((timer_cb_t)&api_command_timer_cb,(void*)txp,ms,TIMER_PERIODIC);
		if(NULL != tx_timer){
			txp->ptimer = tx_timer;
			ret = !soft_timer_start(tx_timer);
		}else{
			emf_free_and_clear(txp->buf);
		}
	}
	
	return ret;
}
#endif

/*******************************************************************
** Parameters:	
** Returns:	
** Description:	发送指令,可以带一个arg 用于嵌入式方便调用
*******************************************************************/
bool api_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len)
{
	bool ret = false;
	command_tx_t tx;
	uint8_t mtu = api_transport_get_mtu(phandle);

	api_command_tx_fill(&tx, phandle, cmd, buf, len);
	
	while(tx.index < tx.len){
		ret = command_frame_tx(&tx);
		if(!ret) break;
	}
	return ret;
}
bool api_command_arg_tx(trp_handle_t* phandle,uint8_t cmd, uint16_t arg, uint8_t *buf,uint16_t len)
{
	bool ret = false;
	command_tx_t tx;
	uint8_t mtu = api_transport_get_mtu(phandle);

	api_command_tx_fill(&tx, phandle, CMD_ARG_EN_MASK | ((uint32_t)arg<<16) | cmd, buf, len);
	
	while(tx.index < tx.len){
		ret = command_frame_tx(&tx);
		if(!ret) break;
	}
	return ret;
}



bool command_rx_free(command_rx_t *rxp)
{
	emf_free(rxp->pcmd);
	memset(rxp, 0, sizeof(command_rx_t));
	return true;
}

/*******************************************************************
** Parameters:	rxp:接收数据缓存, 注意rxp必须初始化防止野指针
				buf:输入指令包
** Returns:	
** Description:	从指令帧获取完整数据指令包
		注意:rxp数据处理完后必须调用 command_rx_free 释放内存!!!
*******************************************************************/
bool api_command_rx(command_rx_t* rxp,uint8_t* buf,uint8_t len)
{
	uint8_t ret = false;
	uint16_t cmd_max_len;
	command_head_t *phead = (command_head_t*)buf;

	if(len < buf[1]) return ret;								//保证数据完整
	if(buf[buf[1]-1] != (uint8_t)check_sum(buf,buf[1]-1)){		//可以忽略这一步方便调试
		return ret;		
	}
	
	if(CMD_SHEAD == buf[0]){									//start packet
		command_rx_free(rxp);									//防止数据未释放

		cmd_max_len = (phead->pack_index) * phead->len;		//phead->len为单包接收数据长度,可以做为mtu
		rxp->pcmd = emf_malloc(cmd_max_len);			
		if(NULL != rxp->pcmd){
			rxp->pcmd[0] = phead->cmd;							//cmd
			memcpy(rxp->pcmd, buf, COM_HEAD_LEN);
			rxp->len = COM_HEAD_LEN;
		}else{
			loge_r("command_rx ERROR_NO_MEM!\n");
		}
	}

	if(NULL != rxp->pcmd){
		if( (rxp->pcmd[3] == phead->cmd) 
			&& (rxp->pcmd[2] == phead->pack_index)
			&& (rxp->pcmd[1] >= phead->len) ){			//MTU保证buf不溢出

			memcpy(rxp->pcmd+rxp->len, buf+4, len-5);
			rxp->len += len-5;
			rxp->pcmd[2]--;
			
			if(1 == phead->pack_index){
				rxp->pcmd[rxp->len++] = 0;	//add sum len		
				ret = true;
			}
		}else{
			loge_r("command_rx cmd err!\n");
		}
		// if(ret) {logd("cmd in %d:",rxp->len);dumpd(rxp->pcmd,rxp->len);}
	}else{
		loge_r("command_rx buff err!\n");
	}
	return ret;
}


/*******************************************************************
** Parameters:	rxp:接收数据缓存, 注意rxp缓存buf在接收数据后malloc动态分配内存
				mtu:用于限制数据,防止内存溢出
				c: 输入的字节
				s_buf,s_plen : 零时缓存指令, 必须初始化 s_plen = 0
** Returns:	
** Description:	从输入流中通过字节包解包并获取完整数据指令包
	注意:rx数据处理完后必须调用 command_rx_free 释放内存!!!
*******************************************************************/
bool api_command_rx_byte(command_rx_t *rxp, uint8_t mtu, uint8_t c, uint8_t *s_buf, uint8_t *s_plen)
{	
	uint8_t ret = false;

	if(command_frame_rx(mtu, c,s_buf,s_plen)){
		ret = api_command_rx(rxp,s_buf,*s_plen);
		*s_plen = 0;
	}

	return ret;
}






