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
// m_usbd_class 在 usbd_assign_configuration_desc 获取配置描述符的时候设置
usbd_class_t m_usbd_class[USBD_NUM][USBD_MAX_ITF_NUM];	//注意由于有itf_alt的存在,接口下标和接口号不是一一对应的

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
AT_RAM_CODE()
usbd_class_t *usbd_class_find_by_ep(uint8_t id, uint8_t ep)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;
	if(0 == (API_USBD_BIT_ENABLE & BIT(id))) return NULL;
	if(!ep) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if(((pclass->endpin.addr | TUSB_DIR_IN_MASK) == ep) || (pclass->endpout.addr == ep)){
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
	if(0 == (API_USBD_BIT_ENABLE & BIT(id))) return NULL;

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
usbd_class_t *usbd_class_find_by_type(uint8_t id, dev_type_t type, uint8_t sub)
{
	uint8_t i;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_class_t *pclass;
	
	if(NULL == pdev) return NULL;
	if(0 == (API_USBD_BIT_ENABLE & BIT(id))) return NULL;

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		if((pclass->dev_type) == type){
			if(DEV_TYPE_HID == type){
				if(sub != pclass->hid_type) continue;
				if(HID_TYPE_XBOX == sub){		//xbox特殊处理,xbox音频不在这里处理
					if(pclass->endpin.type != TUSB_ENDP_TYPE_INTER) continue;
					if(pclass->endpout.type != TUSB_ENDP_TYPE_INTER) continue;
				}
			}else if(DEV_TYPE_AUDIO == type){
				if(sub != pclass->itf.if_sub_cls) continue;
			}else if(DEV_TYPE_CDC == type){
				if(sub != pclass->itf.if_cls) continue;
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
** Returns:	 配置输出到 m_usbd_class 中
** Description:	分配设配置描述符, 同时分配接口号和端点	
*******************************************************************/
error_t usbd_assign_configuration_desc(uint8_t id, dev_type_t type,hid_type_t hid_type,itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len)
{
	error_t err = ERROR_UNKNOW;
    uint16_t i, l;
	usbd_class_t * pclass = NULL;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

    for ( i = 0; i < desc_len; i += l ){
        l = pdesc[i];
        if(0 == l) break;
		
		switch(pdesc[i+1]){
		case TUSB_DESC_INTERFACE_ASSOCIATION:{
			usb_desc_interface_assoc_t *pitf_assoc = (usb_desc_interface_assoc_t*)(pdesc+i);
			pitf_assoc->bFirstInterface = pindex->itf_num;
			logd("itf assoc first=%d count=%d\n",pitf_assoc->bFirstInterface,pitf_assoc->bInterfaceCount);
			break;
		}
		case TUSB_DESC_INTERFACE:{
			usb_desc_interface_t *pitf;
    
			pitf = (usb_desc_interface_t*)(pdesc+i);
			if(pindex->itf_num && pitf->bAlternateSetting){			//接口alt为0,表示接口号不相同才需要分配接口号
				pindex->itf_num--;
				pindex->is_alt_itf = true;
			}else{
				pindex->is_alt_itf = false;
			}
			pitf->bInterfaceNumber = pindex->itf_num;
			logd("assign itf=%d\n", pindex->itf_num);
			pclass = &m_usbd_class[id][pindex->itf_index++];
			pclass->dev_type = type;
			pclass->hid_type = hid_type;
			pclass->itf.if_num = pitf->bInterfaceNumber;
			pclass->itf.if_alt = pitf->bAlternateSetting;
			pclass->itf.num_endp = pitf->bNumEndpoints;
			pclass->itf.if_cls = pitf->bInterfaceClass;
			pclass->itf.if_sub_cls = pitf->bInterfaceSubClass;
			pclass->itf.if_pro = pitf->bInterfaceProtocol;
			pindex->itf_num++;
			break;
		}
		case TUSB_DESC_ENDPOINT:{
			usb_endp_t *endp;
			usb_desc_endpoint_t *pep = (usb_desc_endpoint_t*)(pdesc+i);

			if(NULL == pclass){
				loge("usbd desc err!");
				return ERROR_FAILE;
			}

			if(pep->bEndpointAddress & TUSB_DIR_MASK){
				endp = &pclass->endpin;
				pep->bEndpointAddress = TUSB_DIR_IN_MASK | pindex->ep_in_num;
				if(!pindex->is_alt_itf) {			//接口alt为0,表示接口号不相同才需要分配新端点
					pindex->ep_in_num++;
					
				}
			}else{
				endp = &pclass->endpout;
				pep->bEndpointAddress = pindex->ep_out_num;
				if(!pindex->is_alt_itf) {
					pindex->ep_out_num++;
				}
			}
			logd("assign ep=%x\n",pep->bEndpointAddress);
			endp->addr = pep->bEndpointAddress & 0x0F;
			endp->type = pep->bmAttributes.xfer;
			endp->sync = 0;
			endp->dir = (pep->bEndpointAddress & TUSB_DIR_MASK)? TUSB_DIR_IN:TUSB_DIR_OUT;
			endp->interval = pep->bInterval;
			endp->mtu = SWAP16_L(pep->wMaxPacketSize) & 0X3FF;
        	break;
		}
		case TUSB_DESC_CS_INTERFACE:
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			if(AUDIO_CS_AC_INTERFACE_HEADER == pdesc[i+2]){
				uint16_t n;
				uint8_t *pControls;
				audio_desc_cs_ac_interface_t* paudio_ac = (audio_desc_cs_ac_interface_t*)(pdesc+i);
				
				if((SWAP16_H(paudio_ac->bcdADC) > 2) || (SWAP16_H(paudio_ac->wTotalLength) > 2)) continue;	//这里简单筛选
				
				pControls = &paudio_ac->bmControls;
				for(n = 0; n < SWAP16_H(paudio_ac->wTotalLength); n++){
					*pControls = n + pindex->itf_num;
					logd("assign audio cs ac itf=%d\n",*pControls);
					pControls++;
				}
			}
			#endif
        	break;
		case TUSB_DESC_FUNCTIONAL:
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			if((DEV_TYPE_HID == type) && (HID_TYPE_X360 == hid_type)){
				uint8_t *descp = (pdesc+i);
				uint8_t j,len,assign_ep_num=0;	//x360端点分配
				for(j=5; j < l; ){				//从第五个开始
					len = (descp[j] & 0x0F) + 2;	

					assign_ep_num++;
					if(descp[j+1] & TUSB_DIR_MASK){
						descp[j+1] = TUSB_DIR_IN_MASK | pindex->ep_in_num;
					}else {
						descp[j+1] = pindex->ep_out_num;
					}
					if(3 == assign_ep_num || 4 == assign_ep_num){		//一个接口超过2个同向端点,简单特殊处理不实际分配
						descp[j+1]++;
					}

					logd("x360 assign ep=%x\n",descp[j+1]);
					
					j += len;
				}
			}
			#endif
			break;
		}
    }
    return( err );
}

/*******************************************************************
** Parameters:		
** Returns:	 return 获取到的接口数, 配置输出到 m_usbd_class 中
** Description:	重新设配置描述符, 同时分配接口号和端点	
*******************************************************************/
uint16_t usbd_class_get_itf_desc(uint8_t id, uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index)
{
	uint8_t type;
	itf_ep_index_t index;
	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
	bool xbox_audio = false;			//特殊处理xbox audio
	if((m_usbd_types[id] & BIT(DEV_TYPE_HID)) && (m_usbd_hid_types[id] & BIT(HID_TYPE_XBOX))){
		xbox_audio = true;
	}
	#endif

	//每次调用的时候从0开始, 不会因为多次调用导致数据不一致
	memset(&index,0,sizeof(index));
	index.ep_in_num = 1;					//endp from 1
	index.ep_out_num = 1;

	for(type=0; type<DEV_TYPE_NONE; type++){
		if(m_usbd_types[id] & (1UL<<type)){
			switch(type){
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HID)
			case DEV_TYPE_HID	:
				usbd_hid_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUDIO)
			case DEV_TYPE_AUDIO	:
				if(!xbox_audio){
					usbd_audio_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				}
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_PRINTER)
			case DEV_TYPE_PRINTER:
				usbd_printer_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD)
			case DEV_TYPE_MSD	:
				usbd_msd_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC)
			case DEV_TYPE_CDC	:
				usbd_cdc_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
			case DEV_TYPE_IAP2 :
				usbd_iap2_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
			case DEV_TYPE_AUTO :
				usbd_auto_get_itf_desc(id,&index,pdesc,desc_len,pdesc_index);
				break;
			#endif
			default:
				break;
			}
		}
	}

	return index.itf_num;
}


/*******************************************************************
** Parameters:	
** Returns:	
** Description:usb请求分发给所以接口类处理
	注意为了通用请求能被分发给所有的接口,接口类请求处理返回err的时候自己的类请求才返回成功
	其他消息返回ERROR_STALL继续分发
*******************************************************************/
error_t usbd_class_control_request_process(uint8_t id, usbd_req_t* const preq)
{
	uint8_t i;
	error_t err = ERROR_STALL;
	usbd_class_t *pclass;

	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
	if(m_usbd_types[id] == BIT(DEV_TYPE_AUTO)){		//auto 特殊处理
		err = usbd_auto_control_request_process(id,NULL,preq);
		return err;
	}
	#endif
	
	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];

		//接口请求只发给对应接口,注意 TUSB_REQ_TYPE_VENDOR 请求wIndex不一定是接口号
    	if((TUSB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type)
			&& (TUSB_REQ_RCPT_INTERFACE == preq->req.bmRequestType.bits.recipient)){	
			uint8_t itf = preq->req.wIndex & 0XFF;

			if(pclass->itf.if_num != itf) continue;
			if(NULL == usbd_class_find_by_itf(id, itf)) continue;
		}	//其他请求全部设备都会分发

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
		default:
			break;
		}

		if(err != ERROR_STALL) break;			//处理成功退出
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
		if((DEV_TYPE_HID == type) && (sub_type == HID_TYPE_VENDOR) && (len < pclass->endpin.mtu)){	//hid vendor 需要根据MTU长度发送数据
			uint8_t *mtu_pbuf = emf_malloc(pclass->endpin.mtu);
			if(NULL != mtu_pbuf){
				memset(mtu_pbuf, 0, pclass->endpin.mtu);
				memcpy(mtu_pbuf, buf, MIN(len,pclass->endpin.mtu));
				err = usbd_in(id, pclass->endpin.addr, mtu_pbuf, pclass->endpin.mtu);		
				emf_free(mtu_pbuf);
			}
		}else{
			err = usbd_in(id, pclass->endpin.addr, buf, len);
		}
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

	#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AUTO)
	if(m_usbd_types[id] == BIT(DEV_TYPE_AUTO)){		//auto 特殊处理
		usbd_auto_process(id, pclass, event, val);
		return ERROR_SUCCESS;
	}
	#endif

	for(i=0; i<countof(m_usbd_class[id]); i++){
		pclass = &m_usbd_class[id][i];
		if(pclass->endpin.addr || pclass->endpout.addr){
			usbd_class_process(id, pclass, event, val);
		}
	}
	
	return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:		
** Returns:		true: 用户自定义处理, false: 会走通用处理	
** Description: usb事件用户处理
*******************************************************************/
#if WEAK_ENABLE
__WEAK bool usbd_class_event_weak(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
	switch(evt){
		case  USBD_EVENT_RESET:
			break;
		case  USBD_EVENT_SUSPEND:
			break;
		case  USBD_EVENT_EP_OUT:
			break;
		case USBD_EVENT_EP_IN:
			break;
		default:
			break;
	}
	return false;
}
#endif

/*******************************************************************
** Parameters:		
** Returns:	
** Description: usb事件触发任务,有3种usb事件处理方式
	1. 轮询方式 USBD_LOOP_XX_ENABLE 置1
	2. 中断方式 USBD_LOOP_XX_ENABLE 置0
	3. 事件方式 USBD_LOOP_XX_ENABLE 置0, 修改 usbd_xxx_event WEAK 在函数中发送事件
*******************************************************************/
void usbd_class_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val)
{
	if(usbd_class_event_weak(id, pclass, evt, val)){	//用户自定义事件处理
		return;
	}

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
	default:
		break;
	}
}


error_t usbd_class_init(uint8_t id)
{
	uint8_t type, i;

	logd_r("usbd types=%x hid_types=%x\n",m_usbd_types[id], m_usbd_hid_types[id]);
	
	memset(&m_usbd_class[id], 0, sizeof(m_usbd_class[id]));
	for(i=0; i<countof(m_usbd_class[id]); i++){
		usbd_class_t *pclass = &m_usbd_class[id][i];
		pclass->dev_type = DEV_TYPE_NONE;
	}

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






