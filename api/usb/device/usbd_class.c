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
#if APP_USBD_ENABLE
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
usbd_class_t m_usbd_class[USBD_NUM][USBD_MAX_ITF_NUM];			//注意接口下班和接口号不是一一对应的

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

		if((pclass->itf.if_num | USB_DIR_IN_MASK) == itf){
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

error_t usbd_reset_configuration_desc(uint8_t id, dev_type_t type,hid_type_t hid_type,itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len)
{
	error_t err = ERROR_UNKNOW;
    uint8_t ep,inf_num=0;
    uint16_t i, l;
	usb_desc_interface_t *pitf;
    usb_desc_endpoint_t *pep;
	usbd_class_t * pclass = NULL;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

    for ( i = 0; i < desc_len; i += l ){
        l = pdesc[i];
        if(0 == l) break;

		if(USB_DESC_INTERFACE == pdesc[i+2]){
			pitf = (usb_desc_interface_t*)(pdesc+i);
			pitf->bInterfaceNumber = pindex->itf_num;
			if(pitf->bAlternateSetting){			//接口alt为0,表示接口号不相同才需要分配接口号
				pindex->itf_num++;
			}

			pclass = &m_usbd_class[id][pindex->itf_index++];
			pclass->dev_type = type;
			pclass->hid_type = hid_type;
			pclass->itf.if_num = pitf->bInterfaceNumber;
			pclass->itf.if_alt = pitf->bAlternateSetting;
			pclass->itf.num_endp = pitf->bNumEndpoints;
			pclass->itf.if_cls = pitf->bInterfaceClass;
			pclass->itf.if_sub_cls = pitf->bInterfaceSubClass;
			pclass->itf.if_pro = pitf->bInterfaceProtocol;

        }else if(USB_DESC_ENDPOINT == pdesc[i+2]){
			usb_endp_t *endp;
        	pep = (usb_desc_endpoint_t*)(pdesc+i);

			if(NULL == pclass){
				return ERROR_FAILE;
				loge("usbd desc err!");
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

	for(type=0; type<16; type++){
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


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbd_class_reset(uint8_t id)
{
	dev_type_t type;
	for(type=0; type<16; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				usbd_audio_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_reset(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_reset(id);
				break;
			#endif
			default:
				break;
		}
		}
	}
	
	return ERROR_SUCCESS;
}
error_t usbd_class_suspend(uint8_t id)
{
	dev_type_t type;
	for(type=0; type<16; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				usbd_audio_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_suspend(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_suspend(id);
				break;
			#endif
			default:
				break;
			}
		}
	}
	
	return ERROR_SUCCESS;
}


error_t usbd_class_control_request_process(uint8_t id, usbd_req_t* const preq)
{
	error_t err = ERROR_STALL;

	uint8_t itf;
	usbd_class_t *pclass;
	dev_type_t type;

	if(USB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bit.recipient){
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
	}else{			//特殊处理
		if (USB_REQ_TYPE_VENDOR == preq->req.bmRequestType.bit.type){	//vendor
			if (USB_REQ_RCPT_DEVICE == preq->req.bmRequestType.bit.recipient) {
				
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
		}

		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)	//自动识别需要
		if(m_usbd_types[id] == BIT(DEV_TYPE_AUTO)){
			pclass = usbd_class_find_by_type(id, DEV_TYPE_AUTO, 0);
			usbd_auto_control_request_process(id,pclass,preq);
		}
		#endif
	}

	return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbd_class_out_process(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t len)
{
	usbd_class_t *pclass;

	pclass = usbd_class_find_by_ep(id, ep);
	if(NULL == pclass) return ERROR_NOT_FOUND;

	switch(pclass->dev_type){
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
		case DEV_TYPE_HID	:
			usbd_hid_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
		case DEV_TYPE_AUDIO	:
			usbd_audio_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
		case DEV_TYPE_PRINTER:
			usbd_printer_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
		case DEV_TYPE_MSD	:
			usbd_msd_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
		case DEV_TYPE_CDC	:
			usbd_cdc_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
		case DEV_TYPE_IAP2 :
			usbd_iap2_out_process(id, pclass, buf, len);
			break;
		#endif
		#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
		case DEV_TYPE_AUTO :
			usbd_auto_out_process(id, pclass, buf, len);
			break;
		#endif
		default:
			break;
	}
	return ERROR_SUCCESS;
}


/*******************************************************************
** Parameters:
** Returns:	
** Description:		
*******************************************************************/
error_t usbd_class_in(uint8_t id, dev_type_t type, uint8_t sub_type, uint8_t* buf,uint8_t len)
{
	error_t err = ERROR_FAILE;
	usbd_class_t* pclass;

	pclass = usbd_class_find_by_type(id, type,sub_type);
	if(NULL == pclass) return ERROR_NOT_FOUND;

	if(pclass->endpin.addr){
		err = usbd_in(id, pclass->endpin.addr, buf, len);
	}

	return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbd_class_handler(uint8_t id)
{
	dev_type_t type;

	#if USBD_LOOP_OUT_ENABLE
	error_t err;
	uint8_t  usb_rxbuf[64];
	uint16_t usb_rxlen = sizeof(usb_rxbuf);
	uint8_t ep;

	for(ep=1; ep<USBD_ENDP_NUM; ep++){
		err = usbd_out(id,ep,usb_rxbuf,&usb_rxlen);
		if((ERROR_SUCCESS == err) && usb_rxlen){
			usbd_class_out_process(id,ep,usb_rxbuf,usb_rxlen);
		}
	}
	#endif
	
	for(type=0; type<16; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				usbd_audio_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_handler(id);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_handler(id);
				break;
			#endif
			default:
				break;
			}
		}
	}
}

error_t usbd_class_init(uint8_t id)
{
	dev_type_t type;

	memset(&m_usbd_class[id], 0, sizeof(m_usbd_class[id]));

	for(type=0; type<16; type++){
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
	dev_type_t type;

	for(type=0; type<16; type++){
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






