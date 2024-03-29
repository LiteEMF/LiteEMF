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

//ps3
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x04,        // Usage (Joystick)
0xA1, 0x01,        // Collection (Application)
0xA1, 0x02,        //   Collection (Logical)
0x85, 0x01,        //     Report ID (1)
0x75, 0x08,        //     Report Size (8)
0x95, 0x01,        //     Report Count (1)
0x15, 0x00,        //     Logical Minimum (0)
0x26, 0xFF, 0x00,  //     Logical Maximum (255)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x75, 0x01,        //     Report Size (1)
0x95, 0x13,        //     Report Count (19)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x35, 0x00,        //     Physical Minimum (0)
0x45, 0x01,        //     Physical Maximum (1)
0x05, 0x09,        //     Usage Page (Button)
0x19, 0x01,        //     Usage Minimum (0x01)
0x29, 0x13,        //     Usage Maximum (0x13)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x75, 0x01,        //     Report Size (1)
0x95, 0x0D,        //     Report Count (13)
0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x15, 0x00,        //     Logical Minimum (0)
0x26, 0xFF, 0x00,  //     Logical Maximum (255)
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x09, 0x01,        //     Usage (Pointer)
0xA1, 0x00,        //     Collection (Physical)
0x75, 0x08,        //       Report Size (8)
0x95, 0x04,        //       Report Count (4)
0x35, 0x00,        //       Physical Minimum (0)
0x46, 0xFF, 0x00,  //       Physical Maximum (255)
0x09, 0x30,        //       Usage (X)
0x09, 0x31,        //       Usage (Y)
0x09, 0x32,        //       Usage (Z)
0x09, 0x35,        //       Usage (Rz)
0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //     End Collection
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x75, 0x08,        //     Report Size (8)
0x95, 0x27,        //     Report Count (39)
0x09, 0x01,        //     Usage (Pointer)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x75, 0x08,        //     Report Size (8)
0x95, 0x30,        //     Report Count (48)
0x09, 0x01,        //     Usage (Pointer)
0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x75, 0x08,        //     Report Size (8)
0x95, 0x30,        //     Report Count (48)
0x09, 0x01,        //     Usage (Pointer)
0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //   End Collection
0xA1, 0x02,        //   Collection (Logical)
0x85, 0x02,        //     Report ID (2)
0x75, 0x08,        //     Report Size (8)
0x95, 0x30,        //     Report Count (48)
0x09, 0x01,        //     Usage (Pointer)
0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //   End Collection
0xA1, 0x02,        //   Collection (Logical)
0x85, 0xEE,        //     Report ID (-18)
0x75, 0x08,        //     Report Size (8)
0x95, 0x30,        //     Report Count (48)
0x09, 0x01,        //     Usage (Pointer)
0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //   End Collection
0xA1, 0x02,        //   Collection (Logical)
0x85, 0xEF,        //     Report ID (-17)
0x75, 0x08,        //     Report Size (8)
0x95, 0x30,        //     Report Count (48)
0x09, 0x01,        //     Usage (Pointer)
0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              //   End Collection
0xC0,              // End Collection

// 148 bytes
