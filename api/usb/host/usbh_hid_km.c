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
#if USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))
#include "utils/emf_utils.h"
#include "api/usb/host/usbh.h"
#include "km_typedef.h"
#include "app_km.h"
#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
kb_t usbh_kb;
app_mouse_t usbh_mouse;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
km_items_t m_km_items[MAX_KM_ITEMS_NUM];


/*****************************************************************************************************
**	static Function
******************************************************************************************************/
void usbh_hid_km_pa_init(void)
{
    memset(&m_km_items, 0, sizeof(m_km_items));
    memset(&usbh_kb, 0, sizeof(usbh_kb));
    memset(&usbh_mouse, 0, sizeof(usbh_mouse));
}

km_items_t* malloc_hid_km_items(void)
{
	uint8d_t i;
	for(i=0; i<countof(m_km_items); i++){
		if(m_km_items[i].magic == 0){
			memset(&m_km_items[i],0,sizeof(m_km_items[i]));
            m_km_items[i].magic = 1;
			return &m_km_items[i];
		}
	}
	return NULL;
}





/*******************************************************************
** Parameters:		
** Returns:	
** Description:	keyboard set led 
*******************************************************************/
void usbh_hid_kb_set_led(kb_led_t *pled, uint8_t* pkb, uint8_t len) //TODO app_km call
{
	kb_led_t led,set_port;

	uint8_t i,id = USBH_NULL;
	usbh_dev_t *pdev = (usbh_dev_t *)m_usbh_dev;
	usbh_class_t *pos;
    hid_items_t* pitem;

    led.val = 0;
	for(i=1; i<len; i++){
		if(KB_NUM == pkb[i]) led.bit.numLock = 1;
		if(KB_CAP_LOCK == pkb[i]) led.bit.capsLock = 1;
		if(KB_SCROLL == pkb[i]) led.bit.scrollLock = 1;
	}

	if(pled->val != led.val){
		set_port.val = (pled->val ^ led.val) & led.val;
		pled->val = led.val;

        for(i = 0; i < USBH_MAX_PORTS * (HUB_MAX_PORTS+1); i++,pdev++){
            list_for_each_entry(pos,&pdev->class_list,usbh_class_t,list){
                if((pos->dev_type == DEV_TYPE_HID) && (pos->hid_type == HID_TYPE_KB)){
                    id = (i / (HUB_MAX_PORTS+1) <<4) | (i % (HUB_MAX_PORTS+1));

                    pitem = &((km_items_t*)pos->pdata)->items.kb.led;
                    if( pitem->report_length){
                        logd("usbh%d kb id%d set report=%x\n",id,pitem->report_id,set_port.val);
                        usbh_hid_set_report(id, pos->itf.if_num, HID_REPORT_TYPE_OUTPUT, pitem->report_id, &set_port.val,1);  
                    }
                }
            }
        }
    }
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t kb_get_normal_key(hid_items_t *pitem, uint8_t* pkb, uint8_t kb_len, uint8_t* buf, uint16_t len)
{
    uint8_t i,j,k=0;
    uint8_t val;
    if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
        if(1 == pitem->bit_size){
            for(i = 0,k = 0; i < (pitem->bit_count * pitem->bit_size + 7)/8; i++){
                val = buf[pitem->bit_offset / 8];
                if(0 == val) continue;
                if(k > kb_len) break;
                for(j=0; j<8; j++){
                    if(val & (0x01<<j)){
                        if(k > kb_len) break;
                        pkb[k++] = pitem->usageMinimum + i*8 + j;
                    }
                }
            }
        }else if(8 == pitem->bit_size){
            k = MIN(pitem->bit_count, kb_len-1);
            memcpy(pkb, &buf[pitem->bit_offset / 8], MIN(pitem->bit_count, kb_len-1));
        }
    }
    return k;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdata used storage hub port numbers	
*******************************************************************/
void usbh_hid_km_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
    km_items_t *pkm_items = (km_items_t*)pclass;
    hid_items_t *pitem;
    uint8_t i;
    kb_t kb;
    app_mouse_t mouse;

    if(NULL == pkm_items) return;

    if(HID_TYPE_KB == pclass->hid_type){

        memset(&kb,0, sizeof(kb));

        pitem = &pkm_items->items.kb.kb_fun;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            kb.fn = buf[pitem->bit_offset / 8];
        }
        pitem = &pkm_items->items.kb.kb_normal;
        i = kb_get_normal_key(pitem, kb.key+1, sizeof(kb.key), buf, len);
        pitem = &pkm_items->items.kb.kb2_normal;
        kb_get_normal_key(pitem, kb.key+1+i, sizeof(kb.key)-i, buf, len);

        pitem = &pkm_items->items.kb.led;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            kb.fn = buf[pitem->bit_offset / 8];
        }

        usbh_kb = kb;
        logd("usbh kb in:");dumpd(&kb,sizeof(kb));
    }else if(HID_TYPE_MOUSE == pclass->hid_type){

        memset(&mouse,0, sizeof(mouse));
        pitem = &pkm_items->items.mouse.button;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            mouse.but = buf[pitem->bit_offset / 8];
        }

        pitem = &pkm_items->items.mouse.x;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            if(pitem->bit_size == 12){
                bit12_to_int(&buf[pitem->bit_offset / 8], &mouse.x, &mouse.y);
            }else{
                mouse.x = ((int16_t)buf[pitem->bit_offset / 8 + 1] << 8) | buf[pitem->bit_offset / 8];
                mouse.x = ((int16_t)buf[pitem->bit_offset / 8 + 3] << 8) | buf[pitem->bit_offset / 8 + 2];
            }
        }

        pitem = &pkm_items->items.mouse.y;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            mouse.x = buf[(pitem->bit_offset + 7) / 8];
        }

        pitem = &pkm_items->items.mouse.w;
        if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
            mouse.w = buf[pitem->bit_offset / 8];
        }
        usbh_mouse.but = mouse.but;
        usbh_mouse.x += mouse.x;
        usbh_mouse.y += mouse.y;
        usbh_mouse.w += mouse.w;
        logd("usbh mouse in:");dumpd(&mouse,sizeof(mouse));
    }
}


error_t usbh_hid_km_open( uint8_t id, usbh_class_t *pclass) 
{
    return ERROR_SUCCESS;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_kb_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo)
{
    uint8_t i;
    error_t err = ERROR_NOT_FOUND;
    km_items_t item;

    memset(&item, 0, sizeof(item));
    item.magic = 1;
    
    for(i=0; i<pinfo->reportItems; i++){
        if(0 == item.items.kb.kb_fun.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X07, 0XE0,&item.items.kb.kb_fun)){
                logd("usbh kb_fun:");
                hid_items_dump(&item.items.kb.kb_fun);
                err = ERROR_SUCCESS;
            }
        }

        if(0 == item.items.kb.kb_normal.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.items.kb.kb_normal)){
                logd("usbh kb_normal:");
                hid_items_dump(&item.items.kb.kb_normal);
                err = ERROR_SUCCESS;
            }
        }else if(0 == item.items.kb.kb2_normal.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.items.kb.kb2_normal)){
                logd("usbh kb2_normal:");
                hid_items_dump(&item.items.kb.kb2_normal);
                err = ERROR_SUCCESS;
            }
        }

        if(0 == item.items.kb.led.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_OUTPUT,0X07, KB_A, &item.items.kb.led)){
                logd("usbh kb led:");
                hid_items_dump(&item.items.kb.led);
            }
        }
    }
    
    if(err){
        km_items_t *pitem;
        pitem = malloc_hid_km_items();
        if(NULL != pitem){
            *pitem = item;
            pclass->pdata = pitem;
            pclass->hid_type = HID_TYPE_KB;
        }else{
            err = ERROR_NO_MEM;
        }
    }

    return err;
}

error_t usbh_hid_mouse_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo)
{
    uint8_t i;
    error_t err = ERROR_NOT_FOUND;
    km_items_t item;

    memset(&item, 0, sizeof(item));
    item.magic = 1;
    
    for(i=0; i<pinfo->reportItems; i++){
        if(0 == item.items.mouse.button.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X09, 0X01,&item.items.mouse.button)){
                logd("mouse button:");
                hid_items_dump(&item.items.mouse.button);
                err = ERROR_SUCCESS;
            }
        }

        if(0 == item.items.mouse.x.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X01, 0X30, &item.items.mouse.x)){
                logd("mouse x:");
                hid_items_dump(&item.items.mouse.x);
                err = ERROR_SUCCESS;
            }
        }

        if(0 == item.items.mouse.y.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X01, 0X31, &item.items.mouse.y)){
                logd("mouse y:");
                hid_items_dump(&item.items.mouse.y);
                err = ERROR_SUCCESS;
            }
        }

        if(0 == item.items.mouse.w.report_length){
            if(hid_find_items(pinfo,i,HID_REPORT_TYPE_INPUT,0X01, 0X38, &item.items.mouse.w)){
                logd("mouse w:");
                hid_items_dump(&item.items.mouse.w);
                err = ERROR_SUCCESS;
            }
        }
    }
    
    if(ERROR_SUCCESS == err){
        km_items_t *pitem;
        pitem = malloc_hid_km_items();
        if(NULL != pitem){
            *pitem = item;
            pclass->pdata = pitem;
            pclass->hid_type = HID_TYPE_MOUSE;
        }else{
            err = ERROR_NO_MEM;
        }
    }

    return err;
}


error_t usbh_hid_km_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo)
{
    error_t err = ERROR_NOT_FOUND;
    err = usbh_hid_kb_init(id, pclass, pinfo);
    if(err){
        err = usbh_hid_mouse_init(id, pclass, pinfo);
    }
    return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_hid_km_deinit( uint8_t id, usbh_class_t *pclass) 
{
    if(NULL != pclass->pdata){
        if( ((uintptr_t)pclass->pdata >= (uintptr_t)&m_km_items[0]) 
            && ((uintptr_t)pclass->pdata <= (uintptr_t)&m_km_items[MAX_KM_ITEMS_NUM]) ){
            
            if(HID_TYPE_KB == pclass->hid_type){
                memset(&usbh_kb, 0, sizeof(usbh_kb));
            }
            if(HID_TYPE_MOUSE == pclass->hid_type){
                memset(&usbh_mouse, 0, sizeof(usbh_mouse));
            }

            memset(pclass->pdata, 0, sizeof(km_items_t));       //释放内存, 注意内存溢出
        }
    }
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_hid_km_task(uint8_t id, usbh_class_t *pclass)
{
    UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}

#endif






