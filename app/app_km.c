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
#include  "hw_config.h"
#if APP_KM_ENABLE

#include  "api/api_tick.h"
#include  "app/app_km.h"
#include  "app/io_keyboard.h"
#include  "api/api_system.h"
#include  "api/api_commander.h"
#include  "app/app_command.h"
#include "api/hid/hid_dev_desc.h"

#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))) && (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
#include "api/usb/host/usbh_hid_km.h"
#endif
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
app_km_t m_app_km;

#if APP_BIT_KB_ENABLE
kb_bit_t m_kb_bit;			//全键无冲突
#endif

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_bitkb_to_stdkb(kb_bit_t* pbit, kb_t* out)
{
	uint8_t i,v,k=0; 

	memset(out, 0, sizeof(kb_t));
	out->fn = pbit->fn;
	for(i= 0; i<sizeof(pbit->key); i++){
		for (v= 0; v<8; v++){
			if(pbit->key[i] & (0x01<<v)){
				if(k < sizeof(out->key)){
					out->key[k++] = i*8 + v;
				}
			}
		}
	}
}


void app_fill_bitkb(uint8_t key, kb_bit_t* pbit)
{
	if(key){
		if(key & 0XE0 == 0XE0) {
			pbit->fn |= 0X01 << (key-0xE0);
		}else{
			uint8_t index = key / 8;
			if(index < sizeof(pbit->key)){
				pbit->key[index] |= 0X01 << (key % 8);
			}
		}
	}
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	合并km数据, 将不同数据来源的数据合并到一起
*******************************************************************/
bool app_km_fill(app_km_t* pkey, app_km_t* pkey_in)
{
	uint8_t i,k=0;
	if(pkey_in->active){
		pkey->active = true;

		pkey->kb.fn |= pkey_in->kb.fn;

		for(i=0; i<sizeof(pkey->kb.key); i++){
			if(0 == pkey->kb.key[i]){
				if(0 == pkey_in->kb.key[k]) break;
				pkey->kb.key[i] = pkey_in->kb.key[k++];
			}
		}

		pkey->mouse.but |= pkey_in->mouse.but;
		pkey->mouse.x += pkey_in->mouse.x;
		pkey->mouse.y += pkey_in->mouse.y;
		pkey->mouse.w += pkey_in->mouse.w;
	}
	return pkey_in->active;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	缓存km数据, 将相同数据来源的数据暂存起来, 主要是鼠标数据特殊处理
*******************************************************************/
bool app_km_cache(app_km_t* pkey, app_km_t* pkey_in)
{
	uint8_t i;
	if(pkey_in->active){
		pkey->active = true;

		pkey->kb = pkey_in->kb;
		pkey->mouse.but = pkey_in->mouse.but;
		pkey->mouse.x += pkey_in->mouse.x;
		pkey->mouse.y += pkey_in->mouse.y;
		pkey->mouse.w += pkey_in->mouse.w;
	}
	return pkey_in->active;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	消费app km 数据, 清除active和鼠标数据
*******************************************************************/
void app_km_clean(app_km_t* pkey)
{
	uint8_t tmp_but = pkey->mouse.but;

	pkey->active = false;
	memset(&pkey->mouse, 0 ,sizeof(pkey->mouse));
	pkey->mouse.but = tmp_but;
}

#if WEAK_ENABLE
__WEAK void app_km_vendor_scan(app_km_t* pkey, kb_bit_t *pkey_bit)
{
	#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))) && (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
	API_ENTER_CRITICAL();

	if(app_km_fill(pkey, &usbh_km)){
		app_km_clean(&usbh_km);
	}
	API_EXIT_CRITICAL();
	#endif
}
#endif


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_init(void)
{
	memset(&m_app_km, 0, sizeof(m_app_km));
	#if APP_BIT_KB_ENABLE
	memset(&m_kb_bit, 0, sizeof(m_kb_bit));
	#endif
	
	io_keyboard_init();
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_km_deinit(void)
{
	io_keyboard_deinit();
	return true;
}



void app_km_scan_task(void*pa)
{
    app_km_t key;
	#if APP_BIT_KB_ENABLE
	kb_bit_t kb_bit;			//全键无冲突
	kb_bit_t* pkb_bit = &kb_bit;
	
	memset(&kb_bit, 0, sizeof(kb_bit));
	#else
	kb_bit_t* pkb_bit = NULL;
	#endif

	memset(&key,0,sizeof(key));
	
	io_keyboard_scan(&key.kb, pkb_bit);
	if(memcmp(&key.kb, &m_app_km.kb, sizeof(app_kb_t))){
		m_app_km.active = true;
	}
	app_km_vendor_scan(&key,pkb_bit);
	app_km_cache(&m_app_km, &key);

	#if APP_BIT_KB_ENABLE
	m_kb_bit = kb_bit;
	#endif

	UNUSED_PARAMETER(pa);
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_km_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_km_scan_task(NULL);
	}
}
#endif


#endif

#include "km_typedef.h"
#include "api/api_transport.h"
#include "api/hid/hid_dev_desc.h"
#include "api/api_log.h"
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	km report
	mouse dir: 右移动x正,下移动y是正
		0--------------->x
		|
		|
		|
		|
	   \|/
	   y
*******************************************************************/
#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)	//TODO
bool app_mouse_key_send(trp_handle_t *phandle,app_mouse_t *pmouse)
{
	mouse_t mouse;
	mouse.id = MOUSE_REPORT_ID;
	mouse.but = pmouse->but;
	mouse.x = SWAP16_L(pmouse->x);
	mouse.y = SWAP16_L(pmouse->y);
	mouse.w = SWAP16_L(pmouse->w);
	//logd("mouse:%x %d %d\n",pmouse->but,pmouse->x,pmouse->y);
	return api_transport_tx(phandle, &mouse, sizeof(mouse));	
}
#endif
#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_KB)
bool app_kb_key_send(trp_handle_t *phandle,app_kb_t *pkey)
{
	kb_bit_t kb;    

	kb.id = KB_REPORT_ID;
	kb.fn = pkey->fn;
	memcpy(kb.key,pkey->key, sizoef(kb.key));
	return api_transport_tx(phandle,&kb, sizeof(kb));
}
#endif


