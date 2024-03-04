/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/** \ingroup group_class
 *  \defgroup ClassDriver_HID Human Interface Device (HID)
 *  @{ */

#ifndef _USB_HID_TYPEDEFH_
#define _USB_HID_TYPEDEFH_

#include "emf_typedef.h" 

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// Common Definitions
//--------------------------------------------------------------------+
/** \defgroup ClassDriver_HID_Common Common Definitions
 *  @{ */

/// HID Subclass
typedef enum
{
  HID_SUBCLASS_NONE = 0, ///< No Subclass
  HID_SUBCLASS_BOOT = 1  ///< Boot Interface Subclass
}hid_subclass_enum_t;

/// HID Interface Protocol
typedef enum
{
  HID_ITF_PROTOCOL_NONE     = 0, ///< None
  HID_ITF_PROTOCOL_KEYBOARD = 1, ///< Keyboard
  HID_ITF_PROTOCOL_MOUSE    = 2  ///< Mouse
}hid_interface_protocol_enum_t;

/// HID Descriptor Type
typedef enum
{
  HID_DESC_TYPE_HID      = 0x21, ///< HID Descriptor
  HID_DESC_TYPE_REPORT   = 0x22, ///< Report Descriptor
  HID_DESC_TYPE_PHYSICAL = 0x23  ///< Physical Descriptor
}hid_descriptor_enum_t;

/// HID Request Report Type
typedef enum
{
  HID_REPORT_TYPE_INVALID = 0,  //other
  HID_REPORT_TYPE_INPUT,        //< Input
  HID_REPORT_TYPE_OUTPUT,       //< Output
  HID_REPORT_TYPE_FEATURE       //< Feature
}hid_report_type_t;

//BT EDR HID CONTROL
typedef enum
{
  EDR_HID_REQ_TYPE_HANDSHAKE       = 0x00,     //device 
  EDR_HID_REQ_TYPE_CONTROL         = 0x10,     //device&host
  EDR_HID_REQ_TYPE_GET_REPORT      = 0x40,     //host
  EDR_HID_REQ_TYPE_SET_REPORT      = 0x50,     //host
  EDR_HID_REQ_TYPE_GET_PROTOCOL    = 0x60,     //host
  EDR_HID_REQ_TYPE_SET_PROTOCOL    = 0x70,     //host
  EDR_HID_REQ_TYPE_GET_IDLE        = 0x80,     //host
  EDR_HID_REQ_TYPE_SET_IDLE        = 0x90,     //host
  EDR_HID_REQ_TYPE_DATA            = 0xA0,     //device&host
  EDR_HID_REQ_TYPE_DATC            = 0xB0,     //device&host    
}edr_hid_request_type_t;

// HID Class Specific Control Request
typedef enum
{
  HID_REQ_CONTROL_GET_REPORT   = 0x01, ///< Get Report
  HID_REQ_CONTROL_GET_IDLE     = 0x02, ///< Get Idle
  HID_REQ_CONTROL_GET_PROTOCOL = 0x03, ///< Get Protocol
  HID_REQ_CONTROL_SET_REPORT   = 0x09, ///< Set Report
  HID_REQ_CONTROL_SET_IDLE     = 0x0a, ///< Set Idle
  HID_REQ_CONTROL_SET_PROTOCOL = 0x0b  ///< Set Protocol
}hid_request_enum_t;


/// HID Local Code
typedef enum
{
  HID_LOCAL_NotSupported = 0   , ///< NotSupported
  HID_LOCAL_Arabic             , ///< Arabic
  HID_LOCAL_Belgian            , ///< Belgian
  HID_LOCAL_Canadian_Bilingual , ///< Canadian_Bilingual
  HID_LOCAL_Canadian_French    , ///< Canadian_French
  HID_LOCAL_Czech_Republic     , ///< Czech_Republic
  HID_LOCAL_Danish             , ///< Danish
  HID_LOCAL_Finnish            , ///< Finnish
  HID_LOCAL_French             , ///< French
  HID_LOCAL_German             , ///< German
  HID_LOCAL_Greek              , ///< Greek
  HID_LOCAL_Hebrew             , ///< Hebrew
  HID_LOCAL_Hungary            , ///< Hungary
  HID_LOCAL_International      , ///< International
  HID_LOCAL_Italian            , ///< Italian
  HID_LOCAL_Japan_Katakana     , ///< Japan_Katakana
  HID_LOCAL_Korean             , ///< Korean
  HID_LOCAL_Latin_American     , ///< Latin_American
  HID_LOCAL_Netherlands_Dutch  , ///< Netherlands/Dutch
  HID_LOCAL_Norwegian          , ///< Norwegian
  HID_LOCAL_Persian_Farsi      , ///< Persian (Farsi)
  HID_LOCAL_Poland             , ///< Poland
  HID_LOCAL_Portuguese         , ///< Portuguese
  HID_LOCAL_Russia             , ///< Russia
  HID_LOCAL_Slovakia           , ///< Slovakia
  HID_LOCAL_Spanish            , ///< Spanish
  HID_LOCAL_Swedish            , ///< Swedish
  HID_LOCAL_Swiss_French       , ///< Swiss/French
  HID_LOCAL_Swiss_German       , ///< Swiss/German
  HID_LOCAL_Switzerland        , ///< Switzerland
  HID_LOCAL_Taiwan             , ///< Taiwan
  HID_LOCAL_Turkish_Q          , ///< Turkish-Q
  HID_LOCAL_UK                 , ///< UK
  HID_LOCAL_US                 , ///< US
  HID_LOCAL_Yugoslavia         , ///< Yugoslavia
  HID_LOCAL_Turkish_F            ///< Turkish-F
} hid_local_enum_t;

// HID protocol value used by GetProtocol / SetProtocol
typedef enum
{
  HID_PROTOCOL_BOOT = 0,
  HID_PROTOCOL_REPORT = 1
} hid_protocol_mode_enum_t;


#ifndef pack
#pragma pack(1)
#endif
/// USB HID Descriptor
typedef struct {
  uint8_t  bLength;         /**< Numeric expression that is the total size of the HID descriptor */
  uint8_t  bDescriptorType; /**< Constant name specifying type of HID descriptor. */

  uint16_t bcdHID;          /**< Numeric expression identifying the HID Class Specification release */
  uint8_t  bCountryCode;    /**< Numeric expression identifying country code of the localized hardware.  */
  uint8_t  bNumDescriptors; /**< Numeric expression specifying the number of class descriptors */

  uint8_t  bReportType;     /**< Type of HID class report. */
  uint16_t wReportLength;   /**< the total size of the Report descriptor. */
} usb_hid_descriptor_hid_t;

#ifndef pack
#pragma pack()
#endif



#ifdef __cplusplus
 }
#endif

#endif /* _USB_HID_H__ */
