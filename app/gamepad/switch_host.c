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
#if (HIDH_SUPPORT & HID_SWITCH_MASK)
#include  "utils/emf_utils.h"
#include "api/api_tick.h"
#include "app/gamepad/switch_host.h"

#if USBH_HID_SUPPORT & HID_SWITCH_MASK
#include "api/usb/host/usbh.h"
#endif


#define DEBUG_LOG_ENABLE     	1
#include "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/


#define SWITCH_ENUM_RETRY_NUM   10
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
switch_enum_t usb_switch_enump;
switch_enum_t bt_switch_enump;


//switch 手柄校准参数,如果是switch主机m_switch_cal在枚举的时候读取
switch_cal_t m_switch_cal;     
switch_ctb_t switch_host_ctb;
#if APP_NFC_ENABLE
switch_nfc_t switch_host_nfc;
#endif
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

static void switch_dump_cmd_in(char* tag, uint8_t* buf, uint16_t len)
{
    static uint8_t s_cmd, s_sub_cmd;
    static uint32_t s_pa;

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

    if(SWITCH_MOTOR_ID == switch_ctrlp->id) return;
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
void switch_dump_cal(void)
{
    logd("switch_cal:\n");
    logd("lx:%d %d %d, ",m_switch_cal.l_min.x,m_switch_cal.l_mid.x,m_switch_cal.l_max.x);
    logd("ly:%d %d %d, ",m_switch_cal.l_min.y,m_switch_cal.l_mid.y,m_switch_cal.l_max.y);
    logd("rx:%d %d %d, ",m_switch_cal.r_min.x,m_switch_cal.r_mid.x,m_switch_cal.r_max.x);
    logd("ry:%d %d %d, ",m_switch_cal.r_min.y,m_switch_cal.r_mid.y,m_switch_cal.r_max.y);
    logd("acc: %d %d %d, ",m_switch_cal.acc.x,m_switch_cal.acc.y,m_switch_cal.acc.z);
    logd("gyro:%d %d %d \n",m_switch_cal.gyro.x,m_switch_cal.gyro.y,m_switch_cal.gyro.z);
}


/*********************************************************************************
** Function:
** Parameters:
** Returns:
** details:
	freq <= 1252.0f;
	encoded_hex_freq = (uint8_t)round(log2((double)freq/10.0)*32.0);
	uint16_t hf = encoded_hex_freq;
	uint8_t lf = encoded_hex_freq/2;

	uint16_t hf_amp = encoded_hex_amp;
	uint8_t lf_amp = encoded_hex_amp + 0x80;// (encoded_hex_amp>>1)+0x40;

	默认高频320:0x100 低频160:0x80
*********************************************************************************/
switch_motor_bit_t switch_motor_encode(uint8_t duty)
{
	switch_motor_bit_t motor_bit;
	uint16_t encoded_hex_amp;
	//uint16_t freq = 160;
	//uint16_t encoded_hex_freq;

	// if(freq > 1252) freq = 1252;
	// encoded_hex_freq = (uint8_t)round(log2(freq/10.0)*32.0);
	// motor_bit.hf = (encoded_hex_freq-0x60)*4;
	// motor_bit.lf = encoded_hex_freq-0x40;

	motor_bit.lf = 0x40; //160hz
	motor_bit.hf = 0x080; //160hz 暂时用不到频率,默认写死

	encoded_hex_amp = remap(duty, 0, 255 ,0, 64);
	motor_bit.hf_amp = encoded_hex_amp;
	motor_bit.lf_amp = encoded_hex_amp + 0x80;

	// logd("freq=%d %d, amp=%d %d\n",pmotor->hf,pmotor->lf,pmotor->hf_amp,pmotor->lf_amp);
	// logd("hf=%x lf=%x ,hf_amp=%x, lf_amp=%x\n",motor_bit.hf,motor_bit.lf,motor_bit.hf_amp,motor_bit.lf_amp);
	return motor_bit;
}

bool switch_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
    static uint8_t motor_index=0;
    
    switch_motor_t motor;
    uint8_t motor1 = prumble->duty[0];
	uint8_t motor2 = prumble->duty[1];
    motor.id = 0x10;
    motor.index=motor_index++;

    motor.motor1 = switch_motor_encode(motor1);
    motor.motor2 = switch_motor_encode(motor2);
    return api_transport_tx(phandle,(uint8_t*)&motor,sizeof(motor));
}



bool switch_key_decode(trp_handle_t *phandle,uint8_t* buf, uint16_t len, app_key_t*keyp)
{
	bool ret = false;
	uint32_t key=0;
    axis3i_t axis;
    uint32_t buttonl;
    switch_report_t* switch_inp = (switch_report_t*)buf;

    //switch 自动切换可以不解析 SWITCH_NORMAL_REPORT_ID
	if((SWITCH_LARGE_REPORT_ID == switch_inp->id) || (SWITCH_STANDARD_REPORT_ID == switch_inp->id)){
        buttonl = SWAP16_L(switch_inp->buttonl);

		key = buttonl | ((uint32_t)(switch_inp->buttonh)<<16);
		key &= ~0x8000UL;
		keyp->key = key;
    
        keyp->trigger.y = (SWITCH_R2 & key)? 0xff : 0;
        keyp->trigger.x = (SWITCH_L2 & key)? 0xff : 0;

		bit12_to_uint(switch_inp->l_xy,&keyp->stick_l.x, &keyp->stick_l.y);

        keyp->stick_l.x = remap(keyp->stick_l.x, m_switch_cal.l_min.x,m_switch_cal.l_max.x,-32768,32767);
        keyp->stick_l.y = remap(keyp->stick_l.y, m_switch_cal.l_min.y,m_switch_cal.l_max.y,-32768,32767);

		bit12_to_uint(switch_inp->r_xy,&keyp->stick_r.x, &keyp->stick_r.y);
        keyp->stick_r.x = remap(keyp->stick_r.x, m_switch_cal.r_min.x,m_switch_cal.r_max.x,-32768,32767);
        keyp->stick_r.y = remap(keyp->stick_r.y, m_switch_cal.r_min.y,m_switch_cal.r_max.y,-32768,32767);

		axis = switch_inp->acc;
        axis3i_swapl(&axis);
        axis3i_sub(&axis, &m_switch_cal.acc);
        axis.x = 0 - axis.x;
        keyp->acc = axis;

		axis = switch_inp->gyro;
		axis3i_swapl(&axis);
        axis3i_sub(&axis, &m_switch_cal.gyro);
        axis.y = 0 - axis.y;
        axis.z = 0 - axis.z;
        keyp->gyro = axis;
		ret = true;
	}
	return ret;
}




/*
SWITCH_USB_DIS_TIMEOUT              //0x80 0x02     //usb only
SUB_SET_INPUT_REPORT_MODE           //0x03 0x30     //usb only  蓝牙不在这里切换模式, 发送02后自动会切换到30, 如果没有切换, 再去发03切换
SWITCH_USB_DIS_TIMEOUT              //0x80 0x04     //usb only
SUB_REQUEST_DEVICE_INFO             //0x02 0x00
    //SUB_SET_SHIPMENT_LOW_POWER_STATE        //0x08 0x00   测试运行后不会连接蓝牙
SUB_SPI_FLASH_READ                  //0x10    0X603D  
SUB_SPI_FLASH_READ                  //0x10    0X6020  
SUB_SPI_FLASH_READ                  //0x10    0X8010  
SUB_SPI_FLASH_READ                  //0x10    0X8026  
SUB_ENABLE_IMU                      //0x40 0x01
SUB_ENABLE_VIBRATION                //0x48 0x01
SUB_SET_NFC_IR_MCU_CONFIGURATION    //0x21 0x21		
SUB_SET_PLAYER_LIGHTS               //0x30 0x01
*/
//pa:小端参数
void switch_set_emu(switch_enum_t* enump, uint8_t cmd,uint8_t sub_cmd,uint32_t pa)
{
    enump->cmd = cmd;
    enump->sub_cmd = sub_cmd;
    enump->param = pa;
    enump->retry = SWITCH_ENUM_RETRY_NUM;
}

bool switch_emu_ctrl_send(trp_handle_t* phandle, switch_enum_t* enump)
{
    static uint8_t switch_ctrl_index = 0;           //暂时没有影响
    bool ret = false;
    uint8_t buf[49];
    uint8_t len = 0;
    switch_ctrl_t* switch_ctrlp=(switch_ctrl_t*)buf;

    memset(buf,0,sizeof(buf));
    if(enump->retry >= SWITCH_ENUM_RETRY_NUM){

        if(SWITCH_USB_ID == enump->cmd){
            buf[0] = SWITCH_USB_ID;
            buf[1] = enump->param;
            len = 2;
            ret = api_transport_tx(phandle,buf,len);
        }else if(enump->cmd){
            switch_ctrlp->id = enump->cmd;
            switch_ctrlp->index = switch_ctrl_index++;
            switch_ctrlp->sub_cmd =  enump->sub_cmd;

            switch_ctrlp->motor1 = switch_motor_encode(0);
            switch_ctrlp->motor2 = switch_ctrlp->motor1;
            switch_ctrlp->cmd_data[0] = enump->param & 0xff;
            switch_ctrlp->cmd_data[1] = (enump->param>>8) & 0xff;
            switch_ctrlp->cmd_data[2] = (enump->param>>16) & 0xff;
            switch_ctrlp->cmd_data[3] = (enump->param>>24) & 0xff;

            if(SWITCH_CTRL_ID == enump->cmd){
                if(SUB_SPI_FLASH_READ == enump->sub_cmd){               //flash 度指令 需要指定长度
                    if(0X603D == enump->param){
                        switch_ctrlp->cmd_data[4] = sizeof(switch_stick_cal_t);
                    }else if(0X6020 == enump->param){
                        switch_ctrlp->cmd_data[4] = sizeof(switch_imu_cal_t);
                    }else if(0X8010 == enump->param){
                        switch_ctrlp->cmd_data[4] = 24;
                    }else if(0X8026 == enump->param){
                        switch_ctrlp->cmd_data[4] = sizeof(switch_user_imu_cal_t);
                    }
                }
            #if APP_NFC_ENABLE
            }else if(SWITCH_MCU_CTRL_ID == enump->cmd && MCU_SUB_REQ_NFC_DATA == enump->sub_cmd){
                switch_nfc_req_t* nfc_reqp = (switch_nfc_req_t*)switch_ctrlp->cmd_data;
                nfc_reqp->fragments = 0x08;
                if(SWITCH_NFC_START_POLLING_CMD == nfc_reqp->nfc_cmd){      //start polling 需要带5个参数
                    nfc_reqp->nfc_len = 0x05;
					logi("pnfc->nfc_ic_state=%d\n",switch_host_nfc.nfc_ic_state);
                    if(NFC_GET_TAG_SN == switch_host_nfc.nfc_ic_state){
                        memcpy(nfc_reqp->nfc_data,"\x00\xE8\x03\x2C\x01",5);		//第二次读取SN,可以忽略这个步骤,这里为了测试用
                    }else{
                        memcpy(nfc_reqp->nfc_data,"\x00\x00\x00\x2C\x01",5);		//第一次读取SN
                    }
                }else if(SWITCH_NFC_NTAG_READ_CMD == nfc_reqp->nfc_cmd){ 
                    nfc_reqp->nfc_len = 0x13;
                    memcpy(nfc_reqp->nfc_data,"\xD0\x07\x00\x00\x00\x00\x00\x00\x00\x01\x03\x00\x3B\x3C\x77\x78\x86\x00\x00",0x13);
                }
            #endif
            }

            len = sizeof(switch_ctrl_t);
            if(SWITCH_MCU_CTRL_ID == switch_ctrlp->id){
                switch_ctrlp->crc8 = crc8(switch_ctrlp->cmd_data,sizeof(switch_ctrlp->cmd_data));
            }else if (SWITCH_CTRL_ID == switch_ctrlp->id && SUB_SET_NFC_IR_MCU_CONFIGURATION == switch_ctrlp->sub_cmd){
                switch_ctrlp->sub_crc8 = crc8(switch_ctrlp->cmd_data+1,sizeof(switch_ctrlp->cmd_data));
            }
            ret = api_transport_tx(phandle,(uint8_t*)switch_ctrlp,len);
        }

        if(ret){
            switch_dump_cmd_out("enum:",switch_ctrlp,len);
            enump->cmd = 0;
            enump->retry = 0;
        }
    }
    return ret;
}

/*******************************************************************************
* 函数名  : 处理switch枚举  
* 描述    : 
* 输入    :
* 输出    : None
* 返回值  :true :是usb 枚举数据, false: 否
*******************************************************************************/
bool switch_in_process(trp_handle_t* phandle, uint8_t* buf,uint16_t len)
{
    bool ret = true;
    switch_enum_t* enump;

    if (len < 2) return ret;
    switch_dump_cmd_in("enum:",buf,len);
    
    if(api_trp_is_usb(phandle->trp)){
        enump = &usb_switch_enump;
    }else{
        enump = &bt_switch_enump;
    }
    
    switch (buf[0]){
    case SWITCH_USB_REPLIES_ID:             //0x80
        switch (buf[1]){
        case SWITCH_USB_REQUEST_MAC:        //0x01
            switch_set_emu(enump,SWITCH_USB_ID,0x00,SWITCH_USB_HANDSHAKE);   //0x80
            break;
        case SWITCH_USB_HANDSHAKE:          //0x02
            switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SET_INPUT_REPORT_MODE,0x30);   //0x03
            switch_host_ctb.report_mode = SWITCH_STANDARD_REPORT_ID;
            break;
        default:
            break;
        }
        break;
     case SWITCH_CTRL_REPLIES_ID:           //0x21
     {
        switch_ctrl_replies_t * ctrl_repliesp;
        if(len < 15) break;
        ctrl_repliesp = (switch_ctrl_replies_t*)buf;

        switch (ctrl_repliesp->sub_cmd){
        case SUB_BLUETOOTH_MANUAL_PAIRING:      //0x01
            break;
        case SUB_REQUEST_DEVICE_INFO:           //0x02
            switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SPI_FLASH_READ,0X603D);   //0x40
            break;
        case SUB_SET_INPUT_REPORT_MODE:         //0x03
            if(TR_USBH == phandle->trp){             //蓝牙不需要发0X80 04指令
                switch_set_emu(enump,SWITCH_CTRL_ID,SWITCH_USB_ID,SWITCH_USB_DIS_TIMEOUT);   //0X80
            }
            break;
        case SUB_SET_SHIPMENT_LOW_POWER_STATE:           //0x08
            break;
         case SUB_SPI_FLASH_READ:                       //0x10
         {
            axis2i_t min, mid,max;
            switch_flash_data_t *flash_reportp = (switch_flash_data_t*)ctrl_repliesp->cmd_data;
            if(enump->param == 0X603D) {
                switch_stick_cal_t* switch_stick_calp = (switch_stick_cal_t*)flash_reportp->spi_data;
                logd("flash read add=%lx len=%x\n",flash_reportp->addr,(uint16_t)(flash_reportp->length));

                bit12_to_uint(switch_stick_calp->lxy_max_sid,&max.x, &max.y);
                bit12_to_uint(switch_stick_calp->lxy_mid,&mid.x, &mid.y);
                bit12_to_uint(switch_stick_calp->lxy_min_sid,&min.x, &min.y);

                if( (max.x <= 3000) && (min.x <= 3000) ) {          //简单判断数据有效性
                    m_switch_cal.l_mid = mid;
                    m_switch_cal.l_min = mid;
                    m_switch_cal.l_max = mid;
                    axis2i_sub( &m_switch_cal.l_min, &min);
                    axis2i_add( &m_switch_cal.l_max, &max);
                }

                bit12_to_uint(switch_stick_calp->rxy_mid,&mid.x, &mid.y);
                bit12_to_uint(switch_stick_calp->rxy_min_sid,&min.x, &min.y);
                bit12_to_uint(switch_stick_calp->rxy_max_sid,&max.x, &max.y);
                if( (max.x <= 3000) && (min.x <= 3000) ) {          //简单判断数据有效性
                    m_switch_cal.r_mid = mid;
                    m_switch_cal.r_min = mid;
                    m_switch_cal.r_max = mid;
                    axis2i_sub( &m_switch_cal.r_min, &min);
                    axis2i_add( &m_switch_cal.r_max, &max);
                }

                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SPI_FLASH_READ,0X6020);   //0x10
            }else if(enump->param == 0X6020) {
                switch_imu_cal_t* switch_imu_calp = (switch_imu_cal_t*)flash_reportp->spi_data;
                logd("flash read add=%lx len=%x\n",flash_reportp->addr,(uint16_t)(flash_reportp->length));
                m_switch_cal.acc = switch_imu_calp->acc_cal;
                m_switch_cal.gyro = switch_imu_calp->gyro_cal;
                axis3i_swapl(&m_switch_cal.acc);
                axis3i_swapl(&m_switch_cal.gyro);
                
                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SPI_FLASH_READ,0X8010);   //0x10
            }else if(enump->param == 0X8010) {
                switch_user_cal_t* switch_user_calp = (switch_user_cal_t*)flash_reportp->spi_data;
                logd("flash read add=%lx len=%x, lMagic=%X, rMagic=%X,imu_Magic=%X\n",flash_reportp->addr,(uint16_t)(flash_reportp->length),switch_user_calp->lMagic,switch_user_calp->rMagic,switch_user_calp->imu_Magic);

                if(switch_user_calp->lMagic == SWITCH_SPI_MAGIC){
                    bit12_to_uint(switch_user_calp->lxy_max_sid,&max.x, &max.y);
                    bit12_to_uint(switch_user_calp->lxy_mid,&mid.x, &mid.y);
                    bit12_to_uint(switch_user_calp->lxy_min_sid,&min.x, &min.y);
                    m_switch_cal.l_mid = mid;
                    m_switch_cal.l_min = mid;
                    m_switch_cal.l_max = mid;
                    axis2i_sub( &m_switch_cal.l_min, &min);
                    axis2i_add( &m_switch_cal.l_max, &max);
                }
                if(switch_user_calp->rMagic == SWITCH_SPI_MAGIC){
                    bit12_to_uint(switch_user_calp->rxy_mid,&mid.x, &mid.y);
                    bit12_to_uint(switch_user_calp->rxy_min_sid,&min.x, &min.y);
                    bit12_to_uint(switch_user_calp->rxy_max_sid,&max.x, &max.y);
                    m_switch_cal.r_mid = mid;
                    m_switch_cal.r_min = mid;
                    m_switch_cal.r_max = mid;
                    axis2i_sub( &m_switch_cal.r_min, &min);
                    axis2i_add( &m_switch_cal.r_max, &max);
                }
                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SPI_FLASH_READ,0X8026);   //0x10
            }else if(enump->param == 0X8026) {
                switch_user_imu_cal_t* switch_user_imu_calp = (switch_user_imu_cal_t*)flash_reportp->spi_data;
                logd("flash read add=%lx len=%x,imu_Magic=%x\n",flash_reportp->addr,(uint16_t)(flash_reportp->length),switch_user_imu_calp->imu_Magic);

                if(switch_user_imu_calp->imu_Magic == SWITCH_SPI_MAGIC){
                    m_switch_cal.acc = switch_user_imu_calp->imu_cal.acc_cal;
                    m_switch_cal.gyro = switch_user_imu_calp->imu_cal.gyro_cal;
                    axis3i_swapl(&m_switch_cal.acc);
                    axis3i_swapl(&m_switch_cal.gyro);
                }
                if(!enump->ready){
                    switch_set_emu(enump,SWITCH_CTRL_ID,SUB_ENABLE_IMU,0x01);   //0x40
                }
            }
            switch_dump_cal();
            break;
        }
        case SUB_ENABLE_IMU:
            switch_host_ctb.imu_enable = true;
            if(!enump->ready){
                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_ENABLE_VIBRATION,0x01);   //0x48
            }
            break;
        case SUB_ENABLE_VIBRATION:
            switch_host_ctb.vibration_enable = true;
            if(!enump->ready){
                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SET_NFC_IR_MCU_CONFIGURATION,0x21);
            }
            break;
        case SUB_SET_NFC_IR_MCU_CONFIGURATION:
            if(!enump->ready){
        	    switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SET_PLAYER_LIGHTS,0x01);   //0X30  //手柄编号
            }else{
                #if APP_NFC_ENABLE
                switch_set_emu(enump, SWITCH_MCU_CTRL_ID,MCU_SUB_REQ_STATUS,0);
                #endif
            }
            break;
        case SUB_SET_PLAYER_LIGHTS:             //0x30
            if(!enump->ready){
                enump->cmd = 0;
                enump->ready = 1;
                switch_host_ctb.lights = 0x01;
                logi("switch lights=%x,switch enum ready...\n",switch_host_ctb.lights);   
            }  
            break;
        default:			
            break;
        }
        break;
     }
    case SWITCH_NORMAL_REPORT_ID:       //0X3F
    case SWITCH_LARGE_REPORT_ID:        //0X31
    case SWITCH_STANDARD_REPORT_ID:     //0X30
		enump->retry++;
        if((enump->cmd == SWITCH_USB_ID) && ( enump->param == SWITCH_USB_DIS_TIMEOUT)){	//usb自动触发枚举
            switch_set_emu(enump,SWITCH_CTRL_ID,SUB_REQUEST_DEVICE_INFO,0x00);
        }else if(api_trp_is_bt(phandle->trp)){   										
            if(!enump->ready){															//蓝牙触发枚举
                if(enump->retry > 20){
                    switch_set_emu(enump,SWITCH_CTRL_ID,SUB_REQUEST_DEVICE_INFO,0x00);
                }
            }else if(SWITCH_NORMAL_REPORT_ID == buf[0]){                                //枚举成功后切换30 
                switch_set_emu(enump,SWITCH_CTRL_ID,SUB_SET_INPUT_REPORT_MODE,0x30);
                switch_host_ctb.report_mode = SWITCH_STANDARD_REPORT_ID;
            }else if(SWITCH_LARGE_REPORT_ID == buf[0]){									//处理nfc 请求
				#if APP_NFC_ENABLE
				switch_nfc_emun_process(&switch_host_nfc,enump, buf,len);
				#endif
            }
		}  
        break;
    default:
        ret = false;
        logd("in id=%x:",buf[0]);
        break;
    }

    switch_emu_ctrl_send(phandle,enump);
	return ret;
}



#if 0
void switch_cal_data_test(void)
{
    //SWITCH_FACTORY_IMU_CAL_ADD				0X6020  
    //switch_imu_cal_t
    uint8c_t switch_imu_cal[] = {
        0xF7,0xFF,0xDA,0xFF,0x7B,0x02,
        0x00,0x40,0x00,0x40,0x00,0x40,
        
        0xEA,0xFF,0x02,0x00,0x01,0x00,
        0xE7,0x3B,0xE7,0x3B,0xE7,0x3B,
        
        };

    //SWITCH_FACTORY_STICK_CAL_ADD 0X603D
    //switch_stick_cal_t
    uint8c_t switch_stick_cal[] = {
        0xB3, 0xC5, 0x68,0x89, 0xE7, 0x78, 0xBC, 0x35, 0x59, 0xaa, 0x57, 0x7D, 0x17, 0x26, 0x64, 0xF3,
        0xE5, 0x61};
        
    //SWITCH_USER_CAL_ADD						0X8010
    //switch_user_cal_t
    uint8c_t switch_user_cal[] =
    {
        //数据来源：根据C1实际校准数据获得
        0xb2, 0xa1, 0xff, 0xf7, 0x7f, 0x00, 0x08, 0x80, 0x00, 0x08, 0x80, 
        0xb2, 0xa1, 0x00, 0x08, 0x80, 0x00, 0x08, 0x80, 0xff, 0xf7, 0x7f, 
        0xb2, 0xa1, 0xd7, 0x01, 0xf7, 0xff, 0xb3, 0x01, 
                    0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 
                    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
                    0xe7, 0x3b, 0xe7, 0x3b, 0xe7, 0x3b,
    };

    axis2i_t min, mid,max;
    switch_stick_cal_t* switch_stick_calp;
    switch_imu_cal_t* switch_imu_calp;
    switch_user_cal_t* switch_user_calp;

    memset(&m_switch_cal,0,sizeof(m_switch_cal));

    switch_stick_calp = (switch_stick_cal_t*)switch_stick_cal;
    bit12_to_uint(switch_stick_calp->lxy_max_sid,&max.x, &max.y);
    bit12_to_uint(switch_stick_calp->lxy_mid,&mid.x, &mid.y);
    bit12_to_uint(switch_stick_calp->lxy_min_sid,&min.x, &min.y);
    m_switch_cal.l_mid = mid;
    m_switch_cal.l_min = mid;
    m_switch_cal.l_max = mid;
    axis2i_sub( &m_switch_cal.l_min, &min);
    axis2i_add( &m_switch_cal.l_max, &max);

    bit12_to_uint(switch_stick_calp->rxy_mid,&mid.x, &mid.y);
    bit12_to_uint(switch_stick_calp->rxy_min_sid,&min.x, &min.y);
    bit12_to_uint(switch_stick_calp->rxy_max_sid,&max.x, &max.y);
    m_switch_cal.r_mid = mid;
    m_switch_cal.r_min = mid;
    m_switch_cal.r_max = mid;
    axis2i_sub( &m_switch_cal.r_min, &min);
    axis2i_add( &m_switch_cal.r_max, &max);

    switch_imu_calp = (switch_imu_cal_t*)switch_imu_cal;
    m_switch_cal.acc = switch_imu_calp->acc_cal;
    m_switch_cal.gyro = switch_imu_calp->gyro_cal;
    axis3i_swapl(&m_switch_cal.acc);
    axis3i_swapl(&m_switch_cal.gyro);
    switch_dump_cal();
                
    
    switch_user_calp = (switch_user_cal_t*)switch_user_cal;
    logd("lMagic=%X, rMagic=%X,imu_Magic=%x\n",switch_user_calp->lMagic,switch_user_calp->rMagic,switch_user_calp->imu_Magic);

    if(switch_user_calp->lMagic == SWITCH_SPI_MAGIC){
        bit12_to_uint(switch_user_calp->lxy_max_sid,&max.x, &max.y);
        bit12_to_uint(switch_user_calp->lxy_mid,&mid.x, &mid.y);
        bit12_to_uint(switch_user_calp->lxy_min_sid,&min.x, &min.y);
        m_switch_cal.l_mid = mid;
        m_switch_cal.l_min = mid;
        m_switch_cal.l_max = mid;
        axis2i_sub( &m_switch_cal.l_min, &min);
        axis2i_add( &m_switch_cal.l_max, &max);
    }
    if(switch_user_calp->rMagic == SWITCH_SPI_MAGIC){
        bit12_to_uint(switch_user_calp->rxy_mid,&mid.x, &mid.y);
        bit12_to_uint(switch_user_calp->rxy_min_sid,&min.x, &min.y);
        bit12_to_uint(switch_user_calp->rxy_max_sid,&max.x, &max.y);
        m_switch_cal.r_mid = mid;
        m_switch_cal.r_min = mid;
        m_switch_cal.r_max = mid;
        axis2i_sub( &m_switch_cal.r_min, &min);
        axis2i_add( &m_switch_cal.r_max, &max);
    }
            

    if(switch_user_calp->imu_Magic == SWITCH_SPI_MAGIC){
        m_switch_cal.acc = switch_user_calp->imu_cal.acc_cal;
        m_switch_cal.gyro = switch_user_calp->imu_cal.gyro_cal;
        axis3i_swapl(&m_switch_cal.acc);
        axis3i_swapl(&m_switch_cal.gyro);
    }
    switch_dump_cal();
}
#endif

bool switch_host_init(trp_handle_t *phandle)
{   
    if(api_trp_is_usb){
        memset(&usb_switch_enump,0,sizeof(usb_switch_enump));
    }else{
        memset(&bt_switch_enump,0,sizeof(bt_switch_enump));
    }

    switch_host_ctb.vibration_enable = false;
    switch_host_ctb.imu_enable = false;
    switch_host_ctb.report_mode=SWITCH_NORMAL_REPORT_ID;           //默认switch repot id
    switch_host_ctb.lights = 0;
   
    memset(&m_switch_cal,0,sizeof(m_switch_cal));
    m_switch_cal.l_mid.x = 2048;
    m_switch_cal.l_mid.y = 2048;
    m_switch_cal.l_max.x = 4095;
    m_switch_cal.l_max.y = 4095;
    m_switch_cal.r_mid =  m_switch_cal.l_mid;
    m_switch_cal.r_max =  m_switch_cal.l_max;
    switch_dump_cal();

    return true;
}

bool switch_host_deinit(trp_handle_t *phandle)
{
    if(api_trp_is_usb){
        memset(&usb_switch_enump,0,sizeof(usb_switch_enump));
    }else{
        memset(&bt_switch_enump,0,sizeof(bt_switch_enump));
    }
    return true;
}

void switch_host_handler(trp_handle_t *phandle)
{

}

#endif






