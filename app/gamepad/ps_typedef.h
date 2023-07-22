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

#ifndef	_ps_typedef_h
#define	_ps_typedef_h
#include "hw_config.h"
#include "emf_typedef.h" 

/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
#define   PS_VID			0X054c
#define   PS4_S1_PID	    0x05c4
#define   PS4_PID	        0X09CC
#define   PS5_PID	        0X0CE6
#define   PS3_PID	        0X0268


//第三方ps4手柄手柄
#define  COOVE_VID			0x7545				/*coovelite 	dongle*/ 
#define  COOVELITE_PID		0x1073


#define PS4_TOUCH_MAX_X		1920
#define PS4_TOUCH_MAX_Y		900
#define PS4_SLOT_MAX		2

#ifndef PS_S2_SPK_RATE							/*PS4_SERIES2 */
#define PS_S2_SPK_RATE           32000
#endif
#ifndef PS_S2_MIC_RATE
#define PS_S2_MIC_RATE           16000
#endif
#ifndef PS_S1_SPK_RATE							/*PS4_SERIES1 */
#define PS_S1_SPK_RATE           48000
#endif
#ifndef PS_S1_MIC_RATE
#define PS_S1_MIC_RATE           24000
#endif
#ifndef PS_P3_SPK_RATE							/*PS4_PARTY3,默认自定义修改的手柄 */
#define PS_P3_SPK_RATE           48000
#endif
#ifndef PS_P3_MIC_RATE
#define PS_P3_MIC_RATE           48000
#endif



typedef enum {
	PS_SERIES_NONE=0,
	PS3_SERIES,				//ps3系列
	PS4_SERIES1,			//ps4一代手柄
	PS4_SERIES2,			//ps4二代手柄
	PS4_PARTY3,				//ps第三方手柄
	PS5_SERIES,				//PS5手柄
}ps_series_t;				//ps系列手柄


#define  PS3_SELECT_POS					(0)
#define  PS3_L3_POS						(1)
#define  PS3_R3_POS						(2)
#define  PS3_START_POS					(3)
#define  PS3_UP_POS						(4 )
#define  PS3_RIGHT_POS					(5 )
#define  PS3_DOWN_POS					(6 )
#define  PS3_LEFT_POS					(7 ) 		// HAT SWITCH
#define  PS3_L2_POS						(8)
#define  PS3_R2_POS						(9)
#define  PS3_L1_POS						(10 )
#define  PS3_R1_POS						(11 )
#define  PS3_TRIANGLE_POS				(12 ) 		//三角形
#define  PS3_O_POS						(13 ) 		//圆
#define  PS3_X_POS						(14 ) 		//X形
#define  PS3_SQUARE_POS					(15 ) 		//方
#define  PS3_PS_POS						(16)
//#define  PS3_Z						(17)

#define  PS3_L_UP_POS					(24)
#define  PS3_L_DOWN_POS					(25)
#define  PS3_L_LEFT_POS					(26)
#define  PS3_L_RIGHT_POS				(27)
#define  PS3_R_UP_POS					(28)
#define  PS3_R_DOWN_POS					(29)
#define  PS3_R_LEFT_POS					(30)
#define  PS3_R_RIGHT_POS				(31)

#define  PS3_SELECT					(0x01UL<<PS3_SELECT_POS	 )
#define  PS3_L3						(0x01UL<<PS3_L3_POS		 )
#define  PS3_R3						(0x01UL<<PS3_R3_POS		 )
#define  PS3_START					(0x01UL<<PS3_START_POS	 )
#define  PS3_UP						(0x01UL<<PS3_UP_POS		 )
#define  PS3_RIGHT					(0x01UL<<PS3_RIGHT_POS	 )
#define  PS3_DOWN					(0x01UL<<PS3_DOWN_POS	 )
#define  PS3_LEFT					(0x01UL<<PS3_LEFT_POS	)	/* HAT SWITCH */
#define  PS3_L2						(0x01UL<<PS3_L2_POS		 )
#define  PS3_R2						(0x01UL<<PS3_R2_POS		 )
#define  PS3_L1						(0x01UL<<PS3_L1_POS		 )
#define  PS3_R1						(0x01UL<<PS3_R1_POS		 )
#define  PS3_TRIANGLE				(0x01UL<<PS3_TRIANGLE_POS)	/*三角形 */
#define  PS3_O						(0x01UL<<PS3_O_POS		)	/*圆 */
#define  PS3_X						(0x01UL<<PS3_X_POS		)	/*X形 */
#define  PS3_SQUARE					(0x01UL<<PS3_SQUARE_POS	)	/*方 */
#define  PS3_PS						(0x01UL<<PS3_PS_POS		 )
//#define  PS3_Z					(0x01UL<<  PS3_Z		 )
#define  PS3_L_UP					(0x01UL<<PS3_L_UP_POS	 )
#define  PS3_L_DOWN				(0x01UL<<PS3_L_DOWN_POS )
#define  PS3_L_LEFT				(0x01UL<<PS3_L_LEFT_POS )
#define  PS3_L_RIGHT				(0x01UL<<PS3_L_RIGHT_POS)
#define  PS3_R_UP					(0x01UL<<PS3_R_UP_POS	 )
#define  PS3_R_DOWN				(0x01UL<<PS3_R_DOWN_POS )
#define  PS3_R_LEFT				(0x01UL<<PS3_R_LEFT_POS )
#define  PS3_R_RIGHT				(0x01UL<<PS3_R_RIGHT_POS)


#define  PS4_UP_POS						(0 )
#define  PS4_RIGHT_POS					(1 )
#define  PS4_DOWN_POS					(2 )
#define  PS4_LEFT_POS					(3 ) 		/* HAT SWITCH */
#define  PS4_SQUARE_POS					(4 )
#define  PS4_X_POS						(5 )
#define  PS4_O_POS						(6 )
#define  PS4_TRIANGLE_POS				(7 )
#define  PS4_L1_POS						(8 )
#define  PS4_R1_POS						(9 )
#define  PS4_L2_POS						(10)
#define  PS4_R2_POS						(11)
#define  PS4_SHARE_POS					(12)
#define  PS4_OPTION_POS					(13)
#define  PS4_L3_POS						(14)
#define  PS4_R3_POS						(15)
#define  PS4_PS_POS						(16)
#define  PS4_TOUCH_KEY_POS				(17)
										//(18)						

#define  PS4_TOUCH_U_POS				(20)	/*touch面板上约12-1点钟方向的区域(为了兼容rewasd平台) */
#define  PS4_TOUCH_D_POS				(21)	/*touch面板上约6-1点钟方向的区域 */		
#define  PS4_TOUCH_L_POS				(22)	/*touch面板上约9-1点钟方向的区域 */
#define  PS4_TOUCH_R_POS	    		(23)	/*touch面板上约3-1点钟方向的区域 */
#define  PS4_L_UP_POS					(24)
#define  PS4_L_DOWN_POS					(25)
#define  PS4_L_LEFT_POS					(26)
#define  PS4_L_RIGHT_POS				(27)
#define  PS4_R_UP_POS					(28)
#define  PS4_R_DOWN_POS					(29)
#define  PS4_R_LEFT_POS					(30)
#define  PS4_R_RIGHT_POS				(31)

#define  PS4_UP						(0x01UL<<PS4_UP_POS			)
#define  PS4_RIGHT					(0x01UL<<PS4_RIGHT_POS		)
#define  PS4_DOWN					(0x01UL<<PS4_DOWN_POS		)
#define  PS4_LEFT					(0x01UL<<PS4_LEFT_POS		)/* HAT SWITCH */
#define  PS4_SQUARE					(0x01UL<<PS4_SQUARE_POS		)
#define  PS4_X						(0x01UL<<PS4_X_POS			)
#define  PS4_O						(0x01UL<<PS4_O_POS			)
#define  PS4_TRIANGLE				(0x01UL<<PS4_TRIANGLE_POS	)
#define  PS4_L1						(0x01UL<<PS4_L1_POS			)
#define  PS4_R1						(0x01UL<<PS4_R1_POS			)
#define  PS4_L2						(0x01UL<<PS4_L2_POS			)
#define  PS4_R2						(0x01UL<<PS4_R2_POS			)
#define  PS4_SHARE					(0x01UL<<PS4_SHARE_POS		)
#define  PS4_OPTION					(0x01UL<<PS4_OPTION_POS		)
#define  PS4_L3						(0x01UL<<PS4_L3_POS			)
#define  PS4_R3						(0x01UL<<PS4_R3_POS			)
#define  PS4_PS						(0x01UL<<PS4_PS_POS			)
#define  PS4_TOUCH_KEY				(0x01UL<<PS4_TOUCH_KEY_POS	)
#define  PS4_TOUCH_U				(0x01UL<<PS4_TOUCH_U_POS	)
#define  PS4_TOUCH_D				(0x01UL<<PS4_TOUCH_D_POS	)
#define  PS4_TOUCH_L				(0x01UL<<PS4_TOUCH_L_POS	)
#define  PS4_TOUCH_R				(0x01UL<<PS4_TOUCH_R_POS	)


#define  PS4_L_UP					(0x01UL<<PS4_L_UP_POS		)
#define  PS4_L_DOWN					(0x01UL<<PS4_L_DOWN_POS	)
#define  PS4_L_LEFT					(0x01UL<<PS4_L_LEFT_POS	)
#define  PS4_L_RIGHT				(0x01UL<<PS4_L_RIGHT_POS	)
#define  PS4_R_UP					(0x01UL<<PS4_R_UP_POS		)
#define  PS4_R_DOWN					(0x01UL<<PS4_R_DOWN_POS	)
#define  PS4_R_LEFT					(0x01UL<<PS4_R_LEFT_POS	)
#define  PS4_R_RIGHT				(0x01UL<<PS4_R_RIGHT_POS	)



#define PS4_USB_EFFECTS_ID			 	0x05		/*USB 马达指令 */
#define PS4_BT_EFFECTS_ID				0x11		/*蓝牙协议 马达指令 */


#define PS4_REPORT_ID			 		0x01
#define PS4_BT_LARGE_REPORT_ID			0x11		/*蓝牙协议发送完整数据, */
#define PS4_BT_LARGE_REPORT1_ID			0x12		/*0x11~0X1A都是 */
#define PS4_BT_LARGE_REPORT2_ID			0x13		
#define PS4_BT_LARGE_REPORT3_ID			0x14		
#define PS4_BT_LARGE_REPORT4_ID			0x15		
#define PS4_BT_LARGE_REPORT5_ID			0x16		
#define PS4_BT_LARGE_REPORT6_ID			0x18		
#define PS4_BT_LARGE_REPORT7_ID			0x19
#define PS4_BT_LARGE_REPORT8_ID			0x1A


#define PS_CHALLENGE_CMD				0xF0			/*256	byte */
#define PS_CALCULATE_CHECK_CMD			0xF2			/*doing: F2 index 00 , complete:F2 index 10 */
#define PS_ANSWE_CMD					0xF1			/*1040 */

#define PS5_USB_EFFECTS_ID			 	0x02		/*USB 马达指令 */
#define PS5_BT_EFFECTS_ID				0x31		/*蓝牙协议 马达指令 */

/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/

#ifndef pack
#pragma pack(1)
#endif

typedef struct {
	uint8_t id:7;
	uint8_t active:1;
	int16_t x;
	int16_t y;
}ps_touch_t;

typedef struct {
	uint8_t id;			//0x12
	uint8_t mac[6];
	uint8_t res[3];		//08 25 00
	uint8_t host_mac[6];
}ps_bt_mac_t;

typedef struct {
	uint8_t id;			//0x13
	uint8_t mac[6];
	uint8_t bt_link_key[16];		//猜测
}ps_bt_link_t;

typedef struct {
	uint8_t cmd;
	uint8_t index;
	uint8_t data_index;			//from 0
	uint8_t res;
	uint8_t buf[56];
	uint8_t sum[4];				//不清楚可以写0
}ps4_cmd_t;

typedef struct {
	uint8_t rumble_r;
    uint8_t rumble_l;
    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;
    uint8_t led_delay_on;
    uint8_t led_delay_off;
    uint8_t pad0[ 8 ];
    uint8_t volume_l;
    uint8_t volume_r;
    uint8_t volume_mic;
    uint8_t volume_speaker;
	uint8_t pad1[ 9 ];
}ps4_effects_t;			//28

typedef struct {
	uint8_t cmd;				//0x5
	uint8_t magic;				//0x07 Magic value,按位控制哪些数据有效, 0X01是马达,其他位要测试
	uint8_t res[2];				//0x00
	ps4_effects_t effects;
}ps4_usb_effects_t;			//32

typedef struct {
	uint8_t cmd;				//0x11
	uint8_t magic;				//0xC0 | 0x04;  bit7:HID, bit6:是否需要CRC,bit0~3:interval,  Magic value HID + CRC, also sets interval to 4ms for samples */
	uint8_t res;
	uint8_t ctrl;				//0x03;  /* 0x1 is rumble, 0x2 is lightbar, 0x4 is the blink interval */
	uint8_t res2[2];			//0x00
	ps4_effects_t effects;
	uint8_t res3[40];
	uint32_t crc;
}ps4_bt_effects_t;		//78

typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint8_t id;				//固定01
	uint8_t lx;
	uint8_t ly;
	uint8_t rx;
	uint8_t ry;
	uint16_t buttonl;
	uint8_t  buttonh:2;
	uint8_t index:6;	  		//"帧号 普通按键数据+4, PS按键按下+5, 按键抬起+3
	uint8_t l2; 				//0-0XFF
	uint8_t r2; 				//0-0XFF

	uint16_t time_index; 		// 默认4ms对应704
	int8_t mark;				//默认0x13		0x12 0x13 0x0d都有

	axis3i_t gyro;				//对应pich roll yaw ,参考acc坐标,左手定律旋转,范围-32768~32767
	axis3i_t acc;				//系统:x向左,Y向下,Z向内范围0~8192		,25
	uint8_t res[5];
	uint8_t battery:5;			//bit4:USB接如, bit0~4:电量比例,比如5=50%, 超过10都=100%
	uint8_t ear_stu:3;			//拔出耳机: 0:拔出耳机 1: 耳机接入过程? 3:接入耳机
	uint8_t res2[4];				//35

	uint8_t touch_pad1_index;		//bit7:0:press,1:up, bit0~6: index
    uint8_t touch_pad1[ 3 ];		//12bit x,y小端
    uint8_t touch_pad2_index;
    uint8_t touch_pad2[ 3 ];		//43

	uint8_t pad_res;
	uint8_t touch_pad3_index;		//一下是猜测,21个保留数据 ,padn_index必须是80
    uint8_t touch_pad3[ 3 ];		
    uint8_t touch_pad4_index;
    uint8_t touch_pad4[ 3 ];
	uint8_t pad_res2;
	uint8_t touch_pad5_index;		
    uint8_t touch_pad5[ 3 ];		
    uint8_t touch_pad6_index;
    uint8_t touch_pad6[ 3 ];
	uint8_t pad_res3;
	uint8_t touch_pad7_index;	
	uint8_t touch_pad7[ 1 ];	
}ps4_report_t;			//64


typedef struct	 //mini ps4报告结构体舍去没有用到结构
{
	uint8_t id;			//PS4_BT_LARGE_REPORT_ID~PS4_BT_LARGE_REPORT8_ID
	uint8_t magic;		//0xC0, bit7:HID, bit6:是否需要CRC
	uint8_t res0;

	uint8_t lx;
	uint8_t ly;			//下面和ps4_report_t一样
	uint8_t rx;
	uint8_t ry;
	uint16_t buttonl;
	uint8_t  buttonh:2;
	uint8_t index:6;	  		//"帧号 普通按键数据+4, PS按键按下+5, 按键抬起+3
	uint8_t l2; 				//0-0XFF
	uint8_t r2; 				//0-0XFF
	int16_t time_index; 		// 默认4ms对应704
	int8_t mark;				//默认0x13		0x12 0x13 0x0d都有
	axis3i_t gyro;				//对应pich roll yaw ,参考acc坐标,左手定律旋转,范围-32768~32767
	axis3i_t acc;				//系统:x向左,Y向下,Z向内范围0~8192		,25
	uint8_t res[5];
	uint8_t battery:5;			//bit4:USB接如, bit0~4:电量比例,比如5=50%, 超过10都=100%
	uint8_t ear_stu:3;			//拔出耳机: 0:拔出耳机 1: 耳机接入过程? 3:接入耳机
	uint8_t res2[4];				//35
	uint8_t touch_pad1_index;		//bit:0:press,1:up, bit0~6: index
    uint8_t touch_pad1[ 3 ];		//12bit x,y小端
    uint8_t touch_pad2_index;
    uint8_t touch_pad2[ 3 ];		//43

	uint8_t pad_res;
	uint8_t touch_pad3_index;		//一下是猜测,27个保留数据 ,padn_index必须是80
    uint8_t touch_pad3[ 3 ];		
    uint8_t touch_pad4_index;
    uint8_t touch_pad4[ 3 ];
	uint8_t pad_res2;
	uint8_t touch_pad5_index;		
    uint8_t touch_pad5[ 3 ];		
    uint8_t touch_pad6_index;
    uint8_t touch_pad6[ 3 ];
	uint8_t pad_res3;
	uint8_t touch_pad7_index;		
    uint8_t touch_pad7[ 3 ];		
    uint8_t touch_pad8_index;
    uint8_t touch_pad8[ 3 ];

	uint8_t res4[2];
	uint32_t crc;
}ps4_large_report_t;			//78



typedef struct {
	uint8_t bit1_en;                // bit0: 1,Enable rumble emulation,
									// bit1:1,Disable audio haptics
    uint8_t bit2_en;                // bit0: 1,Enable microphone light(mic_light_mode)
									// bit2: 1,Enable LED color(rgb 中设置颜色),
									// bit3: 1,Reset LED state
									// bit4: 1,Enable touchpad light

    uint8_t rumble_r;                /* 2 */
    uint8_t rumble_l;                 /* 3 */
    uint8_t headphone_volume;            /* 4 */
    uint8_t spk_volume;              /* 5 */
    uint8_t mic_volume;           	/* 6 */
    uint8_t audio_enble;            /* 7 */
    uint8_t mic_light_mode;        	//Bitmask, 0x00 = off, 0x01 = solid, 0x02 = pulse
    uint8_t audio_mute;              /* 9 */
    uint8_t trigger_r_effect[11];   /* 10 */
    uint8_t trigger_l_effect[11];    /* 21 */
    uint8_t unknown1[6];              /* 32 */
    uint8_t led_flags;               /* 38 */
    uint8_t unknown2[2];             /* 39 */
    uint8_t led_anim;                /* 41 */
    uint8_t led_brightness;          /* 42 */
    uint8_t pad_lights;              //Enable touchpad lights
    uint8_t led_r;                   /* 44 */
    uint8_t led_g;                   /* 45 */
    uint8_t led_b;                   /* 46 */
}ps5_effects_t;			//28

typedef struct {
	uint8_t cmd;				//0x5
	ps5_effects_t effects;
}ps5_usb_effects_t;			//32

typedef struct {
	uint8_t cmd;				//0x11
	uint8_t magic;				//0x02
	ps5_effects_t effects;
}ps5_bt_effects_t;		//34, 78?

typedef struct		//小端
{
	uint8_t id;					//固定01
	int8_t lx;					//-128 ~ 127
	int8_t ly;
	int8_t rx;
	int8_t ry;
	uint8_t l2; 				//0-0XFF
	uint8_t r2; 				//0-0XFF
	uint8_t index;	  			//"帧号
	uint32_t button;			//底3位为方向键
	uint16_t index2; 			//+1
	uint16_t mark;				//每次开机随机, 以上数据加密	16
	axis3i_t gyro;				//对应pich roll yaw ,参考acc坐标,左手定律旋转,范围-32768~32767
	axis3i_t acc;				//系统:x向左,Y向下,Z向内范围0~8192
	uint16_t time_us; 			//3us为一个单位
	uint16_t time_ms; 			//20ms 为一个单位		16

	uint8_t battery_res;
	uint8_t touch_pad1_index;			//bit:0:press,1:up, bit0~6: index
    uint8_t touch_pad1[ 3 ];			//12bit x,y小端
    uint8_t touch_pad2_index;
    uint8_t touch_pad2[ 3 ];			//25
	uint8_t res[8]; 					//保留数据
	uint16_t time_us2; 					//3us为一个单位
	uint16_t time_ms2; 					//20ms 为一个单位
	uint8_t battery:5;					//bit4:USB接如, bit0~4:电量比例,比如5=50%, 超过10都=100%
	uint8_t ear_stu:3;					//拔出耳机: 0:拔出耳机 1: 耳机接入过程? 3:接入耳机
	uint16_t res2; 						// 0x1800 					24

	uint8_t secucrt[8]; //保留数据  鼠标键盘用不上
}ps5_report_t;



typedef struct	 //只定义有用数据  其他数据最后附加
{
	uint8_t id; 		//1
	uint8_t res;		//0
	uint32_t button; //小端
	uint8_t lx;
	uint8_t ly;
	uint8_t rx;
	uint8_t ry;
	uint8_t res3[4];	// 0
	uint8_t up; 		//0~ff
	uint8_t right;		//0~ff
	uint8_t down;		//0~ff
	uint8_t left;		//0~ff
	uint8_t l2;
	uint8_t r2;
	uint8_t l1;
	uint8_t r1;
	uint8_t bu; 	//三角形
	uint8_t br; 	//圆
	uint8_t bd; 	//X形
	uint8_t bl; 	//方

	uint8_t res4[3];
	uint8_t charge1;		//0x03
	uint8_t charge2;		//0xef
	uint8_t charge3;		//0x14
	uint8_t res5[8];		//ox
	uint8_t port_data;		//0x1d
	uint16_t sensor_x;
	uint16_t sensor_y;
	uint16_t sensor_z;
	uint16_t sensor_speed;

}ps3_report_t;




#ifndef pack
#pragma pack()
#endif



/*******************************************************************************************************************
**	Functions
********************************************************************************************************************/

#endif

