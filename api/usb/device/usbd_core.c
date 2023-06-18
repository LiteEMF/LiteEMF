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
#include  "api/api_system.h"
#include "api/usb/device/usbd_core.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
usbd_dev_t m_usbd_dev[USBD_NUM];
usbd_req_t m_usbd_req[USBD_NUM];


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
usbd_dev_t *usbd_get_dev(uint8_t id)
{
	usbd_dev_t *pdev = NULL;
	if(id < USBD_NUM){
		pdev = &m_usbd_dev[id];
	}
	return pdev;
} 

usbd_req_t *usbd_get_req(uint8_t id)
{
	usbd_req_t *preq = NULL;
	if(id < USBD_NUM){
		preq = &m_usbd_req[id];
	}
	return preq;
}

error_t usbd_malloc_setup_buffer(uint8_t id, usbd_req_t *preq)
{
	error_t err = ERROR_SUCCESS;

	if(NULL != preq->setup_buf){					//防止出错内存未释放
		logd("usbd malloc err,please note it %x!!!\n",(uint32_t)(preq->setup_buf));
		emf_free((void*)preq->setup_buf);
	}

	preq->setup_index = 0;
	preq->setup_len = 0;
	if(preq->req.wLength){
		preq->setup_buf = emf_malloc(preq->req.wLength);
		if(NULL == preq->setup_buf){
			loge("usbd setup no memory %d!\n",preq->req.wLength);
			// logd("setup:"); dumpd((void*)&(preq->req), 8);
			// emf_mem_stats();
			err = ERROR_NO_MEM;
		}
	}else{
		preq->setup_buf = NULL;
	}
	UNUSED_PARAMETER(id);
	return err;
}
error_t usbd_free_setup_buffer(usbd_req_t *preq)
{
	if(preq->setup_buf){
		emf_free((void*)preq->setup_buf);
	}
	preq->setup_len = 0;
	preq->setup_index = 0;
	preq->setup_buf = NULL;
	return ERROR_SUCCESS;
}

error_t usbd_endp_dma_init(uint8_t id)
{
	if(id >= USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_dma_init(id);
}

error_t usbd_endp_open(uint8_t id, usb_endp_t *pendp)
{
	if(id >= USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_open(id, pendp);
}

error_t usbd_endp_close(uint8_t id, uint8_t ep)
{
	if(id >= USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_close(id, ep);
}
error_t usbd_endp_ack(uint8_t id, uint8_t ep, uint16_t len)
{
	if(id >= USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_ack(id, ep, len);
}
error_t usbd_endp_nak(uint8_t id, uint8_t ep)
{
	if(id >= USBD_NUM) return ERROR_PARAM;
    return hal_usbd_endp_nak(id, ep);
}
error_t usbd_clear_endp_stall(uint8_t id, uint8_t ep)
{
	error_t err = ERROR_SUCCESS;
	uint8_t dir;
	uint8_t ep_addr = ep & 0x7f;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;

	if ( pdev->ep_status[ep_addr][dir].stalled ){
		err = hal_usbd_clear_endp_stall(id, ep);
		pdev->ep_status[ep_addr][dir].stalled = false;
		pdev->ep_status[ep_addr][dir].busy = false;
  	}
	
    return err;
}
error_t usbd_endp_stall(uint8_t id, uint8_t ep)
{
	error_t err = ERROR_SUCCESS;
	uint8_t dir;
	uint8_t ep_addr = ep & 0x7f;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;

	if ( !pdev->ep_status[ep_addr][dir].stalled ){
		err = hal_usbd_endp_stall(id, ep);
		pdev->ep_status[ep_addr][dir].stalled = true;
		pdev->ep_status[ep_addr][dir].busy = true;
  	}
	
    return err;
}
bool usbd_get_endp_stalled(uint8_t id, uint8_t ep)
{
	uint8_t dir;
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;

	if(NULL == pdev) return ERROR_PARAM;

	dir = (ep & USB_DIR_MASK)? 1:0;
    return pdev->ep_status[ep_addr][dir].stalled;
}
void* usbd_get_endp_buffer(uint8_t id, uint8_t ep)
{
	if(id >= USBD_NUM) return NULL;
    return hal_usbd_get_endp_buffer(id, ep);
}

/*******************************************************************
** Parameters:
** Returns:
** Description: 注意: 端点0发送完成后必须使用 usbd_free_setup_buffer 释放内存
*******************************************************************/
error_t usbd_in(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t len)
{
	error_t err;
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;
	ep |= USB_DIR_IN_MASK;			//防止端点信息出错

	if(0 == (ep_addr)){
		usbd_req_t *preq = usbd_get_req(id);

		API_ENTER_CRITICAL();

		pdev->enpd_in_busy[ep_addr] = false;			//注意endp0开始发发送接触busy状态
		preq->setup_len = len;
		preq->setup_index = 0;
		if(preq->setup_buf != buf){
			memcpy((void*)preq->setup_buf, buf, len);
		}
		err = hal_usbd_in(id, ep, (void*)preq->setup_buf, preq->setup_len);

		API_EXIT_CRITICAL();
	}else{
		if (pdev->enpd_in_busy[ep_addr]) return ERROR_BUSY;

		pdev->enpd_in_busy[ep_addr] = true;
		err = hal_usbd_in(id, ep, buf, len);
	}
    return err;
}
error_t usbd_out(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t* plen)
{
	error_t err = ERROR_NULL;
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;
	
	if(pdev->enpd_out_len[ep_addr]){
		*plen = MIN(pdev->enpd_out_len[ep_addr],*plen);
		pdev->enpd_out_len[ep_addr] = 0;
		err = hal_usbd_out(id, 0, buf, plen);
	}

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

error_t usbd_reset(uint8_t id)
{
	error_t err;

	err = hal_usbd_reset(id);
	if(ERROR_SUCCESS == err){
		usbd_reset_event(id);
	}
    return err;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	usb 事件, 在usb 中断事件产生后调用	
	用户可消息多种处理方式,用户可以自定义修改:
	1. 轮训方式	(默认)
	2. 任务消息推送方式
	3. 中断直接处理方式
*******************************************************************/
__WEAK void usbd_reset_event(uint8_t id)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	if(NULL != pdev){
		pdev->dev.reset = 1;
	}
}
__WEAK void usbd_suspend_event(uint8_t id)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	if(NULL != pdev){
		pdev->dev.suspend = 1;
	}
}
__WEAK void usbd_resume_event(uint8_t id)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	if(NULL != pdev){
		pdev->dev.resume = 1;
	}
}
__WEAK void usbd_sof_event(uint8_t id)
{

}
__WEAK void usbd_endp_in_event(uint8_t id ,uint8_t ep)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;

	ep |= USB_DIR_IN_MASK; 		//防止出错
	if(0 == ep_addr){
		usbd_req_t *preq = usbd_get_req(id);
		usbd_dev_t *pdev = usbd_get_dev(id);

		 if (USB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type){
			if (USB_REQ_SET_ADDRESS == preq->req.bRequest) {
				usbd_set_address(id, (uint8_t)preq->req.wValue);
			}
		 }

		hal_usbd_in(id, ep, NULL,0);			//TODO 考虑简化
	}else{
		hal_usbd_endp_nak(id, USB_DIR_IN_MASK | ep);
		pdev->enpd_in_busy[ ep_addr ] = false;
	}
}
__WEAK void usbd_endp_out_event(uint8_t id ,uint8_t ep, uint8_t len)
{
	usbd_dev_t *pdev = usbd_get_dev(id);

	ep &= ~USB_DIR_IN_MASK; 		//防止出错
	if (len) {
		hal_usbd_endp_nak(id,ep);
	}
	pdev->enpd_out_len[ep] = len;
}
__WEAK void usbd_setup_event(uint8_t id,usb_control_request_t *pctrl_req ,uint8_t pctrl_len)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	pdev->enpd_in_busy[0] = true;
	pdev->enpd_out_len[0] = 0;
	if(NULL != pdev){
		pdev->dev.setup = 1;
		preq->req = *pctrl_req;
		usbd_malloc_setup_buffer(id, preq);
	}

	if((USB_DIR_OUT == preq->req.bmRequestType.bits.direction) && preq->req.wLength){		//设置ack 继续接收OUT数据
		hal_usbd_endp_ack(id, 0x00, 0);
	}
}



error_t usbd_core_init(uint8_t id)
{
	error_t err = ERROR_UNSUPPORT;
	memset((void*)&m_usbd_req[id],0, sizeof(usbd_req_t));
	memset(&m_usbd_dev[id],0, sizeof(usbd_dev_t));
	m_usbd_dev[id].state = USB_STA_ATTACHED;
	m_usbd_dev[id].endp0_mtu = USBD_ENDP0_MTU;
	
	if(API_USBD_BIT_ENABLE & BIT(id)){
    	err = hal_usbd_init(id);
	}
	return err;
}
error_t usbd_core_deinit(uint8_t id)
{
	error_t err = ERROR_UNSUPPORT;
	usbd_free_setup_buffer(&m_usbd_req[id]);					//防止资源未被释放
	memset((void*)&m_usbd_req[id],0, sizeof(usbd_req_t));
	memset(&m_usbd_dev[id],0, sizeof(usbd_dev_t));

	if(API_USBD_BIT_ENABLE & BIT(id)){
    	err = hal_usbd_deinit(id);
	}
	return err;
}

#endif






