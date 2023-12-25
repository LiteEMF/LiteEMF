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

/*******************************************************************
** Description:	
	xbox one修改描述符,类似dinput模式 VID=0X045E,PID=0X02E0(l2,r2描述符值会影响按键顺序)
	建议PID=0X02E0, button= 0x0a, 无截图方式
测试效果: 
** 0X02FD PC  0x0a无截图按键
	0	1	2	3	4	5	6		7		8	9	10	11	12	 13	14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO	NO	HOME NO	NO	NO
** 0X02E0/0X02FD IOS  0x0a
	0	1	2	3	4	5	6		7		8	9	10	11	12		13	14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO	NO	HOME	NO	NO	NO
** 0X02FD ANDROID  0x0a 按键错乱

** 0X02E0 PC  0x0a无截图按键
	0	1	2	3	4	5	6		7		8	9	10	11	12	 13		14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO	NO	HOME HOME	NO	NO
** 0X02E0 ANDROID  0x0a 按键错乱
	0	1	2	3	4	5	6		7		8	9	10	11	12	 13	14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO	NO	HOME NO	NO	NO


** 0X02E0/0X02FD PC button数量: 0x0f: 有截图按键
	0	1	2	3	4	5	6		7		8	9	10		11		12	13		14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	HOME	HOME	CAP	HOME	NO	NO
** 0X0B13 PC button数量: 0x0f: Unknown Gamepad
** 0X0B13/0X02E0/0X02FD IOS button数量: 0x0f: 无截图按键
	0	1	2	3	4	5	6		7		8	9	10		11	12		13	14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO		NO	HOME	NO	NO	NO
** 	0X02FD ANDROID button数量: 0x0f: 按键异常
** 	0X0B13 ANDROID button数量: 0x0f: hid gamepad 同 xboxx按键
** 	0X02E0 ANDROID button数量: 0x0f
	0	1	2	3	4	5	6		7		8	9	10		11	12		13	14	15
	A	B	X   Y	L1	R1	SELECT	STAR	L3	R3	NO		NO	MODE	NO	NO	NO
*******************************************************************/
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x05,        // Usage (Game Pad)
0xA1, 0x01,        // Collection (Application)
	0x85, 0x01,        //   Report ID (1)
	0x09, 0x01,        //   Usage (Pointer)
	0xA1, 0x00,        //   Collection (Physical)
		0x09, 0x30,        //     Usage (X)
		0x09, 0x31,        //     Usage (Y)
		0x15, 0x00,        //     Logical Minimum (0)
		0x27, 0xFF, 0xFF, 0x00, 0x00,  //     Logical Maximum (65534)
		0x95, 0x02,        //     Report Count (2)
		0x75, 0x10,        //     Report Size (16)
		0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0x09, 0x01,        //   Usage (Pointer)
	0xA1, 0x00,        //   Collection (Physical)
		0x09, 0x33,        //     Usage (Rx)
		0x09, 0x34,        //     Usage (Ry)
		0x15, 0x00,        //     Logical Minimum (0)
		0x27, 0xFF, 0xFF, 0x00, 0x00,  //     Logical Maximum (65534)
		0x95, 0x02,        //     Report Count (2)
		0x75, 0x10,        //     Report Size (16)
		0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x32,        //   Usage (Z)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
	0x95, 0x01,        //   Report Count (1)
	0x75, 0x0A,        //   Report Size (10)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x00,        //   Logical Maximum (0)
	0x75, 0x06,        //   Report Size (6)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x35,        //   Usage (Rz)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
	0x95, 0x01,        //   Report Count (1)
	0x75, 0x0A,        //   Report Size (10)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x00,        //   Logical Maximum (0)
	0x75, 0x06,        //   Report Size (6)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x39,        //   Usage (Hat switch)
	0x15, 0x01,        //   Logical Minimum (1)
	0x25, 0x08,        //   Logical Maximum (8)
	0x35, 0x00,        //   Physical Minimum (0)
	0x46, 0x3B, 0x01,  //   Physical Maximum (315)
	0x66, 0x14, 0x00,  //   Unit (System: English Rotation, Length: Centimeter)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x00,        //   Logical Maximum (0)
	0x35, 0x00,        //   Physical Minimum (0)
	0x45, 0x00,        //   Physical Maximum (0)
	0x65, 0x00,        //   Unit (None)
	0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x0A,        //   Usage Maximum (0x0A)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x0A,        //   Report Count (10)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x00,        //   Logical Maximum (0)
	0x75, 0x06,        //   Report Size (6)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	
	0x75, 0x08,        //   Report Size (8)  RES 
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x03,        //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x80,        //   Usage (Sys Control)
	0x85, 0x02,        //   Report ID (2)
	0xA1, 0x00,        //   Collection (Physical)
		0x09, 0x85,        //     Usage (Sys Main Menu)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x01,        //     Logical Maximum (1)
		0x95, 0x01,        //     Report Count (1)
		0x75, 0x01,        //     Report Size (1)
		0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x00,        //     Logical Maximum (0)
		0x75, 0x07,        //     Report Size (7)
		0x95, 0x01,        //     Report Count (1)
		0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0x05, 0x0F,        //   Usage Page (PID Page)
	0x09, 0x21,        //   Usage (0x21)
	0x85, 0x03,        //   Report ID (3)
	0xA1, 0x02,        //   Collection (Logical)
		0x09, 0x97,        //     Usage (0x97)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x01,        //     Logical Maximum (1)
		0x75, 0x04,        //     Report Size (4)
		0x95, 0x01,        //     Report Count (1)
		0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x00,        //     Logical Maximum (0)
		0x75, 0x04,        //     Report Size (4)
		0x95, 0x01,        //     Report Count (1)
		0x91, 0x03,        //     Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
		0x09, 0x70,        //     Usage (0x70)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x64,        //     Logical Maximum (100)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x04,        //     Report Count (4)
		0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
		0x09, 0x50,        //     Usage (0x50)
		0x66, 0x01, 0x10,  //     Unit (System: SI Linear, Time: Seconds)
		0x55, 0x0E,        //     Unit Exponent (-2)
		0x15, 0x00,        //     Logical Minimum (0)
		0x26, 0xFF, 0x00,  //     Logical Maximum (255)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x01,        //     Report Count (1)
		0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
		0x09, 0xA7,        //     Usage (0xA7)
		0x15, 0x00,        //     Logical Minimum (0)
		0x26, 0xFF, 0x00,  //     Logical Maximum (255)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x01,        //     Report Count (1)
		0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
		0x65, 0x00,        //     Unit (None)
		0x55, 0x00,        //     Unit Exponent (0)
		0x09, 0x7C,        //     Usage (0x7C)
		0x15, 0x00,        //     Logical Minimum (0)
		0x26, 0xFF, 0x00,  //     Logical Maximum (255)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x01,        //     Report Count (1)
		0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              //   End Collection
	0x85, 0x04,        //   Report ID (4)
	0x05, 0x06,        //   Usage Page (Generic Dev Ctrls)
	0x09, 0x20,        //   Usage (Battery Strength)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

// 331 bytes


