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
#if API_USBD_BIT_ENABLE
#include "api/usb/device/usbd_class.h"
#include "api/usb/device/usbd.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
usbd_class_t m_usbd_class[USBD_NUM][USBD_MAX_ITF_NUM];			//注意接口下标和接口号不是一一对应的

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
usbd_class_t *usbd_class_find_by_ep(uint8_t id, uint8_t ep)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;
	if(!ep) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if(((pclass->endpin.addr | USB_DIR_IN_MASK) == ep) || (pclass->endpout.addr == ep)){
			if(pdev->itf_alt[pclass->itf.if_num] == pclass->itf.if_alt){
				return pclass;
			}
		}
	}
	
	return NULL;
}

usbd_class_t *usbd_class_find_by_itf(uint8_t id, uint8_t itf)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if(pclass->itf.if_num == itf){
			if(pdev->itf_alt[pclass->itf.if_num] == pclass->itf.if_alt){
				return pclass;
			}
		}
	}
	
	return NULL;
}
/*******************************************************************
** Parameters:	sub_type: hid 设备: hid_type_t, 其他设备传入sub_ift
** Returns:	
** Description:		
*******************************************************************/
usbd_class_t *usbd_class_find_by_type(uint8_t id, dev_type_t type, uint8_t sub_type)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if((pclass->dev_type) == type){
			if(DEV_TYPE_HID == type){
				if(sub_type != pclass->hid_type) continue;
				if(HID_TYPE_XBOX == sub_type){		//xbox特殊处理,xbox音频不在这里处理
					if(pclass->endpin.type != USB_ENDP_TYPE_INTER) continue;
					if(pclass->endpout.type != USB_ENDP_TYPE_INTER) continue;
				}
			}else if(DEV_TYPE_AUDIO == type){
				if(sub_type != pclass->itf.if_sub_cls) continue;
			}

			if(pdev->itf_alt[pclass->itf.if_num] == pclass->itf.if_alt){
				return pclass;
			}
		}
	}
	
	return NULL;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	重新设配置描述符, 同时分配接口号好端点	
*******************************************************************/
error_t usbd_assign_configuration_desc(uint8_t id, dev_type_t type,hid_type_t hid_type,itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len)
{
	error_t err = ERROR_UNKNOW;
    uint16_t i, l;
	usb_desc_interface_t *pitf;
    usb_desc_endpoint_t *pep;
	usbd_class_t * pclass = NULL;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

    for ( i = 0; i < desc_len; i += l ){
        l = pdesc[i];
        if(0 == l) break;

		if(USB_DESC_INTERFACE == pdesc[i+1]){
			pitf = (usb_desc_interface_t*)(pdesc+i);
			pitf->bInterfaceNumber = pindex->itf_num;
			if(0 == pitf->bAlternateSetting){			//接口alt为0,表示接口号不相同才需要分配接口号
				pindex->itf_num++;
			}
			logd("assign itf =%d\n", pindex->itf_num);
			pclass = &m_usbd_class[id][pindex->itf_index++];
			pclass->dev_type = type;
			pclass->hid_type = hid_type;
			pclass->itf.if_num = pitf->bInterfaceNumber;
			pclass->itf.if_alt = pitf->bAlternateSetting;
			pclass->itf.num_endp = pitf->bNumEndpoints;
			pclass->itf.if_cls = pitf->bInterfaceClass;
			pclass->itf.if_sub_cls = pitf->bInterfaceSubClass;
			pclass->itf.if_pro = pitf->bInterfaceProtocol;

        }else if(USB_DESC_ENDPOINT == pdesc[i+1]){
			usb_endp_t *endp;
        	pep = (usb_desc_endpoint_t*)(pdesc+i);

			if(NULL == pclass){
				loge("usbd desc err!");
				return ERROR_FAILE;
			}

			if(pep->bEndpointAddress & USB_DIR_MASK){
				endp = &pclass->endpin;
				pep->bEndpointAddress = USB_DIR_IN_MASK | pindex->ep_in_num;
				if(0 == pclass->itf.if_alt) {			//接口alt为0,表示接口号不相同才需要分配新端点
					pindex->ep_in_num++;
				}
			}else{
				endp = &pclass->endpout;
				pep->bEndpointAddress = pindex->ep_out_num;
				if(0 == pclass->itf.if_alt) {
					pindex->ep_out_num++;
				}
			}

			endp->addr = pep->bEndpointAddress & 0x0F;
			endp->type = pep->bmAttributes.xfer;
			endp->sync = 0;
			endp->dir = (pep->bEndpointAddress & USB_DIR_MASK)? USB_DIR_IN:USB_DIR_OUT;
			endp->interval = pep->bInterval;
			endp->mtu = SWAP16_L(pep->wMaxPacketSize);
			
        }
    }
    return( err );
}

uint16_t usbd_class_get_itf_desc(uint8_t id, itf_ep_index_t* pindex, uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index)
{
	uint16_t len = 0;
	uint8_t type;
	bool xbox_audio = false;			//特殊处理xbox audio

	if((m_usbd_types[id] & BIT(DEV_TYPE_HID)) && (m_usbd_hid_types[id] & BIT(HID_TYPE_XBOX))){
		xbox_audio = true;
	}

	for(type=0; type<DEV_TYPE_NONE; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				len = usbd_hid_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				if(!xbox_audio){
					len = usbd_audio_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				}
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				len = usbd_printer_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				len = usbd_msd_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				len = usbd_cdc_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				len = usbd_iap2_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				len = usbd_auto_get_itf_desc(id,pindex,pdesc,desc_len,pdesc_index);
				break;
			#endif
			default:
				break;
			}
		}
	}

	return len;
}



error_t usbd_class_control_request_process(uint8_t id, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;

	uint8_t itf;
	usbd_class_t *pclass;

	if (USB_REQ_TYPE_VENDOR == preq->req.bmRequestType.bits.type){	//vendor
		if (USB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bits.recipient) {
			if(m_usbd_types[id] & BIT(DEV_TYPE_HID)){
				if(m_usbd_hid_types[id] & (BIT(HID_TYPE_X360))){
					#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
					pclass = usbd_class_find_by_type(id, DEV_TYPE_HID, HID_TYPE_X360);
					if(pclass){
						err = usbd_hid_x360_control_request_process(id, pclass, preq);
					}
					#endif
				}else if(m_usbd_hid_types[id] & (BIT(HID_TYPE_XBOX))){
					#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
					pclass = usbd_class_find_by_type(id, DEV_TYPE_HID, HID_TYPE_XBOX);
					if(pclass){
						err = usbd_hid_xbox_control_request_process(id, pclass, preq);
					}
					#endif
				}
			}
		}
	}else{
		itf = (uint8_t)(preq->req.wIndex & 0xFF);

		pclass = usbd_class_find_by_itf(id, itf);
		if(NULL != pclass){
			switch(pclass->dev_type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				err = usbd_hid_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				err = usbd_audio_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				err = usbd_printer_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				err = usbd_msd_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				err = usbd_cdc_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				err = usbd_iap2_control_request_process(id,pclass,preq);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				err = usbd_auto_control_request_process(id,pclass,preq);
				break;
			#endif
			default:
				break;
			}
		}
	}
	
	return err;
}


/*******************************************************************
** Parameters:
** Returns:	
** Description:		
*******************************************************************/
error_t usbd_class_in(uint8_t id, dev_type_t type, uint8_t sub_type, uint8_t* buf,uint16_t len)
{
	error_t err = ERROR_FAILE;
	usbd_class_t* pclass;

	pclass = usbd_class_find_by_type(id, type,sub_type);
	if(NULL == pclass) return ERROR_NOT_FOUND;

	if(pclass->endpin.addr){
		err = usbd_in(id, pclass->endpin.addr, buf, len);
	}

	return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbd_class_notify_evt(uint8_t id,usbd_event_t event,uint32_t val)
{
	uint8_t i;
	usbd_class_t *pclass;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];
		if(pclass->endpin.addr || pclass->endpout.addr){
			usbd_class_process(id, pclass, event, 0);
		}
	}
	
	return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description: usb事件触发任务,有3种usb事件处理方式
	1. 轮询方式 USBD_LOOP_ENABLE 置1
	2. 中断方式 USBD_LOOP_ENABLE 置0
	3. 事件方式 USBD_LOOP_ENABLE 置0, 修改 usbd_xxx_event WEAK 在函数中发送事件
*******************************************************************/
void usbd_class_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
	switch(pclass->dev_type){
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
	case DEV_TYPE_HID	:
		usbd_hid_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
	case DEV_TYPE_AUDIO	:
		usbd_audio_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
	case DEV_TYPE_PRINTER:
		usbd_printer_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
	case DEV_TYPE_MSD	:
		usbd_msd_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
	case DEV_TYPE_CDC	:
		usbd_cdc_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
	case DEV_TYPE_IAP2 :
		usbd_iap2_process(id, pclass, evt, val);
		break;
	#endif
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
	case DEV_TYPE_AUTO :
		usbd_auto_process(id, pclass, evt, val);
		break;
	#endif
	default:
		break;
	}
}


error_t usbd_class_init(uint8_t id)
{
	uint8_t type;

	memset(&m_usbd_class[id], 0, sizeof(m_usbd_class[id]));

	for(type=0; type<DEV_TYPE_NONE; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				usbd_audio_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_init(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_init(id);
				break;
			#endif
			default:
				break;
			}
		}
	}
	
	return ERROR_SUCCESS;
}


error_t usbd_class_deinit(uint8_t id)
{
	uint8_t type;

	for(type=0; type<DEV_TYPE_NONE; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				usbd_audio_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_deinit(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_deinit(id);
				break;
			#endif
			default:
				break;
			}
		}
	}
	
	return ERROR_SUCCESS;
}

#endif






