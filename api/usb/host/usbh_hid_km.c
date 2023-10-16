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
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA)))
#include "utils/emf_utils.h"
#include "api/usb/host/usbh.h"
#include "app/km_typedef.h"
#include "app/app_km.h"
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
km_items_t m_km_items[USBH_NUM][MAX_KM_ITEMS_NUM];


/*****************************************************************************************************
**	static Function
******************************************************************************************************/
void usbh_hid_km_pa_init(uint8_t id)
{
    memset(&m_km_items[id>>4], 0, sizeof(m_km_items[id>>4]));
    memset(&usbh_kb, 0, sizeof(usbh_kb));
    memset(&usbh_mouse, 0, sizeof(usbh_mouse));
}

km_items_t* malloc_hid_km_items(uint8_t id)
{
	uint8d_t i;

    id >>= 4;
	for(i=0; i<countof(m_km_items); i++){
		if(m_km_items[id][i].magic == 0){
			memset(&m_km_items[id][i],0,sizeof(m_km_items[id][i]));
            m_km_items[id][i].magic = 1;
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
		if(KB_NUM == pkb[i]) led.bits.numLock = 1;
		if(KB_CAP_LOCK == pkb[i]) led.bits.capsLock = 1;
		if(KB_SCROLL == pkb[i]) led.bits.scrollLock = 1;
	}

	if(pled->val != led.val){
		set_port.val = (pled->val ^ led.val) & led.val;
		pled->val = led.val;

        for(i = 0; i < USBH_NUM * (HUB_MAX_PORTS+1); i++,pdev++){
            list_for_each_entry_type(pos,&pdev->class_list,usbh_class_t,list){
                if((pos->dev_type == DEV_TYPE_HID) && (pos->hid_type == HID_TYPE_KB)){
                    id = (i / (HUB_MAX_PORTS+1) <<4) | (i % (HUB_MAX_PORTS+1));

                    pitem = &((km_items_t*)pos->pdat)->kb.led;
                    if( pitem->report_length){
                        logd("usbh%x kb id%d set report=%x\n",id,pitem->report_id,set_port.val);
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
        memcpy(pkb, &buf[pitem->bit_offset / 8], MIN(pitem->bit_count, kb_len));
    }
    
    return k;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_hid_km_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
    km_items_t *pkm_items = (km_items_t*)pclass->pdat;
    hid_items_t *pitem;
    bool match_id = false;
    uint8_t i;
    kb_t kb;
    app_mouse_t mouse;

    if(NULL == pkm_items) return;

    if(pkm_items->kb.find && !match_id){
        memset(&kb,0, sizeof(kb));

        pitem = &pkm_items->kb.kb_fun;
        if(hid_items_match_id(pitem, buf, len)){
            kb.fn = buf[pitem->bit_offset / 8];
            match_id = true;
        }
        pitem = &pkm_items->kb.kb_normal;
        if(hid_items_match_id(pitem, buf, len)){
            i = kb_get_normal_key(pitem, kb.key, sizeof(kb.key), buf, len);
            match_id = true;
        }

        pitem = &pkm_items->kb.kb2_normal;
        if(hid_items_match_id(pitem, buf, len)){
            kb_get_normal_key(pitem, kb.key+i, sizeof(kb.key)-i, buf, len);
            match_id = true;
        }

        pitem = &pkm_items->kb.led;
        if(hid_items_match_id(pitem, buf, len)){
            kb.fn = buf[pitem->bit_offset / 8];
            match_id = true;
        }
        if(match_id){
            usbh_kb = kb;
            logd("usbh kb in:");dumpd(&kb,sizeof(kb));
        }
    }

    if(pkm_items->mouse.find && !match_id){
        memset(&mouse,0, sizeof(mouse));

        pitem = &pkm_items->mouse.button;
        if(hid_items_match_id(pitem, buf, len)){
            mouse.but = buf[pitem->bit_offset / 8];
            match_id = true;
        }

        pitem = &pkm_items->mouse.x;
        if(hid_items_match_id(pitem, buf, len)){
            mouse.x = bits_to_int(pitem->bit_offset, pitem->bit_size,  buf, len);
        }

        pitem = &pkm_items->mouse.y;
        if(hid_items_match_id(pitem, buf, len)){
            mouse.y = bits_to_int(pitem->bit_offset, pitem->bit_size,  buf, len);
        }

        pitem = &pkm_items->mouse.w;
        if(hid_items_match_id(pitem, buf, len)){
            mouse.w = bits_to_int(pitem->bit_offset, pitem->bit_size,  buf, len);
            match_id = true;
        }

        if(match_id){
            usbh_mouse.but = mouse.but;
            usbh_mouse.x += mouse.x;
            usbh_mouse.y += mouse.y;
            usbh_mouse.w += mouse.w;
            logd("usbh mouse in:");dumpd(&mouse,sizeof(mouse));
        }
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
error_t usbh_hid_kb_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo, kb_items_t *pitems)
{
    uint8_t i;
    error_t err = ERROR_NOT_FOUND;
    hid_collection_t* pcollection;
    kb_items_t item;

    memset(&item, 0, sizeof(item));

    for(i=0; i<pinfo->collections; i++){
        if(hid_match_collection(pinfo, i, 0X01, 0X06)){ //0x05,0x01  0x09,0X06 fix kb
            pcollection = &pinfo->item_list.collectionList[i];
            if(0 == item.kb_fun.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X07, 0XE0,&item.kb_fun)){
                    logd("usbh kb_fun:");
                    hid_items_dump(&item.kb_fun);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.kb_normal.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.kb_normal)){
                    logd("usbh kb_normal:");
                    hid_items_dump(&item.kb_normal);
                    err = ERROR_SUCCESS;
                }
            }else if(0 == item.kb2_normal.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.kb2_normal)){
                    logd("usbh kb2_normal:");
                    hid_items_dump(&item.kb2_normal);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.led.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_OUTPUT,0X07, KB_A, &item.led)){
                    logd("usbh kb led:");
                    hid_items_dump(&item.led);
                }
            }
        }
    }

    if(ERROR_SUCCESS == err){
        item.find = 1;
        *pitems = item;
        logd("usbh find kb...\n");
    }
    return err;
}

error_t usbh_hid_mouse_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo, mouse_items_t *pitem)
{
    uint8_t i;
    error_t err = ERROR_NOT_FOUND;
    hid_collection_t* pcollection;
    mouse_items_t item;
    
    memset(&item, 0, sizeof(item));

    for(i=0; i<pinfo->collections; i++){
        if(hid_match_collection(pinfo, i, 0X01, 0X02)){ //0x05,0x01  0x09,0X02 fix mouse
        logd("mouse match collection %d",i);

            pcollection = &pinfo->item_list.collectionList[i];
            if(0 == item.button.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X09, 0X01,&item.button)){
                    logd("mouse button:");
                    hid_items_dump(&item.button);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.x.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X01, 0X30, &item.x)){
                    logd("mouse x:");
                    hid_items_dump(&item.x);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.y.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X01, 0X31, &item.y)){
                    logd("mouse y:");
                    hid_items_dump(&item.y);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.w.report_length){
                if(hid_collection_find_items(pinfo,pcollection,HID_REPORT_TYPE_INPUT,0X01, 0X38, &item.w)){
                    logd("mouse w:");
                    hid_items_dump(&item.w);
                    err = ERROR_SUCCESS;
                }
            }
        }
    }

    if(ERROR_SUCCESS == err){
        item.find = 1;
        *pitem = item;
        logd("usbh find mouse...\n");
    }
    return err;
}


error_t usbh_hid_km_init(uint8_t id, usbh_class_t *pclass, hid_desc_info_t *pinfo)
{
    error_t err = ERROR_NOT_FOUND;
    km_items_t *pitem;

    pitem = malloc_hid_km_items(id);
    if(NULL != pitem){
        memset(pitem, 0, sizeof(km_items_t));
   
        usbh_hid_mouse_init(id, pclass, pinfo,&pitem->mouse);
        usbh_hid_kb_init(id, pclass, pinfo,&pitem->kb);

        if(pitem->mouse.find || pitem->kb.find){
            pitem->magic = 1;
            pclass->pdat = pitem;
            if(HID_ITF_PROTOCOL_MOUSE == pclass->itf.if_pro){
                pclass->hid_type = HID_TYPE_MOUSE;
            }else{
                pclass->hid_type = HID_TYPE_KB;
            }
            err = ERROR_SUCCESS;
        }
    }else{
        err = ERROR_NO_MEM;
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
    if(NULL != pclass->pdat){
        if( ((uintptr_t)pclass->pdat >= (uintptr_t)&m_km_items[0]) 
            && ((uintptr_t)pclass->pdat <= (uintptr_t)&m_km_items[MAX_KM_ITEMS_NUM]) ){
            
            if(HID_TYPE_KB == pclass->hid_type){
                memset(&usbh_kb, 0, sizeof(usbh_kb));
            }
            if(HID_TYPE_MOUSE == pclass->hid_type){
                memset(&usbh_mouse, 0, sizeof(usbh_mouse));
            }

            memset(pclass->pdat, 0, sizeof(km_items_t));       //释放内存, 注意内存溢出
        }
    }
	return ERROR_SUCCESS;
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






