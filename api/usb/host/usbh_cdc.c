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

**	USB CDC是USB通信设备类协议，用于在USB设备和主机之间进行通信。CDC协议提供了一种通用的方法来描述USB设备如何与主机进行通信，
包括定义通信数据的格式和传输方式。CDC协议支持多种数据传输方式，如异步串行通信、同步串行通信、网络通信等。
因此，CDC协议可以应用于各种不同类型的USB设备，如调制解调器、串口设备、打印机等。

**	VCP是虚拟串口协议（Virtual COM Port），它将USB口模拟成一个标准的串行通信接口，以便与应用程序或操作系统进行通信。
通过使用VCP协议，USB设备可以像标准串行端口一样使用，而无需编写特定的USB驱动程序。
因此，VCP协议适用于需要通过串口与计算机通信的各种USB设备，如GPS接收器、遥控器、传感器等。
************************************************************************************************************/
#include "hw_config.h"
#if API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_CDC))
#include "api/usb/host/usbh.h"


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

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

error_t usbh_cdc_set_control_line_state(uint8_t id, uint8_t itf, uint16_t line_state)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = CDC_REQUEST_SET_CONTROL_LINE_STATE;

    req.wValue = SWAP16_L(line_state);
    req.wIndex = SWAP16_L(itf);
    req.wLength = 0;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}
error_t usbh_cdc_set_line_coding(uint8_t id, uint8_t itf, cdc_line_coding_t const* line_coding)  
{
    error_t err;
	usb_control_request_t req;
    uint16_t tr_len;

    req.bmRequestType.bits.recipient = TUSB_REQ_RCPT_INTERFACE;
    req.bmRequestType.bits.type = TUSB_REQ_TYPE_CLASS;
	req.bmRequestType.bits.direction = TUSB_DIR_OUT;
    req.bRequest = CDC_REQUEST_SET_LINE_CODING;

    req.wValue = 0;
    req.wIndex = SWAP16_L(itf);
    req.wLength = SWAP16_L(sizeof(cdc_line_coding_t));;

    err = usbh_ctrl_transfer(id, &req, NULL, NULL);
    return err;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	pclass->pdat used storage hub port numbers	
*******************************************************************/
void usbh_cdc_in_process(uint8_t id, usbh_class_t *pclass, uint8_t* buf, uint16_t len)
{
	logd("cdc endp%d in%d:",pclass->endpin.addr,len);dumpd(buf,len);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_match_cdc( uint8_t id, usbh_class_t *pclass)
{
	error_t err = ERROR_NOT_FOUND;

	// Only support ACM subclass,Protocol 0xFF can be RNDIS device for windows XP
	if ( (TUSB_CLASS_CDC == pclass->itf.if_cls) 
		&& (CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL == pclass->itf.if_sub_cls)
		&& (0XFF != pclass->itf.if_pro)){

		if( pclass->endpin.addr && pclass->endpout.addr ){
			err = ERROR_SUCCESS;
		}
	}

	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_cdc_open( uint8_t id, usbh_class_t *pclass) 
{
    error_t err = ERROR_UNSUPPORT;
	usbh_dev_t* pdev = get_usbh_dev(id);
	cdc_line_coding_t line_coding = CFG_TUH_CDC_LINE_CODING_ON_ENUM;

	if(NULL == pdev) return err;
	err = usbh_set_status(id, TUSB_STA_CONFIGURED, 0);
	if(err) return err;

	err = usbh_cdc_set_control_line_state(id, pclass->itf.if_num, CFG_TUH_CDC_LINE_CONTROL_ON_ENUM);
	if(err) return err;  

	err = usbh_cdc_set_line_coding(id, pclass->itf.if_num, &line_coding);
	if(err) return err;

	pdev->class_ready = true;
    return err;
}

error_t usbh_cdc_init( uint8_t id, usbh_class_t *pclass, uint8_t* pdesc, uint16_t len)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	UNUSED_PARAMETER(pdesc);
	UNUSED_PARAMETER(len);
    return ERROR_UNKNOW;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_cdc_deinit( uint8_t id, usbh_class_t *pclass) 
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
	return 0;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_cdc_task(uint8_t id, usbh_class_t *pclass)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(pclass);
}


#endif






