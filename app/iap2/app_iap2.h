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


#include "hw_config.h"
#ifndef _app_iap2_h
#define _app_iap2_h

#include "emf_typedef.h"
#include "iap2_packet.h"




/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define CP_ENCRYPTION_ADDR	0X20
#define CP_PAGE_SIZE 		128
#define CP_IIC_ID			0

#define IAP2_VID     VID_DEFAULT
#define IAP2_PID     (USBD_HID_SUPPORT | USBD_TYPE_SUPPORT)

//for iap device mode 
#define IAP2_USBH_HID_MTU		63
#define IAP2_SET_REPORT_ID		9

#define LCB_CONTINUATION		0X01
#define LCB_MORE_FLOW			0X02



#ifndef IAP2_DEFAULT_POWERLIMIT				//2.1A
#define IAP2_DEFAULT_POWERLIMIT				2100		
#endif 
#ifndef DEFAULT_MODEL_DATA
#define DEFAULT_MODEL_DATA			'e','m','f','-','d','e','m','o'	
#endif 


#define IAP2_EA_ENABLED         1			//APP SUPPORT


//iap2 usb device 
typedef enum{
	PACK_MID		= 0X0 ,
	PACK_END		= 0x01,
	PACK_START		= 0X02,
	PACK_START_END  = PACK_START | PACK_END,
}iap2_packet_ctrl_t;


/**********************************************************
**	link Packet Information
**********************************************************/
#define IAP2_LINK_VERSION  					0x01	 			/*固定0x01 */
#define MAX_NUM_OUTSTANDING_PACKET  		0x02			 	/*最大未确认包数 */
#define MAX_PACKET_LEN      				(64)				/*最大接收包长度 64字节 */
#define RETRANSMISSION_TIMEOUT 				0x03e8				/*重传超时时间 1.5s */
#define CUMULATIVE_ACK_TIMEOUT 				0x64				/*不发确认最大累积时间 100ms  */
#define MAX_NUM_RETRANSMISSION 				0x1E				/*最大重传次数 */
#define MAX_CUMULATIVE_ACK     				0x02				/*最大累积ACK次数 */
	
#define CONTROL_SESSION_ID     				0x01				/*CONTROL_SESSION_ID */
#define CONTROL_SESSION_TYPE   				Control_Session	
#define CONTROL_SESSION_VERSION         	0x01				/*version */
	
#define EXTERNAL_SESSION_ID           		0x02				
#define EXTERNAL_SESSION_TYPE         		External_Accessory_Session
#define EXTERNALSESSION_VERSION         	0x01				/*version */


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

typedef enum
{
    IAP2_STA_IDEL = 0,
	IAP2_STA_LINK,
	IAP2_STA_NEGOTIATE_LINK,					
	IAP2_STA_REQ_CERTIFICATE,					//请求证书
	IAP2_STA_REQ_AUTH_CHALLENGE,				//请求加密数据
	IAP2_STA_REQ_AUTH_SERIAL_NUM,				//请求加密数序列号
	IAP2_STA_START_IDENTIFICATION,				//启动身份识别
	IAP2_STA_START_ACCESSORY,					//启动设备
	IAP2_STA_SUCCESS,
}iap2_state_t;


/*******************************************************************************************************************
**	Function
********************************************************************************************************************/
void app_iap2_in_process(uint8_t* buf,uint16_t len);
bool app_iap2_hidreport(uint16_t index,uint8_t* report_buf,uint8_t len);
void app_iap2_app_in_process(uint8_t* report_buf,uint8_t len);				//WEAK
void app_iap2_init(void);
void app_iap2_task(void* pa);


#endif
