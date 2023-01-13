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
#if APP_BT_ENABLE & EXTERNAL_MODULE
#include "api/bt/api_bt.h"
#include "api/api_log.h"


#define DRIVER_MODULE   EXTERNAL_MODULE
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
bool bt_driver_get_mac(bt_t bt, uint8_t *buf )
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE == bt) || (BT_BLE_RF == bt)){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC == bt) || (BT_BLEC_RF == bt)){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR == bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC == bt){

    }
    #endif
    
    return ret;
}

bool bt_driver_is_bonded(bt_t bt)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE | BT_BLE_RF) & bt){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC | BT_BLEC_RF) & bt){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR & bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC & bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_debond(bt_t bt)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE | BT_BLE_RF) & bt){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC | BT_BLEC_RF) & bt){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR & bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC & bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_disconnect(bt_t bt)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE | BT_BLE_RF) & bt){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC | BT_BLEC_RF) & bt){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR & bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC & bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_enable(bt_t bt,bool en)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE | BT_BLE_RF) & bt){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC | BT_BLEC_RF) & bt){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR & bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC & bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_uart_tx(bt_t bt,uint8_t *buf, uint16_t len)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE == bt) || (BT_BLE_RF == bt)){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC == bt) || (BT_BLEC_RF == bt)){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR == bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC == bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE == bt) || (BT_BLE_RF == bt)){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC == bt) || (BT_BLEC_RF == bt)){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR == bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC == bt){

    }
    #endif
    
    return ret;
}
bool bt_driver_init(void)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE

    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE

    #endif

    #if EDR_ENABLE & DRIVER_MODULE

    #endif
    #if EDRC_ENABLE & DRIVER_MODULE

    #endif
    
    return ret;
}
bool bt_driver_deinit(bt_t bt)
{
    bool ret = false;

    #if (BLE_ENABLE | BLE_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLE | BT_BLE_RF) & bt){

    }
    #endif
    #if (BLEC_ENABLE | BLEC_RF_ENABLE) & DRIVER_MODULE
    if((BT_BLEC | BT_BLEC_RF) & bt){

    }
    #endif

    #if EDR_ENABLE & DRIVER_MODULE
    if(BT_EDR & bt){

    }
    #endif
    #if EDRC_ENABLE & DRIVER_MODULE
    if(BT_EDRC & bt){

    }
    #endif
    
    return ret;
}
void bt_driver_handler(void)
{

}


#endif

