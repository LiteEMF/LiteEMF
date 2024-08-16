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
#if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK)
#include  "utils/emf_utils.h"
#include "app/gamepad/switch_device.h"
#include  "gamepad_controller.h"		//TODO
#include "api/api_tick.h"

#if APP_BATTERY_ENABLE
#include "app_battery.h"
#endif
#if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & HID_SWITCH_MASK)
#include "api/usb/device/usbd.h"
#endif
#if BT_ENABLE
#include  "api/bt/api_bt.h"
#endif

#define DEBUG_LOG_ENABLE     	1
#include "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/
//SWITCH_SERIAL_NUMBER_ADD				0X6000
static uint8c_t switch_serial_number_default[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

//factory 数据:lx:461 1929 3388, ly:507 1934 3610, rx:403 1962 3485, ry:403 2005 3571, acc: -9 -38 635, gyro:-22 2 1 
//SWITCH_FACTORY_IMU_CAL_ADD				0X6020  
//switch_imu_cal_t
static uint8c_t switch_imu_cal_default[] = {
        0xF7,0xFF,0xDA,0xFF,0x7B,0x02,
        0x00,0x40,0x00,0x40,0x00,0x40,
        0xEA,0xFF,0x02,0x00,0x01,0x00,
        0xE7,0x3B,0xE7,0x3B,0xE7,0x3B};

//SWITCH_FACTORY_STICK_CAL_ADD 0X603D
//switch_stick_cal_t
static uint8c_t switch_stick_cal_default[] = {
    0xfd,0xd7,0x7f, //lxymax - lxymid
    
    0x0,0x8,0x80, //lxymid
    0xfd,0xd7,0x7f, //lxymid - lxymin
    0x0,0x8,0x80, //rxymid
    0xfd,0xd7,0x7f, //rxymid - rxymin
    0xfd,0xd7,0x7f, //rxymax - rxymid
};
//SWITCH_RGB_ADD							0X6050
//switch_flash_rgb_t
static uint8c_t switch_flash_rgb_default[] = {
    0x32, 0x32, 0x32, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

//SWITCH_FACTORY_IMU_STICK_PARA_ADD		0x6080
//switch_imu_stick_para_t
static uint8c_t switch_imu_stick_para_default[] = {
    0x50, 0xfd, 0x00, 0x00, 0xc6, 0x0f, 0x0f, 0x30, 0x61, 0x96, 0x30, 0xf3, 0xd4, 0x14, 0x54, 0x41,
    0x15, 0x54, 0xc7, 0x79, 0x9c, 0x33, 0x36, 0x63, 0x0f, 0x30, 0x61, 0xae, 0x90, 0xd9, 0xd4, 0x14,
    0x54, 0x41, 0x15, 0x54, 0xc7, 0x79, 0x9c, 0x33, 0x36, 0x63};
    
//SWITCH_USER_CAL_ADD						0X8010
//switch_user_cal_t
//之前版本校准数 lx:0 2048 4095, ly:0 2048 4095, rx:0 2048 4095, ry:0 2048 4095, acc: 471 -9 435, gyro:0 0 1 
//修改到        lx:0 2048 4095, ly:0 2048 4095, rx:0 2048 4095, ry:0 2048 4095, acc: 0 0 0, gyro:0 0 0 
static uint8c_t switch_user_cal_default[] =
{
    //数据来源：根据C1实际校准数据获得
    0xb2, 0xa1, 0xff, 0xf7, 0x7f, 0x00, 0x08, 0x80, 0x00, 0x08, 0x80, 0xb2, 0xa1, 0x00, 0x08, 0x80, 
    0x00, 0x08, 0x80, 0xff, 0xf7, 0x7f, 0xb2, 0xa1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
    0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0x3b, 0xe7, 0x3b, 0xe7, 0x3b,
};

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#if API_NFC_ENABLE
switch_nfc_t switch_dev_nfc;
#endif
switch_ctb_t switch_dev_ctb;
//switch 手柄校准参数,默认80,由产品自行校准
//switch repot id,表示switch从机当前report模式,目前只对蓝牙模式有效, 蓝牙断开后要恢复0X3F


#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)      //延时0.5s发送蓝牙数据包
timer_t m_switch_bt_timer;  
#endif


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static timer_t switch_ctrl_replies_timer;    //packet cmd timer,Timer. Increments, Joy-Con uses 4.96ms as the timer tick rate
static timer_t swtich_report_timer;         //report timer

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

static void switch_dump_cmd_in(char* tag, uint8_t* buf, uint16_t len)
{
    static uint8_t s_cmd, s_sub_cmd;

    switch_ctrl_replies_t* switch_repliesp=(switch_ctrl_replies_t*)buf;
    
    if((s_cmd != switch_repliesp->id) || (s_sub_cmd != switch_repliesp->sub_cmd)){
        s_cmd = switch_repliesp->id;
        s_sub_cmd = switch_repliesp->sub_cmd;
        logd("%sin cmd=%x,%x, len=%d:",tag,switch_repliesp->id,switch_repliesp->sub_cmd,len);dumpd(buf,len);
    }
}
static void switch_dump_cmd_out(char* tag, uint8_t* buf, uint16_t len)
{
    static uint8_t s_cmd, s_sub_cmd;
    static uint32_t s_pa;

    uint32_t pa;
    switch_ctrl_t* switch_ctrlp=(switch_ctrl_t*)buf;

    if(SWITCH_RUMBLE_ID == switch_ctrlp->id) return;
    memcpy(&pa,switch_ctrlp->cmd_data,4);
    pa = SWAP32_L(pa);

    if((s_cmd != switch_ctrlp->id) || (s_sub_cmd != switch_ctrlp->sub_cmd) || (s_pa != pa)){
        s_cmd = switch_ctrlp->id;
        s_sub_cmd = switch_ctrlp->sub_cmd;
        s_pa = pa;
        logd("%sout cmd=%x,%x pa=%x,  len=%d:",tag,switch_ctrlp->id,switch_ctrlp->sub_cmd,pa,len);dumpd(buf,len);
    }
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Function: 没有必要注释的函数可以不注释函数说明	
**	Description:
    bit7~5: 电量, bit4: 充电状态, bit0~3 Connection info
    电量:
        0x01: <10%电量不足
        0x02: <30%
        0x03: <75%
        0x04: <100% ?
** Parameters:		
** Returns:			
*******************************************************************/
uint8_t switch_battery_encode(trp_handle_t *phandle)
{
    uint8_t switch_battery;
    #if APP_BATTERY_ENABLE
        if(m_battery < 10){
            switch_battery = ((0x01) << 5);
        }else if(m_battery < 30){
            switch_battery = ((0x02) << 5);
        }else if(m_battery < 70){
            switch_battery = ((0x03) << 5);
        }else{
            switch_battery = ((0x03) << 5);
        }
        if((BAT_CHARGE_DONE_STA == m_battery_sta) || (BAT_CHARGE_STA == m_battery_sta)){
            switch_battery |= (1 << 4) | 1;
        }
    #else
        switch_battery = 0x91;
    #endif

    return switch_battery;
}

void get_switch_mac_addr(uint8_t* macp)
{
    memcpy(macp, "\xCA\xB9\x4D\xE9\xB6\x98", 6);    //谷粒 default

    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID0, BT_EDR, macp);
	#elif BT1_SUPPORT & BIT_ENUM(TR_EDR)
	api_bt_get_mac(BT_ID1, BT_EDR, macp);
	#endif
	
    swap_buf(macp,6);              //MAC address is Big Endian
}

bool switch_info_reply(trp_handle_t *phandle)
{
    uint8_t buf[64];
    switch_usb_mac_replies_t switch_info;

    switch_info.id = SWITCH_USB_REPLIES_ID;
    switch_info.sub_cmd = SWITCH_USB_REQUEST_MAC;
    switch_info.res = 0;
    switch_info.type = SWITCH_TYPE_PROCONTROLLER;
    get_switch_mac_addr(switch_info.mac);
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &switch_info, sizeof(switch_info));

    return api_transport_tx(phandle,buf,64);
}


/*********************************************************************************
** Function:
** Parameters:
** Returns:
** details:
	parse:
	encoded_hex_freq = hf;
	encoded_hex_freq = lf*2;
	freq = pow(2.0,encoded_hex_freq/32)*10;
	前期只返回最大强度
原理参考：https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/rumble_data_table.md
** 不震动的数据
** {0x00, 0x01, 0x40, 0x40},
** {0x02, 0x00, 0x30, 0x80},
** {0x80, 0x8C, 0x01, 0x80},
** {0x80, 0x8C, 0x61, 0x80},
** {0x64, 0x00, 0x64, 0x80},
** {0x62, 0x0C, 0x63, 0x40},      //switch主机游戏风花雪月USB下发异常震动
** {0xCA, 0x99, 0x31, 0x40},
** {0x06, 0x80, 0xE5, 0x40},
** {0xC6, 0x18, 0x83, 0x40},
** {0xC7, 0x18, 0xE0, 0x40},
** {0xC7, 0x18, 0xE0, 0x40},
** {0x80, 0x8C, 0x61, 0x80},
** {0xCB, 0x99, 0x31, 0x40},
** {0x66, 0x0C, 0x83, 0x40},
** {0x06, 0x20, 0x33, 0x60},
** {0x66, 0x0C, 0x60, 0x60},
** {0xCE, 0x39, 0x33, 0x60},
** {0x07, 0x20, 0xB3, 0x40},
** {0x81, 0x8C, 0x61, 0x80},
** 震动的数据
** {0xe4, 0x02, 0x4d, 0x46},
** {0x03, 0x00, 0x0C, 0x80},
** {0x04, 0x00, 0x33, 0x72},
** {0x80, 0xf9, 0x61, 0x80},
** {0xe4, 0x04, 0x4d, 0x40},
** {0x6d, 0x1e, 0x3c, 0x82},
** {0xe4, 0x02, 0x4d, 0x46},
** {0x6d, 0x00, 0x3c, 0x80},
*********************************************************************************/
static uint8c_t siwtch_rumble_non_vibration[][4] = {        //switch不震动的数据组
    {0x64, 0x00, 0x64, 0x80},\
    {0x62, 0x0C, 0x63, 0x40},\
    {0xCA, 0x99, 0x31, 0x40},\
    {0x06, 0x80, 0xE5, 0x40},\
    {0xC6, 0x18, 0x83, 0x40},\
    {0xC7, 0x18, 0xE0, 0x40},\
    {0x80, 0x8C, 0x61, 0x80},\
    {0xCB, 0x99, 0x31, 0x40},\
    {0x66, 0x0C, 0x83, 0x40},\
    {0x06, 0x20, 0x33, 0x60},\
    {0x66, 0x0C, 0x60, 0x60},\
    {0xCE, 0x39, 0x33, 0x60},\
    {0x07, 0x20, 0xB3, 0x40},\
    {0x81, 0x8C, 0x61, 0x80},\
};  
uint8_t switch_rumble_decode(switch_rumble_bit_t* prumble_bit)
{
    uint16_t lamp=0, hamp=0;
    uint8_t i;
	uint16_t h_freq,l_freq;
	h_freq = (uint16_t)(round)(pow(2.0,(prumble_bit->hf/4+0x60)/32.0)*10);
	l_freq = (uint16_t)(round)(pow(2.0,(prumble_bit->lf+0x40)/32.0)*10);
    for(i=0;i<sizeof(siwtch_rumble_non_vibration)/sizeof(switch_rumble_bit_t);i++){//TODO 协议解析不适用,暂处理
        if(0 == memcmp((uint8_t *)prumble_bit, siwtch_rumble_non_vibration[i],sizeof(switch_rumble_bit_t))){
            return 0;
        }
    }
    if((prumble_bit->lf_amp >= 0x80 && prumble_bit->lf > 1) && prumble_bit->hf > 0x03){
        lamp = prumble_bit->lf_amp-0x80;
        hamp = prumble_bit->hf_amp;
    }
    //logd("l=%d %d h=%d %d",l_freq,lamp,h_freq,hamp);
    return remap(MAX(lamp, hamp), 0, 100, 0, 255);
}


uint16_t switch_normal_key_pack(trp_handle_t *phandle,const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
    switch_normal_report_t* switchp = (switch_normal_report_t*)buf;

    if(len < sizeof(switch_normal_report_t)) return 0;

	switchp->id = SWITCH_NORMAL_REPORT_ID;
	switchp->button = keyp->key & 0xffff;
	switchp->hat_switch = gamepad_key_to_hatswitch(keyp->key);
	switchp->lx = keyp->stick_l.x + 0x8000;
	switchp->ly = keyp->stick_l.y + 0x8000;
	switchp->rx = keyp->stick_r.x + 0x8000;
	switchp->ry = keyp->stick_r.y + 0x8000;

	packet_len = sizeof(switch_normal_report_t);
	return packet_len;
}


uint16_t switch_standard_key_pack(trp_handle_t *phandle,const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
    axis3i_t axis;
	uint16_t adc_x, adc_y; //12 bit
    switch_report_t* switchp = (switch_report_t*)buf;
    
    if(len < sizeof(switch_report_t)) return 0;

    switchp->id = SWITCH_STANDARD_REPORT_ID;
    switchp->battery = switch_battery_encode(phandle);

    if(switchp->battery & (0x01<<4)){
        switchp->buttonl = 0x8000;      //充电按键
    }
    switchp->buttonl |= (keyp->key & 0xffff);
    switchp->buttonh = keyp->key >> 16;
    switchp->buttonl = SWAP16_L( switchp->buttonl );

    adc_x = (keyp->stick_l.x + 0x8000)>>4;
    adc_y = (keyp->stick_l.y + 0x8000)>>4;
    int_to_bit12(switchp->l_xy, adc_x, adc_y);

    adc_x = (keyp->stick_r.x + 0x8000)>>4;
    adc_y = (keyp->stick_r.y + 0x8000)>>4;
    int_to_bit12(switchp->r_xy, adc_x, adc_y);

    switchp->index = (m_systick - swtich_report_timer)/5;

    if(switch_dev_ctb.imu_enable){
        //8G
        axis.x = -keyp->acc.x;
        axis.y = keyp->acc.y;
        axis.z = keyp->acc.z;
        axis3i_swapl(&axis);
        switchp->acc = axis;

        //2K
        axis.x = keyp->gyro.x;
        axis.y = -keyp->gyro.y;
        axis.z = -keyp->gyro.z;
        axis3i_swapl(&axis);
        switchp->gyro = axis;
        
        switchp->acc2 = switchp->acc;
        switchp->gyro2 = switchp->gyro;
        switchp->acc3 = switchp->acc;
        switchp->gyro3 = switchp->gyro;
    }
    
    if(switch_dev_ctb.vibration_enable){
        switchp->vibrator_report = 0x0C;
    }
    
    if(api_trp_is_bt(phandle->trp)){           //蓝牙结构体长度(+id)
        packet_len = sizeof(switch_report_t);
    }else{
        packet_len = 64;
    }

	return packet_len;
}

extern void switch_dump_nfc_data(switch_large_report_t* switchp,uint16_t len);
uint16_t switch_large_key_pack(trp_handle_t *phandle,const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
    switch_large_report_t* switchp = (switch_large_report_t*)buf;

    if(len < sizeof(switch_report_t)) return 0;         //注意 switch_large_report_t 长度是动态的,保证基本长度即可
    switch_standard_key_pack(phandle,keyp, buf,len);     //这里填充按键和体感数据

    switchp->id = SWITCH_LARGE_REPORT_ID;
    
    #if API_NFC_ENABLE
    switch_dev_nfc.nfc_ic_state = m_api_nfc_sta;
    switch_nfc_replies_pack(&switch_dev_nfc,switchp);
    switchp->nfc_crc = crc8(switchp->nfc,sizeof(switchp->nfc));
    switch_dump_nfc_data(switchp,len);
	#endif
	return len;
}

/*******************************************************************
** Function: 	
** Parameters:	
** 注意 switch蓝牙模式必须连接后0.5s后再发送数据
** Returns:			
*******************************************************************/
uint16_t switch_key_pack(trp_handle_t *phandle, const app_gamepad_key_t *keyp, uint8_t* buf,uint16_t len)
{
	uint16_t packet_len=0;
    switch_report_t* switchp = (switch_report_t*)buf;
    memset(buf,0,len);

    if(TR_USBD == phandle->trp || SWITCH_STANDARD_REPORT_ID == switch_dev_ctb.report_mode){
        packet_len = switch_standard_key_pack(phandle,keyp,buf,len);
    #if	BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
    }else if(SWITCH_LARGE_REPORT_ID == switch_dev_ctb.report_mode){
        packet_len = switch_large_key_pack(phandle,keyp,buf,len);
    }else if(SWITCH_NORMAL_REPORT_ID == switch_dev_ctb.report_mode){
        if(m_systick - m_switch_bt_timer > 500){
            packet_len = switch_normal_key_pack(phandle,keyp,buf,len);
        }
    #endif
    }
	return packet_len;
}


/*******************************************************************
** Function: 	
** Parameters:		
** switch主机枚举过程
    out 2:		    
    out 8:		    SUB_SET_SHIPMENT_LOW_POWER_STATE
    out 10:	00 60   
    out 10:	50 60   
    out 3:	30	    
    out 4:		    SUB_TRIGGER_BUTTONS_ELAPSED_TIME
    out 10:	80 60   
    out 10:	98 60   
    out 10: 10 80   
    out 10: 3D 60   
    out 10: 28 80   
    out 40 : 01	    SUB_ENABLE_IMU
    out 48:  01		SUB_ENABLE_VIBRATION ..之后 vibrator_report 变0C
    out 21:  21		SUB_SET_NFC_IR_MCU_CONFIGURATION
    out 30:  01		SUB_SET_PLAYER_LIGHTS
** steam 枚举过程
    out 2:				
    out 10:	e0 60		
    out 40 : 01			SUB_ENABLE_IMU
    out 41 : 03			SUB_SET_IMU_SENSITIVITY
    out 10:	20 60		
    out 10:	80 60		
    out 10: 3D 60		
    out 48: 01			SUB_ENABLE_VIBRATION
    out 30:	00			SUB_SET_PLAYER_LIGHTS	
    out 30:	01			
    out 30:	00			
    out 3:	30			//如果已经打开30 report 就不会再发送		
    out 30:	01			
    out 30:	01			
    out 30:	01		
** Returns:			
*******************************************************************/
bool switch_ctrl_id_process(trp_handle_t* phandle, uint8_t* buf,uint8_t len)
{
    bool ret = 0;
    uint16_t adc_x, adc_y; //12 bit
    
    switch_ctrl_t* switch_ctrlp = (switch_ctrl_t*)buf;
    switch_ctrl_replies_t ctrl_replies;
    uint8_t sum_cmd = switch_ctrlp->sub_cmd;
    uint8_t sub_ack = 0x80;

    memset(&ctrl_replies,0,sizeof(ctrl_replies));

    switch (sum_cmd)
    {
    case SUB_BLUETOOTH_MANUAL_PAIRING:
        sub_ack |= sum_cmd;
        ctrl_replies.cmd_data[0] = 0x03;

        break;
    case SUB_REQUEST_DEVICE_INFO:
    {
        switch_device_info_t *device_infop;
        sub_ack |= sum_cmd;
        device_infop = (switch_device_info_t*)ctrl_replies.cmd_data;
        device_infop->version_l = 0x03;   //firmwart version
        device_infop->version_h = 0x48;   //firmwart version
        device_infop->device_type = 0x03; //switch pro controller
        device_infop->unknown1 = 0x02;        //fixed TODO
        get_switch_mac_addr(device_infop->mac_data);
        // memcpy(&switch_device_info->mac_data, &mac_addr, 6);
        device_infop->unknown2 = 0x01;                                                  //switch pro controller
        device_infop->unknown3 = 0x03;                                                  //fixed
    	// switch_dev_ctb.report_mode = 0;			//如果蓝牙问题解决后可以打开
        break;
    }
    case SUB_SET_INPUT_REPORT_MODE:
        switch_dev_ctb.report_mode = switch_ctrlp->cmd_data[0];
        logi("switch_report_mode=%x\n",switch_dev_ctb.report_mode);
        break;
    case SUB_TRIGGER_BUTTONS_ELAPSED_TIME:
        sub_ack = 0x83;
        ctrl_replies.cmd_data[0] = 0xff;
        ctrl_replies.cmd_data[1] = 0x01;
        break;

    case SUB_SET_SHIPMENT_LOW_POWER_STATE:
        break;
    case SUB_SET_PLAYER_LIGHTS:
        switch_dev_ctb.lights = switch_ctrlp->cmd_data[0];
        logi("switch lights=%x\n",switch_dev_ctb.lights);  
        break;
    case SUB_ENABLE_IMU:
        switch_dev_ctb.imu_enable = switch_ctrlp->cmd_data[0];
        break;
    case SUB_ENABLE_VIBRATION:
        switch_dev_ctb.vibration_enable = switch_ctrlp->cmd_data[0];
        break;
    case SUB_SET_HCI_STATE:
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)          //延时0.5s发送蓝牙数据包
        // if(m_edr_enable) api_bt_enable(BT_EDR,true);     //TODO 调试用
        #endif
        break;
    case SUB_SET_NFC_IR_MCU_STATE:
        #if API_NFC_ENABLE
        if(0x01 == switch_ctrlp->cmd_data[0]){  //00:Suspend,01:Resume,02:Resume for update
            switch_dev_nfc.mcu_resum  = true;                         //收到31模式设置回复后,发reset
        }
        #endif
        break;
    case SUB_SPI_FLASH_WRITE:
    {
        uint32_t address;
        switch_flash_data_t *flash_writep = (switch_flash_data_t*)switch_ctrlp->cmd_data;
        address = SWAP32_L(flash_writep->addr);
        logi("switchd flash write add=%x, len=%d\n", address, flash_writep->length); 
        //dumpd(flash_writep->spi_data, flash_writep->length);
        break;
    }
    case SUB_SPI_FLASH_READ:
    {
        uint32_t address;
        uint32_t offset;
        uint8_t len;
        switch_flash_data_t *flash_readp = (switch_flash_data_t*)switch_ctrlp->cmd_data;
        switch_flash_data_t *flash_reportp = (switch_flash_data_t*)ctrl_replies.cmd_data;
        sub_ack |= sum_cmd;

        address = SWAP32_L(flash_readp->addr);
        len = flash_readp->length;
        flash_reportp->addr = flash_readp->addr;
        flash_reportp->length = flash_readp->length;

        memset(flash_reportp->spi_data,0xff,MIN(sizeof(flash_reportp->spi_data),flash_reportp->length));   //默认发0XFFFF

        if(address >= 0x8010) {              //区分数据块,方便以后修改数据
            offset = address - 0x8010;
            if(offset <= sizeof(switch_user_cal_t)){
                len = MIN(len,sizeof(switch_user_cal_t) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_user_cal_default + offset,len);
            }
        }else if(address >= 0x6080) {
            offset = address - 0x6080;
            if(offset <= sizeof(switch_imu_stick_para_t)){
                len = MIN(len,sizeof(switch_imu_stick_para_t) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_imu_stick_para_default + offset,len);
            }
        }else if(address >= 0X6050) {
            offset = address - 0X6050;
            if(offset <= sizeof(switch_flash_rgb_t)){
                len = MIN(len,sizeof(switch_flash_rgb_t) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_flash_rgb_default + offset,len);
            }
        }else if(address >= 0X603D) {
            offset = address - 0X603D;
            if(offset <= sizeof(switch_stick_cal_t)){
                len = MIN(len,sizeof(switch_stick_cal_t) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_stick_cal_default + offset,len);
            }
        }else if(address >= 0X6020) {
            offset = address - 0X6020;
            if(offset <= sizeof(switch_imu_cal_t)){
                len = MIN(len,sizeof(switch_imu_cal_t) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_imu_cal_default + offset,len);
            }
        }else if(address >= 0X6000) {
            offset = address - 0X6000;
            if(offset <= sizeof(switch_imu_stick_para_t)){
                len = MIN(len,sizeof(switch_serial_number_default) - offset);
                memcpy(flash_reportp->spi_data, (uint8_t*)switch_serial_number_default + offset,len);
            }
        }else{
            logi("switchd flash read add=%x len=%d unknow\n",address,flash_readp->length);
        }

        break;
    }
    case SUB_SET_NFC_IR_MCU_CONFIGURATION:
        //Replies with ACK xA0 x20 and 34 bytes of data.
        sub_ack = 0xA0;

        #if API_NFC_ENABLE
            extern void switch_mcu_state_pack(switch_nfc_t *pnfc,switch_mcu_state_t*mcu_statep);
            switch_dev_nfc.nfc_ic_state = m_api_nfc_sta;
            switch_mcu_state_pack(&switch_dev_nfc,(switch_mcu_state_t*)ctrl_replies.cmd_data);
            ctrl_replies.sub_crc8 = crc8(ctrl_replies.cmd_data,sizeof(ctrl_replies.cmd_data));
            if(0x21 == switch_ctrlp->cmd_data[0] && switch_ctrlp->cmd_data[2]){
                switch_dev_nfc.mcu_state = switch_ctrlp->cmd_data[2];     //设置mcu状态
                logi("set pnfc->mcu_state=%x\n",switch_dev_nfc.mcu_state);
            }
        #else
            ctrl_replies.cmd_data[0] = 0x01;
            ctrl_replies.cmd_data[1] = 0x00;
            ctrl_replies.cmd_data[2] = 0xff;
            ctrl_replies.cmd_data[3] = 0x00;
            ctrl_replies.cmd_data[4] = 0x03;
            ctrl_replies.cmd_data[5] = 0x00;
            ctrl_replies.cmd_data[6] = 0x05;
            ctrl_replies.cmd_data[7] = 0x06;
            ctrl_replies.sub_crc8 = 0x62;
        #endif
        break;
    default:
        //ctrl_replies.cmd_data[0]  = 0x03;
        //All subcommands that do nothing, reply back with ACK 0Xx8 and x03
        break;
    }

    //REPORT ID
    ctrl_replies.id = SWITCH_CTRL_REPLIES_ID;
    ctrl_replies.index = (m_systick - switch_ctrl_replies_timer)/5;

    if(switch_dev_ctb.vibration_enable){
        if(SUB_SET_NFC_IR_MCU_STATE != sum_cmd && SUB_SET_NFC_IR_MCU_CONFIGURATION != sum_cmd){        //0x22 21 指令这里回复0
            ctrl_replies.vibrator_report = 0x0C;
        }
    }
    ctrl_replies.sub_ack = sub_ack;
    ctrl_replies.sub_cmd = sum_cmd;
    ctrl_replies.battery = switch_battery_encode(phandle);
    if(ctrl_replies.battery & (0x01<<4)){
        ctrl_replies.buttonl = 0x8000;      //充电按键
    }
    ctrl_replies.buttonh = 0;
    adc_x = (0x8000) >> 4;
    adc_y = (0x8000) >> 4;
    int_to_bit12(ctrl_replies.l_xy, adc_x, adc_y);
    adc_x = (0x8000) >> 4;
    adc_y = (0x8000) >> 4;
    int_to_bit12(ctrl_replies.r_xy, adc_x, adc_y);

    if(TR_EDR == phandle->trp){
        ret = api_transport_tx(phandle,(uint8_t*)&ctrl_replies,49);     //蓝牙只发送49个字节
    }else{
        ret = api_transport_tx(phandle,(uint8_t*)&ctrl_replies,sizeof(ctrl_replies));
    }
    switch_dump_cmd_in("dev:",(uint8_t*)&ctrl_replies,49);
    return ret;
}


bool switch_usb_id_process(trp_handle_t* phandle, uint8_t* buf,uint8_t len)
{
    uint8_t length = 0;
    bool ret = false;
    uint8_t replies[64];
    memset(replies,0,sizeof(replies));
    replies[0] = 0x81;
    replies[1] = buf[1];

    switch (buf[1])
    {
    case SWITCH_USB_REQUEST_MAC:        //0x01
        replies[2] = 0x00;
        replies[3] = SWITCH_TYPE_PROCONTROLLER; //type
        get_switch_mac_addr(replies + 4);

        length = sizeof(replies);
        break;
    case SWITCH_USB_HANDSHAKE:      //0x02
        length = sizeof(replies);
        break;
    case SWITCH_USB_BAUDRATE:       //0x03
        length = sizeof(replies);
        break;
    case SWITCH_USB_DIS_TIMEOUT: {      //0x04
        #if API_USBD_BIT_ENABLE && (USBD_HID_SUPPORT & HID_SWITCH_MASK)
        usbd_set_ready(phandle->id, true);
        #endif

        logi("switch dev Handshake start\n");
        ret = true;
        break;
    }
    case SWITCH_USB_ENABLE_TIMEOUT:     //0x05
        ret = true;
        break;
    default:
        ret = true;
        break;
    }

    if (length != 0){
        switch_dump_cmd_in("dev:",replies,length);

        if(TR_EDR == phandle->trp){
            ret = api_transport_tx(phandle,replies,49);     //蓝牙只发送49个字节
        }else{
            ret = api_transport_tx(phandle,replies,length);
        }
    }
    return ret;
}


/*******************************************************************************
* 描述    : 主要用于握手阶段处理
* 输入    : buf  指向输出数据   len 数据长度
* 输出    : 是否是switch 枚举指令
* 返回值  :

out 从0快开始,第10,11bit是指令
in  从0快开始,第13,14bit是指令回复
*******************************************************************************/
bool switch_dev_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len)
{
    bool ret = false;
    uint8_t edr_hid_req = 0;
    static rumble_t s_rumble;

    if(TR_EDR == phandle->trp){		//edr 判断 hid_report_type_t
		edr_hid_req = buf[0];
        len -= 1;
		buf += 1;
		if ( 0 == len) return false;
	}

    switch_dump_cmd_out("dev:",buf,len);

    switch (buf[0])
    {
    case SWITCH_USB_ID:             //0x80
        switch_usb_id_process(phandle,buf,len);
        ret = true;
        break;
     case SWITCH_CTRL_ID:           //0x01
        switch_ctrl_id_process(phandle,buf,len);
        ret = true;
        break;
    case SWITCH_RUMBLE_ID:           //0X10
	{
        return false;

        #if APP_RUMBLE_ENABLE
        switch_rumble_t *p = (switch_rumble_t*)buf;
        uint8_t rumble_l,rumble_r;
        logd("rubmle:");dumpd(p, sizeof(switch_rumble_t));
        rumble_l = switch_rumble_decode(&p->rumble_l);
        rumble_r = switch_rumble_decode(&p->rumble_r);
        if(rumble_l != s_rumble.duty[RUMBLE_L] || rumble_l != 0){
            s_rumble.duty[RUMBLE_L] = rumble_l;
            rumble_l = (uint8_t)(pow(rumble_l/255.0, 0.5)*255.0);
            if(rumble_l){                                             //rumble为0不做处理，防止马达震动数据下发太快导致不震动
                app_rumble_set_duty(RUMBLE_L, rumble_l, 150);               //最后震动数据延时150ms 体验感最好
            }
        }
        if(rumble_r != s_rumble.duty[RUMBLE_R] || rumble_r != 0){
            s_rumble.duty[RUMBLE_R] = rumble_r ;
            rumble_r = (uint8_t)(pow(rumble_r/255.0, 0.5)*255.0);
            if(rumble_r){
                app_rumble_set_duty(RUMBLE_R, rumble_r, 150);
            }
        }
        #endif
        
        ret = true;
        break;
	}
    case SWITCH_MCU_CTRL_ID:        	//0X11
        #if API_NFC_ENABLE
        switch_nfc_req_process(&switch_dev_nfc,buf,len);
        #endif
        ret = true;
        break;
    default:
        break;
    }
	return ret;
}


bool switch_device_init(trp_handle_t *phandle)
{
    switch_dev_ctb.vibration_enable = false;
    switch_dev_ctb.imu_enable = false;
    switch_dev_ctb.report_mode=SWITCH_NORMAL_REPORT_ID;           //默认switch repot id
    switch_dev_ctb.lights = 0;
    //nfc
    #if API_NFC_ENABLE
    switch_nfc_init(&switch_dev_nfc);
    #endif
   
    return true;
}

bool switch_device_deinit(trp_handle_t *phandle)
{
    switch_dev_ctb.vibration_enable = false;
    switch_dev_ctb.imu_enable = false;
    switch_dev_ctb.report_mode=SWITCH_NORMAL_REPORT_ID;
    switch_dev_ctb.lights = 0;

    return true;
}

void switch_device_task(trp_handle_t *phandle)
{

}

#endif







