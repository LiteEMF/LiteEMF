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

0x06, 0x80, 0xff,              // USAGE_PAGE (Generic Desktop)
0x09, 0x01,                    // USAGE (Vendor Usage 1)
0xa1, 0x01,                    // COLLECTION (Application)
	// 0X85, VENDOR_REPORT_ID,	//NOT ADD REPORT ID
	0x09, 0x02,                    //   USAGE (Vendor Usage 2)
	0xa1, 0x00,                    //   COLLECTION (Physical)
		0x09, 0x03,                    //     USAGE (Vendor Usage 1)
		0x15, 0x00,                    //     LOGICAL_MINIMUM (-128)
		0x25, 0xff,                    //     LOGICAL_MAXIMUM (127)

		0x75, 0x08,                    //     REPORT_SIZE (8)
		0x95, 0x40,                    //     REPORT_COUNT (26)
		0x81, 0x02,                    //     INPUT (Data,Var,Abs)

		0x09, 0x05,                    //     USAGE (Vendor Usage 1)
		0x91, 0x02,                    //     OUTPUT (Data,Var,Abs)
	0xc0,                          //   END_COLLECTION
0xc0                           // END_COLLECTION