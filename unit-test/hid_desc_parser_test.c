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
//@ref: https://github.com/MicrochipTech/mla_usb

#include  "hw_config.h"
#if HIDD_SUPPORT || HIDH_SUPPORT
#include  "api/hid/hid_desc_parser.h"
#include  "utils/emf_utils.h"
#include "hid/hid_dev_desc.h"
#include "api/api_transport.h"
#include  "api/usb/host/usbh_hid_km.h"
#include "app_km.h"

#include  "api/api_log.h"





uint8c_t keyboard_test_desc[] = {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
        0x85, 0x01,        //   Report ID (1)
        0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
        0x19, 0xE0,        //   Usage Minimum (0xE0)
        0x29, 0xE7,        //   Usage Maximum (0xE7)
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0x01,        //   Logical Maximum (1)
        0x75, 0x01,        //   Report Size (1)
        0x95, 0x08,        //   Report Count (8)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x19, 0x00,        //   Usage Minimum (0x00)
        0x29, 0x97,        //   Usage Maximum (0x97)
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0x01,        //   Logical Maximum (1)
        0x75, 0x01,        //   Report Size (1)
        0x95, 0x98,        //   Report Count (-104)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x08,        //   Usage Page (LEDs)
        0x19, 0x01,        //   Usage Minimum (Num Lock)
        0x29, 0x03,        //   Usage Maximum (Scroll Lock)
        0x25, 0x01,        //   Logical Maximum (1)
        0x75, 0x01,        //   Report Size (1)
        0x95, 0x03,        //   Report Count (3)
        0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0x75, 0x05,        //   Report Size (5)
        0x95, 0x01,        //   Report Count (1)
        0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
        0x85, 0x02,        //   Report ID (2)
        0x05, 0x0C,        //   Usage Page (Consumer)
        0x19, 0x00,        //   Usage Minimum (Unassigned)
        0x2A, 0xFF, 0x0F,  //   Usage Maximum (0x0FFF)
        0x15, 0x00,        //   Logical Minimum (0)
        0x26, 0xFF, 0x0F,  //   Logical Maximum (4095)
        0x75, 0x10,        //   Report Size (16)
        0x95, 0x02,        //   Report Count (2)
        0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0xC0, 0xFF,  // Usage Page (Vendor Defined 0xFFC0)
    0x09, 0x02,        // Usage (0x02)
    0xA1, 0x01,        // Collection (Application)
        0x85, 0x03,        //   Report ID (3)
        0x15, 0x00,        //   Logical Minimum (0)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x95, 0x3F,        //   Report Count (63)
        0x75, 0x08,        //   Report Size (8)
        0x09, 0x02,        //   Usage (0x02)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x06, 0xC2, 0xFF,  // Usage Page (Vendor Defined 0xFFC2)
    0x09, 0x03,        // Usage (0x03)
    0xA1, 0x01,        // Collection (Application)
        0x85, 0x0E,        //   Report ID (14)
        0x15, 0x00,        //   Logical Minimum (0)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x95, 0x3F,        //   Report Count (63)
        0x75, 0x08,        //   Report Size (8)
        0x09, 0x03,        //   Usage (0x03)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
        0x85, 0x05,        //   Report ID (5)
        0x09, 0x01,        //   Usage (Pointer)
        0xA1, 0x00,        //   Collection (Physical)
            0x05, 0x09,        //     Usage Page (Button)
            0x19, 0x01,        //     Usage Minimum (0x01)
            0x29, 0x20,        //     Usage Maximum (0x20)
            0x15, 0x00,        //     Logical Minimum (0)
            0x25, 0x01,        //     Logical Maximum (1)
            0x95, 0x20,        //     Report Count (32)
            0x75, 0x01,        //     Report Size (1)
            0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
            0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
            0x09, 0x30,        //     Usage (X)
            0x09, 0x31,        //     Usage (Y)
            0x16, 0x01, 0x00,  //     Logical Minimum (1)
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
    // 192 bytes
};

void hid_desc_parser_test(void)
{
    error_t err;
    hid_desc_info_t hid_info;
    uint8_t i;
    uint8_t *pdesc;
    uint16_t desc_len;

    desc_len = get_hid_desc_map(TR_USBD, HID_TYPE_KB ,&pdesc);
    // desc_len = sizeof(keyboard_test_desc);
    // pdesc = (uint8_t*)keyboard_test_desc;
    
    err = hid_desc_parse_report(&hid_info, pdesc, desc_len);
    if(0 == err){
        km_items_t item;
        memset(&item, 0, sizeof(item));
  
        for(i=0; i<hid_info.reportItems; i++){
            if(0 == item.items.kb.kb_fun.report_length){
                if(hid_find_items(&hid_info,i,HID_REPORT_TYPE_INPUT,0X07, 0XE0,&item.items.kb.kb_fun)){
                    logd("usbh kb_fun:");
                    hid_items_dump(&item.items.kb.kb_fun);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.items.kb.kb_normal.report_length){
                if(hid_find_items(&hid_info,i,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.items.kb.kb_normal)){
                    logd("usbh kb_normal:");
                    hid_items_dump(&item.items.kb.kb_normal);
                    err = ERROR_SUCCESS;
                }
            }else if(0 == item.items.kb.kb2_normal.report_length){
                if(hid_find_items(&hid_info,i,HID_REPORT_TYPE_INPUT,0X07, KB_A, &item.items.kb.kb2_normal)){
                    logd("usbh kb2_normal:");
                    hid_items_dump(&item.items.kb.kb2_normal);
                    err = ERROR_SUCCESS;
                }
            }

            if(0 == item.items.kb.led.report_length){
                if(hid_find_items(&hid_info,i,HID_REPORT_TYPE_OUTPUT,0X07, KB_A, &item.items.kb.led)){
                    logd("usbh kb led:");
                    hid_items_dump(&item.items.kb.led);
                }
            }
        }

        hid_desc_info_free(&hid_info);
    }else{
         logd("hid_desc_test err=%d\n",err);
    }
}
#endif
