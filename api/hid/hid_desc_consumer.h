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

0x05, 0x0C,        // Usage Page (Consumer)
0x09, 0x01,        // Usage (Consumer Control)
0xA1, 0x01,        // Collection (Application)
	0x85, CONSUMER_REPORT_ID,//   Report ID (3)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0x8C, 0x02,  //   Logical Maximum (652)
	0x19, 0x00,        //   Usage Minimum (Unassigned)
	0x2A, 0x8C, 0x02,  //   Usage Maximum (AC Send)
	0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection


