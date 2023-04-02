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

#if (USBH_TYPE_SUPPORT | USBD_TYPE_SUPPORT) & BIT_ENUM(DEV_TYPE_IAP2)
#include "utils/emf_utils.h"
#include "app/iap2/app_iap2.h"

#include "api/api_log.h"

uint8_t sequence_number = 0; 		// 由接收ACK后更新
uint8_t ack_number = 0; 			// 由接收包更新
bool sequence_number_sync = true; 	// 发送成功跟新SequenceNumber



uint16_t iap2_packet_head_fill(iap2_control_byte_t control, uint8_t* pack_buf,uint16_t payload_len, uint8_t id)
{
    uint16_t packet_len;

    pack_buf[0] = START_PACKET_MSB;
    pack_buf[1] = START_PACKET_LSB;
    if (payload_len) {
        packet_len = payload_len + 10;
    } else {
        packet_len = 9;
    }
    pack_buf[2] = packet_len >> 8;
    pack_buf[3] = packet_len & 0xFF;

    pack_buf[4] = control;
    if (payload_len && sequence_number_sync)
        sequence_number++;

    pack_buf[5] = sequence_number;
    pack_buf[6] = ack_number;
    if (payload_len == 0) {
        pack_buf[7] = 0;
    } else {
        pack_buf[7] = id;
    }
    pack_buf[8] = (uint8_t)check_sum_negative(pack_buf, (IAP2_PACKET_HEADER_LEN - 1));
    // logd("out pack len = %d\n",packet_len);
    return packet_len;
}
uint16_t iap2_paramete_fill(uint8_t* pack_buf, uint8_t* para_buf,uint16_t para_len, uint16_t parm_id)
{
    uint16_t packet_len;
    uint8_t* pValue = pack_buf;

    packet_len = IAP2_MSG_PARA_HEADER_LEN + para_len;
    pValue[0] = packet_len >> 8;
    pValue[1] = packet_len & 0xff;
    pValue[2] = parm_id >> 8;
    pValue[3] = parm_id & 0xff;

    if (NULL != para_buf) { // 无数据可以不拷贝
        memcpy(pValue + 4, para_buf, para_len);
    }
    // logd("msg_len =%d\n",packet_len);
    return packet_len;
}

uint16_t iap2_ctrlsession_head_fill(uint8_t* pack_buf, uint16_t payload_len,uint16_t msg_id)
{
    uint16_t packet_len;

    pack_buf[9] = 0x40;
    pack_buf[10] = 0x40;
    packet_len = IAP2_CTRL_SESSION_HEADER_LEN + payload_len;
    pack_buf[11] = packet_len >> 8;
    pack_buf[12] = packet_len & 0xff;
    pack_buf[13] = msg_id >> 8;
    pack_buf[14] = msg_id & 0xff;
    packet_len = iap2_packet_head_fill(IAP2PacketControlACK, pack_buf,
        packet_len, CONTROL_SESSION_ID);
    return packet_len;
}

uint16_t iap2_EAsession_head_fill(uint8_t id, uint8_t* pack_buf, uint16_t payload_len)
{
    uint16_t packet_len;

    pack_buf[9] = id >> 8;
    pack_buf[10] = id & 0xff;
    packet_len = 2 + payload_len;
    packet_len = iap2_packet_head_fill(IAP2PacketControlACK, pack_buf,
        packet_len, EXTERNAL_SESSION_ID);
    return packet_len;
}




bool iap2_packet_head_parse(uint8_t* data_ptr, ipa2_packet_t* ppacket)
{
    uint16_t temp1;
    uint16_t temp2;
	uint8_t sum;

    if (NULL == data_ptr)
        return false;

    if (check_sum_negative(data_ptr, 8) != data_ptr[8]) {
        logd("pack head sum error!\n");
        dumpd(data_ptr, 9);
        return false;
    }

    ppacket->SOPMSB = data_ptr[0];
    ppacket->SOPLSB = data_ptr[1];
    if ((ppacket->SOPMSB != 0xFF) || (ppacket->SOPLSB != 0x5A)) {
        logd("iAP2_parse_PacketHeader Fail!!\r\n");
        return false;
    }

    temp1 = (uint16_t)data_ptr[2];
    temp2 = (uint16_t)data_ptr[3];
    ppacket->length = (uint16_t)((temp1 << 8) | temp2);

    ppacket->Control = data_ptr[4];
    ppacket->Seq_num = data_ptr[5];
    ppacket->Ack_num = data_ptr[6];
    ppacket->SessionID = data_ptr[7];

    if (ppacket->length > 9) {
        ppacket->ppayload = &data_ptr[9];
		sum = check_sum_negative(ppacket->ppayload, ppacket->length - 10);
        if (sum != ppacket->ppayload[ppacket->length - 10]) {
            logd("payload sum error! %x %x\n",(uint16_t)sum,(uint16_t)(ppacket->ppayload[ppacket->length - 10]));
            return false;
        }
    } else {
        ppacket->ppayload = NULL;
    }

    ack_number = ppacket->Seq_num;
    return true;
}
bool iap2_paramete_parse(uint8_t* data_ptr, ipa2_msg_pa_t* pmsg_pa)
{
    uint16_t temp1;
    uint16_t temp2;

    if (NULL == data_ptr)
        return false;

    temp1 = (uint16_t)data_ptr[0];
    temp2 = (uint16_t)data_ptr[1];
    pmsg_pa->length = (uint16_t)((temp1 << 8) | temp2);

    temp1 = (uint16_t)data_ptr[2];
    temp2 = (uint16_t)data_ptr[3];
    pmsg_pa->id = (uint16_t)((temp1 << 8) | temp2);

    if (pmsg_pa->length > IAP2_MSG_PARA_HEADER_LEN) {
        pmsg_pa->pdata = &data_ptr[4];
    } else {
        pmsg_pa->pdata = NULL;
    }
    return true;
}

static bool iap2_hid_component_parse(ipa2_msg_pa_t* pmsg_pa)
{
    uint16_t i = 0;
    ipa2_msg_pa_t msg_pa;
    uint8_t* param_p = pmsg_pa->pdata;
    uint16_t param_len = pmsg_pa->length - IAP2_MSG_PARA_HEADER_LEN;

    if (NULL == param_p)
        return false;

    while (i < param_len) {
        iap2_paramete_parse(param_p, &msg_pa);
        i += msg_pa.length;
        param_p += msg_pa.length;

        logd("    HIDCom id=%d, ", msg_pa.id);
        switch (msg_pa.id) {
        case 0: // HIDComponentIdentifier
            logd("id=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        case 1: // HIDComponentName_Ptr
            logd("%s\n", msg_pa.pdata);
            break;
        case 2: // HIDComponentFunction
            logd("fun=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        case 3: // TransportComponentIdentifier
            logd("trp id=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        case 4: // TransportInterfaceNumber
            logd("trp num=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        default:
            logd("\n");
            break;
        }
    }
    return true;
}

static bool iap2_transport_component_parse(ipa2_msg_pa_t* pmsg_pa)
{
    uint16_t i = 0;
    ipa2_msg_pa_t msg_pa;
    uint8_t* param_p = pmsg_pa->pdata;
    uint16_t param_len = pmsg_pa->length - IAP2_MSG_PARA_HEADER_LEN;

    if (NULL == param_p)
        return false;

    while (i < param_len) {
        iap2_paramete_parse(param_p, &msg_pa);
        i += msg_pa.length;
        param_p += msg_pa.length;

        logd("    TransportCom id=%d, ", msg_pa.id);
        switch (msg_pa.id) {
        case 0: // HIDComponentIdentifier
            logd("id=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        case 1: // HIDComponentName_Ptr
            logd("%s\n", msg_pa.pdata);
            break;
        case 2: // TrpSupportsIAP2Connection
            logd("iap suupport\n");
            break;
        case 3: // USBDeviceSupportsAudioSP
            logd("rate=");
            dumpd(msg_pa.pdata,
                msg_pa.length - IAP2_MSG_PARA_HEADER_LEN);
            break;
        default:
            logd("\n");
            break;
        }
    }
    return true;
}

void iap2_identification_parse(ipa2_msg_pa_t* pmsg_pa)
{
    logd("  identification id=%d, ", pmsg_pa->id);
    switch (pmsg_pa->id) {
    case IdentiInfoName_IDMSG: // 0,
    case ModelIdentifier_IDMSG: // 1,
    case Manufacturer_IDMSG: // 2,
    case SerialNumber_IDMSG: // 3,
    case FirmwareVersion_IDMSG: // 4,
    case HardwareVersion_IDMSG: // 5,
        logd("%s\n", (char*)pmsg_pa->pdata);
        break;
    case MessagesSentByAccessory_IDMSG: // 6,
    case MessagesReceivedFromDevice_IDMSG: // 7,
    case PowerSourceType_IDMSG: // 8,
    case MaxCurDrawnFromDevice_IDMSG: // 9,
    case SupportedExternalAccessoryProtocol_IDMSG: // 10,
        dumpd(pmsg_pa->pdata,
            pmsg_pa->length - IAP2_MSG_PARA_HEADER_LEN);
        break;
    case PreferredAppBundleSeedIdentifier_IDMSG: // 11,
    case CurrentLanguage_IDMSG: // 12,
    case SupportedLanguage_IDMSG: // 13,
        logd("%s\n", (char*)pmsg_pa->pdata);
        break;
    case SerialTransportComponent_IDMSG: // 14,
    case USBDeviceTransportComponent_IDMSG: // 15,
    case USBHostTransportComponent_IDMSG: // 16,
    case BluetoothTransportComponent_IDMSG: // 17,
        logd("\n");
        iap2_transport_component_parse(pmsg_pa);
        break;
    case iAP2HIDComponent_IDMSG: // 18,
    case USBHostHIDComponent_IDMSG: // 23,
        logd("\n");
        iap2_hid_component_parse(pmsg_pa);
        break;
    case LocationInformationComponent_IDMSG: // 22,
        dumpd(pmsg_pa->pdata,
            pmsg_pa->length - IAP2_MSG_PARA_HEADER_LEN);
        break;
    default:
        logd("\n");
        break;
    }
}

bool iap2_msg_parse(ipa2_ctrl_session_t* pctrl_session)
{
    uint16_t i = 0;
    ipa2_msg_pa_t msg_pa;
    uint8_t* param_p = pctrl_session->pdata;
    uint16_t param_len = pctrl_session->length - IAP2_CTRL_SESSION_HEADER_LEN;

    if (NULL == param_p)
        return false;

    while (i < param_len) {
        iap2_paramete_parse(param_p, &msg_pa);
        i += msg_pa.length;
        param_p += msg_pa.length;

        switch (pctrl_session->msg_id) { // mesg_id
        case StartIdentification: // 0x1D00,
        case IdentificationInformation: // 0x1D01,
        case IdentificationAccepted: // 0x1D02,
        case IdentificationRejected: // 0x1D03,
        case CancelIdentification: // 0x1D05,
        case IdentificationInformationUpdate: // 0x1D06,
            iap2_identification_parse(&msg_pa);
            break;
        case StratPowerUpdates: // 0xAE00,
        case PowerUpdate: // 0xAE01,
        case StopPowerUpdates: // 0xAE02,
        case PowerSourceUpdate: // 0xAE03,
            logd("\n");
            break;
        case StartAssistiveTouch: // 0x5400,			//启动触摸
        case StopAssistiveTouch: // 0x5401,
        case StartAssistiveTouchInfo: // 0x5402,		 //启动触摸信息
        case AssistiveTouchInfo: // 0x5403, //触摸信息使能状态
        case StopAssistiveTouchInfo: // 0x5404,
            logd("\n");
            break;
        case StartHID: //          0x6800, /*StartHID*/
        case DeviceHIDReport: //  0x6801,
        case AccesoryHIDReport: //  0x6802, /*AccesoryHIDReport*/
        case StopHID: //        0x6803 /*StopHID*/
            logd("\n");
            break;
        default:
            logd("	unknow msg_id=%d,len=%d\n", msg_pa.id, msg_pa.length);
            break;
        }
    }

    return true;
}


bool iap2_ctrlsession_parse(ipa2_packet_t* ppacket, ipa2_ctrl_session_t* pctrl_session)
{
    uint16_t temp1;
    uint16_t temp2;

    if (NULL == ppacket->ppayload)
        return false;

    pctrl_session->SOFMSB = ppacket->ppayload[0];
    pctrl_session->SOFLSB = ppacket->ppayload[1];

    if ((pctrl_session->SOFMSB != 0x40) || (pctrl_session->SOFLSB != 0x40)) {
        logd("iap2_ctrlsession_parse Fail!!\r\n");
        return false;
    }

    temp1 = (uint16_t)ppacket->ppayload[2];
    temp2 = (uint16_t)ppacket->ppayload[3];
    pctrl_session->length = ((temp1 << 8) | temp2);

    temp1 = (uint16_t)ppacket->ppayload[4];
    temp2 = (uint16_t)ppacket->ppayload[5];
    pctrl_session->msg_id = ((temp1 << 8) | temp2);

    logd("msg_id=0x%x,len=%d\r\n", pctrl_session->msg_id, pctrl_session->length);

    if (pctrl_session->length > 6) {
        pctrl_session->pdata = &ppacket->ppayload[6];
    } else {
        pctrl_session->pdata = NULL;
    }

    return true;
}


#endif
