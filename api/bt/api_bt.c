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
#if APP_BT_ENABLE
#include "app/app_bt.h"
#include "api/api_tick.h"
#include "api/api_log.h"

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
bool bt_stack_init_ok = false;
bool bt_ex_stack_init_ok = false;
/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_bt_get_driver(bt_t bt)
{
	#if BLE_ENABLE & INTER_MODULE
    if(BT_BLE & bt){
        bt |= BT_BLE;
    }
	#endif
	#if BLEC_ENABLE & INTER_MODULE
    if(BT_BLEC & bt){
        bt |= BT_BLEC;
    }
    #endif
	#if BLE_RF_ENABLE & INTER_MODULE
	if(BT_BLE_RF & bt){
		bt |= BT_BLE_RF;
	}
    #endif
	#if BLEC_RF_ENABLE & INTER_MODULE
	if(BT_BLEC_RF & bt){
		bt |= BT_BLEC_RF;
	}
    #endif
	#if EDR_ENABLE & INTER_MODULE
    if(BT_EDR == bt){
        bt |= BT_EDR;
    }
    #endif
    #if EDRC_ENABLE & INTER_MODULE
    if(BT_EDRC == bt){
        bt |= BT_EDRC;
    }
    #endif

	#if RF_ENABLE & INTER_MODULE
    if(BT_RF & bt){
        bt |= BT_RF;
    }
    #endif
    #if RFC_ENABLE & INTER_MODULE
    if(BT_RFC == bt){
        bt |= BT_RFC;
    }
    #endif

	return bt;
}

bool api_bt_get_ex_driver(bt_t bt)
{
	#if BLE_ENABLE & EXTERNAL_MODULE
    if(BT_BLE & bt){
        bt |= BT_BLE;
    }
	#endif
	#if BLEC_ENABLE & EXTERNAL_MODULE
    if(BT_BLEC & bt){
        bt |= BT_BLEC;
    }
    #endif
	#if BLE_RF_ENABLE & EXTERNAL_MODULE
	if(BT_BLE_RF & bt){
		bt |= BT_BLE_RF;
	}
    #endif
	#if BLEC_RF_ENABLE & EXTERNAL_MODULE
	if(BT_BLEC_RF & bt){
		bt |= BT_BLEC_RF;
	}
    #endif
	#if EDR_ENABLE & EXTERNAL_MODULE
    if(BT_EDR == bt){
        bt |= BT_EDR;
    }
    #endif
    #if EDRC_ENABLE & EXTERNAL_MODULE
    if(BT_EDRC == bt){
        bt |= BT_EDRC;
    }
    #endif

	#if RF_ENABLE & EXTERNAL_MODULE
    if(BT_RF & bt){
        bt |= BT_RF;
    }
    #endif
    #if RFC_ENABLE & EXTERNAL_MODULE
    if(BT_RFC == bt){
        bt |= BT_RFC;
    }
    #endif

	return bt;
}

bool api_bt_get_mac(bt_t bt, uint8_t *buf )		//这里高3byte是public地址,和nrf(大端输出)搜索是反的
{
	bool ret = false;
	bt_t driver_bt;
	uint8_t base_mac[6];

	if(BT_NULL == bt) return false;

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_get_mac(driver_bt,base_mac);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_get_mac(driver_bt,base_mac);
		}
	}
	#endif

	if(!ret) return false;	//get base_mac

	if(BT_BLE & bt){								//edr address 
		base_mac[0] += 1;
	}

	return ret;
}

bool api_bt_is_bonded(bt_t bt)
{
	bool ret = false;
	bt_t driver_bt;

	if(BT_NULL == bt) return false;

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_is_bonded(driver_bt);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_is_bonded(driver_bt);
		}
	}
	#endif

	return ret;
}

bool api_bt_debond(bt_t bt)
{
	bool ret = false;
	bt_t driver_bt;
	if(BT_NULL == bt) return false;
	logd("bt_debond bt=%d\n",bt);

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_debond(driver_bt);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_debond(driver_bt);
		}
	}
	#endif

	return ret;
}



bool api_bt_disconnect(bt_t bt)
{
	bool ret = false;
	bt_t driver_bt;

	if(BT_NULL == bt) return ret;

	app_bt_tx_fifo_fush(bt);				//使用fifo tx 清空fifo

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_disconnect(driver_bt);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_disconnect(driver_bt);
		}
	}
	#endif
	return ret;
}

/*******************************************************************
** Parameters:	
** Returns:			
** Description:	btc adv enable
*******************************************************************/
bool api_bt_enable(bt_t bt,bool en)
{
	bool ret = true;
	bt_t driver_bt;

	if(BT_NULL == bt) return false;
	logi("bt %d enable=%d\n", bt, en);

	#if BLE_ENABLE
	if(BT_BLE & bt){
		m_ble.enable = en;
	}
	#endif
	#if BLEC_ENABLE
	if(BT_BLEC & bt){
		m_blec.enable = en;
	}
	#endif
	#if BLE_RF_ENABLE
	if(BT_BLE_RF & bt){
		m_ble_rf.enable = en;
	}
	#endif
	#if BLEC_RF_ENABLE
	if(BT_BLEC_RF & bt){
		m_blec_rf.enable = en;
	}
	#endif
	#if EDR_ENABLE
	if(BT_EDR & bt){
		m_edr.enable = en;
	}
	#endif
	#if EDRC_ENABLE
	if(BT_EDRC & bt){
		m_edrc.enable = en;
	}
	#endif

	#if RF_ENABLE
	if(BT_RF & bt){
		m_rf.enable = en;
	}
	#endif
	#if RFC_ENABLE
	if(BT_RFC & bt){
		m_rfc.enable = en;
	}
	#endif

	if(!en)	api_bt_disconnect(bt);

	app_bt_tx_fifo_fush(bt);				//使用fifo tx 清空fifo

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_enable(driver_bt,en);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_enable(driver_bt,en);
		}
	}
	#endif
	
	return ret;
}


bool api_bt_uart_fifo_tx(bt_t bt,app_fifo_t * fifop, uint8_t *buf, uint16_t len)
{
	bool ret = false;
	uint16_t fifo_len = len;

	if(BT_NULL == bt) return false;
	if(fifop->p_buf == NULL) return false;

	if(FIFO_EMPTY_LENGTH(fifop) < fifo_len){
		// logd("fifo trp full!\n");
		ret = false;
	}else if(ERROR_SUCCESS == app_fifo_write(fifop, (uint8_t const *)buf, &fifo_len)){
		ret = true;
	}else{
		logi_r("fifo trp err!\n");
		ret = false;
	}

	return ret;
}

bool api_bt_uart_tx(bt_t bt,uint8_t *buf, uint16_t len)
{
	bool ret = false;
	bt_t driver_bt;
	
	if(BT_NULL == bt) return false;

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_uart_tx(driver_bt,buf, len);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_uart_tx(driver_bt,buf, len);
		}
	}
	#endif

	return ret;
}

/*******************************************************************
** Function: 	
** Parameters: id:高8bit为参数:, 低8bit为:id //TODO
** Returns:
*******************************************************************/
bool api_bt_hid_tx(bt_t bt,uint16_t id, uint8_t*buf, uint16_t len)
{
	bool ret = false;
	bt_t driver_bt;

	if(BT_NULL == bt) return false;

	#if APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		driver_bt = api_bt_get_ex_driver(bt);
		if(BT_NULL != driver_bt){
			ret |= bt_driver_hid_tx(driver_bt,id, buf, len);
		}
	}
	#endif

	#if APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		driver_bt = api_bt_get_driver(bt);;
		if(BT_NULL != driver_bt){
			ret |= hal_bt_hid_tx(driver_bt,id, buf, len);
		}
	}
	#endif

    return ret;
}


bool api_bt_stack_init_ok(bt_t bt)
{
	bool ret = false;

	#if APP_APP_BT_ENABLE & EXTERNAL_MODULE
	bt_ex_stack_init_ok = true;
    #endif

	#if APP_APP_BT_ENABLE & INTER_MODULE
	bt_stack_init_ok = true;
    #endif

	return ret;
}


bool api_bt_init(void)
{
	bool ret = false;

	#if APP_APP_BT_ENABLE & EXTERNAL_MODULE
	bt_ex_stack_init_ok = false;
	ret |= bt_driver_init();
    #endif

	#if APP_APP_BT_ENABLE & INTER_MODULE
	bt_stack_init_ok = false;
	ret |= hal_bt_init();
    #endif

	return ret;
}


bool api_bt_deinit(void)
{
	bool ret = false;

	#if APP_APP_BT_ENABLE & EXTERNAL_MODULE
	if(bt_ex_stack_init_ok){
		bt_ex_stack_init_ok = false;
		ret |= bt_driver_deinit();
	}
    #endif

	#if APP_APP_BT_ENABLE & INTER_MODULE
	if(bt_stack_init_ok){
		bt_stack_init_ok = false;
		ret |= hal_bt_deinit();
	}
    #endif

	return ret;
}

void api_bt_handler(void)
{
	#if APP_BT_ENABLE & EXTERNAL_MODULE
    bt_driver_handler();
    #endif

	#if APP_BT_ENABLE & INTER_MODULE
    hal_bt_handler();
    #endif
}

#endif






