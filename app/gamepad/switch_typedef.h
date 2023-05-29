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

#ifndef	_switch_typedef_h
#define	_switch_typedef_h

#include "utils/emf_typedef.h" 
#include "utils/emf_utils.h"

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define   SWITCH_VID	    0X057E
#define   SWITCH_PID	    0X2009
#define   SWITCH2_PID	    0X2006
#define   SWITCH3_PID	    0X2007
#define   SWITCH4_PID	    0X200e

#ifndef SWITCH_SPK_RATE							//默认自定义音频码率
#define SWITCH_SPK_RATE           48000
#endif
#ifndef SWITCH_MIC_RATE
#define SWITCH_MIC_RATE           48000
#endif



#define  SWITCH_Y_POS					(0 )		/*SWITCH_NORMAL_REPORT_ID B */
#define  SWITCH_X_POS					(1 )		/*SWITCH_NORMAL_REPORT_ID A */
#define  SWITCH_B_POS					(2 )		/*SWITCH_NORMAL_REPORT_ID Y */
#define  SWITCH_A_POS					(3 )		/*SWITCH_NORMAL_REPORT_ID X */
//#define  SWITCH_Z_POS 				(4 )		/*SWITCH_NORMAL_REPORT_ID L1 */
//#define  SWITCH_Z_POS 				(5 )		/*SWITCH_NORMAL_REPORT_ID R1 */
#define  SWITCH_R1_POS					(6 )		/*SWITCH_NORMAL_REPORT_ID L2 */
#define  SWITCH_R2_POS					(7 )		/*SWITCH_NORMAL_REPORT_ID R2 */
#define  SWITCH_MINUS_POS				(8 )
#define  SWITCH_POSITIVE_POS			(9 )
#define  SWITCH_R3_POS					(10)
#define  SWITCH_L3_POS					(11)
#define  SWITCH_HOME_POS				(12)
#define  SWITCH_CAPTURE_POS				(13)
//#define  SWITCH_Z 					(14)
//#define  SWITCH_Z 					(15)
#define  SWITCH_DN_POS					(16)
#define  SWITCH_UP_POS					(17)
#define  SWITCH_RIGHT_POS				(18)
#define  SWITCH_LEFT_POS				(19)
//#define  SWITCH_Z_POS 				(20)
//#define  SWITCH_Z_POS 				(21)
#define  SWITCH_L1_POS					(22)
#define  SWITCH_L2_POS					(23)

#define  SWITCH_L_UP_POS				(24)
#define  SWITCH_L_DOWN_POS				(25)
#define  SWITCH_L_LEFT_POS				(26)
#define  SWITCH_L_RIGHT_POS				(27)
#define  SWITCH_R_UP_POS				(28)
#define  SWITCH_R_DOWN_POS				(29)
#define  SWITCH_R_LEFT_POS				(30)
#define  SWITCH_R_RIGHT_POS				(31)


#define  SWITCH_Y					(0x01UL<<SWITCH_Y_POS		)
#define  SWITCH_X					(0x01UL<<SWITCH_X_POS		)
#define  SWITCH_B					(0x01UL<<SWITCH_B_POS		)
#define  SWITCH_A					(0x01UL<<SWITCH_A_POS		)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z_POS 	)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z_POS 	)
#define  SWITCH_R1					(0x01UL<<SWITCH_R1_POS		)
#define  SWITCH_R2					(0x01UL<<SWITCH_R2_POS		)
#define  SWITCH_MINUS				(0x01UL<<SWITCH_MINUS_POS	)
#define  SWITCH_POSITIVE			(0x01UL<<SWITCH_POSITIVE_POS)
#define  SWITCH_R3					(0x01UL<<SWITCH_R3_POS		)
#define  SWITCH_L3					(0x01UL<<SWITCH_L3_POS		)
#define  SWITCH_HOME				(0x01UL<<SWITCH_HOME_POS	)
#define  SWITCH_CAPTURE				(0x01UL<<SWITCH_CAPTURE_POS	)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z 		)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z 		)
#define  SWITCH_DN					(0x01UL<<SWITCH_DN_POS		)
#define  SWITCH_UP					(0x01UL<<SWITCH_UP_POS		)
#define  SWITCH_RIGHT				(0x01UL<<SWITCH_RIGHT_POS	)
#define  SWITCH_LEFT				(0x01UL<<SWITCH_LEFT_POS	)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z_POS 	)
//#define  SWITCH_Z 				(0x01UL<<  SWITCH_Z_POS 	)
#define  SWITCH_L1					(0x01UL<<SWITCH_L1_POS		)
#define  SWITCH_L2					(0x01UL<<SWITCH_L2_POS		)
#define  SWITCH_L_UP				(0x01UL<<SWITCH_L_UP_POS	)
#define  SWITCH_L_DOWN				(0x01UL<<SWITCH_L_DOWN_POS	)
#define  SWITCH_L_LEFT				(0x01UL<<SWITCH_L_LEFT_POS	)
#define  SWITCH_L_RIGHT				(0x01UL<<SWITCH_L_RIGHT_POS)
#define  SWITCH_R_UP				(0x01UL<<SWITCH_R_UP_POS	)
#define  SWITCH_R_DOWN				(0x01UL<<SWITCH_R_DOWN_POS	)
#define  SWITCH_R_LEFT				(0x01UL<<SWITCH_R_LEFT_POS	)
#define  SWITCH_R_RIGHT				(0x01UL<<SWITCH_R_RIGHT_POS)


// SPI FLASH READ / WRIET Regions
#define SWITCH_SERIAL_NUMBER_ADD				0X6000						/* 16 */
#define SWITCH_FACTORY_IMU_CAL_ADD				0X6020						/* 24? */
#define SWITCH_FACTORY_STICK_CAL_ADD			0X603D						/* 18 */
#define SWITCH_RGB_ADD							0X6050						/* 12 */
#define SWITCH_FACTORY_IMU_STICK_PARA_ADD		0x6080						/* 6 */
#define SWITCH_USER_JOYSTICK_CAL_ADD			0X8010						/* 48 */
#define SWITCH_USER_IMU_CAL_ADD					0X8026						/* 48 */

#define SWITCH_SPI_MAGIC						SWAP16_L(0XA1B2)
/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/
#ifndef PRAGMA_PACK_IGNORED
#pragma pack(1)
#endif


//参考 https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering
/******************************************************************************************************
**									USB /UART CMD
*******************************************************************************************************/
#define SWITCH_USB_ID			 0x80		/*USB 设备握手指令 */
#define SWITCH_USB_REPLIES_ID	 0x81		/*USB 设备握手回复指令 */
// Sub-types of the 0x80 output report, used for initialization.
typedef enum{								/*设备HID 指令 */
	SWITCH_USB_REQUEST_MAC = 0x01,
	SWITCH_USB_HANDSHAKE = 0x02,
	SWITCH_USB_BAUDRATE = 0x03,
	SWITCH_USB_DIS_TIMEOUT = 0x04,
	SWITCH_USB_ENABLE_TIMEOUT = 0x05,
}switch_usb_cmd_t;


typedef struct{				/*USB 指令 */
	uint8_t id;				/*SWITCH_USB_REPLIES_ID */
	uint8_t sub_cmd;		/*SWITCH_USB_REQUEST_MAC */
	uint8_t type;
	uint8_t mac[6];
}switch_usb_mac_replies_t;


/******************************************************************************************************
** 								HID CMD
*******************************************************************************************************/
typedef enum{								//设备HID 指令
	SWITCH_CTRL_ID				=0x01,		//OUT
	SWITCH_MCU_UPDATA_ID		=0X03,
	SWITCH_MOTOR_ID				=0X10,		//马达控制 report id
	SWITCH_MCU_CTRL_ID			=0X11,		//Request specific data from the NFC/IR MCU. Can also send rumble.
	SWITCH_REQ_UNKNOWN_ID		=0X12,
	
	SWITCH_NORMAL_REPORT_ID		=0X3F,		//IN
	SWITCH_CTRL_REPLIES_ID		=0X21,
	SWITCH_UPDATA_REPORT_ID		=0X23,
	SWITCH_STANDARD_REPORT_ID	=0X30,		//标准摇杆按键数据report id
	SWITCH_LARGE_REPORT_ID		=0X31,
	SWITCH_UNKNOWN_REPORT_ID	=0X32,
	SWITCH_UNKNOWN2_REPORT_ID	=0X33,
}switch_hid_id_t;
typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint16_t hf:9; //high frequency
	uint16_t hf_amp:7; //high frequency altitude
	uint16_t lf:7; //low frequency
	uint16_t lf_amp:9;
}switch_rumble_bit_t;	//4
typedef struct
{
	uint8_t id;			//0x01/0x11	SWITCH_CTRL_ID/SWITCH_MCU_CTRL_ID
	uint8_t index:4;	//index
	uint8_t res:4;		//0x00;
	switch_rumble_bit_t	motor1;
	switch_rumble_bit_t	motor2;
	uint8_t sub_cmd;			//11byte
	uint8_t cmd_data[36];		//
	uint8_t crc8;				//only SWITCH_MCU_CTRL_ID:cmd_data crc8
	uint8_t sub_crc8;			//only SUB_SET_NFC_IR_MCU_CONFIGURATION cmd_data[1] ~ crc8 
}switch_ctrl_t;					//49

typedef struct
{
	uint8_t id;				//固定0x21	SWITCH_CTRL_REPLIES_ID
	uint8_t index;			//Timer. Increments,Joy-Con uses 4.96ms as the timer tick rate
	uint8_t battery;		//bit7~5: 电量, bit4: 充电状态, bit0~3 Connection info
	uint16_t buttonl;		//小端
	uint8_t buttonh;		//小端
	uint8_t l_xy[3];		//12bit: 0~4096
	uint8_t r_xy[3];
	uint8_t vibrator_report; //不完全包含[0xA0, 0xB0, 0xC0, 0x90],也可以是0X0C/0X80 固定是否需要发送马达数据
	
	uint8_t sub_ack; 		//bit7:0:NACK,1:ACK, bitL6~0: sum_cmd & 0x7f??,注意有samle ack 也发80,还有部分指令很特殊
	uint8_t sub_cmd;		//15
	uint8_t cmd_data[33];	//
	uint8_t sub_crc8;		//byte49, only SUB_SET_NFC_IR_MCU_CONFIGURATION:0x21

	uint8_t reserve[15]; //保留数据
}switch_ctrl_replies_t;		//64 bt:49, usb:64


//*********************************************************************************//
//                              SWITCH_MOTOR_ID  0x10                              //
//*********************************************************************************//
typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint16_t hf;
	uint8_t  hf_amp;
	uint16_t lf;
	uint8_t  lf_amp;
}switch_motor_decode_t;					//switch马达是低频和高频叠加效果
typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint8_t id;		//固定0x10	SWITCH_MOTOR_ID
	uint8_t index;		//+1 0~0f
	switch_rumble_bit_t	rumble_l;
	switch_rumble_bit_t	rumble_r;
}switch_rumble_t;

//*********************************************************************************//
//                               SWITCH_CTRL_ID   -x01                            	//
//*********************************************************************************//
//参考:https://github.com/CTCaer/Nintendo_Switch_Reverse_Engineering/blob/ir-nfc/mcu_ir_nfc_notes.md
typedef enum{							//SWITCH_CTRL_ID
	SUB_BLUETOOTH_MANUAL_PAIRING        = (0X01),		//1: x01 [{BD_ADDR_LE}] (Send host MAC and acquire Joy-Con MAC)
	                                    	        	//2: x02 (Acquire the XORed LTK hash)
	                                      	         	//3: x03 (saves pairing info in Joy-Con)
	SUB_REQUEST_DEVICE_INFO             = (0X02),      //switch_device_info_t
	SUB_SET_INPUT_REPORT_MODE           = (0X03),      //一般选Standard full mode. Pushes current state @60Hz
	SUB_TRIGGER_BUTTONS_ELAPSED_TIME    = (0X04),
	SUB_GET_PAGE_LIST_STATE             = (0X05),
	SUB_SET_HCI_STATE                   = (0X06),      //(disconnect/page/pair/turn off)
	SUB_RESET_PAIRING_INFO              = (0X07),      //Initializes the 0x2000 SPI section.
	SUB_SET_SHIPMENT_LOW_POWER_STATE    = (0X08),
	SUB_SPI_FLASH_READ                  = (0x10),
	SUB_SPI_FLASH_WRITE                 = (0x11),
	SUB_SPI_SECTOR_ERASE                = (0x12),
	SUB_RESET_NFC_IR_MCU                = (0x20),
	SUB_SET_NFC_IR_MCU_CONFIGURATION    = (0x21),
	SUB_SET_NFC_IR_MCU_STATE            = (0x22),  		//00:Suspend,01:Resume,02:Resume for update
	SUB_SET_UNKNOWN_DATA                = (0X24),  		// (fw 3.86 and up)
	SUB_RESET_X24_UNKNOWN_DATA          = (0X25),  		// (fw 3.86 and up)
	SUB_SET_UNKNOWN_NFC_IR_MCU_DATA     = (0X28),		//Does the same job with OUTPUT report 0x12.
	SUB_GET_X28_NFC_IR_MCU_DATA         = (0X29),
	SUB_SET_GPIO2_PIN_OUTPUT_VALUE      = (0X2A),		// @Port 2
	SUB_GET_XS29_NFC_IR_MCU_DATA        = (0x2B),
	SUB_SET_PLAYER_LIGHTS               = (0X30),		//手柄编号,指示灯,参数bit0~3:对应灯常亮,bit4~7:对应灯闪烁
	SUB_GET_PLAYER_LIGHTS               = (0X31),
	SUB_UNKNOWN_0X33                    = (0X33), 		//在连接WINDOWS的时候使用到的
	SUB_SET_HOME_LIGHTS                 = (0X38),
	SUB_ENABLE_IMU                      = (0x40),  		//0:disable,1:enable
	SUB_SET_IMU_SENSITIVITY             = (0x41),
	SUB_WRITE_IMU_REGISTERS             = (0x42),
	SUB_READ_IMU_REGISTERS              = (0x43),
	SUB_ENABLE_VIBRATION                = (0X48),  		//0:disable,1:enable
	SUB_GET_REGULATED_VOLTAGE           = (0X50), 
	SUB_SET_GPIO1_PIN_OUTPUT_VALUE      = (0X51),  		// @Port 1
	SUB_GET_GPIO_PIN_OUTPUT_VALUE       = (0X52),
}switch_ctrl_sub_cmd_t;

/*--------------replales by switch_ctrl_replies_t--------------------*/
typedef struct {
  uint8_t version_l;
  uint8_t version_h;
  uint8_t device_type;	//1=Left Joy-Con, 2=Right Joy-Con, 3=Pro Controller.
  uint8_t unknown1; //0x02
  uint8_t mac_data[6];
  uint8_t unknown2; //0x03
  uint8_t unknown3; //0x02
}switch_device_info_t;

typedef struct  {					//	0X6020 小端
	axis3i_t	acc_cal;
	axis3i_t	acc_sensitivity;			//默认0x4000:±8G.
	axis3i_t	gyro_cal;
	axis3i_t	gyro_sensitivity;			//默认0x343B:±2000dps.
}switch_imu_cal_t;			//24

typedef struct  {					//0X603D 小端, 12bit
	uint8_t lxy_max_sid[3];			//xy_max_sid = xy_max - xy_mid
	uint8_t lxy_mid[3];				
	uint8_t lxy_min_sid[3];			//xy_min_sid = xy_mid - xy_min

	uint8_t rxy_mid[3];				
	uint8_t rxy_min_sid[3];			//xy_min_sid = xy_mid - xy_min
	uint8_t rxy_max_sid[3];			//xy_max_sid = xy_max - xy_mid
}switch_stick_cal_t;				//18

typedef struct  {					//0X6050
	uint8_t		body[3];			//rgb
	uint8_t		button[3];
	uint8_t		grip_l[3];
	uint8_t		grip_r[3];
}switch_flash_rgb_t;			//12


typedef struct  {					//摇杆参数
	uint8_t 	unused[3];			//以下都是12bit	
	uint8_t 	dead_zone_range[3];	//Dead-zone and Range ratio
	uint8_t 	xy_unknown[12];
}switch_stick_para_t;			//18

typedef struct  {					//0x6080
	axis3i_t	imu_offset;			//imu horizontal offset 默认xFD50 x000 xFC6 
	switch_stick_para_t	pa1;
	switch_stick_para_t	pa2;		//和pa1一样就可以
}switch_imu_stick_para_t;			//42

typedef struct  {					//0X8010
	uint16_t    lMagic;				//xA1B2 for user available calibration
	uint8_t 	lxy_max_sid[3];		//xy_max_sid = xy_max - xy_mid
	uint8_t 	lxy_mid[3];				
	uint8_t 	lxy_min_sid[3];		//xy_min_sid = xy_mid - xy_min

	uint16_t    rMagic;				//xA1B2 for user available calibration
	uint8_t 	rxy_mid[3];				
	uint8_t 	rxy_min_sid[3];		//xy_min_sid = xy_mid - xy_min
	uint8_t		rxy_max_sid[3];		//xy_max_sid = xy_max - xy_mid

	uint16_t    imu_Magic;			//0X8026 xA1B2 for user available calibration
	switch_imu_cal_t imu_cal;		//注意这个是大端数据	
}switch_user_cal_t;			//48


typedef struct  {					//0X8010
	uint16_t    imu_Magic;			//xB2A1 for user available calibration
	switch_imu_cal_t imu_cal;		
}switch_user_imu_cal_t;			//48

typedef struct  {
  uint32_t addr;			//小端
  uint8_t length;
  uint8_t spi_data[30];
}switch_flash_data_t;

//*********************************************************************************//
//                               SWITCH_MCU_CTRL_ID 0x11                           //
//	*resplies by SWITCH_LARGE_REPORT_ID											   //
//*********************************************************************************//
typedef enum{							//SWITCH_MCU_CTRL_ID, 
	MCU_SUB_NONE			= (0X00),
	MCU_SUB_REQ_STATUS    	= (0X01),	//request mcu status send switch_mcu_state_t
	MCU_SUB_REQ_NFC_DATA  	= (0X02),  	//NFC sub_cmd,Request NFC data report
	MCU_SUB_REQ_IR_DATA    	= (0X03),	//IR sub_cmd
}switch_mcu_ctrl_sub_cmd_t;


/*-------------------------request----------------------------*/
typedef enum{						//sub cmd:MCU_SUB_REQ_NFC_DATA  0X02
	SWITCH_NFC_CANCEL_CMD 			=0x00,			
	SWITCH_NFC_START_POLLING_CMD	=0X01,
	SWITCH_NFC_STOP_POLLING_CMD		=0X02,
	SWITCH_NFC_UNKNOWN_CMD			=0X03,
	SWITCH_NFC_START_DISCOVERY_CMD	=0X04,	//(following data:1,0,0,2c,0)
	SWITCH_NFC_START_DISCOVERY2_CMD	=0X05,
	SWITCH_NFC_NTAG_READ_CMD		=0X06,
	SWITCH_NFC_NTAG_WRITE_CMD		=0X08,	//(has args. UID/Blocks to write/blockdata/etc)
	SWITCH_NFC_SEND_RAW_CMD			=0X09,
	SWITCH_NFC_MIFARE_R_W_CMD		=0X0F,
	SWITCH_NFC_MIFARE_KEY_CMD		=0X11,	//Register/Clear Mifare Key
}nfc_req_cmd_t;
typedef struct
{
	uint8_t nfc_cmd;			//nfc_req_cmd_t
	uint8_t packet_nb;
	uint8_t ack;				//猜测: nfc_state:0x04时: 0x01接收成功, 0x02接收完成退出接收??
	uint8_t fragments;			//x00: more cmd fragments will follow, x08: This is the last cmd fragment.
	uint8_t nfc_len;			//Length of following data
	uint8_t nfc_data[31];		//Max 31 bytes.
}switch_nfc_req_t;			//38

/*------------------------replay----------------------------*/
typedef enum{								//resplies by SWITCH_LARGE_REPORT_ID nfcdata
	SWITCH_MCU_NONE_ID			=0x00,		//SUM CMD 22 设置01后 MCU 进入IDLE状态
	SWITCH_MCU_STATE_ID			=0X01,
	SWITCH_MCU_REPORT_ID		=0X03,
	SWITCH_MCU_BUSY_ID			=0x0b,
	SWITCH_MCU_MODE_STATE_ID	=0x13,
	SWITCH_MCU_SENSOR_ID		=0x1b,
	SWITCH_MCU_NFC_STATE_ID		=0x2a,
	SWITCH_MCU_TAG_READ_ID		=0x3a,
	SWITCH_MCU_IDLE_ID			=0XFF,		//IN
}switch_mcu_report_id_t;

typedef enum{				//MCU_SUB_REQ_STATUS->SWITCH_MCU_STATE_ID in switch_mcu_state_t
    SWITCH_MCU_STATE_STAND_BY 	= 1,
	SWITCH_MCU_STATE_BACKGROUND = 2,
    SWITCH_MCU_STATE_NFC 		= 4,
    SWITCH_MCU_STATE_IRC 		= 5,
    SWITCH_MCU_STATE_BUSY 		= 6,		//Initializing/Busy/FW Update?
} mcu_state_t;

typedef enum{				//SWITCH_MCU_NFC_STATE_ID:0x2a in switch_nfc_state_t
	SWITCH_NFC_IDLE = 0x00,				//0 等待命令
	SWITCH_NFC_POLLING_TAG,				//1 标签轮询		
	SWITCH_NFC_READ_TAG,				//2 标签读
	SWITCH_NFC_WRITE_TAG,				//3 标签写
	SWITCH_NFC_IREAD_FINLISH,			//4 标签读取完成
	SWITCH_NFC_WRITE_FINLISH=5,			//5 标签写入完成
	SWITCH_NFC_BYPASS_SEND,				//6 直通发送
	SWITCH_NFC_ERROR,					//7 错误（这会激活nfc_result)
	SWITCH_NFC_DETECTED,				//8 NFC 已停用
	SWITCH_NFC_TAG_DETECTED,			//9 检测到标签
	SWITCH_NFC_FACTORY_MODE=0X0A,		//A 出厂模式
	SWITCH_NFC_INIT,					//B 初始化
	SWITCH_NFC_BYPASS_FINLISH,			//C 通关完毕
	SWITCH_NFC_REQUIRED_RESET,			//D 需要重置
	SWITCH_NFC_HW_RESET,				//E 硬件RESET
	SWITCH_NFC_MI_SENDING=0X0F,			//F Mifare 发送
	SWITCH_NFC_MI_FINLISH,				//10 Mifare 完成
	SWITCH_NFC_MI_KEY_WRITE,			//11 Mifare 键写入
	SWITCH_NFC_KEY_WRITE_FINLISH=18,	//12 Mifare 键写入完成
}nfc_state_t;

typedef enum{				//SWITCH_MCU_NFC_STATE_ID in switch_nfc_state_t
	SWITCH_NFC_NONE_ERR = 0x00,			
	SWITCH_NFC_FUNTION_ERR = 0X3C,				
	SWITCH_NFC_REQUIRED_RESET_ERR,
	SWITCH_NFC_READ_ERR,
	SWITCH_NFC_WRITE_ERR,		
	SWITCH_NFC_ARGUMENT_ERR = 0X40,		
	SWITCH_NFC_TIMEOUT_ERR,		
	SWITCH_NFC_INVALID_UID_ERR,	
	SWITCH_NFC_UNKNOWN_ERR,		
	SWITCH_NFC_PASSWORD_ERR,	
	SWITCH_NFC_VERIFY_ERR = 0X45,			
	SWITCH_NFC_ACTIVATION_ERR,		
	SWITCH_NFC_INVALID_TAG_ERR,	
	SWITCH_NFC_INVALID_FORMAT_ERR,	
	SWITCH_NFC_AUTH_ERR,	
	SWITCH_NFC_SEQUENCE_ERR = 0X4A,
	SWITCH_NFC_CMD_TIMEOUT_ERR,
	SWITCH_NFC_MI_ERR = 0X4C,		
}nfc_result_t;


typedef struct  {
	uint8_t id;				//SWITCH_MCU_STATE_ID 0x01
	uint8_t res;			//0
	uint8_t busy;			//mcu_state busy or mcu busy? res[1]=0xff
	uint16_t major_version;	//大端	0x03/0x0004
	uint16_t minor_version;	//大端	0x05/0x0012
	uint8_t mcu_state;		// mcu_state_t
}switch_mcu_state_t;		//8

typedef struct  {
	uint8_t id;				//SWITCH_MCU_MODE_STATE_ID 0x13
	uint8_t res;			//0
	uint8_t ir_mode;
	uint16_t major_version;	//大端	0x03/0x0004
	uint16_t minor_version;	//大端	0x05/0x0012
}switch_mcu_mode_t;	//7

//0x3a 发送顺序, natg 卡一共发送540个字节
// "3a 0007 0100 0131 02000000 01   02 0007"	len = 245
// "3a 0007 0200 0927"							len = 295
// "2a 0005 0000 0931 04000000 0101 02 0007"	SWITCH_NFC_IREAD_FINLISH
typedef struct  {
	uint8_t id;				//SWITCH_MCU_NFC_STATE_ID 0x2a/0x3a
	uint8_t nfc_result;		//nfc_result_t This shows up when nfc_state is x07.
	uint8_t input_type;		//5: State info, 7: Ntag read data, xA: Pass-through data, x10: Mifare data
	uint8_t fragment_id;	//片段id
	uint8_t res;	
	// uint16_t last_fragment:5;	//1:last fragment,注意这里是大端
	// uint16_t payload_size:12;	//size:max 0x131
	uint8_t payload_size_h:3;	//size:max 0x131
	uint8_t last_fragment:5;	//默认: 0x01
	uint8_t payload_size;		//size:max 0x131
	uint8_t payload[0x131];		//NFC tag info(Tag UID)
}switch_nfc_payload_t;			//312

typedef struct  {
	uint8_t nfc_state;			// nfc_state_t
	uint8_t res2[3];
	uint8_t has_tag_info;		//0: No Tag info data, 1: Has Tag info data
	uint8_t has_tag_info2;		//实际未知
	uint8_t nfc_tag_ic;			//2: Ntag, 4: Mifare.
	uint8_t nfc_type;			//0: Type A, 1: Type B, 2: Type F, 6: ISO/IEC 15693 type
	uint8_t tag_info_size;		//Size of the following Tag UID
	uint8_t tag_info_data[40];	//NFC tag info(Tag UID)
}switch_nfc_state_t;			//49

typedef struct  {
	uint8_t nfc_state;			// nfc_state_t
	uint8_t res2[3];
	uint8_t has_tag_info;		//0: No Tag info data, 1: Has Tag info data
	uint8_t nfc_tag_ic;			//2: Ntag, 4: Mifare.
	uint8_t nfc_type;			//0: Type A, 1: Type B, 2: Type F, 6: ISO/IEC 15693 type
	uint8_t tag_uuid_size;		//Size of the following Tag UID
	uint8_t tag_uuid_data[7];	//NFC tag info(Tag UID)
	uint8_t res3[4];			//0x00
	uint8_t unknow[32];			//"7DFDF0793651ABD7466E39C191BABEB856CEEDF1CE44CC75EAFB27094D087AE8",	//45
	uint8_t read_req_pa[9];		//03003B3C7778860000	SWITCH_NFC_NTAG_READ_CMD parameter
}switch_nfc_read_head_t;		//60	,0x3a数据发送需拆包,第一包包含有head

	
		
//*********************************************************************************//
//         						SWITCH_REPORT_ID           						   //
//*********************************************************************************//
typedef struct  {
	uint8_t id;				//固定0x3F	SWITCH_NORMAL_REPORT_ID
	uint16_t button;
	uint8_t hat_switch;
	int16_t lx;
	int16_t ly;
	int16_t rx;
	int16_t ry;
}switch_normal_report_t;

typedef struct
{
	uint8_t id;				//固定0x30	SWITCH_STANDARD_REPORT_ID
	uint8_t index;			//Timer. Increments
	uint8_t battery;		//bit7~5: 电量, bit4: 充电状态, bit0~3 Connection info
	uint16_t buttonl;		//小端
	uint8_t buttonh;		//小端
	uint8_t l_xy[3];		//12bit: 0~4096
	uint8_t r_xy[3];
	uint8_t vibrator_report;//不完全包含[0xA0, 0xB0, 0xC0, 0x90],也可以是0X0C/0X80 固定是否需要发送马达数据 13

	axis3i_t acc; 	//小端,系统:Y向右,Z 向下,x向外范围0~4096
	axis3i_t gyro; 	///参考acc坐标,左手定律旋转,范围-32768~32767
	axis3i_t acc2;
	axis3i_t gyro2;
	axis3i_t acc3;
	axis3i_t gyro3;			//49
}switch_report_t;			//49, USB 发送64个字节

typedef struct
{
	uint8_t id;				//固定0x31	SWITCH_LARGE_REPORT_ID
	uint8_t index;			//Timer. Increments
	uint8_t battery;		//bit7~5: 电量, bit4: 充电状态, bit0~3 Connection info
	uint16_t buttonl;		//小端
	uint8_t buttonh;		//小端
	uint8_t l_xy[3];		//12bit: 0~4096
	uint8_t r_xy[3];
	uint8_t vibrator_report;//不完全包含[0xA0, 0xB0, 0xC0, 0x90],也可以是0X0C/0X80 固定是否需要发送马达数据 13

	axis3i_t acc; 	//小端,系统:Y向右,Z 向下,x向外范围0~4096
	axis3i_t gyro; 	///参考acc坐标,左手定律旋转,范围-32768~32767
	axis3i_t acc2;
	axis3i_t gyro2;
	axis3i_t acc3;
	axis3i_t gyro3;			//49
		
	uint8_t nfc[312]; 		//以下是NFC 数据 max:312+1
	uint8_t nfc_crc;		//crc8
}switch_large_report_t;		//max:362


typedef struct
{
	axis2i_t l_min;
    axis2i_t l_mid;
	axis2i_t l_max;

    axis2i_t r_min;
    axis2i_t r_mid;
	axis2i_t r_max;

	axis3i_t acc;
	axis3i_t gyro;
} switch_cal_t;

typedef struct  {			//用于枚举switch设备
	uint8_t ready;
  	uint8_t	cmd;
	uint8_t	sub_cmd;
	uint8_t retry;
	uint32_t param;
}switch_enum_t;


#ifndef PRAGMA_PACK_IGNORED
#pragma pack()
#endif


typedef struct {
    uint8_t report_mode;   	//switch repot id,表示switch从机当前report模式,目前只对蓝牙模式有效, 蓝牙断开后要恢复0X3F
    uint8_t lights;
    uint8_t vibration_enable;
    uint8_t imu_enable;
}switch_ctb_t;


#endif

