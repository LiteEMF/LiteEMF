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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "hw_config.h"
#if HIDD_SUPPORT
#include  "api/hid/hid_dev_desc.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
static uint8c_t hid_desc_gamepad_map[] =
{
    #include "api/hid/hid_desc_gamepad.h"
};
#endif
#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_DINPUT)
static uint8c_t hid_desc_dinput[] =
{
    #include "api/hid/hid_desc_dinput.h"
};
#endif


#if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
static uint8c_t hid_desc_switch_bt_map[] =
{
    #include "hid_desc_switch_bt.h"
};
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
static uint8c_t hid_desc_switch_usb_map[] =
{
    #include "hid_desc_switch_usb.h"
};
#endif

#if EDR_HID_SUPPORT & (BIT_ENUM(HID_TYPE_XBOX) | BIT_ENUM(HID_TYPE_X360))				//only edr
static uint8c_t hid_desc_xbox_edr_map[] =
{
    #include "hid_desc_xbox_edr.h"
};
#endif
#if (BLE_RF_HID_SUPPORT | BLE_HID_SUPPORT) & (BIT_ENUM(HID_TYPE_XBOX) | BIT_ENUM(HID_TYPE_X360))	//only ble
static uint8c_t hid_desc_xbox_ble_map[] =
{
    #include "hid_desc_xbox_ble.h"
};
#endif

#if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
static uint8c_t hid_desc_ps3_map[] =
{
    #include "hid_desc_ps3.h"
};
#endif
#if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
static uint8c_t hid_desc_ps4_bt_map[] =
{
    #include "hid_desc_ps4_bt.h"
};
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
static uint8c_t hid_desc_ps4_usb_map[] =
{
    #include "hid_desc_ps4_usb.h"
};
#endif

#if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
static uint8c_t hid_desc_ps5_bt_map[] =
{
    #include "hid_desc_ps5_edge_bt.h"
};
#endif
#if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
static uint8c_t hid_desc_ps5_usb_map[] =
{
    #include "hid_desc_ps5_edge_usb.h"
};
#endif

#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_KB)
static uint8c_t hid_desc_kb_map[] =
{
    #include "hid_desc_keyboard.h"
};
#endif
#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
static uint8c_t hid_desc_mouse_map[] =
{
    #include "hid_desc_mouse.h"
};
#endif
#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_CONSUMER)
static uint8c_t hid_desc_consumer_map[] =
{
    #include "hid_desc_consumer.h"
};
#endif
#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_TOUCH)        //单点触控
static uint8c_t hid_desc_touch_map[] =
{
    #include "hid_desc_touch.h"
};
#endif
#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MT)        //多点触控
static uint8c_t hid_desc_mt_map[] =
{
    #include "hid_desc_mt.h"
};
#endif

#if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
static uint8c_t hid_desc_vendor_map[] =
{
    #include "hid_desc_vendor.h"
};
#endif




/*****************************************************************************************************
**  Function
******************************************************************************************************/



/*******************************************************************
** Parameters:
** Returns:	 return: report map len
** Description:		
*******************************************************************/
uint16_t get_hid_desc_map(trp_t trp, hid_type_t hid_type, uint8_t** ppmap)
{
	uint16_t map_len = 0;
	
    switch(hid_type){
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_gamepad_map;
            map_len = sizeof(hid_desc_gamepad_map);
            break;
        #endif
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_DINPUT)
        case HID_TYPE_DINPUT:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_dinput;
            map_len = sizeof(hid_desc_dinput);
            break;
        #endif
        case HID_TYPE_SWITCH:
            if(api_trp_is_bt(trp)){
                #if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_switch_bt_map;
                map_len = sizeof(hid_desc_switch_bt_map);
                #endif
            }else if(api_trp_is_usb(trp)){
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_SWITCH)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_switch_usb_map;
                map_len = sizeof(hid_desc_switch_usb_map);
                #endif
            }
            break;
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_PS3)
        case HID_TYPE_PS3	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_ps3_map;
            map_len = sizeof(hid_desc_ps3_map);
            break;
        #endif
        case HID_TYPE_PS4	:
            if(api_trp_is_bt(trp)){
                #if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_ps4_bt_map;
                map_len = sizeof(hid_desc_ps4_bt_map);
                #endif
            }else if(api_trp_is_usb(trp)){
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS4)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_ps4_usb_map;
                map_len = sizeof(hid_desc_ps4_usb_map);
                #endif
            }
            break;
        case HID_TYPE_PS5	:
            if(api_trp_is_bt(trp)){
                #if BT_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_ps5_bt_map;
                map_len = sizeof(hid_desc_ps5_bt_map);
                #endif
            }else if(api_trp_is_usb(trp)){
                #if USBD_HID_SUPPORT & BIT_ENUM(HID_TYPE_PS5)
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_ps5_usb_map;
                map_len = sizeof(hid_desc_ps5_usb_map);
                #endif
            }
            break;
        #if BT_HID_SUPPORT & (BIT_ENUM(HID_TYPE_XBOX) | BIT_ENUM(HID_TYPE_X360))
        case HID_TYPE_XBOX	:
        case HID_TYPE_X360  :
            if(TR_EDR == trp){
                #if EDR_HID_SUPPORT & (BIT_ENUM(HID_TYPE_XBOX) | BIT_ENUM(HID_TYPE_X360))
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_xbox_edr_map;
                map_len = sizeof(hid_desc_xbox_edr_map);
                #endif
            }else if((TR_BLE_RF == trp) || (TR_BLE == trp)){
                #if (BLE_RF_HID_SUPPORT | BLE_HID_SUPPORT) & (BIT_ENUM(HID_TYPE_XBOX) | BIT_ENUM(HID_TYPE_X360))
                if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_xbox_ble_map;
                map_len = sizeof(hid_desc_xbox_ble_map);
                #endif
            }
            break;
        #endif
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_KB)
        case HID_TYPE_KB 	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_kb_map;
            map_len = sizeof(hid_desc_kb_map);
            break;
        #endif
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE)
        case HID_TYPE_MOUSE :
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_mouse_map;
            map_len = sizeof(hid_desc_mouse_map);
            break;
        #endif
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_CONSUMER)
        case HID_TYPE_CONSUMER	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_consumer_map;
            map_len = sizeof(hid_desc_consumer_map);
            break;
        #endif
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_TOUCH)
        case HID_TYPE_TOUCH	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_touch_map;
            map_len = sizeof(hid_desc_touch_map);
            break;
        #endif
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MT)
        case HID_TYPE_MT	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_mt_map;
            map_len = sizeof(hid_desc_mt_map);
            break;
        #endif
        #if	HIDD_SUPPORT & BIT_ENUM(HID_TYPE_VENDOR)
        case HID_TYPE_VENDOR	:
            if(NULL != ppmap) *ppmap = (uint8_t*)hid_desc_vendor_map;
            map_len = sizeof(hid_desc_vendor_map);
            break;
        #endif
        default :			//comble device
            break;
    }

    return map_len;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool hid_report_desc_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool hid_report_desc_deinit(void)
{
	return true;
}



#endif






