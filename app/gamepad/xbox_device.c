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
#include  "gamepad_controller.h"		//TODO
#include  "api/api_tick.h"
#if API_AUDIO_ENABLE
#include "api/audio/api_audio.h"
#endif
#if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
#include "api/usb/device/usbd.h"
#endif
#if BT_ENABLE
#include  "api/bt/api_bt.h"
#endif

#include  "api/api_log.h"


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#if (HIDD_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))

//xboxx 的音频在PC上是调整的是数字音音量,xbox one发的是指令调整音量但mic无法调整
// xbox 不同版本数据可以混着发送
#if 0X500 <= XBOX_DEV_VERSION
//50D
uint8c_t xbox_version[8] = {XBOX_DEV_VERSION>>8, 0x00, XBOX_DEV_VERSION&0XFF, 0x00, 0x47, 0x0C, 0x00, 0x00};		//firmware version 5.5.2641.0 xboxx
uint8c_t xbox_info_unknow[8] = {0x08,0x04,0x01,0x00,0x01,0x00,0x01,0x00};
#else
//V408
uint8c_t xbox_version[8] = {XBOX_DEV_VERSION>>8, 0x00, XBOX_DEV_VERSION&0XFF, 0x00, 0x83, 0x07, 0x00, 0x00};		//firmware version 5.5.2641.0 xboxone
uint8c_t xbox_info_unknow[8] = {0x04,0x05,0x01,0x00,0x01,0x00,0x01,0x00};
#endif
// uint8c_t xbox_cmd_04[] = {	
// 	0x10,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xBC,0x00,0x7D,0x00,0x16,0x00,0x1B,0x00,0x1C,0x00,0x26,0x00,0x2F,0x00,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x00,0x08,0x00,0x00,0x09,0x01,0x02,0x03,0x04,0x06,0x07,0x0C,0x1C,0x1E,0x08,0x01,0x04,0x05,
// 	0x06,0x0A,0x0C,0x1C,0x1E,0x01,0x1A,0x00,0x57,0x69,0x6E,0x64,0x6F,0x77,0x73,0x2E,0x58,0x62,0x6F,0x78,0x2E,0x49,0x6E,0x70,0x75,0x74,0x2E,0x47,0x61,0x6D,0x65,0x70,0x61,0x64,0x03,0x56,0xFF,0x76,0x97,0xFD,0x9B,0x81,0x45,0xAD,0x45,0xB6,0x45,0xBB,0xA5,0x26,0xD6,0x2C,0x40,0x2E,0x08,0xDF,0x07,0xE1,
// 	0x45,0xA5,0xAB,0xA3,0x12,0x7A,0xF1,0x97,0xB5,0xE7,0x1F,0xF3,0xB8,0x86,0x73,0xE9,0x40,0xA9,0xF8,0x2F,0x21,0x26,0x3A,0xCF,0xB7,0x02,0x17,0x00,0x20,0x0E,0x00,0x01,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x00,0x09,0x3C,0x00,0x01,0x00,0x08,0x00,
// 	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#if API_AUDIO_ENABLE
//uint8c_t xbox_uac_cmd_04[] = {
//	0x10,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6C,0x00,0x5B,0x00,0x16,0x00,0x1B,0x00,0x20,0x00,0x27,0x00,0x2D,0x00,0x4A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x00,0x0D,0x00,0x02,0x09,0x10,0x09,0x09,0x06,0x01,0x02,0x03,0x04,0x08,0x60,0x05,0x01,0x04,
//	0x05,0x08,0x60,0x01,0x1A,0x00,0x57,0x69,0x6E,0x64,0x6F,0x77,0x73,0x2E,0x58,0x62,0x6F,0x78,0x2E,0x49,0x6E,0x70,0x75,0x74,0x2E,0x48,0x65,0x61,0x64,0x73,0x65,0x74,0x01,0xA3,0xD1,0x25,0xBC,0x4E,0xC2,0x92,0x49,0x9D,0xDA,0xEF,0x4F,0x12,0x3E,0xF5,0xDC,0x00,};
uint8c_t xbox_uac_cmd_04[] = {
	0x10,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6E,0x00,0x5D,0x00,0x16,0x00,0x1B,0x00,0x20,0x00,0x28,0x00,0x2F,0x00,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x00,0x08,0x00,0x02,0x09,0x10,0x09,0x09,0x07,0x01,0x02,0x03,0x04,0x08,0x1C,0x60,0x06,0x01,
	0x04,0x05,0x08,0x1C,0x60,0x01,0x1A,0x00,0x57,0x69,0x6E,0x64,0x6F,0x77,0x73,0x2E,0x58,0x62,0x6F,0x78,0x2E,0x49,0x6E,0x70,0x75,0x74,0x2E,0x48,0x65,0x61,0x64,0x73,0x65,0x74,0x01,0xA3,0xD1,0x25,0xBC,0x4E,0xC2,0x92,0x49,0x9D,0xDA,0xEF,0x4F,0x12,0x3E,0xF5,0xDC,0x00};
#endif

#endif


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t m_controller_index=0;
xbox_command_t xbox_dev_cmd_rx;
xbox_command_t xbox_dev_cmd_tx;
xbox_cmd_index_t xbox_dev_index;
bool xbox_dev_uac_en;						//是否开启音频

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

static uint8_t xbox_logo_key_pack(trp_handle_t *phandle,uint32_t key,xbox_logo_report_t * xbox_logp)
{
	static uint8_t xobx_log_index;
	static uint32_t xbox_key;

	uint32_t change_key = xbox_key ^ key;

	if(api_trp_is_usb(phandle->trp)){
		if(change_key & XBOX_HOME){	// DOWN 6, UP 2

			xbox_logp->cmd = XBOX_HOME_REP_CMD;
			xbox_logp->bctrl = 0x30;
			xbox_logp->index = xobx_log_index++;
			xbox_logp->len = 0x02;
			
			if(change_key & key & XBOX_HOME){
				xbox_logp->button = 1;
			}
			xbox_logp->home_key = 0x5b;
			
			xbox_key =  key;
			return sizeof(xbox_logo_report_t);
		}
	}else if(api_trp_is_bt(phandle->trp)){
		if(change_key & GAMEPAD_HOME){
			xbox_bt_logo_report_t* xboxbt_logp = (xbox_bt_logo_report_t*)xbox_logp;
			xboxbt_logp->id = XBOX_BT_HOME_REP_CMD;
			if(change_key & key & GAMEPAD_HOME){
				xboxbt_logp->home_key = 1;
			}
			xbox_key =  key;
			return sizeof(xbox_bt_logo_report_t);
		}
	}

	return 0;
}



static uint16_t xboxs_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	xbox_report_t* xboxp = (xbox_report_t*)buf;

	if(len < sizeof(xbox_report_t)) return 0;

	memset(xboxp,0,sizeof(xbox_report_t));
	packet_len = xbox_logo_key_pack(phandle,keyp->key,(xbox_logo_report_t*)xboxp);

	if(0 == packet_len){
		if(api_trp_is_usb(phandle->trp)){
			#if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
			usbd_dev_t *pdev = usbd_get_dev(phandle->id);

			xboxp->id = XBOX_KEY_REP_CMD;
			xboxp->button = SWAP16_L(keyp->key);

			xboxp->l2 = (uint16_t)((keyp->l2)>>5);
			xboxp->r2 = (uint16_t)((keyp->r2)>>5);

			xboxp->l2 = SWAP16_L(xboxp->l2);
			xboxp->r2 = SWAP16_L(xboxp->r2);
			xboxp->x = SWAP16_L(keyp->stick_l.x);
			xboxp->y = SWAP16_L(keyp->stick_l.y);
			xboxp->rx = SWAP16_L(keyp->stick_r.x);
			xboxp->ry = SWAP16_L(keyp->stick_r.y);
			if(0 == ++m_controller_index) m_controller_index = 1;
			xboxp->index = m_controller_index;

			xboxp->len = 0x0e;
			packet_len = sizeof(xboxone_report_t);

			if(XBOX_DEV_VERSION >= 0X500){				//根据xbox 版本发送数据
				xboxp->button |= SWAP16_L( 1 );
				if(keyp->key & XBOX_SHARE){		//only xboxx
					xboxp->res[4]=1;
				}

				xboxp->time_us = SWAP16_L((m_systick & 0x3f)*1000);//简化运算,m_systick%64*1000
				xboxp->time_ms = SWAP16_L(m_systick/64);
				xboxp->time_us2 = xboxp->time_us;				//原装手柄time2要大800us?
				xboxp->time_ms2 = xboxp->time_ms;

				xboxp->len = 0x2C;
				packet_len = sizeof(xbox_report_t);
			}
			#endif
		}else{				//蓝牙
			xbox_bt_report_t* bt_xboxp = (xbox_bt_report_t*)xboxp;

			bt_xboxp->id = XBOX_BT_KEY_REP_CMD;
			bt_xboxp->button = SWAP16_L(keyp->key);

			//GAMEPAD_BACK == XBOX_EDR_SHARE 这里edr share其实是无效的只作用在ble上
			if(keyp->key & XBOX_EDR_SHARE)	bt_xboxp->back_share_key = 1;
			bt_xboxp->hat_switch = (gamepad_key_to_hatswitch(keyp->key)+1) & 0x0f;//从1开始

			bt_xboxp->l2 = (uint16_t)((keyp->l2) >> 5);
			bt_xboxp->r2 = (uint16_t)((keyp->r2) >> 5);
			bt_xboxp->l2 = SWAP16_L(bt_xboxp->l2);
			bt_xboxp->r2 = SWAP16_L(bt_xboxp->r2);

			bt_xboxp->x = keyp->stick_l.x + 0x8000;
			bt_xboxp->y = 0xFFFF - (keyp->stick_l.y + 0x8000);
			bt_xboxp->rx = keyp->stick_r.x + 0x8000;
			bt_xboxp->ry =  0xFFFF - (keyp->stick_r.y + 0x8000);
			bt_xboxp->x = SWAP16_L(bt_xboxp->x);
			bt_xboxp->y = SWAP16_L(bt_xboxp->y);
			bt_xboxp->rx = SWAP16_L(bt_xboxp->rx);
			bt_xboxp->ry = SWAP16_L(bt_xboxp->ry);

			packet_len = sizeof(xbox_bt_report_t);
		}
	}
	return packet_len;
}



static uint16_t x360_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	x360_report_t* x360p = (x360_report_t*)buf;

	if(len < sizeof(x360_report_t)) return 0;
	memset(x360p,0,sizeof(x360_report_t));

	x360p->cmd =  X360_KEY_REP_CMD;
	x360p->button = SWAP16_L(keyp->key);
	x360p->l2 = keyp->l2 >> 7;
    x360p->r2 = keyp->r2 >> 7;

	x360p->x = SWAP16_L(keyp->stick_l.x);
	x360p->y = SWAP16_L(keyp->stick_l.y);
	x360p->rx = SWAP16_L(keyp->stick_r.x);
	x360p->ry = SWAP16_L(keyp->stick_r.y);
	packet_len = sizeof(x360_report_t);

	return packet_len;
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint16_t xbox_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	hid_type_t hid_type = phandle->index & 0XFF;

	if(HID_TYPE_X360 == hid_type){
		packet_len = x360_key_pack(phandle, keyp, buf, len);
	}else if(HID_TYPE_XBOX == hid_type){
		packet_len = xboxs_key_pack(phandle, keyp, buf, len);
	}

	return packet_len;
}


void get_xbox_info(xbox_info_t* infp)
{
	uint8_t mac_buf[8]={0x7E, 0xED, 0x80, 0x76, 0xFF, 0x76};
	memset(infp,0,sizeof(xbox_info_t));

	#if BT0_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID0, BT_EDR, mac_buf);
	#elif BT1_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID1, BT_EDR, mac_buf);
	#endif

	memcpy(infp->mac,mac_buf,sizeof(infp->mac));
	infp->vid = SWAP16_L(XBOX_VID);
	if(xbox_dev_uac_en){
		infp->pid = SWAP16_L(XBOX_UAC_PID);
	}else{
		infp->pid = SWAP16_L(XBOX_PID);
	}
	#if (HIDD_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
	memcpy(infp->version,xbox_version,sizeof(xbox_version));
	memcpy(infp->unknown,xbox_info_unknow,sizeof(xbox_info_unknow));
	#endif
}

#if API_AUDIO_ENABLE && API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
void xbox_mic_transfer(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t frame_len)
{
	uint8_t  mic_buffer[64];
	static uint8_t mic_head[]={0x60,0x21,0x00,0x32,0xc0,0x00};
	uint32_t mic_frame_len = API_AUDIO_MIC_SIZE(&usbd_audio_info);

	mic_head[2]++;
	memset(mic_buffer,0,sizeof(mic_buffer));
	memcpy(mic_buffer,mic_head,6);

	if(mic_frame_len == frame_len){
		memcpy(mic_buffer+6,buf,mic_frame_len);
	}
	usbd_in(id, ep, mic_buffer,sizeof(mic_buffer));
}

/*******************************************************************
** Description:	xbox 模式下音量需要保存音量大小,上电后主机会读取保存在手柄中的音频数据
				目前每次接如xobx都固定音量大小,没有做保存
** Parameters:		
** Returns:			
*******************************************************************/
void get_xbox_vol(uint8_t id,xbox_vol_ctrl_t* volp)
{
	uint16_t vol_l=100, vol_r=100, vol_mic=100;
	#if API_AUDIO_ENABLE && API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
	api_audio_spk_get_vol_percent(id,&usbd_audio_info,&vol_l, &vol_r);
	vol_mic = api_audio_mic_get_vol_percent(id,&usbd_audio_info);
	#endif

	memset(volp,0,sizeof(xbox_vol_ctrl_t));
	volp->type = 0x03;
	volp->off = 0x04;			//0x04 打开音频, 0x5关闭音频
	volp->off_dir = 1;			//1:out设置, 0:in设备回复
	volp->spk_vol = vol_l;		//0~100
	volp->spk_dir = 1;			//1:out设置, 0:in设备回复
	volp->chat_vol = 50;		//0~100	耳机聊天控制台音量
	volp->chat_dir = 1;			//1:out设置, 0:in设备回复
	volp->mic_vol = vol_mic;	//0~100
	volp->mic_dir = 1;			//1:out设置, 0:in设备回复
	volp->res_vol = 64;
	volp->res_dir = 1;			//1:out设置, 0:in设备回复
}
void set_xbox_vol(uint8_t id,xbox_vol_ctrl_t*volp)
{
	#if API_AUDIO_ENABLE && API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
	if(0x05 == volp->off){
		logd_r("set_xbox_vol mult\n");
		api_audio_spk_set_vol(id,&usbd_audio_info,0,0);
		api_audio_mic_set_vol(id,&usbd_audio_info,0);
	}else{
		logd_r("set_xbox_vol %x %x\n",volp->spk_vol,volp->mic_vol);
		api_audio_spk_set_vol(id,&usbd_audio_info,volp->spk_vol,volp->spk_vol);
		if(volp->mic_vol){								//xboxone 方式打开音频,PC一直设个MIC为0问题未得到解决
			api_audio_mic_set_vol(id,&usbd_audio_info,volp->mic_vol);		
		}
	}
	#endif
}


bool xbox_earphone_enable(trp_handle_t *phandle,xbox_command_t* cmdp,bool enable)
{
	bool ret = false;
	if(1){				//m_xbox_cmd_ready,xboxone 老手柄没有E1无法判断ready,修改到延时
		logd("xbox_earphone_enable = %d\n",enable);
		if(enable){							//耳机接入
			xbox_info_t info;
			xbox_dev_uac_en = true;
			get_xbox_info(&info);
			ret = xbox_command_fill(phandle,cmdp,&xbox_dev_index,xbox_dev_uac_en,XBOX_INFO_CMD, (uint8_t*)&info, sizeof(info));	
		}else{								//耳机拔出
			xbox_state_t state;
			memset(&state,0,sizeof(state));
			ret = xbox_command_fill(phandle,cmdp,&xbox_dev_index,xbox_dev_uac_en,XBOX_STATUS_CMD,(uint8_t*)&state,sizeof(state));
			if(ret) xbox_dev_uac_en = false;
		}
	}
	return ret;
}
#endif

/*******************************************************************************
* 描述    : xbox 指令处理,和其他主机相比XBOX比较特殊,需要对指令组包
* 输入    : buf  指向输出数据   len 数据长度
* 输出    : cmdp 返回获取到的指令
* 返回值  : 是否成功
*******************************************************************************/
bool xbox_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len)
{
	bool ret = false;
	uint8_t hid_type = HID_REPORT_TYPE_OUTPUT;
	
	if(TR_EDR == phandle->trp){		//edr 判断 hid_report_type_t
		hid_type = buf[0] & 0X0F;
		len -= 1;
		buf += 1;
		if ( 0 == len) return false;
	}

	if(api_trp_is_usb(phandle->trp)){
		#if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX))
		xbox_vol_ctrl_t vol_ctrl;
		if(XBOX_KEY_REP_CMD == buf[0]){		//按键数据太多这里不处理
			return false;
		}else if(XBOX_ACK_CMD == buf[0]){	//xbox dev 从机引导过程,xbox_dev_cmd_tx没有发送数据不会走这里
			xbox_get_ack(phandle, &xbox_dev_cmd_tx, buf,len);
		}else{
			if(xbox_get_cmd(phandle, &xbox_dev_cmd_rx, buf,len)){
				xbox_command_t* rx_cmdp = &xbox_dev_cmd_rx;
				xbox_command_t* tx_cmdp = &xbox_dev_cmd_tx;
				logd("xboxd cmd out:%x\n",rx_cmdp->cmd);
				switch(rx_cmdp->cmd) {
				case XBOX_RUMBLE_CMD:{
					#if APP_RUMBLE_ENABLE
					rumble_t rumble;
					xbox_rumble_t *prumble =(xbox_rumble_t*)rx_cmdp->pbuf;
					rumble.duty[RUMBLE_L] = remap(prumble->rumble1,0,100,0,255);			//Xbox马达范围0~100
					rumble.duty[RUMBLE_R] = remap(prumble->rumble2,0,100,0,255);
					rumble.duty[RUMBLE_SL] = remap(prumble->srumble1,0,100,0,255);		//Xbox马达范围0~100
					rumble.duty[RUMBLE_SR] = remap(prumble->srumble2,0,100,0,255);
					app_set_rumble(&rumble, prumble->duration*20);
					#endif
					ret = true;
					break;
				}

				#if API_AUDIO_ENABLE && API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
				case XBOX_UNKNOWN_04_CMD:							//注意:引导手柄如果没有接耳机, 不会回复指令
					if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
						xbox_command_fill(phandle,tx_cmdp,&xbox_dev_index,xbox_dev_uac_en,XBOX_UNKNOWN_04_CMD,(uint8_t*)xbox_uac_cmd_04,sizeof(xbox_uac_cmd_04));	//TODO 选择UAC和PUT 
					}
					break;
				case XBOX_VOLUME_CTRL_CMD:
					dumpd(rx_cmdp->pbuf,rx_cmdp->len);
					if(rx_cmdp->bctrl & XBOX_CTRL_UAC){			//回复主机发过来的数据
						memcpy(&vol_ctrl,rx_cmdp->pbuf,rx_cmdp->len);
						if(0x03 == vol_ctrl.type){
							vol_ctrl.off_dir = 1;
							vol_ctrl.spk_dir = 1;
							vol_ctrl.chat_dir = 1;
							vol_ctrl.mic_dir = 1;
							vol_ctrl.res_dir = 1;
							set_xbox_vol(0,&vol_ctrl);
						}
						xbox_command_fill(phandle,tx_cmdp,&xbox_dev_index,xbox_dev_uac_en,XBOX_VOLUME_CTRL_CMD,(uint8_t*)&vol_ctrl,rx_cmdp->len);
					}
					break;
				case XBOX_START_CMD:
					if(rx_cmdp->bctrl & XBOX_CTRL_UAC){
						if(rx_cmdp->pbuf[0]){		//关闭音频
							api_audio_close_mic(0,&usbd_audio_info);
							api_audio_close_spk(0,&usbd_audio_info);
							api_audio_earphone_in = false;
						}else{					//打开音频
							usbd_class_t * pclass = usbd_xbox_audio_find(phandle->id);
							api_audio_earphone_in = true;
							get_xbox_vol(0,&vol_ctrl);				//设置上电默认音量大小,必须设置否则无法设置xbox音频
							xbox_command_fill(phandle,tx_cmdp,&xbox_dev_index,xbox_dev_uac_en,XBOX_VOLUME_CTRL_CMD,(uint8_t*)&vol_ctrl,sizeof(vol_ctrl));

							api_audio_open_spk(0,&usbd_audio_info,XBOX_SPK_RATE,16,2);
							if(NULL != pclass){
								api_audio_open_mic(0,&usbd_audio_info,XBOX_MIC_RATE,16,1);
								xbox_mic_transfer(phandle->id, pclass->endpin.addr, NULL, 0);	//开启USB device mic的传输
							}
						}
					}
					break;
				#endif
				default:
					break;
				}

				xbox_command_free(rx_cmdp);			//释放资源
			}
		}
		#endif
	}else {
		if(XBOX_BT_RUMBLE_CMD == buf[0]){
			#if APP_RUMBLE_ENABLE
			xbox_rumble_t* rumblep = (xbox_rumble_t*)buf;
			rumble_t rumble;
			// logd("xboxd out %x:",endp); dumpd(buf,len);
			rumble.duty[RUMBLE_L] = remap(rumblep->rumble1,0,100,0,255);		//Xbox马达范围0~100
			rumble.duty[RUMBLE_R] = remap(rumblep->rumble2,0,100,0,255);
			rumble.duty[RUMBLE_SL] = remap(rumblep->srumble1,0,100,0,255);		//Xbox马达范围0~100
			rumble.duty[RUMBLE_SR] = remap(rumblep->srumble2,0,100,0,255);
			app_set_rumble(&rumble, 20000);
			#endif
			
			ret = true;
		}
	}

	return ret;
}



bool x360_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len)
{
    bool ret = false;
	uint8_t hid_type = HID_REPORT_TYPE_OUTPUT;
	rumble_t rumble;
	x360_usb_rumble_t *rumblep;

	if(TR_EDR == phandle->trp){		//edr 判断 hid_report_type_t
		hid_type = buf[0] & 0X0F;
		len -= 1;
		buf += 1;
		if ( 0 == len) return false;
	}

	rumblep = (x360_usb_rumble_t *)buf;	
	if (X360_RUMBLE_CMD == rumblep->cmd){
		#if APP_RUMBLE_ENABLE
		app_rumble_set_duty(RUMBLE_L, rumblep->rumble1, 10000);
		app_rumble_set_duty(RUMBLE_R, rumblep->rumble2, 10000);
		#endif
		ret = true;
	}
	
	return ret;
}

void xbox_device_init(trp_handle_t *phandle)
{
	xbox_dev_uac_en = false;
	
	memset(&xbox_dev_cmd_rx,0,sizeof(xbox_dev_cmd_rx));
	memset(&xbox_dev_cmd_tx,0,sizeof(xbox_dev_cmd_tx));
	memset(&xbox_dev_index,0,sizeof(xbox_dev_index));
	xbox_dev_index.cmd_index = 1;
}

void xbox_device_deinit(trp_handle_t *phandle)
{
	xbox_dev_uac_en = false;
	xbox_command_free(&xbox_dev_cmd_rx);			//用于异常释放资源
	xbox_command_free(&xbox_dev_cmd_tx);
}

void xbox_device_task(trp_handle_t *phandle)
{
	xbox_command_tx_process(&xbox_dev_cmd_tx);
}


#endif






