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
#ifndef	_xbox_typedef_h
#define	_xbox_typedef_h

#include "utils/emf_typedef.h" 

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define   XBOX_VID    		0X045E
#define   XBOX_PID         	0X02EA		/* 0X02D1(不带3.5手柄)	0X0B00 */
#define   XBOX_UAC_PID      0X02F6		/* xbox/xboxx接音频后的PID */
#define   X360_PID         	0X028E
#define   XBOXX_PID         0X0B12		/* 0X02EA */
#define   XBOX_BT_PID		0X02E0		/* 0X02FD */



#ifndef XBOX_SPK_RATE
#define XBOX_SPK_RATE           48000
#endif
#ifndef XBOX_MIC_RATE
#define XBOX_MIC_RATE           24000
#endif

//------------------xbox 360 mode------------------
#define X360_UP_POS			(0)
#define X360_DOWN_POS		(1)
#define X360_LEFT_POS		(2)
#define X360_RIGHT_POS		(3)
#define X360_START_POS		(4)
#define X360_BACK_POS		(5)
#define X360_L3_POS			(6)
#define X360_R3_POS			(7)
#define X360_L1_POS			(8)
#define X360_R1_POS			(9)
#define X360_HOME_POS		(10)
#define X360_C_POS			(11)
#define X360_A_POS			(12)
#define X360_B_POS			(13)
#define X360_X_POS			(14)
#define X360_Y_POS		    (15)

#define X360_L2_POS			(16)
#define X360_R2_POS		    (17)
#define X360_L_UP_POS		(24)
#define X360_L_DOWN_POS		(25)
#define X360_L_LEFT_POS		(26)
#define X360_L_RIGHT_POS	(27)
#define X360_R_UP_POS		(28)
#define X360_R_DOWN_POS		(29)
#define X360_R_LEFT_POS		(30)
#define X360_R_RIGHT_POS	(31)

#define X360_UP			(0x01UL<<X360_UP_POS		)
#define X360_DOWN		(0x01UL<<X360_DOWN_POS	  )
#define X360_LEFT		(0x01UL<<X360_LEFT_POS	  )
#define X360_RIGHT		(0x01UL<<X360_RIGHT_POS	  )
#define X360_START		(0x01UL<<X360_START_POS	  )
#define X360_BACK		(0x01UL<<X360_BACK_POS	  )
#define X360_L3			(0x01UL<<X360_L3_POS		)
#define X360_R3			(0x01UL<<X360_R3_POS		)
#define X360_L1			(0x01UL<<X360_L1_POS		)
#define X360_R1			(0x01UL<<X360_R1_POS		)
#define X360_HOME		(0x01UL<<X360_HOME_POS	  )
#define X360_C			(0x01UL<<X360_C_POS		  )
#define X360_A			(0x01UL<<X360_A_POS		  )
#define X360_B			(0x01UL<<X360_B_POS		  )
#define X360_X			(0x01UL<<X360_X_POS		  )
#define X360_Y		    (0x01UL<<X360_Y_POS		  )

#define X360_L2			(0x01UL<<X360_L2_POS		)
#define X360_R2		    (0x01UL<<X360_R2_POS		)
#define X360_L_UP		(0x01UL<<X360_L_UP_POS	  )
#define X360_L_DOWN		(0x01UL<<X360_L_DOWN_POS )
#define X360_L_LEFT		(0x01UL<<X360_L_LEFT_POS )
#define X360_L_RIGHT	(0x01UL<<X360_L_RIGHT_POS)
#define X360_R_UP		(0x01UL<<X360_R_UP_POS	  )
#define X360_R_DOWN		(0x01UL<<X360_R_DOWN_POS )
#define X360_R_LEFT		(0x01UL<<X360_R_LEFT_POS )
#define X360_R_RIGHT	(0x01UL<<X360_R_RIGHT_POS)

//------------------xbox one mode------------------
//#define  XBOX_Z						(0 )
//#define  XBOX_Z						(1 )
#define  XBOX_MENU_POS					(2 )
#define  XBOX_MAP_POS					(3 )
#define  XBOX_A_POS						(4 )
#define  XBOX_B_POS						(5 )
#define  XBOX_X_POS						(6 )
#define  XBOX_Y_POS						(7 )
#define  XBOX_UP_POS					(8 )
#define  XBOX_DOWN_POS					(9 )
#define  XBOX_LEFT_POS					(10)
#define  XBOX_RIGHT_POS					(11)
#define  XBOX_L1_POS					(12)
#define  XBOX_R1_POS					(13)
#define  XBOX_L3_POS					(14)
#define  XBOX_R3_POS					(15)

#define  XBOX_L2_POS					(16)
#define  XBOX_R2_POS					(17)
#define  XBOX_HOME_POS					(18)
#define  XBOX_SHARE_POS					(19) 		/*only xboxxs */

#define  XBOX_L_UP_POS					(24)
#define  XBOX_L_DOWN_POS				(25)
#define  XBOX_L_LEFT_POS				(26)
#define  XBOX_L_RIGHT_POS				(27)
#define  XBOX_R_UP_POS					(28)
#define  XBOX_R_DOWN_POS				(29)
#define  XBOX_R_LEFT_POS				(30)
#define  XBOX_R_RIGHT_POS				(31)

#define  XBOX_MENU					(0x01UL<<XBOX_MENU_POS		)
#define  XBOX_MAP					(0x01UL<<XBOX_MAP_POS		)
#define  XBOX_A						(0x01UL<<XBOX_A_POS			)
#define  XBOX_B						(0x01UL<<XBOX_B_POS			)
#define  XBOX_X						(0x01UL<<XBOX_X_POS			)
#define  XBOX_Y						(0x01UL<<XBOX_Y_POS			)
#define  XBOX_UP					(0x01UL<<XBOX_UP_POS		)
#define  XBOX_DOWN					(0x01UL<<XBOX_DOWN_POS		)
#define  XBOX_LEFT					(0x01UL<<XBOX_LEFT_POS		)
#define  XBOX_RIGHT					(0x01UL<<XBOX_RIGHT_POS		)
#define  XBOX_L1					(0x01UL<<XBOX_L1_POS		)
#define  XBOX_R1					(0x01UL<<XBOX_R1_POS		)
#define  XBOX_L3					(0x01UL<<XBOX_L3_POS		)
#define  XBOX_R3					(0x01UL<<XBOX_R3_POS		)

#define  XBOX_L2					(0x01UL<<XBOX_L2_POS		)
#define  XBOX_R2					(0x01UL<<XBOX_R2_POS		)
#define  XBOX_HOME					(0x01UL<<XBOX_HOME_POS		)
#define  XBOX_SHARE					(0x01UL<<XBOX_SHARE_POS		)//only xboxx
#define  XBOX_L_UP					(0x01UL<<XBOX_L_UP_POS		)
#define  XBOX_L_DOWN				(0x01UL<<XBOX_L_DOWN_POS	)
#define  XBOX_L_LEFT				(0x01UL<<XBOX_L_LEFT_POS	)
#define  XBOX_L_RIGHT				(0x01UL<<XBOX_L_RIGHT_POS	)
#define  XBOX_R_UP					(0x01UL<<XBOX_R_UP_POS		)
#define  XBOX_R_DOWN				(0x01UL<<XBOX_R_DOWN_POS	)
#define  XBOX_R_LEFT				(0x01UL<<XBOX_R_LEFT_POS	)
#define  XBOX_R_RIGHT				(0x01UL<<XBOX_R_RIGHT_POS	)


/*-----------------------ID和指令-------------------------------------------------*/
//xbox bt 
#define 	XBOX_BT_KEY_REP_CMD			(0x01)				/*普通按键键 report cmd, 单独序号 */
#define 	XBOX_BT_HOME_REP_CMD		(0x02)				/*只有XBOX one模式下有效,xboxxs在button中发送 */
#define 	XBOX_BT_RUMBLE_CMD			(0X03)				/*设置马达, 单独序号 */
//usb x360
#define   X360_KEY_REP_CMD	SWAP16_L(0x1400)
#define   X360_RUMBLE_CMD	SWAP16_L(0x0800)
//USB xboxone 
#define XBOX_NONE_CMD			(0x00)
#define XBOX_ACK_CMD			(0x01)				/*返回ACK(手柄输入数据的((data[1] & 0x30) == 0x30) 才需要ACK), index=in/out包中的index */
#define XBOX_INFO_CMD			(0x02)				/*手柄信息(mac,PID,VID,version,other)	//index 从0x02开始, 是通用cmd_index/cmd_uac_index的开始 */
#define XBOX_STATUS_CMD			(0X03)				/*xbox手柄状态更新, 具体状态不清楚 */
#define XBOX_UNKNOWN_04_CMD		(0x04)				/* 0Xxx04未知 */
#define XBOX_START_CMD			(0X05)				/*开始 controller */
#define XBOX_SECURITY_CMD		(0X06)				/*加密指令,必须要ACK val 0X01: SECURITY 加密成功	,index 单独存在 */
#define XBOX_HOME_REP_CMD		(0x07)				/*home键 report cmd	需要ACK */
#define XBOX_VOLUME_CTRL_CMD	(0X08)				/*音量设置,设置音量在这里 */
#define XBOX_RUMBLE_CMD			(0X09)				/*设置马达, 单独序号 */
#define XBOX_EN_LED_CMD			(0X0A)				/*打开logo led */
#define XBOX_UNKNOWN_0D_CMD		(0x0D)
#define XBOX_SERIAL_CMD			(0X1E)				/*必须要ACK 0x00, 0x01, 0x04三种信息, 0x04是serial numbe */
#define XBOX_KEY_REP_CMD		(0x20)				/*普通按键键 report cmd, 单独序号 */
//XBOX bctrl bit
typedef enum{
	XBOX_CTRL_UAC		= (0X01<<0),		//切换到音频设备(设备PID,MAC会修改)时候需要设置该位,如果手柄没有接耳机可以不回复信息给主机
	XBOX_CTRL_RES		= (0X01<<1),		
	XBOX_CTRL_NEED_ACK	= (0X01<<4),		//是否需要ACK, 如果长包每4组合数据后会要求同步一次,头尾必须同步, 第二次同步的时候长度是39?
	XBOX_CTRL_HSH		= (0X01<<5),		//是否是握手包 0x20 ?? 好像只要马达和按键数据是0 ,其他都是0x20
	XBOX_CTRL_HEAD		= (0X01<<6),		//长包起始包,只有发长包的起始包的时候为1
	XBOX_CTRL_LPACKET	= (0X01<<7),		//是否是长包,长包 lpacket_t 格式
}xbox_ctrl_t;

/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/

typedef enum {
	XBOX_ONE=0,
	XBOX_SERIESX,
}xbox_series_t;

#ifndef pack
#pragma pack(1)
#endif

//*********************************************************************************//
//                               x360 			                          //
//*********************************************************************************//

//hid gamepad
typedef struct
{
	uint16_t cmd;			//固定为 X360_KEY_REP_CMD
	uint16_t button;		//按键
	uint8_t l2; 			//0-0xff
	uint8_t r2; 			//0-0xff
	int16_t x;				/* -32768 ~ 32767 小端*/
	int16_t y;
	int16_t rx;
	int16_t ry;
	uint8_t res[6]; 		//保留 全为0
}x360_report_t;

typedef struct {
	uint16_t cmd;			//固定0x800 小端
	uint8_t res;      		//0x00
	uint8_t motor1; 		//大马达 left
	uint8_t motor2; 		//大马达 right
	uint8_t res2[3];     	//0x00
}x360_usb_motor_t;		//8


//*********************************************************************************//
//                                xboxone 			                          //
//*********************************************************************************//

typedef struct {
	uint8_t len0;		//如果开始包:要发送数据总长度, 如果是中间包:已经发送的数据, 结束包:发 0
 	uint8_t len1;
	uint8_t buf[58];	//最大58
}xbox_lpacket_t;		//xbox long packet 长包



typedef struct{
	uint8_t val[15];		//当一个字节的时候,0x00	0:开始初始化, 1:停止复位初始化???
}xbox_start_t;

/* cmd:0x05, Start controller 有两种指令格式,另外一种数据长度不一样*/
typedef struct{
	uint8_t state;	//0x83
	uint8_t uac;
	uint8_t res[2];
}xbox_state_t;		//4

/* cmd:0x06 This controller passed security check, 不只有这一个指令长度ctrl指令b_ack是1 */
typedef struct{
	uint8_t buf[2];		//0x01, 0x00(表示成功成功)
}xbox_security_pass_t;

/* Enable LED */
typedef struct{			//固定0x0A
	uint8_t led_ctrl[3];		//0x00, 0x01, 0x14
}xbox_led_en_t;

typedef struct
{
	uint8_t mac[6]; 		//mac(注意接入音频后MAC和PID值有改变)
	uint8_t res2[2];		//0x00 0x00
	int16_t vid;
	int16_t pid;
	uint8_t version[8];		//e.g. 0x05 0x00 0x05 0x00 0x51 0x0a 0x00 0x00 is firmware version 5.5.2641.0,and product version 0x0505 = 1285
	uint8_t unknown[8]; 	//04 05 01 00 01 00 01 00
}xbox_info_t;			//28

/*手柄上报*/
typedef struct				//固定0x1E,0X20/b_ack=1  (0x20的情况下是serial number,其他情况不清楚)
{
	uint8_t res[2];			//0x04 0x00
	uint8_t sl[16]; 		//serial number
}serial_number_t;			//0x10

typedef struct				//XBOX_VOLUME_CTRL_CMD
{
	uint8_t type;			//0x02:未知(内容固定0x02,0x09,0x10),0x03:volume ctal
	uint8_t off:7;			//0x04 打开音频, 0x5关闭音频
	uint8_t off_dir:1;		//1:out设置, 0:in设备回复
	uint8_t spk_vol:7;		//0~100
	uint8_t spk_dir:1;		//1:out设置, 0:in设备回复
	uint8_t chat_vol:7;		//0~100	耳机聊天控制台音量
	uint8_t chat_dir:1;		//1:out设置, 0:in设备回复
	uint8_t res_vol:7;		//64	未知预留音量控制位
	uint8_t res_dir:1;		//1:out设置, 0:in设备回复
	uint8_t mic_vol:7;		//0~100
	uint8_t mic_dir:1;		//1:out设置, 0:in设备回复
	uint8_t res[2];
}xbox_vol_ctrl_t;			//8


typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint8_t bt_cmd;			//XBOX_BT_RUMBLE_CMD	0X03, USB: 0X00
	uint8_t rumble_mask; 	//0x0f
	uint8_t smotor1; 		//小马达 left
	uint8_t smotor2; 		//小马达 right
	uint8_t motor1; 		//大马达 left
	uint8_t motor2; 		//大马达 right
	uint8_t duration;   	//0xff
	uint8_t res2[2];		//0x00 0xeb  period?
}xbox_motor_t;				//9


////report struct
typedef struct
{
	uint8_t cmd;			//包头 固定为0x3007
	uint8_t bctrl;			//0x30
	uint8_t index;			//帧号  每次自增1
	uint8_t len;			//0x02
	uint8_t button; 		//bit0 表示logo是否按下  bit1-bit7保留
	uint8_t home_key;		 //key  固定为0x5b
}xbox_logo_report_t;		// bit0,1:press,0:up,  xbox 手柄logo键报告数据


typedef struct
{
	uint8_t id;				//XBOX_BT_HOME_REP_CMD
	uint8_t home_key;		//1:按下,0:抬起
}xbox_bt_logo_report_t;		//2

typedef struct {
	uint8_t id;				//0x20
	uint8_t bctrl;				//0x00
	uint8_t index;
	uint8_t len;						//接下来数据长度不包含护自己
	uint16_t button; /* Button 1 ~ 16 */
	int16_t l2; /* 0 ~ 0x3FF */
	int16_t r2; /* 0 ~ 0x3FF */
	int16_t x; /* -32768 ~ 32767 */ 		//小端
	int16_t y;
	int16_t rx;
	int16_t ry;
}xboxone_report_t;			//18

typedef struct {
	uint8_t id;				//0x20
	uint8_t bctrl;				//0x00
	uint8_t index;
	uint8_t len;		//0x2c
	uint16_t button; /* Button 1 ~ 16 */
	int16_t l2; /* 0 ~ 0x3FF */
	int16_t r2; /* 0 ~ 0x3FF */
	int16_t x; /* -32768 ~ 32767 */ 		//小端
	int16_t y;
	int16_t rx;
	int16_t ry; 				//18
	uint8_t res[22]; 			//res[4] share key 1 按下, 0 抬起
	uint16_t time_us; 		//1us为一个单位
	uint16_t time_ms; 		//65ms 为一个单位time_us 的溢出		16
	uint16_t time_us2; 		//同上
	uint16_t time_ms2; 		//同上
}xbox_report_t;			//48


typedef struct {
	uint8_t id;				//XBOX_BT_KEY_REP_CMD
	uint16_t x; 			/* -32768 ~ 32767 */ 		//小端
	uint16_t y;
	uint16_t rx;
	uint16_t ry;
	int16_t l2; 			/* 0 ~ 0x3FF */
	int16_t r2; 			/* 0 ~ 0x3FF */
	uint8_t hat_switch;		//1~8
	uint16_t button; 		//采用的是android GAMEPAD 数据格式
	uint8_t back_share_key:1;		//xboxone back, xboxxs sharekey
	uint8_t res_key:7;
}xbox_bt_report_t;			//17	xboxone,xboxxs都是这个结构体


#ifndef pack
#pragma pack()
#endif




#endif

