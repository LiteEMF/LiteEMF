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
		logd_r("usbd free err,please note it %lx!!!\n",(uint32_t)(preq->setup_buf));
		emf_free((void*)preq->setup_buf);
	}

	preq->setup_index = 0;
	preq->setup_len = 0;
	if(preq->req.wLength){

		if(preq->req.wLength > 0XFF){			//字符串pc会读取1k多字节, 这里节省内存空间
			if ((TUSB_REQ_TYPE_STANDARD == preq->req.bmRequestType.bits.type)
				&& (TUSB_REQ_GET_DESCRIPTOR == preq->req.bRequest)
				&& (TUSB_DESC_STRING == (preq->req.wValue >> 8)) ){
					
				preq->req.wLength = 0XFF;
			}
		}

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
	preq->setup_index = 0xffff;   	//set  setup_index > setup_len
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

	dir = (ep & TUSB_DIR_MASK)? 1:0;

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

	dir = (ep & TUSB_DIR_MASK)? 1:0;

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

	dir = (ep & TUSB_DIR_MASK)? 1:0;
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
AT_RAM_CODE()
error_t usbd_in(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t len)
{
	error_t err;
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;
	ep |= TUSB_DIR_IN_MASK;			//防止端点信息出错

	if(0 == (ep_addr)){
		usbd_req_t *preq = usbd_get_req(id);

		API_ENTER_CRITICAL();
		preq->setup_len = len;
		preq->setup_index = 0;
		if(preq->setup_buf != buf){			//TODO out 处理
			memcpy((void*)preq->setup_buf, buf, len);
		}
		err = hal_usbd_in(id, ep, (void*)preq->setup_buf, preq->setup_len);

		API_EXIT_CRITICAL();
	}else{
		if(TUSB_STA_CONFIGURED != pdev->state) return ERROR_DISCON;	//保证端点已经正常打开
		err = hal_usbd_in(id, ep, buf, len);
	}
    return err;
}
error_t usbd_out(uint8_t id, uint8_t ep, uint8_t* buf, uint16_t* plen)
{
	error_t err = ERROR_NULL;
	usbd_dev_t *pdev = usbd_get_dev(id);
	uint8_t ep_addr = ep & 0x7f;
	ep &= 0x7f;
	
	if(pdev->enpd_out_len[ep_addr]){
		*plen = MIN(pdev->enpd_out_len[ep_addr],*plen);
		pdev->enpd_out_len[ep_addr] = 0;
		err = hal_usbd_out(id, ep, buf, plen);
	}
	if(ERROR_SUCCESS != err){
		*plen = 0;
	}

	return err;
}

error_t usbd_set_address(uint8_t id, uint8_t address)
{
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	pdev->address = address;
	pdev->state = TUSB_STA_ADDRESSING;

    return hal_usbd_set_address(id, address);
}

error_t usbd_set_ready(uint8_t id, uint8_t ready)
{
	error_t err = ERROR_NOT_FOUND;
	usbd_dev_t *pdev = usbd_get_dev(id);

	if(NULL == pdev) return ERROR_PARAM;

	if(ready && (TUSB_STA_CONFIGURED != pdev->state)){
		err = ERROR_STATE;
		return err;
	}

	if(pdev->ready != ready){
		pdev->ready = ready;       					//枚举完
		extern error_t usbd_class_notify_evt(uint8_t id, usbd_event_t event,uint32_t val);
		usbd_class_notify_evt(id,USBD_EVENT_READY,pdev->ready);
		logd_g("usbd%d ready=%d...\n",(uint16_t)id,(uint16_t)ready);
	}
	err = ERROR_SUCCESS;
	
    return err;
}

extern void usbd_reset_event(uint8_t id);
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
** Description:	usb参数初始化	
*******************************************************************/
error_t usbd_core_pa_init(uint8_t id)
{
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	memset(preq,0, sizeof(usbd_req_t));
	memset(pdev,0, sizeof(usbd_dev_t));

	return ERROR_SUCCESS;
}

error_t usbd_core_init(uint8_t id)
{
	error_t err = ERROR_UNSUPPORT;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	memset(preq,0, sizeof(usbd_req_t));
	memset(pdev,0, sizeof(usbd_dev_t));
	
	pdev->state = TUSB_STA_ATTACHED;
	pdev->endp0_mtu = USBD_ENDP0_MTU;
	
	if(API_USBD_BIT_ENABLE & BIT(id)){
    	err = hal_usbd_init(id);
	}
	return err;
}
error_t usbd_core_deinit(uint8_t id)
{
	error_t err = ERROR_UNSUPPORT;
	usbd_dev_t *pdev = usbd_get_dev(id);
	usbd_req_t *preq = usbd_get_req(id);

	usbd_free_setup_buffer(preq);					//防止资源未被释放
	memset(preq,0, sizeof(usbd_req_t));
	memset(pdev,0, sizeof(usbd_dev_t));

	if(API_USBD_BIT_ENABLE & BIT(id)){
    	err = hal_usbd_deinit(id);
	}
	return err;
}

#endif






