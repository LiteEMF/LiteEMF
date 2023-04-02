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

0x05, 0x0D,        //   Usage Page (Digitizer)
0x09, 0x20,        //   Usage (Stylus)
0xA1, 0x02,        //   Collection (Logical)
	0x09, 0x42,        //     Usage (Tip Switch)
	0x15, 0x00,        //     Logical Minimum (0)
	0x25, 0x01,        //     Logical Maximum (1)
	0x75, 0x01,        //     Report Size (1)
	0x95, 0x01,        //     Report Count (1)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x32,        //     Usage (In Range)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x02,        //     Report Size (2)
	0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x04,        //     Report Size (4)
	0x25, 0x0F,        //     Logical Maximum (15)
	0x09, 0x51,        //     Usage (0x51)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
	0x26, TOUCH_X_LOGICAL_MAX&0xff, TOUCH_X_LOGICAL_MAX>>8,  //     Logical Maximum (257)
	0x75, 0x10,        //     Report Size (18)
	0x55, 0x00,        //     Unit Exponent (-2)
	0x65, 0x00,        //     Unit (System: English Linear, Length: Inch)
	0x09, 0x30,        //     Usage (X)
	0x35, 0x00,        //     Physical Minimum (0)
	0x46, 0x00, 0x00,  //     Physical Maximum (0)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x26, TOUCH_Y_LOGICAL_MAX&0xff, TOUCH_Y_LOGICAL_MAX>>8,  //     Logical Maximum (257)
	0x09, 0x31,        //     Usage (Y)
	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
	

