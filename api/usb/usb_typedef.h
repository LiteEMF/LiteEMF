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
// @ref: https://github.com/hathach/tinyusb

#ifndef _usb_typedef_h
#define _usb_typedef_h
#include "emf_typedef.h" 
#include "api/hid/hid_typedef.h"


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif

// DETACHED （断开）状态：表示 USB 设备未连接到主机。该状态可以转换到 ATTACHED（连接）状态。
// ATTACHED （连接）状态：表示 USB 设备已连接到主机，但尚未被枚举。该状态可以转换到 POWERED（已上电）状态。
// POWERED （已上电）状态：表示 USB 设备已经上电可以开始枚举。该状态可以转换到 DEFAULT（默认）状态。
// DEFAULT （默认）状态：表示 USB 设备并准备好进行数据传输,默认地址0状态。该状态可以转换到 ADDRESSING（寻址）状态。
// ADDRESSING （寻址）状态：表示主机已经为设备分配了地址。该状态可以转换到 CONFIGURED（已配置）状态。
// CONFIGURED （已配置）状态：表示 USB 设备已被配置并正在进行数据传输。该状态可以转换到 SUSPENDED（已挂起）状态。
// SUSPENDED （已挂起）状态：表示 USB 设备已被暂停以节省电源，但仍然保持连接。该状态可以转换到 CONFIGURED（已配置）或 DETACHED（断开）状态。
typedef enum {
	TUSB_STA_DETACHED = 0,		//断开链接		usbh_disconnect(),或枚举失败设置
	TUSB_STA_ATTACHED,			//连接USB接入	usbh_det_event(true) 中设置

	TUSB_STA_POWERED,			//上电
	TUSB_STA_DEFAULT,			//默认状态		usbh_reset() 中设置,设备在可寻址的默认地址0状态
	TUSB_STA_ADDRESSING,		//地址状态		CtrlSetUsbAddress() 中设置

	TUSB_STA_CONFIGURED,		//配置状态 		枚举成功正确配置后设置
	TUSB_STA_SUSPENDED,			//挂起状态		未使用
}usb_state_t;


/// defined base on EHCI specs value for Endpoint Speed
typedef enum
{
	TUSB_SPEED_FULL = 0,
	TUSB_SPEED_LOW  = 1,
	TUSB_SPEED_HIGH = 2,
	TUSB_SPEED_INVALID = 0xff,
}usb_speed_t;

// defined base on USB Specs Endpoint's bmAttributes
#ifndef TUSB_ENDP_TYPE_CTRL
typedef enum
{
	TUSB_ENDP_TYPE_CTRL = 0,
	TUSB_ENDP_TYPE_ISOCH,
	TUSB_ENDP_TYPE_BULK,	//BULK 端点是双buf,需要两倍空间,如果和BULK in 不同时使用(比如U盘协议),可以优化共用BULK in buf
	TUSB_ENDP_TYPE_INTER
}usb_endp_type_t;
#endif

#define  TUSB_DIR_POST  		7
#define  TUSB_DIR_MASK  		0x80
#define  TUSB_DIR_IN_MASK  	0x80
typedef enum
{
	TUSB_DIR_OUT = 0,
	TUSB_DIR_IN  = 0x01,
}usb_dir_t;


/// USB Descriptor Types
typedef enum
{
	TUSB_DESC_DEVICE                = 0x01,
	TUSB_DESC_CONFIGURATION         = 0x02,
	TUSB_DESC_STRING                = 0x03,
	TUSB_DESC_INTERFACE             = 0x04,
	TUSB_DESC_ENDPOINT              = 0x05,
	TUSB_DESC_DEVICE_QUALIFIER      = 0x06,
	TUSB_DESC_SPEED_CONFIG    	   = 0x07,
	TUSB_DESC_INTERFACE_POWER       = 0x08,
	TUSB_DESC_OTG                   = 0x09,
	TUSB_DESC_DEBUG                 = 0x0A,
	TUSB_DESC_INTERFACE_ASSOCIATION = 0x0B,

	TUSB_DESC_BOS                   = 0x0F,		//Binary Device Object Store Descriptor
	TUSB_DESC_DEVICE_CAPABILITY     = 0x10,

	TUSB_DESC_FUNCTIONAL            = 0x21,

	// Class Specific Descriptor
	TUSB_DESC_CS_DEVICE             = 0x21,
	TUSB_DESC_CS_CONFIGURATION      = 0x22,
	TUSB_DESC_CS_STRING             = 0x23,
	TUSB_DESC_CS_INTERFACE          = 0x24,
	TUSB_DESC_CS_ENDPOINT           = 0x25,

	TUSB_DESC_SUPERSPEED_ENDPOINT_COMPANION     = 0x30,
	TUSB_DESC_SUPERSPEED_ISO_ENDPOINT_COMPANION = 0x31
}usb_desc_type_t;

/* USB standard device request code */
typedef enum
{
	TUSB_REQ_GET_STATUS        = 0  ,
	TUSB_REQ_CLEAR_FEATURE     = 1  ,
	TUSB_REQ_RESERVED          = 2  ,
	TUSB_REQ_SET_FEATURE       = 3  ,
	TUSB_REQ_RESERVED2         = 4  ,
	TUSB_REQ_SET_ADDRESS       = 5  ,
	TUSB_REQ_GET_DESCRIPTOR    = 6  ,
	TUSB_REQ_SET_DESCRIPTOR    = 7  ,
	TUSB_REQ_GET_CONFIGURATION = 8  ,
	TUSB_REQ_SET_CONFIGURATION = 9  ,
	TUSB_REQ_GET_INTERFACE     = 10 ,
	TUSB_REQ_SET_INTERFACE     = 11 ,
	TUSB_REQ_SYNCH_FRAME       = 12
}usb_request_code_t;

typedef enum
{
	TUSB_REQ_FEATURE_EDPT_HALT     = 0,
	TUSB_REQ_FEATURE_REMOTE_WAKEUP = 1,
	TUSB_REQ_FEATURE_TEST_MODE     = 2
}usb_request_feature_selector_t;

typedef enum
{
	TUSB_REQ_TYPE_STANDARD = 0,
	TUSB_REQ_TYPE_CLASS,
	TUSB_REQ_TYPE_VENDOR,
	TUSB_REQ_TYPE_INVALID
} usb_request_type_t;

typedef enum
{
	TUSB_REQ_RCPT_DEVICE = 0,
	TUSB_REQ_RCPT_INTERFACE,
	TUSB_REQ_RCPT_ENDPOINT,
	TUSB_REQ_RCPT_OTHER
} usb_request_recipient_t;



// @ref:https://www.usb.org/defined-class-codes
typedef enum
{
	TUSB_CLASS_NONE          	   	= 0    ,
	TUSB_CLASS_AUDIO                	= 1    ,
	TUSB_CLASS_CDC                  	= 2    ,
	TUSB_CLASS_HID                  	= 3    ,
	TUSB_CLASS_MONITOR	           	= 4    ,
	TUSB_CLASS_PHYSICAL             	= 5    ,
	TUSB_CLASS_IMAGE                	= 6    ,
	TUSB_CLASS_PRINTER              	= 7    ,
	TUSB_CLASS_MSD                  	= 8    ,			//mass storage devices
	TUSB_CLASS_HUB                  	= 9    ,
	TUSB_CLASS_CDC_DATA             	= 10   ,
	TUSB_CLASS_SMART_CARD           	= 11   ,
	TUSB_CLASS_RESERVED_12          	= 12   ,
	TUSB_CLASS_CONTENT_SECURITY     	= 13   ,
	TUSB_CLASS_VIDEO                	= 14   ,
	TUSB_CLASS_PERSONAL_HEALTHCARE  	= 15   ,			//保健
	TUSB_CLASS_AUDIO_VIDEO          	= 16   ,

	TUSB_CLASS_DIAGNOSTIC           	= 0xDC ,			//诊断设备
	TUSB_CLASS_WIRELESS_CONTROLLER  	= 0xE0 ,			//无线控制器
	TUSB_CLASS_MISC                 	= 0xEF ,			//杂项
	TUSB_CLASS_APP_SPECIFIC 		   	= 0xFE ,
	TUSB_CLASS_VENDOR      		   	= 0xFF
}usb_class_code_t;


typedef enum
{
	TUSB_CAPABILITY_WIRELESS_USB               = 0x01,
	TUSB_CAPABILITY_USB20_EXTENSION            = 0x02,
	TUSB_CAPABILITY_SUPERSPEED_USB             = 0x03,
	TUSB_CAPABILITY_CONTAINER_id               = 0x04,
	TUSB_CAPABILITY_PLATFORM                   = 0x05,
	TUSB_CAPABILITY_POWER_DELIVERY             = 0x06,
	TUSB_CAPABILITY_BATTERY_INFO               = 0x07,
	TUSB_CAPABILITY_PD_CONSUMER_PORT           = 0x08,
	TUSB_CAPABILITY_PD_PROVIDER_PORT           = 0x09,
	TUSB_CAPABILITY_SUPERSPEED_PLUS            = 0x0A,
	TUSB_CAPABILITY_PRECESION_TIME_MEASUREMENT = 0x0B,
	TUSB_CAPABILITY_WIRELESS_USB_EXT           = 0x0C,
	TUSB_CAPABILITY_BILLBOARD                  = 0x0D,
	TUSB_CAPABILITY_AUTHENTICATION             = 0x0E,
	TUSB_CAPABILITY_BILLBOARD_EX               = 0x0F,
	TUSB_CAPABILITY_CONFIGURATION_SUMMARY      = 0x10
}usb_capability_type_t;


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	uint8_t		addr:4;
	uint8_t		type:2;
	uint8_t		sync:1;			//端点同步翻转标志位
	uint8_t		dir :1;
}usb_ep_t;

typedef struct
{
	uint8_t		addr:4;
	uint8_t		type:2;
	uint8_t		sync:1;			//端点同步翻转标志位
	uint8_t		dir :1;

	uint8_t		interval;
	uint16_t	mtu;
}usb_endp_t;

typedef struct
{
	uint8_t  if_num;				//interface 
	uint8_t  if_alt;				//bAlternateSetting
	uint8_t  num_endp;
	uint8_t  if_cls;
	uint8_t  if_sub_cls;
	uint8_t  if_pro;
}usb_inf_t;


#ifndef pack
#pragma pack(1)
#endif
//--------------------------------------------------------------------+
// USB Descriptors
//--------------------------------------------------------------------+

// Start of all packed definitions for compiler without per-type packed

// USB Device Descriptor
typedef struct {
	uint8_t  bLength            ; ///< Size of this descriptor in bytes.
	uint8_t  bDescriptorType    ; ///< DEVICE Descriptor Type.
	uint16_t bcdUSB             ; ///< BUSB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H). This field identifies the release of the USB Specification with which the device and its descriptors are compliant.

	uint8_t  bDeviceClass       ; ///< Class code (assigned by the USB-IF). \li If this field is reset to zero, each interface within a configuration specifies its own class information and the various interfaces operate independently. \li If this field is set to a value between 1 and FEH, the device supports different class specifications on different interfaces and the interfaces may not operate independently. This value identifies the class definition used for the aggregate interfaces. \li If this field is set to FFH, the device class is vendor-specific.
	uint8_t  bDeviceSubClass    ; ///< Subclass code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass field. \li If the bDeviceClass field is reset to zero, this field must also be reset to zero. \li If the bDeviceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
	uint8_t  bDeviceProtocol    ; ///< Protocol code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass and the bDeviceSubClass fields. If a device supports class-specific protocols on a device basis as opposed to an interface basis, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use class-specific protocols on a device basis. However, it may use classspecific protocols on an interface basis. \li If this field is set to FFH, the device uses a vendor-specific protocol on a device basis.
	uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). For HS devices is fixed to 64.

	uint16_t idVendor           ; ///< Vendor ID (assigned by the USB-IF).
	uint16_t idProduct          ; ///< Product ID (assigned by the manufacturer).
	uint16_t bcdDevice          ; ///< Device release number in binary-coded decimal.
	uint8_t  iManufacturer      ; ///< Index of string descriptor describing manufacturer.
	uint8_t  iProduct           ; ///< Index of string descriptor describing product.
	uint8_t  iSerialNumber      ; ///< Index of string descriptor describing the device's serial number.

	uint8_t  bNumConfigurations ; ///< Number of possible configurations.
} usb_desc_device_t;

// USB Binary Device Object Store (BOS) Descriptor
typedef struct {
	uint8_t  bLength         ; ///< Size of this descriptor in bytes
	uint8_t  bDescriptorType ; ///< CONFIGURATION Descriptor Type
	uint16_t wTotalLength    ; ///< Total length of data returned for this descriptor
	uint8_t  bNumDeviceCaps  ; ///< Number of device capability descriptors in the BOS
} usb_desc_bos_t;				//TUSB_DESC_BOS
// USB Binary Device Object Store (BOS)
typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType ;
  uint8_t bDevCapabilityType;
  uint8_t bReserved;
  uint8_t PlatformCapabilityUUID[16];
  uint8_t CapabilityData[1];			//TODO
} usb_desc_bos_platform_t;

/// USB Configuration Descriptor
typedef struct {
	uint8_t  bLength             ; ///< Size of this descriptor in bytes
	uint8_t  bDescriptorType     ; ///< CONFIGURATION Descriptor Type
	uint16_t wTotalLength        ; ///< Total length of data returned for this configuration. Includes the combined length of all descriptors (configuration, interface, endpoint, and class- or vendor-specific) returned for this configuration.

	uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this configuration
	uint8_t  bConfigurationValue ; ///< Value to use as an argument to the SetConfiguration() request to select this configuration.
	uint8_t  iConfiguration      ; ///< Index of string descriptor describing this configuration
	uint8_t  bmAttributes        ; ///< Configuration characteristics \n D7: Reserved (set to one)\n D6: Self-powered \n D5: Remote Wakeup \n D4...0: Reserved (reset to zero) \n D7 is reserved and must be set to one for historical reasons. \n A device configuration that uses power from the bus and a local source reports a non-zero value in bMaxPower to indicate the amount of bus power required and sets D6. The actual power source at runtime may be determined using the GetStatus(DEVICE) request (see USB 2.0 spec Section 9.4.5). \n If a device configuration supports remote wakeup, D5 is set to one.
	uint8_t  bMaxPower           ; ///< Maximum power consumption of the USB device from the bus in this specific configuration when the device is fully operational. Expressed in 2 mA units (i.e., 50 = 100 mA).
} usb_desc_configuration_t;

/// USB Interface Descriptor
typedef struct {
	uint8_t  bLength            ; ///< Size of this descriptor in bytes
	uint8_t  bDescriptorType    ; ///< INTERFACE Descriptor Type

	uint8_t  bInterfaceNumber   ; ///< Number of this interface. Zero-based value identifying the index in the array of concurrent interfaces supported by this configuration.
	uint8_t  bAlternateSetting  ; ///< Value used to select this alternate setting for the interface identified in the prior field
	uint8_t  bNumEndpoints      ; ///< Number of endpoints used by this interface (excluding endpoint zero). If this value is zero, this interface only uses the Default Control Pipe.
	uint8_t  bInterfaceClass    ; ///< Class code (assigned by the USB-IF). \li A value of zero is reserved for future standardization. \li If this field is set to FFH, the interface class is vendor-specific. \li All other values are reserved for assignment by the USB-IF.
	uint8_t  bInterfaceSubClass ; ///< Subclass code (assigned by the USB-IF). \n These codes are qualified by the value of the bInterfaceClass field. \li If the bInterfaceClass field is reset to zero, this field must also be reset to zero. \li If the bInterfaceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
	uint8_t  bInterfaceProtocol ; ///< Protocol code (assigned by the USB). \n These codes are qualified by the value of the bInterfaceClass and the bInterfaceSubClass fields. If an interface supports class-specific requests, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use a class-specific protocol on this interface. \li If this field is set to FFH, the device uses a vendor-specific protocol for this interface.
	uint8_t  iInterface         ; ///< Index of string descriptor describing this interface
} usb_desc_interface_t;

/// USB Endpoint Descriptor
typedef struct {
	uint8_t  bLength          ; // Size of this descriptor in bytes
	uint8_t  bDescriptorType  ; // ENDPOINT Descriptor Type
	uint8_t  bEndpointAddress ; // The address of the endpoint

	struct {
		uint8_t xfer  : 2;        // Control, ISO, Bulk, Interrupt
		uint8_t sync  : 2;        // None, Asynchronous, Adaptive, Synchronous
		uint8_t usage : 2;        // Data, Feedback, Implicit feedback
		uint8_t       : 2;
	} bmAttributes;

	uint16_t wMaxPacketSize   ; // Bit 10..0 : max packet size, bit 12..11 additional transaction per highspeed micro-frame
	uint8_t  bInterval        ; // Polling interval, in frames or microframes depending on the operating speed
} usb_desc_endpoint_t;



/// USB Other Speed Configuration Descriptor
typedef struct {
	uint8_t  bLength             ; ///< Size of descriptor
	uint8_t  bDescriptorType     ; ///< Other_speed_Configuration Type
	uint16_t wTotalLength        ; ///< Total length of data returned

	uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this speed configuration
	uint8_t  bConfigurationValue ; ///< Value to use to select configuration
	uint8_t  iConfiguration      ; ///< Index of string descriptor
	uint8_t  bmAttributes        ; ///< Same as Configuration descriptor
	uint8_t  bMaxPower           ; ///< Same as Configuration descriptor
} usb_desc_speed_cfg_t;		//TUSB_DESC_SPEED_CONFIG

/// USB Device Qualifier Descriptor
typedef struct {
	uint8_t  bLength            ; ///< Size of descriptor
	uint8_t  bDescriptorType    ; ///< Device Qualifier Type
	uint16_t bcdUSB             ; ///< USB specification version number (e.g., 0200H for V2.00)

	uint8_t  bDeviceClass       ; ///< Class Code
	uint8_t  bDeviceSubClass    ; ///< SubClass Code
	uint8_t  bDeviceProtocol    ; ///< Protocol Code

	uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for other speed
	uint8_t  bNumConfigurations ; ///< Number of Other-speed Configurations
	uint8_t  bReserved          ; ///< Reserved for future use, must be zero
} usb_desc_device_qualifier_t;	//TUSB_DESC_DEVICE_QUALIFIER


/// USB Interface Association Descriptor (IAD ECN)
typedef struct {
	uint8_t bLength           ; ///< Size of descriptor
	uint8_t bDescriptorType   ; ///< Other_speed_Configuration Type

	uint8_t bFirstInterface   ; ///< Index of the first associated interface.
	uint8_t bInterfaceCount   ; ///< Total number of associated interfaces.

	uint8_t bFunctionClass    ; ///< Interface class ID.
	uint8_t bFunctionSubClass ; ///< Interface subclass ID.
	uint8_t bFunctionProtocol ; ///< Interface protocol ID.

	uint8_t iFunction         ; ///< Index of the string descriptor describing the interface association.
} usb_desc_interface_assoc_t;	//TUSB_DESC_INTERFACE_ASSOCIATION

// USB String Descriptor
typedef struct {
  uint8_t  bLength         ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType ; ///< Descriptor Type
  uint16_t unicode_string[1];	//TODO
} usb_desc_string_t;		//TUSB_DESC_STRING



// USB WebuSB URL Descriptor
typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bScheme;
  char    url[1];
} usb_desc_webusb_url_t;
// DFU Functional Descriptor
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;

  union {
	struct  {
		uint8_t bitCanDnload             : 1;
		uint8_t bitCanUpload             : 1;
		uint8_t bitManifestationTolerant : 1;
		uint8_t bitWillDetach            : 1;
		uint8_t reserved                 : 4;
	} bmAttributes;

	uint8_t bAttributes;
  }att;

  uint16_t wDetachTimeOut;
  uint16_t wTransferSize;
  uint16_t bcdDFUVersion;
} usb_desc_dfu_functional_t;


/*------------------------------------------------------------------*/
/* request
 *------------------------------------------------------------------*/
typedef struct {
  	union {
		struct  {
			uint8_t recipient :  5; ///< Recipient type usb_request_recipient_t.
			uint8_t type      :  2; ///< Request type usb_request_type_t.
			uint8_t direction :  1; ///< Direction type. usb_dir_t
		}bits;
		uint8_t val;
  	}bmRequestType;

	uint8_t  bRequest;
  	uint16_t wValue;
  	uint16_t wIndex;
  	uint16_t wLength;
} usb_control_request_t;

#ifndef pack
#pragma pack()
#endif


#ifdef __cplusplus
}
#endif
#endif





