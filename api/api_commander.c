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

/*******************************************************************
** Parameters:	phandle: 传输句柄,如果为NULL则输出到packetp中
				cmd: 指令CMD_ARG_EN_MASK表示是否指令带参数
				mtu: 最大包长度
				pack_index:当前包index, pack_num: 最大包数量
				buf: 剩余要发送的数据
				ppacket: 输出的包数据,只有handlep==NULL时有效
** Returns:		true/false
** Description: 指令帧打包
*******************************************************************/
static bool command_frame_pack(trp_handle_t* phandle, uint32_t cmd, uint8_t mtu, uint8_t *buf,uint16_t len,uint8_t *ppacket, uint16_t packet_len)
{
	bool ret = false;
	uint8_t data_mtu;
	uint8_t sub_cmd_len = ((cmd&CMD_ARG_EN_MASK)? 2:0);
	uint8_t pack_num,pack_index,pack_max_size;

	uint8_t send_data_len;	
	uint8_t cmd_len;
	uint8_t *cmd_buf;

	data_mtu = mtu - 5;
	pack_num = (len + sub_cmd_len + (data_mtu - 1) ) / data_mtu;		//data_mtu-1 是为了保存余数
	pack_max_size = len + sub_cmd_len + 5*pack_num;

	if(NULL == phandle){
		if(pack_max_size > packet_len){
			loge("command pack buf overflow except %d but %d\n",pack_max_size , packet_len);
			return false;
		}
	}

	cmd_buf = emf_malloc(mtu);
	if(NULL == cmd_buf){
		loge_r("command_frame_pack ERROR_NO_MEM!\n");
		return false;
	}
	
	for(pack_index = 0; pack_index < pack_num; pack_index++){
		ret = false;
		cmd_len = 0;

		if(0 == pack_index){
			cmd_buf[cmd_len++] = CMD_SHEAD;
			cmd_buf[cmd_len++] = 0;							//len 占位
			cmd_buf[cmd_len++] = pack_num;					//start packet
		}else{
			cmd_buf[cmd_len++] = CMD_HEAD;
			cmd_buf[cmd_len++] = 0;							//len 占位
			cmd_buf[cmd_len++] = pack_index;				//packet index
		}			
		
		cmd_buf[cmd_len++] = (uint8_t)cmd;					//cmd

		if(((cmd&CMD_ARG_EN_MASK)) && (0 == pack_index)){	//pack cmd arg
			cmd_buf[cmd_len++] = (uint8_t)(cmd >> (CMD_ARG_POS+8));
			cmd_buf[cmd_len++] = (uint8_t)(cmd >> CMD_ARG_POS);
			send_data_len = MIN(data_mtu-2, len);
		}else{
			send_data_len = MIN(data_mtu, len);
		}

		memcpy(&cmd_buf[cmd_len], buf, send_data_len);		//data
		cmd_len += send_data_len+1;
		cmd_buf[1] = cmd_len;								//len
		cmd_buf[cmd_len-1] = check_sum(cmd_buf,cmd_len-1);	//sum

		// logd("cmd:");dumpd(cmd_buf,cmd_len);
		if(NULL != phandle){
			ret = api_transport_tx(phandle, cmd_buf,cmd_len);
		}else if(NULL != ppacket){
			memcpy(ppacket, cmd_buf,cmd_len);
			ppacket += cmd_len;
			ret = true;
		}

		if(ret){
			buf += send_data_len;
			len -= send_data_len;
		}else{
			break;
		}
	}
	emf_free(cmd_buf);
	return ret;
}




/*******************************************************************
** Parameters:	pstream: 缓存接收到的指令帧
				mtu: 输入流mtu, 用于判断数据有效性, 如果忽略可以设置最大值0XFF
				c : 输入的数据包字节数据
** Returns:	
** Description:	从输入流中通过字节包解包单帧
		注意:pstream->buf 数据处理完后必须调用 free 释放内存!!!	
*******************************************************************/
static bool command_frame_unpack(command_stream_rx_t* pstream,uint8_t mtu,uint8_t c)
{	
	bool ret = false;

	if(0 == pstream->len){
		if((CMD_HEAD == c) || (CMD_SHEAD == c)){
			pstream->head = c;
			pstream->len = 1;
		}
	}else if(1 == pstream->len){
		if(c > mtu){
			if((CMD_HEAD == c) || (CMD_SHEAD == c)){
				pstream->frame[0] = c;
				pstream->len = 1;
			}else{
				loge_r("cmd len err %x\n",(uint16_t)pstream->frame[1]);
				pstream->len = 0;
			}
		}else{
			pstream->frame = emf_malloc(c);
			if(NULL != pstream->frame){
				pstream->frame[0] = pstream->head;
				pstream->frame[pstream->len++] = c;
			}else{
				pstream->len = 0;
				loge_r("command_frame_unpack ERROR_NO_MEM!\n");
			}
		}
	}else{
		pstream->frame[pstream->len++] = c;
		if(pstream->len >= pstream->frame[1]){
			uint8_t sum = check_sum(pstream->frame, pstream->len-1);
			if(pstream->frame[pstream->len-1] == sum){
				// logd("cmd in:");dumpd(pstream->frame, pstream->len);
				ret = true;
			}else{
				loge_r("cmd sum err except:%x but:%x\n",(uint16_t)pstream->frame[pstream->len-1],(uint16_t)sum);
				dumpe(pstream->frame,pstream->len);
				emf_free(pstream->frame);
				pstream->frame = NULL;
				pstream->len = 0;
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


/*******************************************************************
** Parameters:	
** Returns:	
** Description:	发送指令,可以带一个arg 用于嵌入式方便调用
*******************************************************************/
bool api_command_pack(uint8_t cmd, uint8_t mtu, uint8_t *buf,uint16_t len,uint8_t *ppacket, uint16_t packet_len)
{
	return command_frame_pack(NULL,cmd, mtu,buf,len,ppacket,packet_len);
}
bool api_command_arg_pack(uint8_t cmd, uint16_t arg, uint8_t mtu, uint8_t *buf,uint16_t len,uint8_t *ppacket, uint16_t packet_len)
{
	return command_frame_pack(NULL,cmd | CMD_ARG_EN_MASK | ((uint32_t)arg<<CMD_ARG_POS), mtu,buf,len,ppacket,packet_len);
}

bool api_command_tx(trp_handle_t* phandle,uint8_t cmd, uint8_t *buf,uint16_t len)
{
	uint8_t mtu = api_get_transport_mtu(phandle);
	return command_frame_pack(phandle,cmd,mtu,buf,len,NULL,0);
}
bool api_command_arg_tx(trp_handle_t* phandle,uint8_t cmd, uint16_t arg, uint8_t *buf,uint16_t len)
{
	uint8_t mtu = api_get_transport_mtu(phandle);
	return command_frame_pack(phandle,cmd | CMD_ARG_EN_MASK | ((uint32_t)arg<<16),mtu,buf,len,NULL,0);
} 


/*******************************************************************
** Parameters:	rxp:接收数据缓存, 注意rxp缓存buf在接收数据后malloc动态分配内存
				pcmd:输入指令包
** Returns:	
** Description:	从指令帧获取完整数据指令包
		注意:rxp数据处理完后必须调用 command_rx_free 释放内存!!!
*******************************************************************/
bool api_command_unpack(command_rx_t* rxp,uint8_t* pcmd,uint8_t len)
{
	uint8_t ret = false;
	if(len < pcmd[1]) return ret;									//保证数据完整
	if(pcmd[pcmd[1]-1] != (uint8_t)check_sum(pcmd,pcmd[1]-1))	return ret;		//可以忽略这一步方便调试

	if(CMD_SHEAD == pcmd[0]){										//start packet
		emf_free(rxp->buf);
		rxp->buf = NULL;

		rxp->packet_num = pcmd[2];
		rxp->packet_index = 0;
		rxp->max_len = rxp->packet_num * pcmd[1]; 					//pcmd[1]为单包接收数据长度,可以做为mtu
		rxp->buf = emf_malloc(rxp->max_len);			
		if(NULL != rxp->buf){
			rxp->buf[0] = pcmd[3];									//cmd
			rxp->len = 1;
		}else{
			rxp->max_len = 0;
			loge_r("api_command_unpack ERROR_NO_MEM!\n");
		}
	}

	if(NULL != rxp->buf){
		if((CMD_SHEAD == pcmd[0]) || ((rxp->buf[0] == pcmd[3]) && (rxp->packet_index == pcmd[2])) ){
			if(rxp->len + len-5 <= rxp->max_len){
				memcpy(rxp->buf+rxp->len,pcmd+4,len-5);
				rxp->len += len-5;
				rxp->packet_index++;
			}

			if(rxp->packet_num <= rxp->packet_index){				//end pack
				ret = true;
			}
		}else{
			loge_r("api_command_unpack ERROR_NO_MEM!\n");
		}
		
		// if(ret) {logd("cmd in %d:",rxp->len);dumpd(rxp->buf,rxp->len);}
	}else{
		loge_r("command_unpack ERROR_NO_MEM!\n");
	}
	return ret;
}


/*******************************************************************
** Parameters:	pstream:接收数据缓存, 注意rxp缓存buf在接收数据后malloc动态分配内存
				pfifo:输入fifo数据流
** Returns:	
** Description:	从输入流中通过字节包解包并获取完整数据指令包
	注意:pstream->rx数据处理完后必须调用 command_rx_free 释放内存!!!
*******************************************************************/
bool api_command_byte_unpack(command_stream_rx_t* pstream, uint8_t mtu, uint8_t c)
{	
	uint8_t ret = false;

	if(command_frame_unpack(pstream,mtu, c)){
		ret = api_command_unpack(&pstream->rx,pstream->frame,pstream->len);
		emf_free(pstream->frame);
		pstream->frame = NULL;
		pstream->len = 0;
	}

	return ret;
}


bool command_rx_free(command_rx_t *rxp)
{
	emf_free(rxp->buf);
	memset(rxp, 0, sizeof(command_rx_t));
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_command_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_command_deinit(void)
{
	return true;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_command_handler(uint32_t period_10us)
{
	UNUSED_PARAMETER(period_10us);
}






