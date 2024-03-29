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
0x09, 0x05,        // Usage (Game Pad)
0xA1, 0x01,        // Collection (Application)
	0x06, 0x01, 0xFF,  //   Usage Page (Vendor Defined 0xFF01)
	0x85, 0x21,        //   Report ID (33)
	0x09, 0x21,        //   Usage (0x21)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x30,        //   Report ID (48)
	0x09, 0x30,        //   Usage (0x30)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x31,        //   Report ID (49)
	0x09, 0x31,        //   Usage (0x31)
	0x75, 0x08,        //   Report Size (8)
	0x96, 0x69, 0x01,  //   Report Count (361)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x32,        //   Report ID (50)
	0x09, 0x32,        //   Usage (0x32)
	0x75, 0x08,        //   Report Size (8)
	0x96, 0x69, 0x01,  //   Report Count (361)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x33,        //   Report ID (51)
	0x09, 0x33,        //   Usage (0x33)
	0x75, 0x08,        //   Report Size (8)
	0x96, 0x69, 0x01,  //   Report Count (361)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x3F,        //   Report ID (63)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x10,        //   Usage Maximum (0x10)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x10,        //   Report Count (16)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x39,        //   Usage (Hat switch)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x07,        //   Logical Maximum (7)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x05, 0x09,        //   Usage Page (Button)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,        //   Usage (X)
	0x09, 0x31,        //   Usage (Y)
	0x09, 0x33,        //   Usage (Rx)
	0x09, 0x34,        //   Usage (Ry)
	0x16, 0x00, 0x00,  //   Logical Minimum (0)
	0x27, 0xFF, 0xFF, 0x00, 0x00,  //   Logical Maximum (65534)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x04,        //   Report Count (4)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x01, 0xFF,  //   Usage Page (Vendor Defined 0xFF01)
	0x85, 0x01,        //   Report ID (1)
	0x09, 0x01,        //   Usage (0x01)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0x10,        //   Report ID (16)
	0x09, 0x10,        //   Usage (0x10)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0x11,        //   Report ID (17)
	0x09, 0x11,        //   Usage (0x11)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0x12,        //   Report ID (18)
	0x09, 0x12,        //   Usage (0x12)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x30,        //   Report Count (48)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              // End Collection

// 170 bytes

