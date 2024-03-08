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

0x05, 0x0D,        // Usage Page (Digitizer)
0x09, 0x04,        // Usage (Touch Screen)
0xA1, 0x01,        // Collection (Application)

	0x85, TOUCH_REPORT_ID,        //   Report ID (1)
	#include "hid_desc_touch.h"
		
	#if (MT_CONTACT_MAX > 1)		
	#include "hid_desc_touch.h"
	#endif
	#if (MT_CONTACT_MAX > 2)		
	#include "hid_desc_touch.h"
	#endif
	#if (MT_CONTACT_MAX > 3)
	#include "hid_desc_touch.h"
	#endif
	#if (MT_CONTACT_MAX > 4)
	#include "hid_desc_touch.h"
	#endif

	0x05, 0x0D,        //   Usage Page (Digitizer)
	0x95, 0x01,        //   Report Count (1)
	0x75, 0x08,        //   Report Size (8)
	0x09, 0x55,        //   Usage (0x55)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              // End Collection
