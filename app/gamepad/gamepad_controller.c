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
#if (HIDD_SUPPORT | HIDH_SUPPORT) & (HID_GAMEPAD_MASK)
#include  "gamepad_controller.h"


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


/*********************************************************************************
** Function:
** Parameters:
** Returns:
** details: 从0~7有效数据
** Modified:
**
*********************************************************************************/
uint8_t gamepad_key_to_hatswitch(uint32_t value)
{
	uint32_t tmp;
	uint8_t ret = 0xf;

	tmp = (GAMEPAD_UP | GAMEPAD_DOWN | GAMEPAD_LEFT | GAMEPAD_RIGHT);
	tmp &= (value);
	switch (tmp){
		case GAMEPAD_UP:
			ret =  0;
			break;
		case GAMEPAD_DOWN:
			ret =  4;
			break;
		case GAMEPAD_LEFT:
			ret =  6;
			break;
		case GAMEPAD_RIGHT:
			ret =  2;
			break;
		case GAMEPAD_RIGHT | GAMEPAD_UP:
			ret =  1;
			break;
		case GAMEPAD_RIGHT | GAMEPAD_DOWN:
			ret =  3;
			break;
		case GAMEPAD_LEFT | GAMEPAD_UP:
			ret =  7;
			break;
		case GAMEPAD_LEFT | GAMEPAD_DOWN:
			ret =  5;
			break;
	}

	return ret;
}

/*********************************************************************************
** Function:
** Parameters:
** Returns:
** details:从0~7有效数据
** Modified:
**
*********************************************************************************/
uint32_t gamepad_hatswitch_to_key(uint8_t hat_switch)
{
	uint32_t tmp;

	tmp=0;

	switch (hat_switch){
		case 0:
			tmp = GAMEPAD_UP;
			break;
		case 4:
			tmp = GAMEPAD_DOWN;
			break;
		case 6:
			tmp = GAMEPAD_LEFT;
			break;
		case 2:
			tmp = GAMEPAD_RIGHT;
			break;
		case 1:
			tmp = GAMEPAD_RIGHT | GAMEPAD_UP;
			break;
		case 3:
			tmp = GAMEPAD_RIGHT | GAMEPAD_DOWN;
			break;
		case 7:
			tmp = GAMEPAD_LEFT | GAMEPAD_UP;
			break;
		case 5:
			tmp = GAMEPAD_LEFT | GAMEPAD_DOWN;
			break;
		default:
			tmp = 0;
			break;
	}
	return tmp;
}

#if HIDD_SUPPORT & (BIT_ENUM(HID_TYPE_GAMEPADE) | BIT_ENUM(HID_TYPE_DINPUT))
uint16_t gamepad_key_pack(trp_handle_t *phandle,app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
	gamepad_retport_t* preport = (gamepad_retport_t*)buf;
	
	if(len < sizeof(gamepad_retport_t)) return 0;
	memset(preport,0,sizeof(gamepad_retport_t));
	preport->id = GAMEPAD_REPORT_ID;
	preport->button = keyp->key & 0xffff;
	preport->hat_switch = gamepad_key_to_hatswitch(keyp->key);
	preport->lx = (keyp->stick_l.x >> 8) + 0x80;
	preport->ly =(keyp->stick_l.y >> 8) + 0x80;
	preport->rx = (keyp->stick_r.x >> 8) + 0x80;
	preport->ry =(keyp->stick_r.y >> 8) + 0x80;

	preport->l2 = keyp->l2 >> 7;
	preport->r2 = keyp->r2 >> 7;

	packet_len = sizeof(gamepad_retport_t);

	return packet_len;
}

bool gamepad_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len)
{
	uint8_t edr_hid_req = 0;
	
	if(TR_EDR == phandle->trp){		//edr 判断 hid_report_type_t
		edr_hid_req = buf[0];
		len -= 1;
		buf += 1;
		if ( 0 == len) return false;
	}
	return false;
}

bool gamepad_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	return true;
}



bool gamepad_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_gamepad_key_t*keyp)
{
	bool ret = false;
	uint32_t key;
	gamepad_retport_t* dinput_inp = (gamepad_retport_t*)buf;
	 
	if(GAMEPAD_REPORT_ID == buf[0]){
		key = SWAP16_L(dinput_inp->button);
		key |= gamepad_hatswitch_to_key(dinput_inp->hat_switch);

		keyp->key = key;

		keyp->stick_l.x = remap((int8_t)(dinput_inp->lx-0x80),-128,127,-32768,32767);
		keyp->stick_l.y = remap((int8_t)(dinput_inp->ly-0x80),-128,127,-32768,32767);
		keyp->stick_r.x = remap((int8_t)(dinput_inp->rx-0x80),-128,127,-32768,32767);
		keyp->stick_r.y = remap((int8_t)(dinput_inp->ry-0x80),-128,127,-32768,32767);

		
		keyp->l2 = remap(dinput_inp->l2,0,0XFF,0,32767);
		keyp->r2 = remap(dinput_inp->r2,0,0XFF,0,32767);
		ret = true;
	}
	return ret;
}

bool gamepad_in_process(trp_handle_t* phandle, uint8_t *buf, uint8_t len)
{
	bool ret = false;
	return ret;
}
/*******************************************************************
** Function: 没有必要注释的函数可以不注释函数说明
** Parameters:
** Returns:
*******************************************************************/
void gamepad_controller_init(trp_handle_t *phandle)
{
}

void gamepad_controller_deinit(trp_handle_t *phandle)
{
}

void gamepad_controller_task(trp_handle_t *phandle)
{
}
#endif

#endif






