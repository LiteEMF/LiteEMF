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
#include "api/api_tick.h"
#include "api/api_transport.h"
#include "api/api_iic_host.h"
#include "api/hid/hid_dev_desc.h"
#include "api/usb/device/usbd.h"
#include "api/usb/host/usbh.h"
#include "app/iap2/app_iap2.h"
#include "utils/emf_utils.h"

#include "api/api_log.h"




/******************************************************************************************************
** Defined
*******************************************************************************************************/

static uint8c_t iAP2InitLinkData[6] = { 0xff, 0x55, 0x02, 0x00, 0xee, 0x10 };

// 1,2字节表示长度；2,3字节表示ID号
static uint8c_t ID_Information[] = {
    0x00, 14, 0x00, 0x00, 'A', 'c', 'c', 'e', 's', 's', 'o', 'r', 'y', 0, // name
    DEFAULT_MODEL_DATA, // modelidentifier
    0x00, 11, 0x00, 0x02, 'z', 'i', 'k', 'w', 'a', 'y', 0, // manufacturer
    0x00, 9, 0x00, 0x03, '0', '0', '0', '1', 0, // serialnumber
    0x00, 7, 0x00, 0x04, '1', '0', 0, // firmwareversion
    0x00, 7, 0x00, 0x05, '1', '0', 0, // hardwareversion
    0x00, 5, 0x00, 0x08, 0x02, // 供电，advwance
    0x00, 6, 0x00, 0x09, 0x00, 0x64, // 受电电流
    0x00, 7, 0x00, 0x0c, 'e', 'n', 0, // 当前语言
    0x00, 7, 0x00, 0x0d, 'e', 'n', 0, // 支持的语言

#if IAP2_EA_ENABLED
    0x00, 28, 0x00, 0x06, 0xEA, 0x02, 0xEA, 0x03, 0x68, 0x00, 0x68, 0x03, 0x68, 0x02, 0x54, 0x00, 0x54, 0x01, 0x54, 0x02, 0x54, 0x04, 0xAE, 0x00, 0xAE, 0x02, 0xAE, 0x03, // support send cmd
    0x00, 14, 0x00, 0x07, 0xEA, 0x00, 0xEA, 0x01, 0x68, 0x01, 0x54, 0x03, 0xAE, 0x01, // support receive cmd

    0x00, 39, 0x00, 10, // 支持的外部配件协议
    0x00, 0x05, 0x00, 0x00, 0xA0, // id
    0x00, 0X11, 0x00, 0x01, 'c', 'o', 'm', '.', 'e', 'm', 'f', '.', 'd', 'e', 'm', 'o', 0,
    0x00, 0x05, 0x00, 0x02, 0x01, // 匹配的动作->app匹配
#elif HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
    0x00, 24, 0x00, 0x06, 0x68, 0x00, 0x68, 0x03, 0x68, 0x02, 0x54, 0x00, 0x54, 0x01, 0x54, 0x02, 0x54, 0x04, 0xAE, 0x00, 0xAE, 0x02, 0xAE, 0x03, // support send cmd
    0x00, 10, 0x00, 0x07, 0x68, 0x01, 0x54, 0x03, 0xAE, 0x01, // support receive cmd
#else
    0x00, 16, 0x00, 0x06, 0x68, 0x00, 0x68, 0x03, 0x68, 0x02, 0xAE, 0x00, 0xAE, 0x02, 0xAE, 0x03, // support send cmd
    0x00, 8, 0x00, 0x07, 0x68, 0x01, 0xAE, 0x01, // support receive cmd
#endif

#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
    0x00, 22, 0x00, 16, // USB 主机传输组件
    0x00, 6, 0x00, 0x00, 0x22, 0x22, // id
    0x00, 8, 0x00, 0x01, 'U', 'S', 'B', 0, // name
    0x00, 4, 0x00, 0x02, // 组件支持IAP2连接
#elif USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
    0x00, 22, 0x00, 15, // USB dev传输组件
    0x00, 6, 0x00, 0x00, 0x22, 0x22, // id
    0x00, 8, 0x00, 0x01, 'U', 'S', 'B', 0, // name
    0x00, 4, 0x00, 0x02, // 组件支持IAP2连接

#endif

#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_TOUCH)
#if (MT_CONTACT_NUM > 0)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x01, TOUCH_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '1', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // Touch Pointer
#endif
#if (MT_CONTACT_NUM > 1)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x02, TOUCH_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '2', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // Touch Pointer
#endif
#if (MT_CONTACT_NUM > 2)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x03, TOUCH_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '3', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // Touch Pointer
#endif
#if (MT_CONTACT_NUM > 3)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x04, TOUCH_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '4', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // Touch Pointer
#endif
#if (MT_CONTACT_NUM > 4)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x05, TOUCH_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '5', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // Touch Pointer
#endif
#endif
#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
    0x00, 24, 0x00, 18, // iAP2HID组件
    0x00, 6, 0x00, 0x00, 0x00, MOUSE_REPORT_ID, // id
    0x00, 9, 0x00, 0x01, 'H', 'I', 'D', '6', 0, // name
    0x00, 5, 0x00, 0x02, 0x02, // function:Touch Pointer
#endif
};

uint8c_t StartPowerUpdate[] = {
    0x00, 4, 0x00, 0x00, // max current drawn from accessory
    0x00, 4, 0x00, 0x01, // device battery will charge if power is present
    0x00, 4, 0x00, 0x04, // is enternal charger connnected
};

uint8c_t PowerSRCUpdate[] = {
    0x00, 6, 0x00, 0x00, 0x00, 0x00, // 提供 0, 1A(03,E8)、2.1A(08,34)、2.4A(09,60)
    0x00, 5, 0x00, 0x01, 0x00, // PowerPresent=1
};

uint8c_t StartHIDPara[] = {
    0x00, 6, 0x00, 0x00, 0x00, 0x00, // HIDComponentID (最后两字节，程序动态修改,以匹配ID阶段上报的ID)
    0x00, 6, 0x00, 0x01, IAP2_VID >> 8, IAP2_VID & 0xFF, // VID
    0x00, 6, 0x00, 0x02, IAP2_PID >> 8, IAP2_PID & 0xFF, // PID
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
iap2_state_t m_iap2_stu;
uint16_t m_iap_hid_types = IAP2_HID_SUPPORT;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t iap_need_ack = 0; // 收到payload包需要ACK标志。 needACK的值表示收到的未应答的payload数据包个数。
static uint16_t iap_EA_session_id = 0x01;
static uint8_t iap2_tx_buffer[168];
static ipa2_msg_pa_t iAP2MsgPara;


/*****************************************************************************************************
**	static Function
******************************************************************************************************/
bool cp_read_data(uint8_t reg, uint8_t* buf, uint16_t len)
{
    uint8_t i;
    #ifdef HW_IIC_MAP
    for (i = 0; i < 6; i++) {
        if (api_iic_host_read(CP_IIC_ID,CP_ENCRYPTION_ADDR, reg, buf, len)) {
            // logd("cp_read %x\n",(uint16_t)(reg));//dumpd(buf,len);
            return true;
        }
        api_delay_us(500);
    }
    #endif
    logd("cp_read %x err\n", (uint16_t)(reg));
    return false;
}

bool cp_wite_data(uint8_t reg, uint8_t* buf, uint16_t len)
{
    uint8_t i;
    #ifdef HW_IIC_MAP
    for (i = 0; i < 6; i++) {
        if (api_iic_host_read(CP_IIC_ID,CP_ENCRYPTION_ADDR, reg, buf, len)) {
            logd("cp_write %x\n", (uint16_t)(reg)); // dumpd(buf,len);
            return true;
        }
        api_delay_us(500);
    }
    #endif
    logd("cp_write %x err\n", (uint16_t)(reg));
    return false;
}

bool iap2_write(uint8_t* buf, uint16_t length, iap2_packet_ctrl_t ctrl)
{
    bool ret = false;
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2))
    uint8_t s;
    s = usbd_class_in(0,DEV_TYPE_IAP2,0, buf, length);
    if (ERROR_SUCCESS == s)
        ret = true;
    ctrl = ctrl;

#elif API_USBH_BIT_ENABLE && (USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2))
    static uint16_t s_index = 0;
    static uint8_t s_pack_start = 0;
    static uint8_t s_buf[IAP2_USBH_HID_MTU];
    uint8_t id;
    usbh_class_t* pcalss = NULL;
    uint16_t i = 0;
    uint8_t len;
    uint8_t lcb;

    id = usbh_class_find_by_type_all(DEV_TYPE_IAP2,0, &pcalss);
    if (id == USBH_NULL)
        return ret;

    if (ctrl & PACK_START) {
        s_index = 0;
        s_pack_start = 1;
    }

    while (i < length) {
        len = MIN(length - i, IAP2_USBH_HID_MTU - 2 - s_index);

        memcpy(s_buf + 2 + s_index, buf + i, len);
        s_index += len;

        if ((s_index == IAP2_USBH_HID_MTU - 2) || (ctrl & PACK_END)) {
            lcb = LCB_CONTINUATION | LCB_MORE_FLOW;
            if ((s_pack_start) && (0 == i))
                lcb &= ~LCB_CONTINUATION;
            if ((ctrl & PACK_END) && (i + len) == length)
                lcb &= ~LCB_MORE_FLOW;

            s_buf[0] = IAP2_SET_REPORT_ID;
            s_buf[1] = lcb;
            ret = !usbh_hid_set_report(id, pcalss->itf.if_num, HID_REPORT_TYPE_OUTPUT, IAP2_SET_REPORT_ID, buf, IAP2_USBH_HID_MTU);

            i += len;
            s_index = 0;
            s_pack_start = 0;
            if (!ret)
                break; // 失败
        } else {
            break; // 结束包
        }
    }
#endif

    iap_need_ack = 0; // 清除自动ack标志
    sequence_number_sync = ret;
    return ret;
}

void iap2_auto_ack(void)
{
    if (iap_need_ack) {
        iap_need_ack = 0;
        iap2_packet_head_fill(IAP2PacketControlACK, iap2_tx_buffer, 0, 0);
        iap2_write(iap2_tx_buffer, IAP2_PACKET_HEADER_LEN, PACK_START_END);
    }
}

bool iap2_link_send(void)
{
	memcpy(iap2_tx_buffer, iAP2InitLinkData, sizeof(iAP2InitLinkData));
	return iap2_write(iap2_tx_buffer, sizeof(iAP2InitLinkData), PACK_START_END); // 检测
}

bool iap2_negotiate_link_send(void)
{
    uint16_t len;
    iap2_tx_buffer[9] = IAP2_LINK_VERSION;
    iap2_tx_buffer[10] = MAX_NUM_OUTSTANDING_PACKET;
    iap2_tx_buffer[11] = MAX_PACKET_LEN >> 8;
    iap2_tx_buffer[12] = MAX_PACKET_LEN & 0xff;
    iap2_tx_buffer[13] = RETRANSMISSION_TIMEOUT >> 8;
    iap2_tx_buffer[14] = RETRANSMISSION_TIMEOUT & 0xff;
    iap2_tx_buffer[15] = CUMULATIVE_ACK_TIMEOUT >> 8;
    iap2_tx_buffer[16] = CUMULATIVE_ACK_TIMEOUT & 0xff;
    iap2_tx_buffer[17] = MAX_NUM_RETRANSMISSION;
    iap2_tx_buffer[18] = MAX_CUMULATIVE_ACK;
    iap2_tx_buffer[19] = CONTROL_SESSION_ID;
    iap2_tx_buffer[20] = CONTROL_SESSION_TYPE;
    iap2_tx_buffer[21] = CONTROL_SESSION_VERSION;
#if IAP2_EA_ENABLED
    iap2_tx_buffer[22] = EXTERNAL_SESSION_ID;
    iap2_tx_buffer[23] = EXTERNAL_SESSION_TYPE;
    iap2_tx_buffer[24] = EXTERNALSESSION_VERSION;
    iap2_tx_buffer[25] = check_sum_negative(&iap2_tx_buffer[9], 16);
    len = iap2_packet_head_fill(IAP2PacketControlSYN, iap2_tx_buffer, 16, 0x00);
#else
    iap2_tx_buffer[22] = check_sum_negative(&iap2_tx_buffer[9], 13);
    len = iap2_packet_head_fill(IAP2PacketControlSYN, iap2_tx_buffer, 13, 0x00);
#endif
    return iap2_write(iap2_tx_buffer, len, PACK_START_END); // 检测
}

bool iap2_ctrlsesion_msg_send(uint16_t msg_id, uint8_t* pMsg, uint16_t pMsg_len)
{
    bool ret;
    uint16_t packet_len;
    uint8_t payloadsum;

    packet_len = iap2_ctrlsession_head_fill(iap2_tx_buffer, pMsg_len, msg_id);
    payloadsum = check_sum(iap2_tx_buffer + IAP2_PACKET_HEADER_LEN, IAP2_CTRL_SESSION_HEADER_LEN);
    payloadsum += check_sum(pMsg, pMsg_len);
    payloadsum = 0X100 - payloadsum;

    if (packet_len < sizeof(iap2_tx_buffer)) {
        memcpy(iap2_tx_buffer + IAP2_PAR_OFFSET, pMsg, pMsg_len);
        iap2_tx_buffer[packet_len - 1] = payloadsum;
        ret = iap2_write(iap2_tx_buffer, packet_len, PACK_START_END);
    } else {
        ret = iap2_write(iap2_tx_buffer, IAP2_PACKET_HEADER_LEN + IAP2_CTRL_SESSION_HEADER_LEN, PACK_START); // fix
        ret = iap2_write(pMsg, pMsg_len, PACK_MID);
        ret = iap2_write(&payloadsum, 1, PACK_END);
    }
    return ret;
}



// limit:0A,0.5A,1A,1.5A,2.1A,2.4A,3A
bool iap2_power_source_update(uint16_t limit_ma)
{
    uint8_t power_source_buf[sizeof(PowerSRCUpdate)];

    memcpy(power_source_buf, PowerSRCUpdate, sizeof(PowerSRCUpdate));
    power_source_buf[4] = limit_ma >> 8;
    power_source_buf[5] = limit_ma & 0xff;
    if (0 == limit_ma)
        power_source_buf[10] = 0;
    else
        power_source_buf[10] = 1;
    dumpd((uint8_t*)power_source_buf, sizeof(power_source_buf));
    return iap2_ctrlsesion_msg_send(PowerSourceUpdate,
        (uint8_t*)power_source_buf,
        sizeof(power_source_buf));
}

bool iap2_power_update(ipa2_ctrl_session_t* pctrl_pa)
{
    uint16_t i = 0;
    ipa2_msg_pa_t msg_pa;
    uint8_t* param_p = pctrl_pa->pdat;
    uint16_t param_len = pctrl_pa->length - IAP2_CTRL_SESSION_HEADER_LEN;

    if (NULL == param_p)
        return false;

    while (i < param_len) {
        iap2_paramete_parse(param_p, &msg_pa);
        i += msg_pa.length;
        param_p += msg_pa.length;

        logd("	  power id=%x, ", msg_pa.id);
        switch (msg_pa.id) {
        case 0: // MaximumCrrentDrawnFormAccessory
            logd("MaximumCrrent=%dmA\n", (msg_pa.pdat[0] << 8) + msg_pa.pdat[1]);
            break;
        case 1: // DeviceBatteryWillChargeifPowerIsPresent
            logd("Enable Charge=%d\n", (uint16_t)(msg_pa.pdat[0]));
            break;
        case 4: // IsExternalChargerConnected
            logd("IsExternalChargerConnected=%d\n", (uint16_t)(msg_pa.pdat[0]));
            break;
        case 5: // BatteryChargingState
            logd("Charging=%d\n", (uint16_t)(msg_pa.pdat[0]));
            break;
        case 6: // BatteryChargeLevel
            logd("battery=%d\n", (msg_pa.pdat[0] << 8) + msg_pa.pdat[1]);
            break;
        default:
            logd("\n");
            break;
        }
    }
    return true;
}

bool iap2_certificate_data_send(void)
{
    bool ret;
    uint16_t packet_len, len = 0;
    uint16_t page_num = 0;
    uint16_t i = 0;
    uint16_t CP_addr = 0;
    uint8_t* rbufPtr = 0;
    uint8_t payloadsum = 0;
    uint8_t residue_len = 0;
    uint8_t buf[2];
    uint16_t temp1, temp2;
    cp_read_data(0x30, buf, 2);

    temp1 = (uint16_t)buf[0];
    temp2 = (uint16_t)buf[1];
    len = (uint16_t)((temp1 << 8) | temp2);

    if (len > 1024) {
        len = 1024;
    }
    page_num = len / CP_PAGE_SIZE;
    residue_len = len % 128;

    logd("cp 30 data_len =%d\n", len);

    packet_len = iap2_paramete_fill(iap2_tx_buffer + IAP2_PAR_OFFSET, NULL, len, 0x00);
    packet_len = iap2_ctrlsession_head_fill(iap2_tx_buffer, packet_len, AuthenticationCertificate);
    ret = iap2_write(iap2_tx_buffer, IAP2_PAR_DATA_OFFSET, PACK_START); // fix

    payloadsum = check_sum((iap2_tx_buffer + 9), IAP2_CTRL_SESSION_HEADER_LEN + IAP2_MSG_PARA_HEADER_LEN);

    rbufPtr = (uint8_t*)(iap2_tx_buffer);
    CP_addr = 0x31;
    for (i = 0; i < page_num; i++) {
        cp_read_data(CP_addr, rbufPtr, CP_PAGE_SIZE);
        payloadsum += check_sum(rbufPtr, CP_PAGE_SIZE);
        ret = iap2_write(rbufPtr, 128, PACK_MID);
        CP_addr++;
    }

    if (residue_len) {
        cp_read_data(CP_addr, rbufPtr, CP_PAGE_SIZE);
        payloadsum += check_sum(rbufPtr, residue_len);
    }
    rbufPtr[residue_len] = 0X100 - payloadsum;
    ret = iap2_write(rbufPtr, (residue_len + 1), PACK_END);

    return ret;
}

bool iap2_authent_challenge_send(ipa2_msg_pa_t* pmsg_pa)
{
    bool ret;
    uint16_t len, packet_len;
    uint16_t temp1;
    uint16_t temp2;
    uint8_t auth_buf[2];
    uint8_t time_out = 10; // 50ms
    len = pmsg_pa->length - IAP2_MSG_PARA_HEADER_LEN;

    logd("Write Certificate_length = %d\r\n", len);
    dumpd(pmsg_pa->pdat, len);

    auth_buf[0] = len >> 8;
    auth_buf[1] = len & 0xff;
    cp_wite_data(0x20, auth_buf, 2);
    cp_wite_data(0x21, pmsg_pa->pdat, len);

    auth_buf[0] = 0x01;
    cp_wite_data(0x10, auth_buf, 1);
    auth_buf[0] = 0;
    while (0 == auth_buf[0] && time_out--) {
        cp_read_data(0x10, auth_buf, 1);
        cp_read_data(0x05, auth_buf + 1, 1);
        auth_buf[0] = auth_buf[0] & 0x10;
        logd("st=%x\n", (uint16_t)(auth_buf[0]));
    }
    cp_read_data(0x11, auth_buf, 2);

    temp1 = (uint16_t)auth_buf[0];
    temp2 = (uint16_t)auth_buf[1];
    len = (uint16_t)((temp1 << 8) | temp2);
    if (len > 150) {
        len = 150;
    }

    cp_read_data(0x12, &iap2_tx_buffer[IAP2_PAR_DATA_OFFSET], len);

    packet_len = iap2_paramete_fill(iap2_tx_buffer + IAP2_PAR_OFFSET, NULL, len, 0x00);
    packet_len = iap2_ctrlsession_head_fill(iap2_tx_buffer, packet_len, AuthenticationResponse);

    iap2_tx_buffer[packet_len - 1] = check_sum_negative(iap2_tx_buffer + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
    ret = iap2_write(iap2_tx_buffer, packet_len, PACK_START_END);
    return ret;
}

bool iap2_authent_serialnum_send(void)
{
    bool ret;

    cp_read_data(0x4E, &iap2_tx_buffer[IAP2_PAR_DATA_OFFSET], 31);
    ret = iap2_ctrlsesion_msg_send(AuthenticationSerialNum,
        &iap2_tx_buffer[IAP2_PAR_DATA_OFFSET],
        31);
    return ret;
}

bool iap2_starthid_send(void)
{
    bool ret;
    uint16_t len = 0, packet_len;
    uint8_t i;
    uint8_t *pdesc, desc_len;
    
    #if HIDD_SUPPORT & (BIT_ENUM(HID_TYPE_MT) | BIT_ENUM(HID_TYPE_TOUCH))
    for (i = 1; i <= MT_CONTACT_NUM; i++) {
        memcpy(iap2_tx_buffer + IAP2_PAR_OFFSET, StartHIDPara, sizeof(StartHIDPara));
        iap2_tx_buffer[IAP2_PAR_OFFSET + 5] = (i << 8) | TOUCH_REPORT_ID; // 修改当前ID
        len = sizeof(StartHIDPara);

        desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_TOUCH, &pdesc);
        len += iap2_paramete_fill(iap2_tx_buffer + IAP2_PAR_OFFSET + len, (uint8_t*)pdesc, desc_len, 0x04); // pack id
        
        //ios 13.4
        iap2_tx_buffer[IAP2_PAR_OFFSET + len - desc_len + 3] = 0x05;

        packet_len = iap2_ctrlsession_head_fill(iap2_tx_buffer, len, StartHID);
        iap2_tx_buffer[packet_len - 1] = check_sum_negative(iap2_tx_buffer + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
        if (1 == i) {
            ret = iap2_write(iap2_tx_buffer, packet_len, PACK_START);
        } else {
            #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
            ret = iap2_write(iap2_tx_buffer, packet_len, PACK_MID);
            #else
            if (IAP2_MT_MAX == i) {
                ret = iap2_write(iap2_tx_buffer, packet_len, PACK_END);
            } else {
                ret = iap2_write(iap2_tx_buffer, packet_len, PACK_MID);
            }
            #endif
        }
    }
    #endif

    // mouse
    #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
    memcpy(iap2_tx_buffer + IAP2_PAR_OFFSET, StartHIDPara, sizeof(StartHIDPara));
    iap2_tx_buffer[IAP2_PAR_OFFSET + 5] = MOUSE_REPORT_ID; // 修改当前ID
    len = sizeof(StartHIDPara);

    desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_MOUSE, &pdesc);
    len += iap2_paramete_fill(iap2_tx_buffer + IAP2_PAR_OFFSET + len, (uint8_t*)pdesc, desc_len, 0x04); // pack id
    packet_len = iap2_ctrlsession_head_fill(iap2_tx_buffer, len, StartHID);
    iap2_tx_buffer[packet_len - 1] = check_sum_negative(iap2_tx_buffer + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
    ret = iap2_write(iap2_tx_buffer, packet_len, PACK_END);
    #endif
    return ret;
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if WEAK_ENABLE && IAP2_EA_ENABLED
__WEAK void app_iap2_app_in_process(uint8_t* report_buf, uint8_t len)
{
    logd("in:");
    dumpd(report_buf, len);
}
#endif

bool app_iap2_hidreport(uint16_t index, uint8_t* report_buf, uint8_t len)
{
    static timer_t t;
    bool ret = false;
    uint8_t* send_buf;
    uint16_t packet_len, send_len = 0;
    uint8_t i = 0;
    uint8_t id_buf[2];

    send_buf = iap2_tx_buffer;

    if (0 == index) {
		#if IAP2_EA_ENABLED
        packet_len = len; // pack id
        memcpy(send_buf + IAP2_PACKET_HEADER_LEN + 2, report_buf, packet_len);
        packet_len = iap2_EAsession_head_fill(iap_EA_session_id,send_buf, packet_len);
        send_buf[packet_len - 1] = check_sum_negative(send_buf + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
        ret = iap2_write(iap2_tx_buffer, packet_len, PACK_START_END);
		#endif
	#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_TOUCH)
    }else if (TOUCH_REPORT_ID == index & 0xff) {
        while (i < len) {
            id_buf[0] = 0;
            id_buf[1] = report_buf[i];

            if (id_buf[1] == 0) {
                i += 4;
                continue;
            }
            packet_len = iap2_paramete_fill(send_buf + IAP2_PAR_OFFSET, id_buf, 2, 0x00); // pack id
            packet_len += iap2_paramete_fill(send_buf + IAP2_PAR_OFFSET + packet_len, &report_buf[i], 4, 0x01); // pack report inif
            packet_len = iap2_ctrlsession_head_fill(send_buf, packet_len, AccesoryHIDReport);
            send_buf[packet_len - 1] = check_sum_negative(send_buf + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
            i += 4;
            send_buf += packet_len;
            send_len += packet_len;
            // logd("i=%d %d %d,%d",(uint16_t)(i,(uint16_t)(send_len),(uint16_t)(len),(uint16_t)(packet_len));
        }
        // logd("iap out%d: \n",m_systick - t);t = m_systick;// dumpd(iap2_tx_buffer,send_len);
        ret = iap2_write(iap2_tx_buffer, send_len, PACK_START_END);
	#endif
	#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
    } else if (MOUSE_REPORT_ID == index) {
        id_buf[0] = 0;
        id_buf[1] = MOUSE_REPORT_ID;

        packet_len = iap2_paramete_fill(send_buf + IAP2_PAR_OFFSET, id_buf, 2, 0x00); // pack id
        packet_len += iap2_paramete_fill(send_buf + IAP2_PAR_OFFSET + packet_len, report_buf, len, 0x01); // pack report inif
        packet_len = iap2_ctrlsession_head_fill(send_buf, packet_len, AccesoryHIDReport);
        send_buf[packet_len - 1] = check_sum_negative(send_buf + IAP2_PACKET_HEADER_LEN, packet_len - IAP2_PACKET_HEADER_LEN - 1);
        ret = iap2_write(iap2_tx_buffer, packet_len, PACK_START_END);
	#endif
    }
    // logd("%d iap out%d: \n",(uint16_t)index,m_systick - t);t = m_systick; //dumpd(iap2_tx_buffer,packet_len);
    return ret;
}

static bool iap2_in_parse(uint8_t* p_pack, uint16_t pack_len)
{
    bool ret;
    ipa2_packet_t iAP2Packet;
    ipa2_ctrl_session_t CtrlSession;

    if (NULL == p_pack) return false;
    if (6 > pack_len)  return false;

	// logd("iap in: ");dumpd(iap2_in_buf,iap2_in_buf_size);
    if ((memcmp(p_pack, iAP2InitLinkData, sizeof(iAP2InitLinkData)) == 0)) {
        if (IAP2_STA_LINK == m_iap2_stu) {
            sequence_number = 0; // 由接收ACK后更新
            ack_number = 0; // 由接收包更新
            m_iap2_stu = IAP2_STA_NEGOTIATE_LINK;
        }
        logd("iap2 link success!...\r\n");
        return true;
    }

    ret = iap2_packet_head_parse(p_pack, &iAP2Packet);
    if (!ret)
        return ret;

    if ((iAP2Packet.Control) & IAP2PacketControlSUS) { /* device sleep*/
        logd("iap2 device sleep...\n");
        return ret;
    }

    if ((NULL != iAP2Packet.ppayload)) {
        // ack
        iap_need_ack++;
        m_iap2_stu = IAP2_STA_IDEL;

        if (iAP2Packet.Control == (IAP2PacketControlSYN | IAP2PacketControlACK)){ /* SYN+ACK 0xC0*/
            logd("iap2 Negotiate Link sucess...\n");
            return ret;
        }

		#if IAP2_EA_ENABLED
        if (iAP2Packet.SessionID == EXTERNAL_SESSION_ID) {
            app_iap2_app_in_process(iAP2Packet.ppayload + 2, iAP2Packet.length - IAP2_PACKET_HEADER_LEN - 3);
            return ret;
        }
		#endif

        ret = iap2_ctrlsession_parse(&iAP2Packet, &CtrlSession);
        if (!ret) return ret;
        // iap2_msg_parse(&CtrlSession);

        // Session id
        switch (CtrlSession.msg_id) { // mesg_id
        case RequestAuthenticationCertificate: // 0xAA00,
            if (NULL == CtrlSession.pdat) {
                logd("iap2 IAP2_STA_REQ_CERTIFICATE...\n");
                m_iap2_stu = IAP2_STA_REQ_CERTIFICATE;
            } else {
                logd("iap2 IAP2_STA_REQ_AUTH_SERIAL_NUM...\n");
                m_iap2_stu = IAP2_STA_REQ_AUTH_SERIAL_NUM;
            }
            break;
        case RequestAuthenticationChallengeResponse: // 0xAA02,
            // save value
            iap2_paramete_parse(CtrlSession.pdat, &iAP2MsgPara);
            memcpy(iap2_tx_buffer, iAP2MsgPara.pdat, iAP2MsgPara.length - IAP2_MSG_PARA_HEADER_LEN);
            iAP2MsgPara.pdat = iap2_tx_buffer;

            logd("iap2 IAP2_STA_REQ_AUTH_CHALLENGE...\n");
            m_iap2_stu = IAP2_STA_REQ_AUTH_CHALLENGE;
            break;
        case AuthenticationFailed: // 0xAA04,
            logd("iap2 AuthenticationFailed ...\n");
            break;
        case StartIdentification: // 0x1D00,
            logd("iap2 IAP2_STA_START_IDENTIFICATION...\n");
            m_iap2_stu = IAP2_STA_START_IDENTIFICATION;
            break;
        case IdentificationAccepted: // 0x1D02,
            logd("iap2 IAP2_STA_START_ACCESSORY...\n");
            m_iap2_stu = IAP2_STA_START_ACCESSORY;
            break;
        case PowerUpdate: // 0xAE01,
            logd("iap2 rec PowerUpdate....\n");
            dumpd(CtrlSession.pdat, CtrlSession.length - IAP2_CTRL_SESSION_HEADER_LEN);
            // iap2_power_update(&CtrlSession);
            m_iap2_stu = IAP2_STA_SUCCESS;

            logd("---iap2 config success!!!---\n");
            // 如果电源欠压,可以先提升电压，然后发送AE03(POWER_SOURCE_UPDATE)处理）
            break;
        case AssistiveTouchInfo:
            if (CtrlSession.pdat[IAP2_MSG_PARA_HEADER_LEN + 0]) { // 如果使能成功 可以发送数据
                logd("---iap2 config success!!!---\n");
                m_iap2_stu = IAP2_STA_SUCCESS;
            }
            break;
        case StartEA:
            iap_EA_session_id = ((uint16_t)CtrlSession.pdat[9] << 8) | CtrlSession.pdat[10];
            logd("iap_EA_session_id=%d\n", iap_EA_session_id);
            break;
        default:
            logd("unknow mesg_id=%x\n", CtrlSession.msg_id);
            dumpd(CtrlSession.pdat, CtrlSession.length - IAP2_CTRL_SESSION_HEADER_LEN);
            break;
        }
    }
    return ret;
}


void app_iap2_in_process(uint8_t* buf, uint16_t len)
{
	static uint16_t iap2_in_buf_size = 0;
	static uint8_t iap2_in_buf[148];

#if USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
    if ((iap2_in_buf_size + len) <= sizeof(iap2_in_buf)) {
        memcpy(&iap2_in_buf[iap2_in_buf_size], buf, len);
    	iap2_in_buf_size += len;
    }

    if (64 != len) {
        iap2_in_parse(iap2_in_buf, iap2_in_buf_size);
        iap2_in_buf_size = 0;
    }
#elif USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
    uint8_t lcb;

    lcb = buf[1];
    if (0 == (lcb & LCB_CONTINUATION)){
        iap2_in_buf_size = 0;
	}
    len = MIN(len-2, (uint16_t)(sizeof(iap2_in_buf) - iap2_in_buf_size));
    memcpy(&iap2_in_buf[iap2_in_buf_size], buf + 2, len);
    iap2_in_buf_size += len;

    if (0 == (lcb & LCB_MORE_FLOW)) {
        iap2_in_parse(iap2_in_buf, iap2_in_buf_size);
        iap2_in_buf_size = 0;
    }
#endif

}

void app_iap2_init(void)
{
    sequence_number_sync = true;
    m_iap2_stu = IAP2_STA_LINK;
    iap_EA_session_id = 0x0001;
}

void app_iap2_task(void* pa)
{
	bool ret = true;
	bool iap_retry = true;
    static timer_t retry_timer;

	if (iap_retry) {
		iap_retry = false;
		
		switch (m_iap2_stu) {
		case IAP2_STA_LINK:
			ret = iap2_link_send();
			break;
		case IAP2_STA_NEGOTIATE_LINK:
			ret = iap2_negotiate_link_send();
			break;
		case IAP2_STA_REQ_CERTIFICATE:
			ret = iap2_certificate_data_send();
			break;
		case IAP2_STA_REQ_AUTH_CHALLENGE:
			ret = iap2_authent_challenge_send(&iAP2MsgPara);
			break;
		case IAP2_STA_REQ_AUTH_SERIAL_NUM:
			ret = iap2_authent_serialnum_send(); //					//请求加密序列号
			break;
		case IAP2_STA_START_IDENTIFICATION:
			ret = iap2_ctrlsesion_msg_send(IdentificationInformation, (uint8_t*)ID_Information, sizeof(ID_Information));
			break;
		case IAP2_STA_START_ACCESSORY:
			// 0xAE00,					//启动电源更新
			ret = iap2_ctrlsesion_msg_send(StratPowerUpdates, (uint8_t*)StartPowerUpdate, sizeof(StartPowerUpdate));
			if(!ret) break;
			// 0xAE03, 					//配件供电端属性更新
			ret = iap2_power_source_update(IAP2_DEFAULT_POWERLIMIT);
			if(!ret) break;
			// 0x5402                    //启动触摸信息
			#if HIDD_SUPPORT & (BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_TOUCH))
			ret = iap2_ctrlsesion_msg_send(StartAssistiveTouchInfo, NULL, 0);
			if(!ret) break;
			#endif

			// 0x6800, 					/*StartHID*/
			ret = iap2_starthid_send();
			if(!ret) break;

			// 0x5400,					//启动触摸
			#if HIDD_SUPPORT & (BIT_ENUM(HID_TYPE_MOUSE) | BIT_ENUM(HID_TYPE_TOUCH))
			ret = iap2_ctrlsesion_msg_send(StartAssistiveTouch, NULL, 0);
			if(!ret) break;
			#endif
			break;
		default:
			iap_retry = true;
			break;
		}
	}else if ((m_systick - retry_timer) > 250) {
		retry_timer = m_systick;
		iap_retry = true;
	}

    iap2_auto_ack();
}



#endif
