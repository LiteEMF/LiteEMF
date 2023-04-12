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
#if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK) && API_NFC_ENABLE

#include "app/gamepad/switch_nfc.h"


#define DEBUG_LOG_ENABLE     	1
#include "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#define  NFC_STABLE_TIMES       4


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

extern void switch_set_emu(switch_enum_t* enump, uint8_t cmd,uint8_t sub_cmd,uint32_t pa);
/*****************************************************************************************************
**	static Function
******************************************************************************************************/
void switch_dump_nfc_data(switch_large_report_t* switchp,uint16_t len)
{
    static uint8_t s_crc8;
    switch_nfc_payload_t* nfc_repliesp = (switch_nfc_payload_t*)switchp->nfc; 
    switch_nfc_state_t *nfc_statep = (switch_nfc_state_t*)nfc_repliesp->payload;
    
    if(SWITCH_MCU_IDLE_ID != switchp->nfc[0]){
        if(s_crc8 != switchp->nfc_crc){
            s_crc8 = switchp->nfc_crc;

            logd("nfc cmd=%x,  crc=%x:",switchp->nfc[0],switchp->nfc_crc);
            if (0X07 == switchp->nfc[2]){				//有数据?
                dumpd(switchp->nfc,312);
            }else{
                dumpd(switchp->nfc,32);
            }
        }
	}
}


void switch_mcu_state_pack(switch_nfc_t *pnfc,switch_mcu_state_t*mcu_statep)
{
	mcu_statep->id = SWITCH_MCU_STATE_ID;
	mcu_statep->busy = (SWITCH_MCU_STATE_BUSY != pnfc->mcu_state)? 0x00:0xff;
	mcu_statep->major_version = SWAP16_L(0x03);
	mcu_statep->minor_version = SWAP16_L(0x05);
	mcu_statep->mcu_state = pnfc->mcu_state;
}

static void switch_nfc_state_pack(switch_nfc_t *pnfc,switch_nfc_payload_t* nfc_repliesp)
{
	switch_nfc_state_t* nfc_statep = (switch_nfc_state_t*)nfc_repliesp->payload;
	nfc_repliesp->id = SWITCH_MCU_NFC_STATE_ID; 
	nfc_repliesp->input_type = 5; //5: State info, 7: Ntag read data
	nfc_repliesp->last_fragment = 1;
	nfc_repliesp->payload_size = 0x131&0xff;
	nfc_repliesp->payload_size_h = 0x131>>8;
	nfc_statep->nfc_state = pnfc->nfc_state;
	
	if((SWITCH_NFC_POLLING_TAG == pnfc->nfc_state)
		|| (SWITCH_NFC_TAG_DETECTED == pnfc->nfc_state) 
		|| (SWITCH_NFC_READ_TAG == pnfc->nfc_state)
		|| (SWITCH_NFC_WRITE_TAG == pnfc->nfc_state)){
			
		if(NFC_GET_TAG_SN <= pnfc->nfc_ic_state){
			nfc_statep->has_tag_info = 1;       //0: No Tag info data, 1: Has Tag info data
			nfc_statep->has_tag_info2 = 1;
			nfc_statep->nfc_tag_ic = 2;         //2: Ntag, 4: Mifare.
			nfc_statep->nfc_type = 0;           //0: Type A, 1: Type B, 2: Type F, 6: ISO/IEC 15693 type
			nfc_statep->tag_info_size = 7;
			memcpy(nfc_statep->tag_info_data,m_nfc_sn,7); //crc=0x6f
		}
	}
}
static void switch_nfc_data_pack(switch_nfc_t *pnfc,switch_nfc_payload_t* nfc_repliesp)
{
	switch_nfc_read_head_t* nfc_read_headp = (switch_nfc_read_head_t*)nfc_repliesp->payload;
	if(0 == m_api_nfc_addr){
		nfc_repliesp->id = SWITCH_MCU_NFC_STATE_ID;			//这里忽略第一次发送3A
		nfc_repliesp->input_type = 7; //5: State info, 7: Ntag read data
		nfc_repliesp->fragment_id = 0x01;
		nfc_repliesp->last_fragment = 0;
		nfc_repliesp->payload_size = 0x131&0xff;
		nfc_repliesp->payload_size_h = 0x131>>8;
		nfc_read_headp->nfc_state = pnfc->nfc_state;
		nfc_read_headp->has_tag_info = 1;       //0: No Tag info data, 1: Has Tag info data
		nfc_read_headp->nfc_tag_ic = 2;         //2: Ntag, 4: Mifare.
		nfc_read_headp->nfc_type = 0;           //0: Type A, 1: Type B, 2: Type F, 6: ISO/IEC 15693 type
		nfc_read_headp->tag_uuid_size = 7;
		memcpy(nfc_read_headp->tag_uuid_data,m_nfc_sn,7);
		memcpy(nfc_read_headp->read_req_pa,"\x03\x00\x3B\x3C\x77\x78\x86\x00\x00",9);
		memcpy(nfc_read_headp->unknow,"\x07\x85\xEA\x56\xE8\xDB\xB7\xF4\xB1\x0A\x6B\x9A\x23\x1E\xA6\x44\xB1\x35\xCC\x5E\x42\xFC\x8A\x36\xD1\xD6\x01\x94\x39\x02\x07\x2C",32);
		memcpy(nfc_repliesp->payload+sizeof(switch_nfc_read_head_t), m_nfc_buf,245);
		m_api_nfc_addr = 245;	//total 540
	}else{
		nfc_repliesp->id = SWITCH_MCU_TAG_READ_ID; 
		nfc_repliesp->input_type = 7; //5: State info, 7: Ntag read data
		nfc_repliesp->fragment_id = 0x02;
		nfc_repliesp->last_fragment = 1;
		nfc_repliesp->payload_size = 0x127&0xff;	//295
		nfc_repliesp->payload_size_h = 0x127>>8;
		memcpy(nfc_repliesp->payload, m_nfc_buf + 245, 0x127);
		m_api_nfc_addr = 0;
	}
}
	
void switch_nfc_replies_pack(switch_nfc_t *pnfc, switch_large_report_t* switchp)
{
    switch_nfc_payload_t* nfc_repliesp = (switch_nfc_payload_t*)switchp->nfc;
    
    if(pnfc->mcu_resum){                                 //resume,需要做一些其他操作吗?
        pnfc->mcu_resum = false;
        switchp->nfc[0] = SWITCH_MCU_NONE_ID;
        return;
    }

    switch(pnfc->mcu_report_id){
    case SWITCH_MCU_NONE_ID:                                //第一次切换到0x31
        switch_nfc_init(pnfc);
        switchp->nfc[0] = SWITCH_MCU_NONE_ID;
        switchp->nfc[1] = 0x01;
        break;
    case SWITCH_MCU_IDLE_ID:                                //空闲模式
        switchp->nfc[0] = SWITCH_MCU_IDLE_ID;
        break;
    case SWITCH_MCU_STATE_ID:
		switch_mcu_state_pack(pnfc,(switch_mcu_state_t*)switchp->nfc);
        break;
    case SWITCH_MCU_NFC_STATE_ID:                           //填充0x2a payload数据
		switch_nfc_state_pack(pnfc,(switch_nfc_payload_t*)switchp->nfc);
        break;
    case SWITCH_MCU_TAG_READ_ID:                            //填充0x3a payload数据
		switch_nfc_data_pack(pnfc,(switch_nfc_payload_t*)switchp->nfc);	
        break;
    default:
        break;
    }

    pnfc->mcu_report_id = SWITCH_MCU_IDLE_ID;          //设置mcu report模式
}




void switch_nfc_req_process(switch_nfc_t *pnfc,uint8_t* buf,uint16_t len)
{
    switch_mcu_ctrl_sub_cmd_t mcu_request;              //mcu 请求指令
    nfc_req_cmd_t    nfc_request;                  //nfc 请求指令
    switch_ctrl_t* ctrlp=(switch_ctrl_t*)buf;

    mcu_request = ctrlp->sub_cmd;
    if((MCU_SUB_REQ_STATUS == mcu_request)){                	//request mcu 0x01
        pnfc->mcu_state++;
        pnfc->mcu_report_id = SWITCH_MCU_STATE_ID;         //设置mcu report模式

        if(pnfc->stable_num > NFC_STABLE_TIMES){
            if(SWITCH_MCU_STATE_BUSY == pnfc->mcu_state){
                pnfc->mcu_state = SWITCH_MCU_STATE_STAND_BY; //设置mcu状态
            }
        }
    }else if((MCU_SUB_REQ_NFC_DATA == mcu_request)){       		//request nfc 0x02
        switch_nfc_req_t* nfc_reqp = (switch_nfc_req_t*)ctrlp->cmd_data;
        nfc_request = nfc_reqp->nfc_cmd;
		pnfc->mcu_report_id = SWITCH_MCU_NFC_STATE_ID;
        switch(nfc_request){
        case SWITCH_NFC_CANCEL_CMD 			:   //0x00,
            break;
        case SWITCH_NFC_START_POLLING_CMD	:   //0X01,
            if(SWITCH_NFC_IDLE == pnfc->nfc_state){
                pnfc->nfc_state = SWITCH_NFC_POLLING_TAG;
                pnfc->stable_num = 0;
                api_nfc_start_polling();
            }
            break;
        case SWITCH_NFC_STOP_POLLING_CMD	:   //0X02,
            pnfc->nfc_state = SWITCH_NFC_IDLE;
            pnfc->stable_num = 0;
            api_nfc_stop_polling();
            break;
        case SWITCH_NFC_UNKNOWN_CMD			:   //0X03,
            break;
        case SWITCH_NFC_START_DISCOVERY_CMD	:   //0X04
        case SWITCH_NFC_START_DISCOVERY2_CMD:   //0X05,
            pnfc->stable_num++;

			if(SWITCH_NFC_READ_TAG == pnfc->nfc_state){
				logi("packet_ack=%d...\n",nfc_reqp->ack);
				if(NFC_NTAG_READ_FINLSH ==  pnfc->nfc_ic_state){
					pnfc->mcu_report_id = SWITCH_MCU_TAG_READ_ID;
				}
                if(2 == nfc_reqp->ack){							//ack 2 结数据上报
					api_nfc_start_finlsh();
					pnfc->mcu_report_id = SWITCH_MCU_NFC_STATE_ID;
					pnfc->nfc_state = SWITCH_NFC_IREAD_FINLISH;	
					pnfc->stable_num = 0;
				} 
			}

            if(pnfc->stable_num > NFC_STABLE_TIMES){
				if(SWITCH_NFC_IREAD_FINLISH == pnfc->nfc_state){		//自动回到idle
					pnfc->nfc_state = SWITCH_NFC_IDLE;	
					pnfc->stable_num = 0;
				}else if(SWITCH_NFC_INIT == pnfc->nfc_state){    	//自动回到idle
                    pnfc->nfc_state = SWITCH_NFC_IDLE;
                    pnfc->stable_num = 0;
                }
                if(SWITCH_NFC_POLLING_TAG == pnfc->nfc_state){ 		//识别检测到卡
                    if(NFC_GET_TAG_SN == pnfc->nfc_ic_state){
						pnfc->nfc_state = SWITCH_NFC_TAG_DETECTED;
					}
                }
            }    
            break;
        case SWITCH_NFC_NTAG_READ_CMD		:   //0X06,
            pnfc->nfc_state = SWITCH_NFC_READ_TAG;
			m_api_nfc_addr = 0;
			api_nfc_start_read();
            pnfc->stable_num = 0;
            break;
        case SWITCH_NFC_NTAG_WRITE_CMD		:   //0X08
            pnfc->nfc_state = SWITCH_NFC_WRITE_TAG;
			api_nfc_start_write();
            pnfc->stable_num = 0;
            logi("SWITCH_NFC_NTAG_WRITE_CMD doing...");
            break;
        case SWITCH_NFC_SEND_RAW_CMD		:   //0X09,
            pnfc->stable_num = 0;
            break;
        default:
            break;
        }
    }
}




bool switch_nfc_emun_process(switch_nfc_t *pnfc,switch_enum_t* enump, uint8_t* buf,uint16_t len)
{
    switch_large_report_t* large_inp = (switch_large_report_t*)buf;
	pnfc->mcu_report_id = large_inp->nfc[0];

    if(pnfc->mcu_report_id == SWITCH_MCU_NONE_ID){   	//第一次切换到0x31模式
		if(large_inp->nfc[1] == 0x01){
			pnfc->mcu_resum = true;                      	//为了框架简单调试
			switch_nfc_init(pnfc);
        	switch_set_emu(enump, SWITCH_CTRL_ID,SUB_SET_NFC_IR_MCU_STATE,0x01);
		}else{
			switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_STATUS,0);
		}
    }else if(SWITCH_MCU_STATE_ID == pnfc->mcu_report_id){    						//mcu request 0x01
		switch_mcu_state_t *mcu_statep = (switch_mcu_state_t*)large_inp->nfc;
		pnfc->mcu_state = mcu_statep->mcu_state;

		if(pnfc->stable_num >= NFC_STABLE_TIMES){
			if(SWITCH_MCU_STATE_STAND_BY == pnfc->mcu_state && 0 == mcu_statep->busy){//设置一次0x01,0x21,0x01,0x00,0x04	
				switch_set_emu(enump, SWITCH_CTRL_ID,SUB_SET_NFC_IR_MCU_CONFIGURATION,0x040021);
				pnfc->stable_num = 0;
			}else if(SWITCH_MCU_STATE_NFC == pnfc->mcu_state){
				switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_START_DISCOVERY_CMD);
				pnfc->stable_num = 0;
			}
		}

		if(0 == enump->cmd){          			//置默认读取状态												//默认读取mcu状态
			switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_STATUS,0);
			pnfc->stable_num++;
		}
	}else if(SWITCH_MCU_NFC_STATE_ID == pnfc->mcu_report_id){  			//nfc request 0x2a 
		switch_nfc_payload_t* nfc_repliesp = (switch_nfc_payload_t*)large_inp->nfc; 
		switch_nfc_state_t *nfc_statep = (switch_nfc_state_t*)nfc_repliesp->payload;
		
		if(0x07 != nfc_repliesp->input_type || 0x02 != nfc_repliesp->fragment_id){ //排除纯数据类型
			pnfc->nfc_state = nfc_statep->nfc_state;
		}
		if(pnfc->stable_num >= NFC_STABLE_TIMES){                 //稳定后才可以发送请求
			switch(pnfc->nfc_state){
			case SWITCH_NFC_IDLE:			    //0 等待命令
				if(NFC_NTAG_FINLSH != pnfc->nfc_ic_state){			//读取完成不再读取
					switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_START_POLLING_CMD);
					pnfc->stable_num = 0;
				}
				break;
			case SWITCH_NFC_POLLING_TAG:	    //1 标签轮询
				if(NFC_IDLE >= pnfc->nfc_ic_state){
					pnfc->nfc_ic_state = NFC_POLLING;
				}
				logi("pnfc->nfc_ic_state=%d\n",pnfc->nfc_ic_state);
				break;		
			case SWITCH_NFC_READ_TAG:		    //2 标签读
				if(0x07 == nfc_repliesp->input_type){	//收到数据,ack
					logi("nfc rec data complate...\n");
					pnfc->nfc_ic_state = NFC_NTAG_FINLSH;
					//switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,0x010000|SWITCH_NFC_START_DISCOVERY_CMD);	
					switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,0x020000|SWITCH_NFC_START_DISCOVERY_CMD);
				}
				break;
			case SWITCH_NFC_WRITE_TAG:		    //3 标签写
				break;
			case SWITCH_NFC_IREAD_FINLISH:	    //4 标签读取完成
				switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_STOP_POLLING_CMD);
				pnfc->stable_num = 0;
				break;
			case SWITCH_NFC_WRITE_FINLISH:	    //5 标签写入完成
				break;
			case SWITCH_NFC_BYPASS_SEND:	    //6 直通发送
				break;
			case SWITCH_NFC_ERROR:			    //7 错误（这会激活nfc_
				break;
			case SWITCH_NFC_DETECTED:		    //8 NFC 已停用
				break;
			case SWITCH_NFC_TAG_DETECTED:	    //9 检测到标签
				logi("pnfc->nfc_ic_state=%d\n",pnfc->nfc_ic_state);
				if(NFC_POLLING >= pnfc->nfc_ic_state){      //需要进行两次检测,第一次停止检测,应该可以忽略第一次,这里为了测试
					pnfc->nfc_ic_state = NFC_GET_TAG_SN;		//这里用<NFC_POLLING,防止太快检测到卡
					switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_STOP_POLLING_CMD);
				}else if(NFC_GET_TAG_SN == pnfc->nfc_ic_state){
					switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_NTAG_READ_CMD);
				}
				pnfc->stable_num = 0;
				break;
			case SWITCH_NFC_FACTORY_MODE:	    //A 出厂模式
				break;
			case SWITCH_NFC_INIT:			    //B 初始化
				break;
			case SWITCH_NFC_BYPASS_FINLISH:	    //C 通关完毕
				break;
			case SWITCH_NFC_REQUIRED_RESET:	    //D 需要重置
				break;
			case SWITCH_NFC_HW_RESET:		    //E 硬件RESET
				break;
			default:
				break;
			}
		}

		if(0 == enump->cmd){          			//置默认读取状态
			switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_START_DISCOVERY_CMD);
			pnfc->stable_num++;
		}
	}else if(SWITCH_MCU_TAG_READ_ID == pnfc->mcu_report_id){  					//nfc request 0x3a  
		switch_nfc_payload_t* nfc_repliesp = (switch_nfc_payload_t*)large_inp->nfc; 

		if(0 == enump->cmd){          			//置默认读取状态
			switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_NFC_DATA,SWITCH_NFC_START_DISCOVERY_CMD);
			pnfc->stable_num++;
		}
	}
    switch_dump_nfc_data(large_inp,len);
}


void switch_nfc_init(switch_nfc_t *pnfc)
{
    pnfc->nfc_ic_state = NFC_CLOSE;
	m_api_nfc_addr = 0;
    pnfc->mcu_resum = false;
    pnfc->mcu_report_id = SWITCH_MCU_NONE_ID;       //nfc MCU id
    pnfc->stable_num = 0;
    pnfc->mcu_state = SWITCH_MCU_STATE_BUSY;
    pnfc->nfc_state = SWITCH_NFC_INIT;
}

#endif






