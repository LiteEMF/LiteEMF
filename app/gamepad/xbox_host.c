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
#if (HIDH_SUPPORT & HID_XBOX_MASK)
#include  "xbox_controller.h"
#include  "gamepad_controller.h"		//TODO
#include  "api/api_tick.h"

#if API_USBH_BIT_ENABLE && (USBH_HID_SUPPORT & HID_XBOX_MASK)
#include  "api/usb/host/usbh.h"
#endif

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/
#if (HIDH_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
uint8c_t xbox_start0[]={0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0x53};
uint8c_t xbox_start1[]={0x00};
uint8c_t xbox_set_led[]={0x00,0x01,0x14};
uint8c_t xbox_set_rumble[]={0x00,0x0F,0x00,0x00,0x00,0x00,0xFF,0x00,0x00};
#endif


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint8_t m_xbox_enum_step = 0;
xbox_info_t m_xbox_host_info;
uint8_t m_x360_identify_itf = 0;		//x360 认证接口号

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint32_t xbox_host_key;

xbox_command_t xbox_host_cmd_rx;
xbox_cmd_index_t xbox_host_index;
bool xbox_host_uac_en;				//是否开启音频
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

static bool xboxs_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
    static uint8_t rumble_index=0;
	uint8c_t MIN_DUTY = 3;
	xbox_rumble_t rumble;
    rumble_t rumble_data;
    uint8_t i, rumble1, rumble2, small_l, small_r;
    memcpy(&rumble_data, prumble, sizeof(rumble_t));

    for(i = 0;i<2;  i++){
        /* 解决SWITCH的马达数据remap时候丢失震动的问题, remap(3,0,255,0,100) = 1, remap(2,0,255,0,100) = 0*/
        if((rumble_data.duty[i] > 0) && (rumble_data.duty[i] < MIN_DUTY)){
            rumble_data.duty[i] = MIN_DUTY;
        }
    }

	rumble1 = remap(rumble_data.duty[RUMBLE_L],0,255,0,100);
	rumble2 = remap(rumble_data.duty[RUMBLE_R],0,255,0,100);
	small_l = remap(rumble_data.duty[RUMBLE_SL],0,255,0,100);
	small_r = remap(rumble_data.duty[RUMBLE_SR],0,255,0,100);
	rumble.bt_cmd = 0;
	rumble.rumble_mask = 0x0f;
	rumble.rumble1 = rumble1;
	rumble.rumble2 = rumble2;
	rumble.srumble1 = small_l;
	rumble.srumble2 = small_r;
	rumble.duration = 0xff;
	rumble.res2[1] = 0xeb;
	
	if(api_trp_is_usb(phandle->trp)){
		xbox_usb_pack_t usb_rumble;
		memset(&usb_rumble, 0, sizeof(usb_rumble));
		usb_rumble.cmd = XBOX_RUMBLE_CMD;
		usb_rumble.len = 0x09;
		usb_rumble.index = rumble_index++;
		usb_rumble.p.rumble = rumble;
		return api_transport_tx(phandle,(uint8_t*)&usb_rumble,sizeof(usb_rumble));
	}else{
		rumble.bt_cmd = XBOX_BT_RUMBLE_CMD;
		return api_transport_tx(phandle,(uint8_t*)&rumble,sizeof(rumble));
	}
}


static bool x360_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	uint8_t rumble1 = prumble->duty[RUMBLE_L];
	uint8_t rumble2 = prumble->duty[RUMBLE_R];
	if(api_trp_is_usb(phandle->trp)){
		x360_usb_rumble_t rumble;
		memset(&rumble,0,sizeof(rumble));
		rumble.cmd = X360_RUMBLE_CMD;
		rumble.rumble1 = rumble1;
		rumble.rumble2 = rumble2;
		return api_transport_tx(phandle,(uint8_t*)&rumble,sizeof(rumble));
	}else{
		xbox_rumble_t rumble;
		memset(&rumble,0,sizeof(rumble));
		rumble.bt_cmd = XBOX_BT_RUMBLE_CMD;
		rumble.rumble_mask = 0xff;
		rumble.rumble1 = remap(rumble1,0,255,0,100);
		rumble.rumble2 = remap(rumble2,0,255,0,100);
		rumble.duration = 0xff;
		rumble.res2[1] = 0xeb;
		return api_transport_tx(phandle,(uint8_t*)&rumble,sizeof(rumble));
	}
}

/*******************************************************************
** Function: 	
** 注意  如果是home按键按下触发的按键解析, keyp 中只有keyp->key有效(最好keyp传如static变量)
		如果是普通按键触发的, home按键是同样有效的
** Returns:			
*******************************************************************/
static bool xboxs_key_decode(trp_handle_t* phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	uint32_t key=0;
	xbox_report_t* xbox_inp = (xbox_report_t*)buf;

	if(api_trp_is_usb(phandle->trp)){		//usb
		if(XBOX_KEY_REP_CMD == xbox_inp->id){
			// logd("xbox:");dumpd(xbox_inp,len);
			key = SWAP16_L(xbox_inp->button);
			keyp->key = key | (xbox_host_key&XBOX_HOME? XBOX_HOME : 0);
			if(len == sizeof(xbox_report_t)){
				if(xbox_inp->res[4]){
					keyp->key |= XBOX_SHARE;
				}
			}
			keyp->stick_l.x = SWAP16_L(xbox_inp->x);
			keyp->stick_l.y = SWAP16_L(xbox_inp->y);
			keyp->stick_r.x = SWAP16_L(xbox_inp->rx);
			keyp->stick_r.y = SWAP16_L(xbox_inp->ry);
			keyp->l2 = SWAP16_L(xbox_inp->l2)>>2;
			keyp->r2 = SWAP16_L(xbox_inp->r2)>>2;
			
			if(keyp->l2) keyp->key |= XBOX_L2;
			if(keyp->r2) keyp->key |= XBOX_R2;
            xbox_host_key = keyp->key;
			ret = true;
		}else if(XBOX_HOME_REP_CMD == xbox_inp->id){
			xbox_logo_report_t*logp = (xbox_logo_report_t*)xbox_inp;
	        if(logp->button&BIT(0)){
				xbox_host_key |= XBOX_HOME;
			}else{
				xbox_host_key &= ~XBOX_HOME;
			}
            keyp->key = xbox_host_key;
			xbox_ack(phandle,logp->cmd,logp->bctrl,logp->index, 2,2);			//发送ack
	        ret = true;
	    }
	}else{
		xbox_bt_report_t* bt_xboxp = (xbox_bt_report_t*)xbox_inp;
		if(XBOX_BT_KEY_REP_CMD == bt_xboxp->id){
			key = SWAP16_L(bt_xboxp->button);
			key |= gamepad_hatswitch_to_key(bt_xboxp->hat_switch-1);
			if(bt_xboxp->back_share_key) key |= GAMEPAD_SELECT;

			keyp->key = key;
			keyp->stick_l.x = SWAP16_L(bt_xboxp->x)-0X8000;
			keyp->stick_l.y = SWAP16_L(bt_xboxp->y)-0X8000;
			keyp->stick_r.x = SWAP16_L(bt_xboxp->rx)-0X8000;
			keyp->stick_r.y = SWAP16_L(bt_xboxp->ry)-0X8000;
			keyp->stick_l.y = negative_int16(keyp->stick_l.y);			//取反
			keyp->stick_r.y = negative_int16(keyp->stick_r.y);

			keyp->l2 = SWAP16_L(bt_xboxp->l2)>>2;
			keyp->r2 = SWAP16_L(bt_xboxp->r2)>>2;
			if(keyp->l2) keyp->key |= GAMEPAD_L2;
			if(keyp->r2) keyp->key |= GAMEPAD_R2;
			ret = true;
		}
	}
	return ret;
}

static bool x360_key_decode(trp_handle_t* phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	uint32_t key;
	x360_report_t* x360_inp = (x360_report_t*)buf;
	 
	if(X360_KEY_REP_CMD == x360_inp->cmd){
		key = SWAP16_L(x360_inp->button);

		keyp->key = key;
		keyp->stick_l.x = SWAP16_L(x360_inp->x);
		keyp->stick_l.y = SWAP16_L(x360_inp->y);
		keyp->stick_r.x = SWAP16_L(x360_inp->rx);
		keyp->stick_r.y = SWAP16_L(x360_inp->ry);
		keyp->l2 = x360_inp->l2;
		keyp->r2 = x360_inp->r2;
		if(keyp->l2) keyp->key |= X360_L2;
		if(keyp->r2) keyp->key |= X360_R2;
		ret = true;
	}
	return ret;
}



/*****************************************************************************************************
**  Function
******************************************************************************************************/
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
void xbox_spk_transfer(uint8_t id, usb_endp_t *endpp, uint8_t* buf, uint16_t frame_len)
{
	uint8_t spk_buf[192+6];
	uint32_t spk_frame_len = API_AUDIO_SPK_SIZE(&usbh_audio_info);
	static uint8_t spk_head[]={0x60,0x21,0x00,0xc0,0x81,0x00};

	spk_head[2]++;
	memset(spk_buf,0,sizeof(spk_buf));
	memcpy(spk_buf,spk_head,6);
	if(spk_frame_len == frame_len){
		memcpy(spk_buf+6,buf,frame_len);
	}
	usbh_out(id, endpp, spk_buf,sizeof(spk_buf));
}
#endif


bool xbox_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	bool ret = false;
	hid_type_t hid_type = phandle->index & 0XFF;

	if(HID_TYPE_X360 == hid_type){
		ret = x360_rumble_send(phandle, prumble);
	}else if(HID_TYPE_XBOX == hid_type){
		ret = xboxs_rumble_send(phandle, prumble);
	}

	return ret;
}

bool xbox_key_decode(trp_handle_t* phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	hid_type_t hid_type = phandle->index & 0XFF;
	
	if(HID_TYPE_X360 == hid_type){
		ret = x360_key_decode(phandle, buf, len, keyp);
	}else if(HID_TYPE_XBOX == hid_type){
		ret = xboxs_key_decode(phandle, buf, len, keyp);
	}

	return ret;
}


/*******************************************************************************
* 描述    : xbox 主机指令处理,和其他主机相比XBOX比较特殊,需要对指令组包
* 输入    : buf  指向输出数据   len 数据长度
* 输出    : cmdp 返回获取到的指令
* 返回值  : 是否成功
*******************************************************************************/
bool xbox_in_process(trp_handle_t* phandle, uint8_t *buf, uint8_t len)
{
	bool ret = false;
	#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
	xbox_command_t tx_cmd;
	memset(&tx_cmd,0,sizeof(tx_cmd));
	#endif


	if(xbox_get_cmd(phandle, &xbox_host_cmd_rx, buf,len)){		//xbox主机没有长包发送
		xbox_command_t* rx_cmdp = &xbox_host_cmd_rx;

		switch(rx_cmdp->cmd) {
		case XBOX_INFO_CMD:{
			xbox_info_t* infp = (xbox_info_t*)(rx_cmdp->pbuf);
			m_xbox_host_info = *infp;
			uint32_t version = U32(infp->version[0],infp->version[2],infp->version[3],infp->version[4]);
			if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
				xbox_host_uac_en = true;
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_UNKNOWN_04_CMD,NULL,0);
				#else
				ret = true;
				#endif
			}else{
				#if !USBH_SOCKET_ENABLE
				m_xbox_enum_step = 1;
				#endif
			}
			logd("xboxh info:step=%d,pid=%x,version=%x mac:",m_xbox_enum_step,infp->pid,version);dumpd(infp->mac,6);
			break;
		}
		case XBOX_UNKNOWN_04_CMD:
			if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
				uint8_t buf[] = {0x02,0x09,0x10};
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_VOLUME_CTRL_CMD,buf,3);
				xbox_command_send(&tx_cmd);
				#else		//不支持音频不需回复
				ret = true;
				#endif
			}
			break;
		case XBOX_VOLUME_CTRL_CMD:
			if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
				uint8_t val = 0;
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_START_CMD,&val,1);
				if(xbox_command_send(&tx_cmd)){
					logi("xboxh earphone in\n");
				}
				#else		//不支持音频不需回复
				ret = true;
				#endif
			}
			break;
		case XBOX_STATUS_CMD:
			if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
				xbox_state_t* pstate = (xbox_state_t*)rx_cmdp->pbuf;
				if(0 == pstate->uac && 0 == pstate->state){		//关闭音频
					uint8_t val = 1;
					xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_START_CMD,&val,1);
					if(xbox_command_send(&tx_cmd)){
						xbox_host_uac_en = false;
					}
					logi("xboxh earphone out\n");
				}
				#else		//不支持音频不需回复
				ret = true;
				#endif
			}
			logd("xboxh state:");dumpd(rx_cmdp->pbuf,rx_cmdp->len);
			break;
		default:
			logd("xboxh cmd in:%x",rx_cmdp->cmd);dumpd(rx_cmdp->pbuf,8);		
			break;
		}
	}
	return ret;
}

void xbox_enum_process(trp_handle_t* phandle)		//xbox手柄枚举需要发送开始信号
{
	xbox_command_t tx_cmd;
	static timer_t xbox_enum_timer;
	
	if (m_xbox_enum_step){
		if(m_systick - xbox_enum_timer >= 8){
			
			memset(&tx_cmd,0,sizeof(tx_cmd));
			switch(m_xbox_enum_step){
			case 1:
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_START_CMD,(uint8_t*)xbox_start0,sizeof(xbox_start0));
				if(xbox_command_send(&tx_cmd)){
					m_xbox_enum_step++;
				}
				break;
			case 2:
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_START_CMD,(uint8_t*)xbox_start1,sizeof(xbox_start1));
				if(xbox_command_send(&tx_cmd)){
					m_xbox_enum_step++;
				}
				break;
			case 3:
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_EN_LED_CMD,(uint8_t*)xbox_set_led,sizeof(xbox_set_led));
				if(xbox_command_send(&tx_cmd)){
					m_xbox_enum_step++;
				}
				break;
			case 4:
				xbox_command_fill(phandle,&tx_cmd,&xbox_host_index,xbox_host_uac_en,XBOX_RUMBLE_CMD,(uint8_t*)xbox_set_rumble,sizeof(xbox_set_rumble));
				if(xbox_command_send(&tx_cmd)){
					m_xbox_enum_step++;
					logd_g("usbh gamepad ready...\n");
				}
				break;
			default:
				m_xbox_enum_step = 0;
				break;
			}
		}
	}
}


/*******************************************************************
** Function: 	
** Parameters:		
** Returns:			
*******************************************************************/
void xbox_host_init(trp_handle_t *phandle)
{
    xbox_host_key = 0;
	xbox_host_uac_en = false;
	
	memset(&xbox_host_cmd_rx,0,sizeof(xbox_host_cmd_rx));
	memset(&xbox_host_index,0,sizeof(xbox_host_index));
	xbox_host_index.cmd_index = 1;
}

void xbox_host_deinit(trp_handle_t *phandle)
{
	xbox_host_uac_en = false;
	xbox_command_free(&xbox_host_cmd_rx);			//用于异常释放资源
}

void xbox_host_task(trp_handle_t *phandle)
{
	xbox_enum_process(phandle);		//xbox手柄枚举需要发送开始信号
}





#endif






