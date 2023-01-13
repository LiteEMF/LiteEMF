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
	0x85, CONSUMER_REPORT_ID,        //   Report ID (3)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x10,        //   Report Count (16)
	0x09, 0x30,        //   Usage (Power)
	0x09, 0x40,        //   Usage (Menu)
	0x09, 0xB5,        //   Usage (Scan Next Track)
	0x09, 0xB6,        //   Usage (Scan Previous Track)
	0x09, 0xB8,        //   Usage (Eject)
	0x09, 0xCD,        //   Usage (Play/Pause)
	0x09, 0xE2,        //   Usage (Mute)
	0x09, 0xE9,        //   Usage (Volume Increment)
	0x09, 0xEA,        //   Usage (Volume Decrement)
	0x0A, 0x23, 0x02,  //   Usage (AC Home)
	0x0A, 0x24, 0x02,  //   Usage (AC Back)
	0x0A, 0x25, 0x02,  //   Usage (AC Forward)
	0x0A, 0x26, 0x02,  //   Usage (AC Stop)
	0x0A, 0x27, 0x02,  //   Usage (AC Refresh)
	0x0A, 0x28, 0x02,  //   Usage (AC Previous Link)
	0x0A, 0x29, 0x02,  //   Usage (AC Next Link)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              // End Collection

// 58 bytes
