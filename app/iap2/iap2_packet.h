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


#ifndef iap2_packet_H
#define iap2_packet_H

#include "utils/emf_typedef.h" 




/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define START_PACKET_MSB   					0xFF
#define START_PACKET_LSB   					0x5A
#define IAP2_PACKET_HEADER_LEN               9
#define IAP2_CTRL_SESSION_HEADER_LEN         6
#define IAP2_MSG_PARA_HEADER_LEN             4

#define IAP2_PAR_OFFSET             		(IAP2_PACKET_HEADER_LEN+IAP2_CTRL_SESSION_HEADER_LEN)
#define IAP2_PAR_DATA_OFFSET             	(IAP2_PACKET_HEADER_LEN+IAP2_CTRL_SESSION_HEADER_LEN+IAP2_MSG_PARA_HEADER_LEN)






/**********************************************************
**	iap2 Packet layer
**********************************************************/
typedef enum{
	    /* BitMask defines for bits in control byte */
    IAP2PacketControlSYN = 0x80,   /* synchronization */
    IAP2PacketControlACK = 0x40,   /* acknowledgement */
    IAP2PacketControlEAK = 0x20,   /* extended acknowledgement */
    IAP2PacketControlRST = 0x10,   /* reset */
    IAP2PacketControlSUS = 0x08,   /* suspend (sleep) */
}iap2_control_byte_t;

typedef struct {
    uint8_t SOPMSB;   /* 0xFF */
    uint8_t SOPLSB;   /* 0x5A */
    uint16_t length;
    uint8_t Control;
    uint8_t Seq_num;
    uint8_t Ack_num;
    uint8_t SessionID;
    uint8_t checksum;
	uint8_t *ppayload;
} ipa2_packet_t;	


/**********************************************************
**	Ctrl Session layer
**********************************************************/

enum _iap2_packet_ctrlsession_type{
    Control_Session = 0x00,
    File_Transfer_Session = 0x01,
    External_Accessory_Session = 0x02,
};

typedef struct
{
	uint8_t	 	SOFMSB;	/*0x40*/
	uint8_t	 	SOFLSB;	/*0x40*/
	uint16_t	length;
	uint16_t	msg_id;
	uint8_t	 	*pdat;
} ipa2_ctrl_session_t;


typedef struct{
	uint16_t	length;		
	uint16_t	id;
	uint8_t		*pdat;
} ipa2_msg_pa_t;

enum _iap2_authent_ctrlsession_id{
	  
    RequestAuthenticationCertificate = 0xAA00,		 //请求证书
    AuthenticationCertificate = 0xAA01,				//应答证书
	RequestAuthenticationChallengeResponse = 0xAA02, //请求加密数据
	AuthenticationResponse = 0xAA03,					 //应答加密数据
    AuthenticationFailed = 0xAA04,						//认证失败
	AuthenticationSucceeded = 0xAA05,					//认证成功
	AuthenticationSerialNum = 0xAA06,					//请求加密序列号
	
	StartIdentification = 0x1D00,					//启动身份识别
	IdentificationInformation = 0x1D01,				//身份报告
	IdentificationAccepted = 0x1D02,				//身份识别通过
	IdentificationRejected = 0x1D03,				//身份识别不通过
	CancelIdentification = 0x1D05,
	IdentificationInformationUpdate = 0x1D06,
	
	StratPowerUpdates = 0xAE00,						//启动电源更新
	PowerUpdate = 0xAE01,							//手机发送电源状态
	StopPowerUpdates = 0xAE02,						 
	PowerSourceUpdate = 0xAE03,						//配件供电端属性更新
	
	StartAssistiveTouch = 0x5400,			//启动触摸
	StopAssistiveTouch = 0x5401,
	StartAssistiveTouchInfo = 0x5402,		 //启动触摸信息
	AssistiveTouchInfo = 0x5403,			//触摸信息使能状态
	StopAssistiveTouchInfo = 0x5404,
	
	StartHID  =          0x6800, /*StartHID*/
	DeviceHIDReport   =  0x6801, //发送HID报告
	AccesoryHIDReport =  0x6802, /*AccesoryHIDReport*/
	StopHID     =        0x6803, /*StopHID*/

	StartEA    =        0xEA00, 
	StopEA     =        0xEA01,
	EAStatus   =  		0xEA03, 
};


/*
*********************************************************
**	Identification Information
*********************************************************
*/


enum _iap2_identi_info_message_id{
	IdentiInfoName_IDMSG = 0,
	ModelIdentifier_IDMSG = 1,
	Manufacturer_IDMSG = 2,
	SerialNumber_IDMSG = 3,
	FirmwareVersion_IDMSG = 4,
	HardwareVersion_IDMSG = 5,
	MessagesSentByAccessory_IDMSG = 6,
	MessagesReceivedFromDevice_IDMSG = 7,
	PowerSourceType_IDMSG = 8,
	MaxCurDrawnFromDevice_IDMSG = 9,
	SupportedExternalAccessoryProtocol_IDMSG = 10,
	PreferredAppBundleSeedIdentifier_IDMSG = 11,
	CurrentLanguage_IDMSG = 12,
	SupportedLanguage_IDMSG = 13,
	SerialTransportComponent_IDMSG = 14,
	USBDeviceTransportComponent_IDMSG = 15,
	USBHostTransportComponent_IDMSG = 16,
	BluetoothTransportComponent_IDMSG = 17,
	iAP2HIDComponent_IDMSG = 18,
	LocationInformationComponent_IDMSG = 22,
	USBHostHIDComponent_IDMSG = 23,
};

enum _iap2_identi_hid_component_function_id{
	  
    IAP2Function_Keyboard = 0x00,
	IAP2Function_MediaPlayRemote = 0x01,
	IAP2Function_AssistiveTouchPointer = 0x02,
	IAP2Function_StandardGamepad = 0x03,
	IAP2Function_ExtendedGamepad = 0x04,
	IAP2Function_ExtendedGamepadNon = 0x06,
	IAP2Function_switchControl = 0x07,
};

enum _iap2_power_source_type{
	PowerSourceType_None = 0,
	PowerSourceType_Reserved = 1,
	PowerSourceType_SelfPower = 2,
};

enum iap2_assistive_touch_state{
	StartAssistTouchInfo_State = 0,
	AssistTouchInfo_State = 1,
	AssistTouchInfo_Response_ACK = 2,
	StartAssistTouch_State = 3,
	StopAssistTouch_State = 4,
	AssistTouchInfo_Polling_State = 5,
};

typedef struct {
    uint16_t TrpComponentIdentifier;
	uint8_t  *TrpComponentName_Ptr;
    uint16_t TrpSupportsIAP2Connection;
	uint16_t USBDeviceSupportsAudioSP;		//sample rate
} iap2_identification_usbd_transport_component;


typedef struct
{
    uint16_t TrpComponentIdentifier;
	uint8_t  *TrpComponentName_Ptr;
    uint16_t TrpSupportsIAP2Connection;
} iap2_identification_usbh_transport_component;

typedef struct
{
    uint16_t HIDComponentIdentifier;
	uint8_t  *HIDComponentName_Ptr;
    uint16_t HIDComponentFunction;
	uint16_t USBHostTransportComponentIdentifier;
	uint16_t USBHostTransportInterfaceNumber;
} iap2_identification_usbh_hid_component;

typedef struct
{
	uint16_t HIDComponentIdentifier;
	uint8_t  *HIDComponentName_Ptr;
	uint16_t HIDComponentFunction;
}iap2_identification_hid_component;


extern uint8_t sequence_number;    //由接收ACK后更新
extern uint8_t ack_number; //由接收包更新
extern bool sequence_number_sync; //发送成功跟新SequenceNumber

/*******************************************************************************************************************
**	Function
********************************************************************************************************************/

uint16_t iap2_paramete_fill(uint8_t* pack_buf,uint8_t* para_buf,uint16_t para_len,uint16_t parm_id);
uint16_t iap2_ctrlsession_head_fill(uint8_t* pack_buf,uint16_t payload_len,uint16_t msg_id);
uint16_t iap2_EAsession_head_fill(uint8_t id, uint8_t* pack_buf,uint16_t payload_len);
uint16_t iap2_packet_head_fill(iap2_control_byte_t control,uint8_t* pack_buf,uint16_t payload_len,uint8_t id);

void iap2_identification_parse(ipa2_msg_pa_t* pmsg_pa);
bool iap2_paramete_parse(uint8_t *data_ptr,ipa2_msg_pa_t* pmsg_pa);
bool iap2_msg_parse(ipa2_ctrl_session_t* pctrl_session);
bool iap2_ctrlsession_parse(ipa2_packet_t *ppacket,ipa2_ctrl_session_t* pctrl_session);
bool iap2_packet_head_parse(uint8_t *data_ptr,ipa2_packet_t* ppacket);





#endif

