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

0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x02,        // Usage (Mouse)
0xA1, 0x01,        // Collection (Application)
	0x85, MOUSE_REPORT_ID,        //   Report ID (1)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls),!must add 05 01 switch back 01
	0x09, 0x01,        //   Usage (Pointer)
	0xA1, 0x00,        //   Collection (Physical)
		0x05, 0x09,        //     Usage Page (Button)
		0x19, 0x01,        //     Usage Minimum (0x01)
		0x29, 0x05,        //     Usage Maximum (0x05)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x01,        //     Logical Maximum (1)
		0x75, 0x01,        //     Report Size (1)
		0x95, 0x05,        //     Report Count (5)
		0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
		0x95, 0x03,        //     Report Count (3)
		0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
		0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
		0x09, 0x30,        //     Usage (X)
		0x09, 0x31,        //     Usage (Y)
		0x16, 0x01, 0x80,  //     Logical Minimum (-32767)
		0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
		0x75, 0x10,        //     Report Size (16)
		0x95, 0x02,        //     Report Count (2)
		0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
		0x09, 0x38,        //     Usage (Wheel)
		0x15, 0x81,        //     Logical Minimum (-127)
		0x25, 0x7F,        //     Logical Maximum (127)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x01,        //     Report Count (1)
		0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
0xC0,              // End Collection

// 66 bytes


