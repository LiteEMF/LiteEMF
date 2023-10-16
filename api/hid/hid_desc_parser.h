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



#ifndef _hid_desc_parser_h
#define _hid_desc_parser_h
#include "emf_typedef.h" 
#include "api/hid/hid_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/

#define HIDItem_SizeMask           0x03     // Mask for Size bitfield in Item header
#define HIDItem_TagMask            0xF0     // Mask for Tag bitfield in Item header
#define HIDItem_TagShift           0x04     // Shift Value for Tag bitfield in Item header
#define HIDItem_TypeMask           0xC      // Mask for Type bitfield in Item header
#define HIDItem_TypeShift          0x02     // Shift Value for Tag bitfield

// HID Item Type Definitions
#define HIDType_Main               0        // Main Item value
#define HIDType_Global             1        // Global Item value
#define HIDType_Local              2        // Local Item value
#define HIDType_Long               3        // Long Item value

// HID Item Tag Definitions - Main Items
#define HIDTag_Input               0x08     // Input Tag value
#define HIDTag_Output              0x09     // Output Tag value
#define HIDTag_Collection          0x0A     // Collection Tag value
#define HIDTag_Feature             0x0B     // Feature Tag value
#define HIDTag_EndCollection       0x0C     // End of Collection Tag value

// HID Item Tag Definitions - Globals
#define HIDTag_UsagePage           0x00     // UsagePage Tag value
#define HIDTag_LogicalMinimum      0x01     // Logical Minimum Tag value
#define HIDTag_LogicalMaximum      0x02     // Logical Maximum Tag value
#define HIDTag_PhysicalMinimum     0x03     // Physical Minimum Tag value
#define HIDTag_PhysicalMaximum     0x04     // Physical Maximum Tag value
#define HIDTag_UnitExponent        0x05     // Unit Exponent Tag value
#define HIDTag_Unit                0x06     // Unit  Tag value
#define HIDTag_ReportSize          0x07     // Report Size  Tag value
#define HIDTag_ReportID            0x08     // Report ID  Tag value
#define HIDTag_ReportCount         0x09     // ReportCount  Tag value
#define HIDTag_Push                0x0A     // Push  Tag value
#define HIDTag_Pop                 0x0B     // Pop  Tag value


// HID Item Tag Definitions - Locals
#define HIDTag_Usage               0x00     //  Usage Tag value
#define HIDTag_UsageMinimum        0x01     //  Usage Minimum Tag value
#define HIDTag_UsageMaximum        0x02     //  Usage Maximum Tag value
#define HIDTag_DesignatorIndex     0x03     //  Designator Index Tag value
#define HIDTag_DesignatorMinimum   0x04     //  Designator Minimum Tag value
#define HIDTag_DesignatorMaximum   0x05     //  Designator Maximum Tag value
#define HIDTag_StringIndex         0x07     //  String Index Tag value
#define HIDTag_StringMinimum       0x08     //  String Minimum Tag value
#define HIDTag_StringMaximum       0x09     //  String Maximum Tag value
#define HIDTag_SetDelimiter        0x0A     //  Set Delimiter Tag value


// HID Main Item Header Bit Definitions
#define HIDData_BufferedBytes     0x100     //  HID data bytes are bufferred
#define HIDData_VolatileBit        0x80     //  Volatile bit position
#define HIDData_Volatile           0x80     //  HID data is voaltile
#define HIDData_NullStateBit       0x40     //  NULL state bit position
#define HIDData_NullState          0x40     //  NULL state defined
#define HIDData_NoPreferredBit     0x20     //  No Preferred bt position
#define HIDData_NoPreferred        0x20     //  HID data type No Preferred
#define HIDData_NonlinearBit       0x10     //  NonLinear bit position
#define HIDData_Nonlinear          0x10     //  HID data type NonLinear
#define HIDData_WrapBit            0x08     //  Wrap bit position
#define HIDData_Wrap               0x08     //  HID data type Wrap
#define HIDData_RelativeBit        0x04     //  Relative bit position
#define HIDData_Relative           0x04     //  HID data type relative
#define HIDData_Absolute           0x00     //  HID data type absolute
#define HIDData_VariableBit        0x02     //  Variable bit position
#define HIDData_Variable           0x02     //  HID data type variable
#define HIDData_ArrayBit           0x02     //  Array Bit position
#define HIDData_Array              0x00     //  Array indentifier value
#define HIDData_ConstantBit        0x01     //  Constant Bit position
#define HIDData_Constant           0x01     //  Constant data type indentifier value


// HID Collection Data Definitions
#define HIDCollection_Physical     0x00
#define HIDCollection_Application  0x01


typedef enum
{
    USB_HID_USAGE_PAGE_UNDEFINED                                                = 0x00,
    USB_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS                                 = 0x01,
    USB_HID_USAGE_PAGE_SIMULATION_CONTROLS                                      = 0x02,
    USB_HID_USAGE_PAGE_VR_CONTROLS                                              = 0x03,
    USB_HID_USAGE_PAGE_SPORT_CONTROLS                                           = 0x04,
    USB_HID_USAGE_PAGE_GAME_CONTROLS                                            = 0x05,
    USB_HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS                                  = 0x06,
    USB_HID_USAGE_PAGE_KEYBOARD_KEYPAD                                          = 0x07,
    USB_HID_USAGE_PAGE_LEDS                                                     = 0x08,
    USB_HID_USAGE_PAGE_BUTTON                                                   = 0x09,
    USB_HID_USAGE_PAGE_ORDINAL                                                  = 0x0A,
    USB_HID_USAGE_PAGE_TELEPHONY                                                = 0x0B,
    USB_HID_USAGE_PAGE_CONSUMER                                                 = 0x0C,
    USB_HID_USAGE_PAGE_DIGITIZER                                                = 0x0D,
    /* Reserved                                                                 = 0x0E */
    USB_HID_USAGE_PAGE_PID_PAGE                                                 = 0x0F,
    USB_HID_USAGE_PAGE_UNICODE                                                  = 0x10,
    /* Reserved                                                                 = 0x11-13 */
    USB_HID_USAGE_PAGE_ALPHANUMERIC_DISPLAY                                     = 0x14,
    /* Reserved                                                                 = 0x15-3f */
    USB_HID_USAGE_PAGE_MEDICAL_INSTRUMENTS                                      = 0x40,
    /* Reserved                                                                 = 0x41-7f */
    USB_HID_USAGE_PAGE_MONITOR                                                  = 0x80,
    USB_HID_USAGE_PAGE_MONITOR_ENUMERATED_VALUES                                = 0x81,
    USB_HID_USAGE_PAGE_MONITOR_VESA_VIRTUAL_CONTROLS                            = 0x82,
    USB_HID_USAGE_PAGE_MONITOR_RESERVED                                         = 0x83,
    USB_HID_USAGE_PAGE_POWER_DEVICE                                             = 0x84,
    USB_HID_USAGE_PAGE_BATTERY_SYSTEM                                           = 0x85,
    USB_HID_USAGE_PAGE_POWER_PAGE_3                                             = 0x86,
    USB_HID_USAGE_PAGE_POWER_PAGE_4                                             = 0x87,
    /* Reserved                                                                 = 0x88-8B */
    USB_HID_USAGE_PAGE_BAR_CODE_SCANNER                                         = 0x8C,
    USB_HID_USAGE_PAGE_WEIGHING_DEVICES                                         = 0x8D,
    USB_HID_USAGE_PAGE_MAGNETIC_STRIPE_READING_DEVICES                          = 0x8E,
    USB_HID_USAGE_PAGE_RESERVED_POINT_OF_SALE_PAGES                             = 0x8F,
    USB_HID_USAGE_PAGE_CAMERA_CONTROL_PAGE                                      = 0x90,
    USB_HID_USAGE_PAGE_ARCADE_PAGE                                              = 0x91
    /* Reserved                                                                 = 0x92-FEFF */
    /* Vendor-defined                                                           = 0xFF00-FFFF */
}HID_USAGE_PAGE;
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

typedef struct{
   union{
       struct{
           uint8_t             ItemSize :2;        // Numeric expression specifying size of data
           uint8_t             ItemType :2;        // This field identifies type of item(Main, Global or Local)
           uint8_t             ItemTag  :4;        // This field specifies the function of the item
       }bits;
       uint8_t                 val;                // to access the data in byte format
    }ItemDetails;
    union{
        int32_t                    sItemData;      // Item Data is stored in signed format
        uint32_t                   uItemData;      // Item Data is stored in unsigned format
        uint8_t                    bItemData[4];
    }Data;
}hid_item_info_t;



#ifndef pack
#pragma pack(1)
#endif
typedef struct {
    uint16_t                    usagePage;         // Specifies current Usage Page
    int32_t                     logicalMinimum;    // This is the minimum value that a variable or array item will report
    int32_t                     logicalMaximum;    // This is the maximum value that a variable or array item will report
    // int32_t                     physicalMinimum;   // Minimum value for the physical extent of a variable item
    // int32_t                     physicalMaximum;   // Maximum value for the physical extent of a variable item
    // int32_t                     unitExponent;      // Value of the unit exponent in base 10
    uint8_t                     reportIndex;       // Conter to keep track of report being processed in the parser
    uint8_t                     report_id;          // Report ID. All the reports are preceded by a single byte report ID
    uint8_t                     reportsize;        // Size of current report in bytes
    uint8_t                     reportCount;       // This field determines number of fields in the report
}hid_globals_item_t;


typedef struct {
    uint8_t         report_id;          // Report ID of the associated report
    uint8_t         featureBits;       // If feature report then length of report in bits
    uint16_t        inputBits;         // If input report then length of report in bits
    uint16_t        outputBits;        // If output report then length of report in bits
}hid_report_t;


typedef struct {
    // uint32_t	raw;              // Collection raw data
    uint16_t	usagePage;         // Usage page associated with current level of collection
    uint8_t 	firstUsageItem;    // Index of First Usage Item in the current collection
    uint8_t 	usageItems;        // Number of Usage Items in the current collection
    uint8_t 	firstReportItem;   // Index of First report Item in the current collection
    uint8_t 	reportItems;       // Number of report Items in the current collection
    uint8_t 	parent;            // Index to Parent collection
}hid_collection_t;

typedef struct {
    hid_report_type_t 	reportType;          // Type of Report Input/Output/Feature
    hid_globals_item_t	globals;             // Stores all the global items associated with the current report
    uint8_t           	startBit;            // Starting Bit Position of the report
    uint8_t          	dataModes;           // this tells the data mode is array or not
    uint8_t           	firstUsageItem;      // Index to first usage item related to the report
    uint8_t           	usageItems;          // Number of usage items in the current report
}hid_report_item_t;

typedef struct {
    uint16_t		usagePage;     // Usage page ID asscociated with the Item
    uint16_t		usageMinimum;  // Defines the starting usage associated with an array or bitmap
    uint16_t		usageMaximum;  // Defines the ending usage associated with an array or bitmap
}hid_usage_item_t;

typedef struct {
    uint8_t    		isRange;   // If range of String Item is valid
    uint16_t		index;     // String index for a String descriptor; allows a string to be associated with a particular item or control
    uint16_t		minimum;   // Specifies the first string index when assigning a group of sequential strings to controls in an array or bitmap
    uint16_t		maximum;   // Specifies the last string index when assigning a group of sequential strings to controls in an array or bitmap
}hid_string_item_t, hid_desig_item_t;

typedef struct {
    hid_globals_item_t *globalsStack;      // List of global Items, see HID_GLOBALS for details in the structure
    hid_collection_t *collectionList;
    uint8_t *collectionStack;           // stores the array of parents ids for the collection
    hid_report_t *reportList;              // List of reports , see hid_report_t for details in the structure
    hid_report_item_t *reportItemList;     // List of report Items, see hid_report_item_t for details in the structure
    hid_usage_item_t *usageItemList;       // List of Usage item , see hid_usage_item_t for details in the structure
}hid_item_t;

typedef struct{
    // This set of members are used during parsing of Report descriptor , application does not normally need these details
    uint8_t haveUsageMax;          // True if report descriptor has a valid Usage Max
    uint8_t haveUsageMin;          // True if report descriptor has a valid Usage Min
    uint16_t rangeUsagePage;        // current usage page during parsing
    uint16_t usageMaximum;          // current usage maximum
    uint16_t usageMinimum;          // current usage minimum
    uint16_t usageRanges;           // current usage ranges
    uint8_t collectionNesting;     // this number tells depth of collection nesting
    uint8_t collections;           // total number of collections
    uint8_t maxCollectionNesting;  // Maximum depth of collections
    uint8_t parent;                // Parent collection
    uint8_t firstUsageItem;        // index of first usage item for the current collection
    uint8_t globalsNesting;        // On encountering every PUSH item , this is incremented , keep track of current depth of Globals
    uint8_t maxGlobalsNesting;     // Maximum depth of Globals
    uint8_t reportItems;           // total number of report items
    uint8_t reports;               // total number of reports
    uint8_t usageItems;            // total number of usage items , used to index the array of usage
    uint8_t usages;                // total sumber of usages
    hid_globals_item_t globals;    // holds cuurent globals items

    uint16_t item_list_size;
    hid_item_t item_list;
}hid_desc_info_t;


#ifndef pack
#pragma pack()
#endif



typedef struct{
    uint8_t report_length;        // report_length - the expected length of the parent report.
    uint8_t report_id;            // report_id - report ID

    uint8_t bit_offset;           // BitOffset - bit offset within the report.
    uint8_t bit_size;             // reportsize
    uint8_t bit_count;            // reportCount
    uint8_t usageMinimum;
}hid_items_t;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
void hid_items_dump(hid_items_t *pitems);
void hid_desc_dump(hid_desc_info_t *pinfo);
error_t hid_desc_parse_report(hid_desc_info_t *pinfo, uint8_t* pdesc , uint16_t desc_len);
void hid_desc_info_free(hid_desc_info_t *pinfo);
bool hid_match_collection(hid_desc_info_t *pinfo, uint8_t collection_index, uint16_t usagePage, uint16_t usage);
bool hid_collection_find_items(hid_desc_info_t *pinfo, hid_collection_t* pcollection, hid_report_type_t type, uint16_t usagePage, uint16_t usage, hid_items_t *pitems);
bool hid_reportlist_find_items(hid_desc_info_t *pinfo, uint8_t item_index, hid_report_type_t type, uint16_t usagePage, uint16_t usage, hid_items_t *pitems);
bool hid_items_match_id(hid_items_t *pitem, uint8_t* buf, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif
