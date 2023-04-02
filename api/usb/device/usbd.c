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
#include "utils/emf_utils.h"
#include "api/usb/device/usbd.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
char const* usbd_string_desc[4] =
{
	"\x09\x04",  					// 0: is supported language is English (0x0409)
	"LiteEMF",                		// 1: Manufacturer
	"usb dev",         			// 2: Product
	"123456789012",           		// 3: Serials, should use chip ID
};


/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint16_t m_usbd_types[USBD_NUM];
uint16_t m_usbd_hid_types[USBD_NUM];

usbd_dev_t m_usbd_dev[USBD_NUM];
usbd_req_t m_usbd_req[USBD_NUM];
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
static error_t usbd_malloc_setup_buffer(uint8_t id, usbd_req_t *preq)
{
	error_t err = ERROR_SUCCESS;

	if(preq->setup_buf){					//防止出错内存未释放
		emf_free(preq->setup_buf);
		logd("usbd malloc err,please note it!!!");
	}
	preq->setup_index = 0;
	if(preq->req.wLength){
		preq->setup_len = 0;
		preq->setup_buf = emf_malloc(preq->req.wLength);
		if(NULL == preq->setup_buf){
			loge("usbd setup no memory!\n");
			err = ERROR_NO_MEM;
		}
	}else{
		preq->setup_len = 0;
		preq->setup_buf = NULL;
	}
	UNUSED_PARAMETER(id);
	return err;
}
static error_t usbd_free_setup_buffer(uint8_t id, usbd_req_t *preq)
{
	if(preq->setup_buf){
		emf_free(preq->setup_buf);
	}
	preq->setup_len = 0;
	preq->setup_index = 0;
	preq->setup_buf = NULL;
	UNUSED_PARAMETER(id);
	return ERROR_SUCCESS;
}
/*****************************************************************************************************
**  Function
******************************************************************************************************/
usbd_dev_t *usbd_get_dev(uint8_t id)
{
	usbd_dev_t *pdev = NULL;
	if(id < USBD_NUM){
		pdev = &m_usbd_dev[USBD_NUM];
	}
	return pdev;
} 

usbd_req_t *usbd_get_req_buf(uint8_t id)
{
	usbd_req_t *preq = NULL;
	if(id < USBD_NUM){
		preq = &m_usbd_req[USBD_NUM];
	}
	return preq;
} 


error_t usbd_endp_open(uint8_t id, uint8_t ep)
{
	if(id > USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_open(id, ep);
}

error_t usbd_endp_close(uint8_t id, uint8_t ep)
{
	if(id > USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_close(id, ep);
}
error_t usbd_endp_nak(uint8_t id, uint8_t ep)
{
	if(id > USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_nak(id, ep);
}
error_t usbd_clear_endp_stall(uint8_t id, uint8_t ep)
{
	error_t err = ERROR_SUCCESS;
	uint8_t dir;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;
	ep &= ~USB_DIR_MASK;

	if ( pdev->ep_status[ep][dir].stalled ){
		err = hal_usbd_clear_endp_stall(id, ep);
		pdev->ep_status[ep][dir].stalled = false;
		pdev->ep_status[ep][dir].busy = false;
  	}
	
    return err;
}
error_t usbd_endp_stall(uint8_t id, uint8_t ep)
{
	error_t err = ERROR_SUCCESS;
	uint8_t dir;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;
	ep &= ~USB_DIR_MASK;

	if ( !pdev->ep_status[ep][dir].stalled ){
		err = hal_usbd_endp_stall(id, ep);
		pdev->ep_status[ep][dir].stalled = true;
		pdev->ep_status[ep][dir].busy = true;
  	}
	
    return err;
}
bool usbd_get_endp_stalled(uint8_t id, uint8_t ep)
{
	uint8_t dir;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;
	ep &= ~USB_DIR_MASK;
    return pdev->ep_status[ep][dir].stalled;
}
void* usbd_get_endp_buffer(uint8_t id, uint8_t ep)
{
	if(id > USBD_NUM) return NULL;
    return hal_usbd_get_endp_buffer(id, ep);
}

error_t usbd_cfg_endp_all(uint8_t id)
{
    uint8_t i;
	usb_endp_t 	endp0;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	memset(pdev->ep_status, 0, sizeof(pdev->ep_status));
	memset(&endp0, 0, sizeof(usb_endp_t));					//配置端点0
	endp0.mtu =  USBD_ENDP0_MTU;
	hal_usbd_cfg_endp(id, &endp0);

	for(i=0; i<USBD_MAX_ITF_NUM; i++){						//非0 端点
		usbd_class_t *pclass = &m_usbd_class[id][i];

		if( pdev->itf_alt[pclass->itf.if_num] != pclass->itf.if_alt) continue;		//选择itf alt
		if(pclass->endpin.addr){
			hal_usbd_cfg_endp(id, &pclass->endpin);
		}
		if(pclass->endpout.addr){
			hal_usbd_cfg_endp(id, &pclass->endpout);
		}
	}
}


/*******************************************************************
** Parameters:
** Returns:
** Description: 注意: 端点0发送完成后必须使用 usbd_free_setup_buffer 释放内存
*******************************************************************/
error_t usbd_in(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t len)
{
	error_t err;
	usbd_class_t *pclass;
	
	pclass = usbd_class_find_by_ep(id, ep);
	if(NULL == pclass) return ERROR_PARAM;

	err = hal_usbd_in(id, &pclass->endpin, buf, len);

	if(0 == ep & ~USB_DIR_MASK){						//端点0发送完成释放内存
		usbd_req_t *preq = usbd_get_req_buf(id);
		if((ERROR_SUCCESS == err) && (preq->setup_index == preq->setup_len)){
			usbd_free_setup_buffer(id, preq);
		}
	}
    return err;
}
error_t usbd_out(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t* plen)
{
	error_t err;
	usbd_class_t *pclass;
	
	pclass = usbd_class_find_by_ep(id, ep);
	if(NULL == pclass) return ERROR_PARAM;

    return hal_usbd_out(id, &pclass->endpout, buf, plen);
}
error_t usbd_reset(uint8_t id)
{
	error_t err;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req_buf(id);

	if(NULL == pdev) return ERROR_PARAM;

	usbd_free_setup_buffer(id,preq);
	memset(preq, 0, sizeof(usbd_req_t));
	memset(pdev, 0, sizeof(usbd_dev_t));
	pdev->state = USB_STA_DEFAULT;
	usbd_class_reset(id);
	usbd_cfg_endp_all(id);

	err = hal_usbd_reset(id);
    return err;
}
error_t usbd_set_address(uint8_t id, uint8_t address)
{
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	pdev->address = address;
	pdev->state = USB_STA_ADDRESSING;

    return hal_usbd_set_address(id, address);
}





/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
#if WEAK_ENABLE

#if USBD_HID_SUPPORT & HID_SWITCH_MASK
#include  "app/gamepad/switch_controller.h"
#endif
#if USBD_HID_SUPPORT & HID_PS_MASK
#include  "app/gamepad/ps_controller.h"
#endif
#if USBD_HID_SUPPORT & HID_XBOX_MASK
#include  "app/gamepad/xbox_controller.h"
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
#include  "app/gamepad/gamepad_controller.h"
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

__WEAK error_t usbd_control_request_cb(uint8_t id, usbd_req_t* const preq)
{
	return ERROR_STALL;
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
		if(*pdesc_len <= len + 2) break;
		pdesc[len++] = str[i];
		pdesc[len++] = 0;
	}
	*pdesc_len = len;

	return len;
}

error_t usbd_get_string_desc(uint8_t id, uint8_t index, uint8_t *pdesc, uint16_t *pdesc_len)
{
	error_t err;
	uint16_t i;
	char *pstr;

	if(index > 3) return ERROR_STALL;
	if(*pdesc_len < 2) return ERROR_STALL;

	pstr = usbd_user_get_string(id, index);	//user vendor get string 

	if(pstr){
		err = usbd_pack_unicode_string(pstr,pdesc,pdesc_len);
	}else{
		err = usbd_pack_unicode_string((char*)usbd_string_desc[index],pdesc,pdesc_len);
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

	len = MIN(*pdesc_len, sizeof(usb_desc_device_t));
	memcpy(pdesc, &dev, len);
	*pdesc_len = len;

	return ERROR_SUCCESS;
}



error_t usbd_get_configuration_desc(uint8_t id, uint8_t cfg, uint8_t *pdesc, uint16_t *pdesc_len)
{
	uint16_t len;
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
	desc_index += usbd_class_get_itf_desc(id, &index, pdesc, *pdesc_len, &desc_index);

	pcfg_desc->wTotalLength = SWAP16_L(desc_index);
	pcfg_desc->bNumInterfaces = index.itf_num;

	*pdesc_len = MIN(*pdesc_len, desc_index);
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

	dev_type_t dev_type;

	switch(desc_type){
	case USB_DESC_DEVICE:
		err = usbd_get_device_desc(id, preq->setup_buf, &preq->setup_len);
		break;

	case USB_DESC_CONFIGURATION:
		err = usbd_get_configuration_desc(id,desc_index, preq->setup_buf,&preq->setup_len);
		break;

	case USB_DESC_STRING:
		err = usbd_get_string_desc(id,desc_index, preq->setup_buf,&preq->setup_len);
		break;

	default: 
		err = ERROR_STALL;
		break;
	}
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
	usbd_req_t *preq = usbd_get_req_buf(id);

	if(NULL == pdev) return ERROR_PARAM;

	usbd_control_request_cb(id,preq);

    if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bit.type){
		switch (preq->req.bmRequestType.bit.recipient) {
		case USB_REQ_RCPT_DEVICE:
			switch (preq->req.bRequest) {
			case USB_REQ_SET_ADDRESS:
				// Depending on mcu, status phase could be sent either before or after changing device address,
				err = usbd_set_address(id, (uint8_t)preq->req.wValue);
				break;
			case USB_REQ_GET_CONFIGURATION:
				preq->setup_buf[0] = pdev->cfg_num;
				break;
			case USB_REQ_SET_CONFIGURATION:
				// Only process if new configure is different
				if (pdev->cfg_num != preq->req.wValue) {
					if (pdev->cfg_num) {
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


void usbd_setup_process( uint8_t id )
{
	error_t err = ERROR_SUCCESS;
	uint16_t rx_len;
	usbd_req_t *preq = usbd_get_req_buf(id);
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return;

	if(pdev->dev.setup){
		logd("steup:");dumpd((uint8_t*)&preq->req,8);

		if(preq->setup_len && (NULL == preq->setup_buf)) return;

		//等待接收完整数据
		if((USB_DIR_OUT == preq->req.bmRequestType.bit.direction) && preq->req.wLength){
			rx_len = preq->req.wLength;
			err = usbd_out(id, 0x00, preq->setup_buf, &rx_len);
			if(rx_len && (ERROR_SUCCESS == err)){		//这里简单处理
				preq->setup_len = rx_len;
				pdev->dev.setup = 0;			
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
			}else if(ERROR_STALL == err){
				usbd_endp_stall(id,0x80);
				usbd_free_setup_buffer(id,preq);
			}else{	//数据未就绪
				//这里不能设置NAK,防止SOCKET情况下ACK后又被设置为NAK
			}
		}
	}
}

/*******************************************************************
** Parameters:
** Returns:
** Description:
*******************************************************************/
void usbd_handler(void)
{
	uint8_t id;
	usbd_dev_t *pdev;
	usbd_req_t *preq;
	
	for(id=0; id<USBD_NUM; id++){

		pdev = usbd_get_dev(id);
		preq = usbd_get_req_buf(id);

		if(pdev->dev.reset){
			pdev->dev.reset = 0;

			usbd_free_setup_buffer(id,preq);
			memset(preq, 0, sizeof(usbd_req_t));
			memset(pdev, 0, sizeof(usbd_dev_t));
			pdev->state = USB_STA_DEFAULT;

			usbd_class_reset(id);
			usbd_cfg_endp_all(id);
		}else if(pdev->dev.suspend){
			pdev->dev.suspend = 0;
			pdev->state = USB_STA_SUSPENDED;
			usbd_class_suspend(id);
		}else{
			usbd_setup_process(id);
		}


		if(USB_STA_SUSPENDED == pdev->state){
			usbd_class_handler(id);
		}
	}
}


error_t usbd_init(uint8_t id)
{
	memset(&m_usbd_req[id],0, sizeof(usbd_req_t));
	memset(&m_usbd_dev[id],0, sizeof(usbd_dev_t));
	m_usbd_dev[id].state = USB_STA_ATTACHED;

	usbd_class_init(id);
    return hal_usbd_init(id);
}
error_t usbd_deinit(uint8_t id)
{
	memset(&m_usbd_req[id],0, sizeof(usbd_req_t));
	memset(&m_usbd_dev[id],0, sizeof(usbd_dev_t));

	usbd_class_deinit(id);
    return hal_usbd_deinit(id);
}

#endif
