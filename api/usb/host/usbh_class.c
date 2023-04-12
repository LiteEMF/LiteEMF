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
#if APP_USBH_ENABLE
#include "api/usb/host/usbh_class.h"
#include "api/usb/host/usbh.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
usbh_class_t usbh_dev_class[8];
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_class_buf_init(void)
{
	uint8_t i;
	memset(&usbh_dev_class, 0 ,sizeof(usbh_dev_class));
	for(i=0; i<countof(usbh_dev_class); i++){
		usbh_dev_class[i].id = USBH_NULL;
	}
}
usbh_class_t* malloc_usbh_class(void)
{
	uint8_t i;
	for(i=0; i<countof(usbh_dev_class); i++){
		if(usbh_dev_class[i].id == USBH_NULL){
			memset(&usbh_dev_class[i],0,sizeof(usbh_class_t));
			usbh_dev_class[i].id = i;
			return &usbh_dev_class[i];
		}
	}
	return NULL;
}
void free_usbh_class(usbh_class_t* pclass)
{
	if(NULL != pclass) pclass->id = USBH_NULL;
}

/*******************************************************************
** Parameters:
** Returns:	
** Description:	
*******************************************************************/
usbh_class_t* get_usbh_class(uint8_t class_id)   
{
	uint8_t i;
	if(class_id >= countof(usbh_dev_class)) return NULL;
	if(USBH_NULL == usbh_dev_class[class_id].id) return NULL;	//未分配
	return &usbh_dev_class[class_id];
}

/*******************************************************************
** Parameters:	sub_type: hid 设备: hid_type_t, 其他设备传入sub_ift
** Returns:	
** Description:	这里没有处理interface alt set 很少设备使用到
*******************************************************************/
usbh_class_t *usbh_class_find_by_type(uint8_t id, dev_type_t type,uint8_t sub_type)
{
	error_t err = ERROR_NOT_FOUND;
	usbh_dev_t* pdev = get_usbh_dev(id);
	usbh_class_t *pclass = NULL;

	list_for_each_entry(pclass,&pdev->class_list,usbh_class_t,list){
		if(pclass->dev_type == type){
			if(DEV_TYPE_HID == type){
				if(sub_type != pclass->hid_type) continue;
				if(HID_TYPE_XBOX == sub_type){		//xbox特殊处理,xbox音频不在这里处理
					if(pclass->endpin.type != USB_ENDP_TYPE_INTER) continue;
					if(pclass->endpout.type != USB_ENDP_TYPE_INTER) continue;
				}
			}else if(DEV_TYPE_AUDIO == type){
				if(sub_type != pclass->itf.if_sub_cls) continue;
			}
			break;
		}
	}

	return pclass;
}


/*******************************************************************
** Parameters:	sub_type: hid 设备: hid_type_t, 其他设备传入sub_ift
** Returns:	
** Description:	这里没有处理interface alt set 很少设备使用到
*******************************************************************/
uint8_t usbh_class_find_by_type_all(dev_type_t type,uint8_t sub_type, usbh_class_t **ppcalss)
{
	error_t err = ERROR_NOT_FOUND;
	uint8_t i,id = USBH_NULL;
	usbh_dev_t *pdev = (usbh_dev_t *)m_usbh_dev;
	usbh_class_t *pcalss = NULL;

	for(i = 0; i < USBH_MAX_PORTS * (HUB_MAX_PORTS+1); i++,pdev++){
		uint8_t tmp_id = (i / (HUB_MAX_PORTS+1) <<4) | (i % (HUB_MAX_PORTS+1));
		pcalss = usbh_class_find_by_type(tmp_id,type,sub_type);
		if(NULL != pcalss){
			id = tmp_id;
			if(NULL != ppcalss) *ppcalss = pcalss;
			break;
		}
	}
	return id;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
dev_type_t usbh_match_class( uint8_t id, usbh_class_t *pclass)
{
	dev_type_t type = DEV_TYPE_NONE;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if ( m_usbh_types & BIT(DEV_TYPE_USBMUXD) ){
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
		if(ERROR_SUCCESS == usbh_match_usbmuxd(id, pclass)){
			type = DEV_TYPE_USBMUXD;
		}
		#endif
	}else if ( m_usbh_types & BIT(DEV_TYPE_IAP2) ){
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
		if(ERROR_SUCCESS == usbh_match_iap2(id, pclass)){
			type = DEV_TYPE_IAP2;
		}
		#endif
	}

	if(DEV_TYPE_NONE != type) return type;

	switch(pclass->itf.if_cls){
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
		case USB_CLASS_HID	:
			if(ERROR_SUCCESS == usbh_match_hid(id, pclass)){
				type = DEV_TYPE_HID;
			}
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		case USB_CLASS_AUDIO	:
			if(ERROR_SUCCESS == usbh_match_audio(id, pclass)){
				type = DEV_TYPE_AUDIO;
			}
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
		case USB_CLASS_PRINTER:
			if(ERROR_SUCCESS == usbh_match_printer(id, pclass)){
				type = DEV_TYPE_PRINTER;
			}
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
		case USB_CLASS_MSD	:
			if(ERROR_SUCCESS == usbh_match_msd(id, pclass)){
				type = DEV_TYPE_MSD;
			}
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
		case USB_CLASS_HUB	:
			if(ERROR_SUCCESS == usbh_match_hub(id, pclass)){
				type = DEV_TYPE_HUB;
			}
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
		case USB_CLASS_CDC	:
			if(ERROR_SUCCESS == usbh_match_cdc(id, pclass)){
				type = DEV_TYPE_CDC;
			}
			break;
		#endif
		case USB_CLASS_VENDOR :
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
				if(ERROR_SUCCESS == usbh_match_vendor(id, pclass)){
				type = DEV_TYPE_VENDOR;
				break;
			}
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
				if(ERROR_SUCCESS == usbh_match_adb(id, pclass)){
				type = DEV_TYPE_ADB;
				break;
			}
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
				if(ERROR_SUCCESS == usbh_match_aoa(id, pclass)){
				type = DEV_TYPE_AOA;
				break;
			}
			#endif

			//for xbox
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			if(ERROR_SUCCESS == usbh_match_hid(id, pclass)){
				type = DEV_TYPE_HID;
				break;
			}
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			if(ERROR_SUCCESS == usbh_match_audio(id, pclass)){
				type = DEV_TYPE_AUDIO;
				break;
			}
			#endif
			
			break;
		default:
			break;
	}

	return type;
}



/*******************************************************************
** Parameters:	sub_type: hid 设备: hid_type_t, 其他设备传入sub_ift
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_class_in(dev_type_t type,uint8_t sub_type,uint8_t* buf, uint16_t* plen, uint16_t timeout_ms)
{
	error_t err = ERROR_NOT_FOUND;
	uint8_t id = USBH_NULL;
	usbh_class_t *pclass = NULL;

	id = usbh_class_find_by_type_all(type, sub_type,&pclass);
	if(NULL != pclass){
		err = usbh_in(id, &pclass->endpin, buf, plen,timeout_ms);
	}

	return err;
}

/*******************************************************************
** Parameters:	sub_type: hid 设备: hid_type_t, 其他设备传入sub_ift
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_class_out(dev_type_t type,uint8_t sub_type,uint8_t* buf, uint16_t len)
{
	error_t err = ERROR_NOT_FOUND;
	uint8_t id = USBH_NULL;
	usbh_class_t *pclass = NULL;

	id = usbh_class_find_by_type_all(type, sub_type,&pclass);
	if(NULL != pclass){
		err = usbh_out(id, &pclass->endpout, buf, len);
	}

	return err;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_class_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	switch(pclass->dev_type){
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
		case DEV_TYPE_HID	:
			usbh_hid_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		case DEV_TYPE_AUDIO	:
			usbh_audio_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
		case DEV_TYPE_PRINTER:
			usbh_printer_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
		case DEV_TYPE_MSD	:
			usbh_msd_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
		case DEV_TYPE_HUB	:
			usbh_hub_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
		case DEV_TYPE_CDC	:
			usbh_cdc_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
		case DEV_TYPE_VENDOR :
			usbh_vendor_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
		case DEV_TYPE_ADB	:
			usbh_adb_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
		case DEV_TYPE_AOA	:
			usbh_aoa_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
		case DEV_TYPE_USBMUXD:
			usbh_usbmuxd_in_process(id, pclass, buf, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
		case DEV_TYPE_IAP2	:
			usbh_iap2_in_process(id, pclass, buf, len);
			break;
		#endif
		default:
			break;
	}
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	class open 设备, 有的设备需要特殊的指令初始化打开
*******************************************************************/
error_t usbh_class_open(uint8_t id)
{
	error_t err = ERROR_UNKNOW;
	usbh_class_t *pos;
	usbh_dev_t* pdev = get_usbh_dev(id);

	list_for_each_entry(pos,&pdev->class_list,usbh_class_t,list){
		switch(pos->dev_type){
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				err = usbh_hid_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				err = usbh_audio_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				err = usbh_printer_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				err = usbh_msd_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
			case DEV_TYPE_HUB	:
				err = usbh_hub_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				err = usbh_cdc_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
			case DEV_TYPE_VENDOR :
				err = usbh_vendor_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
			case DEV_TYPE_ADB	:
				err = usbh_adb_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
			case DEV_TYPE_AOA	:
				err = usbh_aoa_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
			case DEV_TYPE_USBMUXD:
				err = usbh_usbmuxd_open(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2	:
				err = usbh_iap2_open(id, pos);
				break;
			#endif
			default:
				break;
		}

		if(err) {
			loge("usbh class%d open err=%d\n", pos->dev_type, err);
			break;
		}
	}

	if(ERROR_SUCCESS == err){				//open endp
		list_for_each_entry(pos,&pdev->class_list,usbh_class_t,list){
			if(pos->endpout.addr){
				err = usbh_endp_register(id,&pos->endpout);
				if(err) {
					loge("usbh class%d endp out%d register err=%d\n", (uint16_t)pos->dev_type, (uint16_t)pos->endpout.addr, err);
					break;
				}
			}
			if(pos->endpin.addr){
				err = usbh_endp_register(id,&pos->endpin);
				if(err) {
					loge("usbh class%d endp in%d register err=%d\n",(uint16_t)pos->dev_type, (uint16_t)pos->endpin.addr, err);
					break;
				}
			}
		}
	}

	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	初始化class类, 获取类的相关描述符信息
*******************************************************************/
error_t  usbh_class_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
	error_t err = ERROR_UNKNOW;
	usbh_dev_t* pdev = get_usbh_dev(id);
	
	switch(pclass->dev_type){
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
		case DEV_TYPE_HID	:
			err = usbh_hid_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		case DEV_TYPE_AUDIO	:
			err = usbh_audio_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
		case DEV_TYPE_PRINTER:
			err = usbh_printer_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
		case DEV_TYPE_MSD	:
			err = usbh_msd_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
		case DEV_TYPE_HUB	:
			err = usbh_hub_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
		case DEV_TYPE_CDC	:
			err = usbh_cdc_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
		case DEV_TYPE_VENDOR :
			err = usbh_vendor_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
		case DEV_TYPE_ADB	:
			err = usbh_adb_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
		case DEV_TYPE_AOA	:
			err = usbh_aoa_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
		case DEV_TYPE_USBMUXD:
			err = usbh_usbmuxd_init(id, pclass, pdesc, len);
			break;
		#endif
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
		case DEV_TYPE_IAP2	:
			err = usbh_iap2_init(id, pclass, pdesc, len);
			break;
		#endif
		default:
			break;
	}
	return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	释放class内存
*******************************************************************/
error_t usbh_class_deinit(uint8_t id)
{
	error_t err = ERROR_UNKNOW;
	usbh_class_t *pos;
	usbh_dev_t* pdev = get_usbh_dev(id);

	list_for_each_entry(pos,&pdev->class_list,usbh_class_t,list){

		if(pos->endpout.addr){
			err = usbh_endp_unregister(id,&pos->endpout);
			if(err) continue;
		}
		if(pos->endpin.addr){
			err = usbh_endp_unregister(id,&pos->endpin);
			if(err) continue;
		}

		switch(pos->dev_type){
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				err = usbh_hid_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				err = usbh_audio_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				err = usbh_printer_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				err = usbh_msd_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
			case DEV_TYPE_HUB	:
				err = usbh_hub_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				err = usbh_cdc_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
			case DEV_TYPE_VENDOR :
				err = usbh_vendor_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
			case DEV_TYPE_ADB	:
				err = usbh_adb_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
			case DEV_TYPE_AOA	:
				err = usbh_aoa_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
			case DEV_TYPE_USBMUXD:
				err = usbh_usbmuxd_deinit(id, pos);
				break;
			#endif
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2	:
				err = usbh_iap2_deinit(id, pos);
				break;
			#endif
			default:
				break;
		}

		free_usbh_class(pos);		//free pclass
	}
	return err;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_class_handler(uint32_t period_10us)
{
	error_t err = ERROR_NOT_FOUND;
	uint8_t i,id = USBH_NULL;
	usbh_dev_t *pdev = (usbh_dev_t *)m_usbh_dev;
	usbh_class_t *pos;

	static timer_t s_class_timer;
	static timer_t s_tick;

	if(m_task_tick10us - s_class_timer >= period_10us){
		s_class_timer = m_task_tick10us;
		s_tick++;

		for(i = 0; i < USBH_MAX_PORTS * (HUB_MAX_PORTS+1); i++,pdev++){
			list_for_each_entry(pos,&pdev->class_list,usbh_class_t,list){
				id = (i / (HUB_MAX_PORTS+1) <<4) | (i % (HUB_MAX_PORTS+1));

				#if USBH_LOOP_IN_ENABLE
				if(0 == (s_tick % pos->endpin.interval)){
					uint8_t buf[64];
					uint16_t len = sizeof(buf);
					err = usbh_in(id, &pos->endpin, buf, &len,0);
					if((ERROR_SUCCESS == err) && len){
						usbh_class_in_process(id, pos, buf, len);
					}
				}
				#endif

				switch(pos->dev_type){
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
				case DEV_TYPE_HID	:
					usbh_hid_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
				case DEV_TYPE_AUDIO	:
					usbh_audio_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
				case DEV_TYPE_PRINTER:
					usbh_printer_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
				case DEV_TYPE_MSD	:
					usbh_msd_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
				case DEV_TYPE_HUB	:
					usbh_hub_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
				case DEV_TYPE_CDC	:
					usbh_cdc_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_VENDOR)
				case DEV_TYPE_VENDOR :
					usbh_vendor_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_ADB)
				case DEV_TYPE_ADB	:
					usbh_adb_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
				case DEV_TYPE_AOA	:
					usbh_aoa_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
				case DEV_TYPE_USBMUXD:
					usbh_usbmuxd_handler(id, pos);
					break;
				#endif
				#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
				case DEV_TYPE_IAP2	:
					usbh_iap2_handler(id, pos);
					break;
				#endif
				default:
					break;
				}
			}
		}
	}
}



#endif






