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
#include "utils/emf_utils.h"
#include "api/usb/device/usbd.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
char const* usbd_string_desc[4] =
{
	"\x09\x04",  					// 0: is supported language is English (0x0409)
	"LiteEMF",                		// 1: Manufacturer
	"usb dev",         				// 2: Product
	"123456789012",           		// 3: Serials, should use chip ID
};


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t m_usbd_types[USBD_NUM];
uint16_t m_usbd_hid_types[USBD_NUM];


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

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
#if WEAK_ENABLE

#if APP_GAMEAPD_ENABLE
#include "app/gamepad/app_gamepad.h"
#endif

__WEAK char* usbd_user_get_string(uint8_t id, uint8_t index)
{
	char *pstr = NULL;

	if(2 == index){		//product string
		if(m_usbd_types[id] & BIT(DEV_TYPE_HID)){
			if(m_usbd_hid_types[id] & HID_SWITCH_MASK){
				pstr = "Pro Controller";
			}else if(m_usbd_hid_types[id] & HID_PS_MASK){
				pstr = "Wireless Controller";
			}else if(m_usbd_hid_types[id] & HID_XBOX_MASK){
				pstr = "Controller";
			}else if(m_usbd_hid_types[id] & BIT(HID_TYPE_GAMEPADE)){
				pstr = "Hid Gamepade";
			}else if(m_usbd_hid_types[id] & (BIT(HID_TYPE_MT) | BIT(HID_TYPE_TOUCH))){
				pstr = "Touch Screen";
			}else if(m_usbd_hid_types[id] & BIT(HID_TYPE_MOUSE)){
				pstr = "mouse";
			}else if(m_usbd_hid_types[id] & BIT(HID_TYPE_KB)){
				pstr = "keyboard";
			}else if(m_usbd_hid_types[id] & BIT(HID_TYPE_VENDOR)){
				pstr = "Hid Vendor";
			}
		}else if(m_usbd_types[id] & BIT(DEV_TYPE_IAP2)){
			pstr = "iap";
		}else if(m_usbd_types[id] & BIT(DEV_TYPE_MSD)){
			pstr = "msd";
		}else if(m_usbd_types[id] & BIT(DEV_TYPE_PRINTER)){
			pstr = "printer";
		}else if(m_usbd_types[id] & BIT(DEV_TYPE_CDC)){
			pstr = "cdc";
		}else if(m_usbd_types[id] & BIT(DEV_TYPE_AUDIO)){
			pstr = "uac";
		}else{
			pstr = (char*)usbd_string_desc[index];
		}
	}
		
	return pstr;
}

__WEAK void usbd_user_set_device_desc(uint8_t id, usb_desc_device_t *pdesc)
{
	if(m_usbd_types[id] & BIT(DEV_TYPE_HID)){
		if(m_usbd_hid_types[id] & HID_SWITCH_MASK){
			#if USBD_HID_SUPPORT & HID_SWITCH_MASK
			pdesc->idVendor = SWITCH_VID;
			pdesc->idProduct = SWITCH_PID;
			#endif
		}else if(m_usbd_hid_types[id] & HID_PS_MASK){
			#if USBD_HID_SUPPORT & HID_PS_MASK
			pdesc->idVendor = PS_VID;
			if(m_usbd_hid_types[id] & BIT(HID_TYPE_PS3)){
				pdesc->idProduct = PS3_PID;
			}else{
				pdesc->idProduct = PS4_PID;
			}
			#endif
		}else if(m_usbd_hid_types[id] & BIT_ENUM(HID_TYPE_XBOX)){
			#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_XBOX)
			pdesc->idVendor = XBOX_VID;	//0xFF,0x47,0xD0
			pdesc->idProduct = XBOX_PID;
			pdesc->bDeviceClass       = 0xFF;
			pdesc->bDeviceSubClass    = 0x47;
			pdesc->bDeviceProtocol    = 0xD0;
			#endif
		}else if(m_usbd_hid_types[id] == BIT_ENUM(HID_TYPE_X360)){		//xinput 复合设备使用自定义vid
			#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_X360)
			pdesc->idVendor = XBOX_VID;	//0xFF,0x47,0xD0
			pdesc->idProduct = X360_PID;	//0xff, 0xff, 0xff
			pdesc->bDeviceClass       = 0xFF;
			pdesc->bDeviceSubClass    = 0xff;
			pdesc->bDeviceProtocol    = 0xff;
			#endif
		}
	}
}

#endif
/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_pack_unicode_string( char *str, uint8_t *pdesc, uint16_t *pdesc_len)
{
	uint16_t len = 2;
	uint16_t i;

	if(*pdesc_len < 2) return ERROR_STALL;

	pdesc[0] = 2 * strlen(str) + 2;;
	pdesc[1] = 0x03;

	for(i=0; i<strlen(str); i++){
		if(*pdesc_len < len + 2) break;

		pdesc[len++] = str[i];
		pdesc[len++] = 0;
	}
	*pdesc_len = len;

	return ERROR_SUCCESS;
}

error_t usbd_get_string_desc(uint8_t id, uint8_t index, uint8_t *pdesc, uint16_t *pdesc_len)
{
	error_t err;
	char *pstr;

	if(index > 3) return ERROR_STALL;
	if(*pdesc_len < 2) return ERROR_STALL;

	pstr = usbd_user_get_string(id, index);	//user vendor get string 

	if(NULL != pstr){
		err = usbd_pack_unicode_string(pstr,pdesc,pdesc_len);
	}else{
		if(0 == index){
			pdesc[0] = 0x04;
			pdesc[1] = 0x03;
			pdesc[2] = 0x09;
			pdesc[3] = 0x04;
			*pdesc_len = 4;
		}else{
			err = usbd_pack_unicode_string((char*)usbd_string_desc[index],pdesc,pdesc_len);
		}
	}

	return err;
}

error_t usbd_get_device_desc(uint8_t id, uint8_t *pdesc, uint16_t *pdesc_len)
{
	uint16_t len;
	usb_desc_device_t dev;
	usbd_dev_t *pdev = usbd_get_dev(id);

	dev.bLength            = sizeof(usb_desc_device_t); 
	dev.bDescriptorType    = USB_DESC_DEVICE; 
	dev.bcdUSB             = SWAP16_L(USBD_BCD_VERSION);
	dev.bDeviceClass       = 0;
	dev.bDeviceSubClass    = 0;
	dev.bDeviceProtocol    = 0;
	dev.bMaxPacketSize0    = USBD_ENDP0_MTU; 
	dev.idVendor           = SWAP16_L(USB_VID);
	dev.idProduct          = SWAP16_L(USB_PID);
	dev.bcdDevice          = SWAP16_L(0x100);	//v1.0
	dev.iManufacturer      = 1;
	dev.iProduct           = 2;
	dev.iSerialNumber      = 3;
	dev.bNumConfigurations = 1; 

	usbd_user_set_device_desc(id, &dev);			//user vendor set dev desc

	pdev->vid = SWAP16_L(dev.idVendor);
	pdev->pid = SWAP16_L(dev.idProduct);
	pdev->endp0_mtu = dev.bMaxPacketSize0;

	len = MIN(*pdesc_len, sizeof(usb_desc_device_t));
	memcpy(pdesc, &dev, len);
	*pdesc_len = len;

	return ERROR_SUCCESS;
}



error_t usbd_get_configuration_desc(uint8_t id, uint8_t cfg, uint8_t *pdesc, uint16_t *pdesc_len)
{
	uint16_t desc_index = 9;
	itf_ep_index_t index;
	usb_desc_configuration_t* pcfg_desc = (usb_desc_configuration_t*)pdesc;

	if(*pdesc_len < 9) 	return ERROR_STALL;

	pcfg_desc->bLength = 0x09;
	pcfg_desc->bDescriptorType = 0x02;
	pcfg_desc->wTotalLength = SWAP16_L(desc_index);
	pcfg_desc->bNumInterfaces = 1;
	pcfg_desc->bConfigurationValue = 0x01;
	pcfg_desc->iConfiguration = 0x00;		//string index	 
	pcfg_desc->bmAttributes = 0x80 | (USBD_REMOTE_WAKEUP << 6) | (USBD_SELF_POWERED << 5);	//D6: Self-powered, remote wakeup, D5
	pcfg_desc->bMaxPower = 0xfa;			//500mA

	memset(&index,0,sizeof(index));
	index.ep_in_num = 1;					//endp from 1
	index.ep_out_num = 1;
	usbd_class_get_itf_desc(id, &index, pdesc, *pdesc_len, &desc_index);

	pcfg_desc->wTotalLength = SWAP16_L(desc_index);
	pcfg_desc->bNumInterfaces = index.itf_num;

	*pdesc_len = MIN(*pdesc_len, desc_index);

	logd("usbd cfg:");dumpd(pdesc,*pdesc_len);
	return ERROR_SUCCESS;
}



/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
error_t usbd_get_descriptor(uint8_t id, usbd_req_t* const preq)
{
	error_t err = ERROR_SUCCESS;
	uint8_t desc_type = (uint8_t)(preq->req.wValue >> 8);
	uint8_t desc_index = (uint8_t)(preq->req.wValue & 0xFF);

	preq->setup_len = preq->req.wLength;
	switch(desc_type){
	case USB_DESC_DEVICE:
		err = usbd_get_device_desc(id, preq->setup_buf, (uint16_t*)&preq->setup_len);
		break;

	case USB_DESC_CONFIGURATION:
		err = usbd_get_configuration_desc(id,desc_index, preq->setup_buf,(uint16_t*)&preq->setup_len);
		break;

	case USB_DESC_STRING:
		err = usbd_get_string_desc(id,desc_index, preq->setup_buf,(uint16_t*)&preq->setup_len);
		break;

	default: 
		err = ERROR_STALL;
		break;
	}
	return err;
}

error_t usbd_cfg_endp_all(uint8_t id)
{
    uint8_t i;
	
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	logd("usbd_cfg_endp_all%d...\n",id);
	
	for(i=1; i<USBD_ENDP_NUM; i++){
		hal_usbd_endp_close(id,i);
	}
	memset(pdev->ep_status, 0, sizeof(pdev->ep_status));
	pdev->endp0_mtu = USBD_ENDP0_MTU;

	usbd_endp_dma_init(id);
	for(i=0; i<USBD_MAX_ITF_NUM; i++){						//非0 端点
		usbd_class_t *pclass = &m_usbd_class[id][i];

		if( pdev->itf_alt[pclass->itf.if_num] != pclass->itf.if_alt) continue;		//选择itf alt
		if(pclass->endpin.addr){
			usbd_endp_open(id, &pclass->endpin);
		}
		if(pclass->endpout.addr){
			usbd_endp_open(id, &pclass->endpout);
		}
	}
	
	return ERROR_SUCCESS;
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
static error_t usbd_control_request_process(uint8_t id)
{
    error_t err = ERROR_SUCCESS;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	if(NULL == pdev) return ERROR_PARAM;

    if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
		switch (preq->req.bmRequestType.bits.recipient) {
		case USB_REQ_RCPT_DEVICE:
			switch (preq->req.bRequest) {
			case USB_REQ_SET_ADDRESS:
				// Depending on mcu, status phase could be sent either before or after changing device address,
				break;
			case USB_REQ_GET_CONFIGURATION:
				preq->setup_buf[0] = pdev->cfg_num;
				break;
			case USB_REQ_SET_CONFIGURATION:
				// Only process if new configure is different
				if (pdev->cfg_num != preq->req.wValue) {
					if (preq->req.wValue) {
						// close all non-control endpoints, cancel all pending transfers if any
						// close all drivers and current configured state except bus speed
						usbd_cfg_endp_all(id);
					}
				}

				pdev->state = USB_STA_CONFIGURED;
				pdev->ready = false;
				pdev->cfg_num = preq->req.wValue;
				err = ERROR_SUCCESS;			//must success
				break;
			case USB_REQ_GET_DESCRIPTOR:
				err = usbd_get_descriptor(id, preq);
				break;
			case USB_REQ_SET_FEATURE:
				// Only support remote wakeup for device feature
				if (USB_REQ_FEATURE_REMOTE_WAKEUP != preq->req.wValue)
					break;
				logd("usbd enable remote wakeup\n");

				// Host may enable remote wake up before suspending especially HID device
				pdev->dev.remote_wakeup_en = true;
				break;
			case USB_REQ_CLEAR_FEATURE:
				// Only support remote wakeup for device feature
				if (USB_REQ_FEATURE_REMOTE_WAKEUP != preq->req.wValue)
					break;
				logd("usbd disable remote wakeup\n");

				// Host may disable remote wake up after resuming
				pdev->dev.remote_wakeup_en = false;
				break;
			case USB_REQ_GET_STATUS:
				// Device status bit mask,Bit 0: Self Powered,Bit 1: Remote Wakeup enabled
				preq->setup_buf[0] = (pdev->dev.self_powered ? 1 : 0) | (pdev->dev.remote_wakeup_en ? 2 : 0);
				preq->setup_buf[1] = 0;
				break;
			default:
				err = ERROR_STALL;
				break;
			}
			break;

		case USB_REQ_RCPT_INTERFACE:
			// it is mandatory to respond even if the class driver doesn't use alternate settings or implement this
			switch (preq->req.bRequest) {
			case USB_REQ_SET_INTERFACE:		//注意是否需要切换和配置端点?
				pdev->itf_alt[preq->req.wIndex & 0xff] = preq->req.wValue & 0xff;
				break;
			case USB_REQ_GET_INTERFACE:
				preq->setup_buf[0] = pdev->itf_alt[preq->req.wIndex & 0xff];
				break;
			default:
				err = ERROR_STALL;
				break;
			}
			break;

		case USB_REQ_RCPT_ENDPOINT:
			// Handle STD request to endpoint
			switch (preq->req.bRequest) {
			case USB_REQ_GET_STATUS: {
				preq->setup_buf[0] = usbd_get_endp_stalled(id, preq->req.wIndex) ? 1 : 0;
			} break;
			case USB_REQ_CLEAR_FEATURE:
				if (USB_REQ_FEATURE_EDPT_HALT == preq->req.wValue) {
					err = usbd_clear_endp_stall(id, preq->req.wIndex);
				}
				break;
			case USB_REQ_SET_FEATURE:
				if (USB_REQ_FEATURE_EDPT_HALT == preq->req.wValue) {
					err = usbd_endp_stall(id, preq->req.wIndex);
				}
				break;
			default:
				err = ERROR_STALL;
				break;
			}
			break;

		default:
			err = ERROR_STALL;
			break;
		}
	}

	if(ERROR_STALL == err){
		err = usbd_class_control_request_process(id, preq);
	}else{				//枚举成功分发setup事件给需要的设备,比如 USB_REQ_SET_INTERFACE
		usbd_class_control_request_process(id, preq);
	}

    return err;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	usb 事件处理函数
*******************************************************************/
void usbd_reset_process( uint8_t id )
{
	usb_endp_t 	endp0;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	if(NULL != pdev){
		pdev->dev.reset = 0;
		usbd_free_setup_buffer(preq);
		memset(preq, 0, sizeof(usbd_req_t));
		memset(pdev, 0, sizeof(usbd_dev_t));
		pdev->state = USB_STA_DEFAULT;
		pdev->endp0_mtu = USBD_ENDP0_MTU;

	}
		
	usbd_class_reset(id);
	usbd_set_address(id, 0);
	
	memset(&endp0, 0, sizeof(usb_endp_t));					
	endp0.mtu = pdev->endp0_mtu;			//配置端点0
	usbd_endp_open(id, &endp0);

	usbd_cfg_endp_all(id);
}
void usbd_suspend_process( uint8_t id )
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	if(NULL != pdev){
		pdev->dev.suspend = 0;
		usbd_free_setup_buffer(preq);
		if(USB_STA_CONFIGURED == pdev->state){
			pdev->state = USB_STA_SUSPENDED;
			usbd_class_suspend(id);
		}else{
			pdev->state = USB_STA_DETACHED;
		}
	}
}
void usbd_resume_process( uint8_t id )
{
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL != pdev){
		pdev->dev.resume = 0;
		if(USB_STA_DETACHED == pdev->state){
			pdev->state = USB_STA_ATTACHED;
		}else{
			pdev->state = USB_STA_CONFIGURED;
		}
	}
}
void usbd_setup_process( uint8_t id )
{
	error_t err = ERROR_SUCCESS;
	uint16_t rx_len;
	usbd_req_t *preq = usbd_get_req(id);
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return;

	if(preq->req.wLength && (NULL == preq->setup_buf)) return;

	logd("steup:");dumpd((uint8_t*)&preq->req,8);
	//等待接收完整数据
	if((USB_DIR_OUT == preq->req.bmRequestType.bits.direction) && preq->req.wLength){
		rx_len = preq->req.wLength;
		err = usbd_out(id, 0x00, preq->setup_buf, &rx_len);
		if(ERROR_SUCCESS == err){
			preq->setup_index += rx_len;
			if(preq->req.wLength == preq->setup_index){		//rx finish
				preq->setup_index = 0;
				pdev->dev.setup = 0;	
			}		
		}
	}else{
		pdev->dev.setup = 0;
	}

	//setup处理
	if(0 == pdev->dev.setup){
		err = usbd_control_request_process(id);
		if(ERROR_SUCCESS == err){
			if(preq->setup_len > preq->req.wLength) preq->setup_len = preq->req.wLength;

			usbd_in(id, 0x80, preq->setup_buf, preq->setup_len);
			if(USB_DIR_OUT == preq->req.bmRequestType.bits.direction){
				usbd_free_setup_buffer(preq);
			}
		}else if(ERROR_STALL == err){
			usbd_endp_stall(id,0x80);
			usbd_free_setup_buffer(preq);
		}else{	//数据未就绪
			//这里不能设置NAK,防止SOCKET情况下ACK后又被设置为NAK
		}
	}
}


error_t usbd_init(uint8_t id)
{
	if(id >= USBD_NUM) return ERROR_FAILE;

	usbd_class_init(id);

    return usbd_core_init(id);
}
error_t usbd_deinit(uint8_t id)
{
	if(id >= USBD_NUM) return ERROR_FAILE;
	usbd_class_deinit(id);
	
    return usbd_core_deinit(id);
}

void usbds_init(void)
{
	uint8_t id;

	for(id=0; id<USBD_NUM; id++){
		usbd_init(id);
	}
}
void usbds_deinit(void)
{
	uint8_t id;

	for(id=0; id<USBD_NUM; id++){
		usbd_deinit(id);
	}
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_task(void*pa)
{
	uint8_t id;
	usbd_dev_t *pdev;

	for(id=0; id<USBD_NUM; id++){
		if(API_USBD_BIT_ENABLE & BIT(id)){
			pdev = usbd_get_dev(id);

			if(pdev->dev.reset){
				usbd_reset_process(id);
			}else if(pdev->dev.suspend){
				usbd_suspend_process(id);
			}else if(pdev->dev.resume){
				usbd_resume_process(id);
			}else if(pdev->dev.setup){
				usbd_setup_process(id);
			}

			if(USB_STA_CONFIGURED == pdev->state){
				usbd_class_task(id);
			}
		}
	}
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbd_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		usbd_task(NULL);
	}
}
#endif



#endif
