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
#if HID_SUPPORT
#include  "hid_desc.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
#if HID_SUPPORT & BIT(HID_TYPE_GAMEPADE)
static uint8c_t hid_desc_gamepad_map[] =
{
    #include "hid_desc_gamepad.h"
};
#endif

#if BT_HID_SUPPORT & BIT(HID_TYPE_SWITCH)
static uint8c_t hid_desc_switch_bt_map[] =
{
    #include "hid_desc_switch_bt.h"
};
#endif
#if USBD_HID_SUPPORT & BIT(HID_TYPE_SWITCH)
static uint8c_t hid_desc_switch_usb_map[] =
{
    #include "hid_desc_switch_bt.h"
};
#endif

#if BT_HID_SUPPORT & BIT(HID_TYPE_XBOX)				//only bt 
static uint8c_t hid_desc_xbox_bt_map[] =
{
    #include "hid_desc_xboxone_bt.h"
};
#endif

#if BT_HID_SUPPORT & BIT(HID_TYPE_PS4)
static uint8c_t hid_desc_ps4_bt_map[] =
{
    #include "hid_desc_ps4_bt.h"
};
#endif
#if USBD_HID_SUPPORT & BIT(HID_TYPE_PS4)
static uint8c_t hid_desc_ps4_usb_map[] =
{
    #include "hid_desc_ps4_usb.h"
};
#endif

#if	HID_SUPPORT & BIT(HID_TYPE_KB)
static uint8c_t hid_desc_kb_map[] =
{
    #include "hid_desc_keyboard.h"
};
#endif
#if	HID_SUPPORT & BIT(HID_TYPE_MOUSE)
static uint8c_t hid_desc_mouse_map[] =
{
    #include "hid_desc_mouse.h"
};
#endif
#if	HID_SUPPORT & BIT(HID_TYPE_CONSUMER)
static uint8c_t hid_desc_consumer_map[] =
{
    #include "hid_desc_consumer.h"
};
#endif

#if	HID_SUPPORT & BIT(HID_TYPE_MT)
static uint8c_t hid_desc_mt_map[] =
{
    #include "hid_desc_mt.h"
};
#endif

#if	HID_SUPPORT & BIT(HID_TYPE_VENDOR)
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
uint16_t get_hid_desc_map(trp_t trp, hid_type_t hid_type ,uint8_t** ppmap)
{
	uint16_t map_len = 0;
    uint8_t i;
    switch(hid_type){
        #if HID_SUPPORT & BIT(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            *ppmap = hid_desc_gamepad_map;
            map_len = sizeof(hid_desc_gamepad_map);
            break;
        #endif
        case HID_TYPE_SWITCH:
            if(api_is_bt_trp(trp)){
                #if BT_HID_SUPPORT & BIT(HID_TYPE_SWITCH)
                *ppmap = hid_desc_switch_bt_map;
                map_len = sizeof(hid_desc_switch_bt_map);
                #endif
            }else if(api_is_usb_trp(trp)){
                #if USBD_HID_SUPPORT & BIT(HID_TYPE_SWITCH)
                *ppmap = hid_desc_switch_usb_map;
                map_len = sizeof(hid_desc_switch_usb_map);
                #endif
            }
            break;
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            if(api_is_bt_trp(trp)){
                #if BT_HID_SUPPORT & BIT(HID_TYPE_PS4)
                *ppmap = hid_desc_ps4_bt_map;
                map_len = sizeof(hid_desc_ps4_bt_map);
                #endif
            }else if(api_is_usb_trp(trp)){
                #if USBD_HID_SUPPORT & BIT(HID_TYPE_PS4)
                *ppmap = hid_desc_ps4_usb_map;
                map_len = sizeof(hid_desc_ps4_usb_map);
                #endif
            }
            break;
        #if BT_HID_SUPPORT & BIT(HID_TYPE_XBOX)
        case HID_TYPE_XBOX	:
            if(api_is_bt_trp(trp)){
                *ppmap = hid_desc_xbox_bt_map;
                map_len = sizeof(hid_desc_xbox_bt_map);
            }
            break;
        #endif
        #if	HID_SUPPORT & BIT(HID_TYPE_KB)
        case HID_TYPE_KB 	:
            *ppmap = hid_desc_kb_map;
            map_len = sizeof(hid_desc_kb_map);
            break;
        #endif
        #if	HID_SUPPORT & BIT(HID_TYPE_MOUSE)
        case HID_TYPE_MOUSE :
            *ppmap = hid_desc_mouse_map;
            map_len = sizeof(hid_desc_mouse_map);
            break;
        #endif
        #if	HID_SUPPORT & BIT(HID_TYPE_CONSUMER)
        case HID_TYPE_CONSUMER	:
            *ppmap = hid_desc_consumer_map;
            map_len = sizeof(hid_desc_consumer_map);
            break;
        #endif
        #if	HID_SUPPORT & BIT(HID_TYPE_MT)
        case HID_TYPE_MT	:
            *ppmap = hid_desc_mt_map;
            map_len = sizeof(hid_desc_mt_map);
            break;
        #endif
        #if	HID_SUPPORT & BIT(HID_TYPE_VENDOR)
        case HID_TYPE_VENDOR	:
            *ppmap = hid_desc_vendor_map;
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






