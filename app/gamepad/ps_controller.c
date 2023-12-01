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
#if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK)
#include  "utils/emf_utils.h"

#include  "ps_controller.h"
#include  "api/api_tick.h"

#if (API_AUDIO_ENABLE)
#include "api/audio/api_audio.h"
#endif
#if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & HID_PS_MASK)
#include "api/usb/device/usbd.h"
#endif
#if API_USBH_BIT_ENABLE && (USBH_HID_SUPPORT & HID_PS_MASK)
#include "api/usb/host/usbh.h"
#endif
#if BT_ENABLE
#include  "api/bt/api_bt.h"
#endif

#if APP_BATTERY_ENABLE
#include "app_battery.h"
#endif
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
//uint8c_t ps_cmd03[20] ={0x03,0x21,0x27,0x04,0x41,0x00,0x2c,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0D};	//HORY 手柄
uint8c_t ps_cmd03[20] ={0x03,0x21,0x27,0x04,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x0D};
uint8c_t ps_cmdF3[4] ={0xF3,0x00,0x38,0x38};		//b8 不支持F2,改成38


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
ps_series_t m_ps_series;			//判断PS产品系列, 后面可以改成gamepad device series
bool m_ps_enhanced_mode = false;	//large report mode, 从机蓝牙模式才有效, 收到马达数据后打开


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t ps_report_index=0;
static ps_touch_t m_ps_touch[PS4_SLOT_MAX];
static uint8_t mcontact_id[PS4_SLOT_MAX];			//for find contact id不能为0

static uint8_t	 pskey_auto_count=0;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	 ps4刚枚举完成时  需要先报告30次以上任意数据以后才会识别ps按键	
*******************************************************************/
static void ps4_auto_ps_key(trp_handle_t *phandle, ps4_report_t *ps4p)
{
	if(!api_transport_ready(phandle)){
		pskey_auto_count = 0;
	}else if(pskey_auto_count < 50){
		pskey_auto_count++;
		if(pskey_auto_count == 50){
			ps4p->buttonh |= PS4_PS>>16;
		}
	}
}
static uint16_t ps4_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	axis3i_t axis;
	ps4_report_t* ps4p;

	memset(buf,0,len);

	if(api_trp_is_bt(phandle->trp)){           //蓝牙结构体长度(+id)
		if(len < sizeof(ps4_large_report_t)) return 0;

		if(m_ps_enhanced_mode){						//打开large report
			ps4_large_report_t* large_ps4p = (ps4_large_report_t*)buf;	//这里特殊适配
			large_ps4p->id = PS4_REPORT_ID;
			large_ps4p->magic = 0xC0;
			large_ps4p->touch_pad8_index = 0x80;

			ps4p = (ps4_report_t*)(buf+2);			
			packet_len = sizeof(ps4_large_report_t);		
		}else{
			ps4p = (ps4_report_t*)buf;
			ps4p->id = PS4_REPORT_ID;
			packet_len = 10;
		}
	}else{
		if(len < sizeof(ps4_report_t)) return 0;

		ps4p = (ps4_report_t*)buf;
		ps4p->id = PS4_REPORT_ID;
		packet_len = sizeof(ps4_report_t);
	}

	
	#if (API_AUDIO_ENABLE)
	if(api_audio_earphone_in){				//ps4耳机接入检测
		ps4p->ear_stu = 0x03;
	}
	#endif

	#if APP_BATTERY_ENABLE
        ps4p->battery = m_battery/10;
        if((BAT_CHARGE_DONE_STA == m_battery_sta) || (BAT_CHARGE_STA == m_battery_sta)){
            ps4p->battery |= 0x10;
        }
    #else
        ps4p->battery = 0x1B;
    #endif
	ps4p->lx = (keyp->stick_l.x >> 8) + 0x80;
	ps4p->ly = 0xFF - ((keyp->stick_l.y >> 8) + 0x80);
	ps4p->rx = (keyp->stick_r.x >> 8) + 0x80;
	ps4p->ry = 0xFF - ((keyp->stick_r.y >> 8) + 0x80);
	ps4p->buttonl = (keyp->key&0xfff0) | ps4_key_to_hatswitch(keyp->key);
	ps4p->buttonh = keyp->key>>16;
	ps4p->buttonl = SWAP16_L( ps4p->buttonl );

	ps4p->l2 = keyp->l2;
	ps4p->r2 = keyp->r2;

	ps4p->time_index = SWAP16_L((m_systick&0xffff)*200);
	ps4p->mark = 0x13;

	axis.x = keyp->acc.y;
	axis.y = 0-keyp->acc.z;
	axis.z = 0-keyp->acc.x;
	axis3i_swapl(&axis);
	ps4p->acc = axis;

	axis.x = keyp->gyro.y;
	axis.y = 0-keyp->gyro.z;
	axis.z = 0-keyp->gyro.x;
	axis3i_swapl(&axis);
	ps4p->gyro = axis;

	ps_report_index++;
	ps4p->index = ps_report_index;

	ps_touch_pack((app_gamepad_key_t*)keyp);

	int_to_bit12(ps4p->touch_pad1, m_ps_touch[0].x, m_ps_touch[0].y);
	int_to_bit12(ps4p->touch_pad2, m_ps_touch[1].x, m_ps_touch[1].y);

	ps4p->touch_pad1_index = m_ps_touch[0].id | (!m_ps_touch[0].active << 7);
	ps4p->touch_pad2_index = m_ps_touch[1].id | (!m_ps_touch[1].active << 7);
	ps4p->touch_pad3_index=0x80;
	ps4p->touch_pad4_index=0x80;
	ps4p->touch_pad5_index=0x80;
	ps4p->touch_pad6_index=0x80;
	ps4p->touch_pad7_index=0x80;


	ps4_auto_ps_key(phandle, ps4p);
	return packet_len;
}

static uint16_t ps3_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	ps3_report_t* ps3p = (ps3_report_t*)buf;

	if(len < sizeof(ps3_report_t)) return 0;
	memset(ps3p,0,sizeof(ps3_report_t));

	ps3p->lx = (keyp->stick_l.x >> 8) + 0x80;
	ps3p->ly = 0xFF - ((keyp->stick_l.y >> 8) + 0x80);
	ps3p->rx = (keyp->stick_r.x >> 8) + 0x80;
	ps3p->ry = 0xFF - ((keyp->stick_r.y >> 8) + 0x80);
	ps3p->button = SWAP32_L(keyp->key);
	ps3p->l2 = keyp->l2;
	ps3p->r2 = keyp->r2;

	if(PS3_UP & keyp->key)	ps3p->up = 0xff;
	if(PS3_DOWN & keyp->key)	ps3p->down = 0xff;
	if(PS3_LEFT & keyp->key)	ps3p->left = 0xff;
	if(PS3_RIGHT & keyp->key)	ps3p->right = 0xff;
	if(PS3_L1 & keyp->key) ps3p->l1 = 0xff;
	if(PS3_R1 & keyp->key) ps3p->r1 = 0xff;
	if(PS3_TRIANGLE & keyp->key) ps3p->bu = 0xff;
	if(PS3_O & keyp->key) ps3p->br = 0xff;
	if(PS3_X & keyp->key) ps3p->bd = 0xff;
	if(PS3_SQUARE & keyp->key) ps3p->bl = 0xff;

	ps3p->id = 0x01;
	ps3p->charge1 = 0x03;		//0x03
	ps3p->charge2 = 0xef;		//0xef
	ps3p->charge3 = 0x14;		//0x14
	ps3p->charge4 = 0x23;		//0x23
	ps3p->constant[0] = 0x03;
	ps3p->constant[1] = 0x77;
	ps3p->constant[2] = 0x01;
	ps3p->port_data = 0x1d;		//0x1d
	ps3p->sensor_x = SWAP16(0X0002);
	ps3p->sensor_y = SWAP16(0X0002);
	ps3p->sensor_z = SWAP16(0X8001);
	ps3p->sensor_speed = SWAP16(0X0002);
	packet_len = sizeof(ps3_report_t);

	return packet_len;
}


static uint16_t ps5_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	ps5_report_t* ps5p = (ps5_report_t*)buf;

	if(len < sizeof(ps5_report_t)) return 0;

	memset(ps5p,0,sizeof(ps5_report_t));
	ps5p->lx = (keyp->stick_l.x >> 8) + 0x80;
	ps5p->ly = 0xFF - ((keyp->stick_l.y >> 8) + 0x80);
	ps5p->rx = (keyp->stick_r.x >> 8) + 0x80;
	ps5p->ry = 0xFF - ((keyp->stick_r.y >> 8) + 0x80);
	ps5p->button = (keyp->key&0xfffffff0) | ps4_key_to_hatswitch(keyp->key);
	ps5p->l2 = keyp->l2;
	ps5p->r2 = keyp->r2;

	ps_report_index++;
	ps5p->index = ps_report_index;
	ps5p->index2 = ps_report_index;

	packet_len = sizeof(ps5_report_t);

	return packet_len;
}



static bool ps4_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	uint8_t rumble1 = prumble->duty[RUMBLE_L];
	uint8_t rumble2 = prumble->duty[RUMBLE_R];
	if(api_trp_is_usb(phandle->trp)){
		ps4_usb_effects_t effects;
		memset(&effects,0,sizeof(effects));
		effects.cmd = PS4_USB_EFFECTS_ID;
		effects.magic = 0x03;				/* Magic value 0x07*/
		effects.effects.rumble_l = rumble1;
		effects.effects.rumble_r = rumble2;
		effects.effects.led_r = 0x20;
		effects.effects.led_g = 0x20;
		effects.effects.led_b = 0x20;

		return api_transport_tx(phandle,(uint8_t*)&effects,sizeof(effects));
	#if BTC_HID_SUPPORT & HID_PS_MASK
	}else{
		ps4_bt_effects_t effects;
		uint8_t ubHdr = 0xA2; /* hidp header is part of the CRC calculation */
		uint32_t unCRC;
		memset(&effects,0,sizeof(effects));
		effects.cmd = PS4_BT_EFFECTS_ID;
		effects.magic = 0xC0 | 0x04;  /* Magic value HID + CRC, also sets interval to 4ms for samples */
		effects.ctrl = 0x03;
		effects.effects.rumble_l = rumble1;
		effects.effects.rumble_r = rumble2;
		effects.effects.led_r = 0x00;
		effects.effects.led_g = 0x00;
		effects.effects.led_b = 0x40;

		/* Bluetooth reports need a CRC at the end of the packet (at least on Linux) */
		#if  CRC32_TABLE_EANBLE
        unCRC = crc32(&ubHdr, 1);
        unCRC += crc32(&effects, (sizeof(effects) - sizeof(unCRC)));
		#else
		#error "ps controller need crc32!"
		#endif

		effects.crc = unCRC;
		return api_transport_tx(phandle,(uint8_t*)&effects,sizeof(effects));
	#endif
	}
}
static bool ps3_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	return false;
}
bool ps5_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	uint8_t rumble1 = prumble->duty[RUMBLE_L];
	uint8_t rumble2 = prumble->duty[RUMBLE_R];
	if(api_trp_is_usb(phandle->trp)){
		ps5_usb_effects_t rumble;
		memset(&rumble,0,sizeof(rumble));
		rumble.cmd = PS5_USB_EFFECTS_ID;
		rumble.effects.bit1_en = 0x01;
		rumble.effects.rumble_l = rumble1;
		rumble.effects.rumble_r = rumble2;
		return api_transport_tx(phandle,(uint8_t*)&rumble,sizeof(rumble));
	}else{
		ps5_bt_effects_t rumble;
		memset(&rumble,0,sizeof(rumble));
		rumble.cmd = PS5_BT_EFFECTS_ID;
		rumble.magic = 0x02;
		rumble.effects.bit1_en = 0x01;
		rumble.effects.rumble_l = rumble1;
		rumble.effects.rumble_r = rumble2;
		return api_transport_tx(phandle,(uint8_t*)&rumble,sizeof(rumble));
	}
}


static bool ps4_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	axis3i_t axis;
	bool ret = false;
	uint32_t key;
	uint16_t buttonl;
	int16_t x,y;
	ps4_report_t* ps4_inp = NULL;

	if((PS4_REPORT_ID == buf[0]) && (len >= 10)){
		ps4_inp = (ps4_report_t*)buf;
	}else if((PS4_BT_LARGE_REPORT_ID <= buf[0]) && (PS4_BT_LARGE_REPORT8_ID >= buf[0]) && (len >= 12)){
		ps4_inp = (ps4_report_t*)(buf+2);		//id不解析这里为了简单处理,实际是 ps4_large_report_t
	}

	if(NULL != ps4_inp){
		buttonl = SWAP16_L(ps4_inp->buttonl);
		key = buttonl | ((uint32_t)ps4_inp->buttonh << 16);
		key &= 0Xfffff0;
		key |= ps4_hatswitch_to_key(buttonl & 0X0F);
		keyp->key = key;
		keyp->stick_l.x = remap((int8_t)(ps4_inp->lx-0x80),-128,127,-32768,32767);
		keyp->stick_l.y = remap((int8_t)(ps4_inp->ly-0x80),-128,127,32767,-32768);
		keyp->stick_r.x = remap((int8_t)(ps4_inp->rx-0x80),-128,127,-32768,32767);
		keyp->stick_r.y = remap((int8_t)(ps4_inp->ry-0x80),-128,127,32767,-32768);

		keyp->l2 = ps4_inp->l2;
		keyp->r2 = ps4_inp->r2;

		if(len > 10){
			if(0 == (ps4_inp->touch_pad1_index & 0x80)){		//touch pad解析
				bit12_to_uint(ps4_inp->touch_pad1, &x,&y);
				// logd("pad:%x %d %d, \n",ps4_inp->touch_pad1_index,x,y);
				if(PS4_TOUCH_MAX_X/2 < x){
					keyp->key |= PS4_TOUCH_L;
				}else{
					keyp->key |= PS4_TOUCH_R;
				}
			}
			if(0 == (ps4_inp->touch_pad1_index & 0x80)){
				bit12_to_uint(ps4_inp->touch_pad2, &x,&y);
				if(PS4_TOUCH_MAX_X/2 < x){
					keyp->key |= PS4_TOUCH_L;
				}else{
					keyp->key |= PS4_TOUCH_R;
				}
			}
			
			axis = ps4_inp->acc;
			axis3i_swapl(&axis);
			keyp->acc.y = 0 - axis.x;
			keyp->acc.z = axis.y;
			keyp->acc.x = axis.z;

			axis = ps4_inp->gyro;
			axis3i_swapl(&axis);
			keyp->gyro.y = axis.x;
			keyp->gyro.z = 0 - axis.y;
			keyp->gyro.x = 0 - axis.z;
		}
		
		ret = true;
	}

	//ps4音频打开判断
	if(ret){
		if(0x03 == ps4_inp->ear_stu){
			// logd("ps host audio earphone in\n");
		}else{
			// logd("ps host audio earphone out\n");
		}
	}
	return ret;
}




static bool ps3_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	uint32_t key;
	ps3_report_t* ps3_inp = (ps3_report_t*)buf;
	if((0x01 == ps3_inp->id) && (0x00 == ps3_inp->res)){
		key = SWAP32_L(ps3_inp->button);

		keyp->key = key;

		keyp->stick_l.x = remap((int8_t)(ps3_inp->lx-0x80),-128,127,-32768,32767);
		keyp->stick_l.y = remap((int8_t)(ps3_inp->ly-0x80),-128,127,32767,-32768);
		keyp->stick_r.x = remap((int8_t)(ps3_inp->rx-0x80),-128,127,-32768,32767);
		keyp->stick_r.y = remap((int8_t)(ps3_inp->ry-0x80),-128,127,32767,-32768);

		keyp->l2 = ps3_inp->l2;
		keyp->r2 = ps3_inp->r2;
		ret = true;
	}

	return ret;
}


static bool ps5_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	uint32_t key;
	ps5_report_t* ps5_inp = (ps5_report_t*)buf;
	if((0x01 == ps5_inp->id)){
		key = ps5_inp->button ;
		key &= 0Xfffff0;
		key |= ps4_hatswitch_to_key(ps5_inp->button & 0X0F);

		keyp->key = key;

		keyp->stick_l.x = remap((int8_t)(ps5_inp->lx-0x80),-128,127,-32768,32767);
		keyp->stick_l.y = remap((int8_t)(ps5_inp->ly-0x80),-128,127,32767,-32768);
		keyp->stick_r.x = remap((int8_t)(ps5_inp->rx-0x80),-128,127,-32768,32767);
		keyp->stick_r.y = remap((int8_t)(ps5_inp->ry-0x80),-128,127,32767,-32768);
		
		keyp->l2 = ps5_inp->l2;
		keyp->r2 = ps5_inp->r2;
		ret = true;
	}
	return ret;
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/



/*******************************************************************
** Parameters:		
** Returns:	从0~7有效数据
** Description:		
*******************************************************************/
uint8_t ps4_key_to_hatswitch(uint32_t ps_key)
{
	uint32_t tmp;
	uint8_t ret = 0x08;

	tmp = (PS4_UP | PS4_DOWN | PS4_LEFT | PS4_RIGHT);
	tmp &= (ps_key);
	switch (tmp){
		case PS4_UP:
			ret =  0;
			break;
		case PS4_DOWN:
			ret =  4;
			break;
		case PS4_LEFT:
			ret =  6;
			break;
		case PS4_RIGHT:
			ret =  2;
			break;
		case PS4_RIGHT | PS4_UP:
			ret =  1;
			break;
		case PS4_RIGHT | PS4_DOWN:
			ret =  3;
			break;
		case PS4_LEFT | PS4_UP:
			ret =  7;
			break;
		case PS4_LEFT | PS4_DOWN:
			ret =  5;
			break;
	}
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	从0~7有效数据
** Description:		
*******************************************************************/
uint32_t ps4_hatswitch_to_key(uint8_t hat_switch)
{
	uint32_t tmp;
	tmp=0;

	switch (hat_switch){
		case 0:
			tmp = PS4_UP;
			break;
		case 4:
			tmp = PS4_DOWN;
			break;
		case 6:
			tmp = PS4_LEFT;
			break;
		case 2:
			tmp = PS4_RIGHT;
			break;
		case 1:
			tmp = PS4_RIGHT | PS4_UP;
			break;
		case 3:
			tmp = PS4_RIGHT | PS4_DOWN;
			break;
		case 7:
			tmp = PS4_LEFT | PS4_UP;
			break;
		case 5:
			tmp = PS4_LEFT | PS4_DOWN;
			break;
		default:
			tmp = 0;
			break;
	}

	return tmp;
}



/*******************************************************************
** Parameters:		
** Returns:
** Description:		
*******************************************************************/
uint8_t ps_touch_find_slot(uint32_t id,uint8_t active)
{
	uint8d_t find_slot = PS4_SLOT_MAX,i;
	static uint8_t touch_index = 0;

	if(0 == id) return PS4_SLOT_MAX;
	//find
	for(i=0; i< PS4_SLOT_MAX; i++){
		if(mcontact_id[i] == id){
			find_slot  = i;
			break;
		}
	}

	if((PS4_SLOT_MAX == find_slot) && active){		//auto slot
		for(i=0; i< PS4_SLOT_MAX; i++){
			if(mcontact_id[i] == ID_NULL){
				find_slot = i;
				mcontact_id[i] = id;
				m_ps_touch[i].id = touch_index++;
				break;
			}
		}
	}
	return find_slot;
}

/*******************************************************************
** Parameters:	id: 同一时间唯一的id才有效,可以为键值
				active: 1 开启触摸  0 关闭触摸
				x: 映射触摸板的x轴坐标
				y: 映射触摸板的y轴坐标  
** Returns:	设定成功返回true,失败返回false	
** Description:	设定PS4触摸功能
*******************************************************************/
bool  ps_touch_event_fill_id(uint8_t id,uint8_t active,int16_t x, int16_t y)
{
	uint8_t find_slot;

	find_slot = ps_touch_find_slot(id, active);
	if(PS4_SLOT_MAX != find_slot){
		// logd("id=0x%x find_slot=%x active=%d x=%d  y=%d",id,find_slot, active, x, y);

		if(!active){
			mcontact_id[find_slot] = ID_NULL;
		}else{
			m_ps_touch[find_slot].x = x;
			m_ps_touch[find_slot].y = y;
		}

		m_ps_touch[find_slot].active = active;

		return true;
	}

	return false;

}

__WEAK void ps_touch_pack(app_gamepad_key_t *keyp)
{
	if(keyp->key & PS4_TOUCH_U){
		ps_touch_event_fill_id(0, 1, PS4_TOUCH_MAX_X/2 - 100,0);
	}else{
		ps_touch_event_fill_id(0, 0,0,0);
	}
	if(keyp->key & PS4_TOUCH_D){
		ps_touch_event_fill_id(1, 1, PS4_TOUCH_MAX_X/2 + 100,PS4_TOUCH_MAX_Y);
	}else{
		ps_touch_event_fill_id(1, 0,0,0);
	}
	if(keyp->key & PS4_TOUCH_L){
		ps_touch_event_fill_id(2, 1,0, PS4_TOUCH_MAX_Y/2 + 50);
	}else{
		ps_touch_event_fill_id(2, 0,0,0);
	}
	if(keyp->key & PS4_TOUCH_R){
		ps_touch_event_fill_id(3, 1,PS4_TOUCH_MAX_X, PS4_TOUCH_MAX_Y/2 -50);
	}else{
		ps_touch_event_fill_id(3, 0,0,0);
	}
}

void ps_get_bt_mac(ps_bt_mac_t *pmac)
{
	ps_bt_mac_t ps_mac;

    memset(&ps_mac,0,sizeof(ps_mac));

	memcpy(ps_mac.mac, "\xFC\xD3\x1D\xB8\xA0\x1C", 6);	//for default
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID0, BT_EDR, ps_mac.mac);
	#elif BT1_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID1, BT_EDR, ps_mac.mac);
	#endif
	memcpy(ps_mac.res, "\x08\x25\x00", 3);
	// swap_buf(ps_mac.mac,6);              //MAC address is Big Endian
	// ps_mac.host_mac 
}

void ps_set_bt_link(ps_bt_link_t *plink)
{
	logd("bt link mac:");dumpd(plink->mac,6);
	logd("bt link key:");dumpd(plink->bt_link_key,16);
}


bool ps_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	bool ret = false;
	hid_type_t hid_type = phandle->index & 0XFF;

	if(HID_TYPE_PS3 == hid_type){
		ret = ps3_rumble_send(phandle, prumble);
	}else if(HID_TYPE_PS4 == hid_type){
		ret = ps4_rumble_send(phandle, prumble);
	}else if(HID_TYPE_PS5 == hid_type){
		ret = ps5_rumble_send(phandle, prumble);
	}
	return ret;
}
uint16_t ps_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	hid_type_t hid_type = phandle->index & 0XFF;

	if(HID_TYPE_PS3 == hid_type){
		packet_len = ps3_key_pack(phandle, keyp, buf, len);
	}else if(HID_TYPE_PS4 == hid_type){
		packet_len = ps4_key_pack(phandle, keyp, buf, len);
	}else if(HID_TYPE_PS5 == hid_type){
		packet_len = ps5_key_pack(phandle, keyp, buf, len);
	}
	return packet_len;
}

/*******************************************************************
** Parameters:		
** Returns:
** Description:	ps 手柄数据处理	
*******************************************************************/
bool ps_dev_process(trp_handle_t* phandle, uint8_t* buf,uint8_t len)
{
    bool ret = false;

    if ( 0 ==  len) return ret;
	
    switch (buf[0]){
     case PS4_BT_EFFECTS_ID:{           //0x11
	 	#if APP_RUMBLE_ENABLE
		ps4_bt_effects_t *peffects;
        peffects = (ps4_bt_effects_t*)buf;
		app_rumble_set_duty(RUMBLE_L,peffects->effects.rumble_l,10000);
		app_rumble_set_duty(RUMBLE_R,peffects->effects.rumble_r,10000);		
		#endif
		m_ps_enhanced_mode =  true;		//蓝牙手柄模式接收到马达/led后可以切换到large_report模式
        ret = true;
        break;
	 }
    case PS4_USB_EFFECTS_ID:{           //0X05
		#if APP_RUMBLE_ENABLE
		ps4_usb_effects_t *peffects;
        peffects = (ps4_usb_effects_t*)buf;
        app_rumble_set_duty(RUMBLE_L,peffects->effects.rumble_l,10000);
		app_rumble_set_duty(RUMBLE_R,peffects->effects.rumble_r,10000);

		#if API_AUDIO_ENABLE && API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO))
		if(peffects->effects.volume_l || peffects->effects.volume_r){
			api_audio_spk_set_vol(0, &usbd_audio_info,peffects->effects.volume_l, peffects->effects.volume_r);
		}
		#endif
		#endif
		
        ret = true;
        break;
	}
    default:
        break;
    }
	return ret;
}




bool ps_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	hid_type_t hid_type = phandle->index & 0XFF;

	if(HID_TYPE_PS3 == hid_type){
		ret = ps3_key_decode(phandle,buf, len, keyp);
	}else if(HID_TYPE_PS4 == hid_type){
		ret = ps4_key_decode(phandle,buf, len, keyp);
	}else if(HID_TYPE_PS5 == hid_type){
		ret = ps5_key_decode(phandle,buf, len, keyp);
	}
	return ret;
}

bool ps_in_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len)
{
	return false;
}

/*******************************************************************
** Parameters:		
** Returns:
** Description:
*******************************************************************/
void ps_controller_init(trp_handle_t *phandle)
{
	if(api_trp_is_slave(phandle->trp)){
		#if (HIDD_SUPPORT & HID_PS_MASK)
		pskey_auto_count = 0;
		m_ps_series = PS_SERIES_NONE;
		m_ps_enhanced_mode = false;
		memset(&mcontact_id, ID_NULL , sizeof(mcontact_id));
		memset(&m_ps_touch, 0 , sizeof(m_ps_touch));
		
		#if defined PS_P2_ENCRYPT_ENABLED || defined PS_7105_ENCRYPT_ENABLED
		ps_encrypt_init();
		#endif

		#endif
	}
}

void ps_controller_deinit(trp_handle_t *phandle)
{
	if(api_trp_is_slave(phandle->trp)){
		pskey_auto_count = 0;
		#if (HIDD_SUPPORT & HID_PS_MASK)
		m_ps_enhanced_mode = false;
		#endif
	}
}

void ps_controller_task(trp_handle_t *phandle)
{
}


#endif




