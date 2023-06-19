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

#include "hw_config.h"
#if API_BT_ENABLE
#include "api/bt/api_bt.h"
#include "api/api_log.h"


/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/


/*******************************************************************************************************************
**	public Parameters
********************************************************************************************************************/

/*******************************************************************************************************************
**	static Parameters
********************************************************************************************************************/


/*****************************************************************************************************
**  hal bt Function
******************************************************************************************************/
bool hal_bt_get_mac(uint8_t id, bt_t bt, uint8_t *buf )
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   

    return ret;
}

bool hal_bt_is_bonded(uint8_t id, bt_t bt)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_debond(uint8_t id, bt_t bt)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_disconnect(uint8_t id, bt_t bt)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_enable(uint8_t id, bt_t bt,bool en)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_uart_tx(uint8_t id, bt_t bt,uint8_t *buf, uint16_t len)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_hid_tx(uint8_t id, bt_t bt,uint8_t*buf, uint16_t len)
{
    bool ret = false;

    if(BT_ID0 != id) return false;
    switch(bt){
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))
    case BT_BLE: 	
    case BT_BLE_RF: 			//BLE模拟2.4G
        break;
    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))
    case BT_BLEC:
    case BT_BLEC_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)
    case BT_EDR: 	    
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)
    case BT_EDRC:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)
    case BT_RF:
        break;
    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)
    case BT_RFC:
        break;
    #endif
    }   
    
    return ret;
}
bool hal_bt_init(uint8_t id)
{
    bool ret = false;

    if(BT_ID0 != id) return false;

    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))

    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))

    #endif

    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)

    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)

    #endif

    #if BT0_SUPPORT & BIT_ENUM(TR_RF)

    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)

    #endif
    
    return ret;
}
bool hal_bt_deinit(uint8_t id)
{
    bool ret = false;

    if(BT_ID0 != id) return false;

    #if BT0_SUPPORT & (BIT_ENUM(TR_BLE) | BIT_ENUM(TR_BLE_RF))

    #endif
    #if BT0_SUPPORT & (BIT_ENUM(TR_BLEC) | BIT_ENUM(TR_BLE_RFC))

    #endif

    #if BT0_SUPPORT & BIT_ENUM(TR_EDR)

    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_EDRC)

    #endif
    
    #if BT0_SUPPORT & BIT_ENUM(TR_RF)

    #endif
    #if BT0_SUPPORT & BIT_ENUM(TR_RFC)

    #endif
    
    return ret;
}
void hal_bt_task(void* pa)
{
    UNUSED_PARAMETER(pa);
}


#endif

