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
#if APP_KEY_ENABLE

#include  "app/app_key.h"
#include  "api/api_tick.h"

#include  "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint32_t m_key_scan;
app_key_t m_app_key;			


bool m_key_power_on = false;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void app_key_dump(uint32_t key)
{
    #if KEY_DUMP_ENABLE
    logd("key=%x:",key);
    #if 1
    if(key & HW_KEY_A 		) logd("    A,");
    if(key & HW_KEY_B 		) logd("	B,");
    if(key & HW_KEY_C 		) logd("	C,");
    if(key & HW_KEY_X 		) logd("	X,");
    if(key & HW_KEY_Y 		) logd("	Y,");
    if(key & HW_KEY_Z 		) logd("	Z,");
    if(key & HW_KEY_L1 		) logd("	L1,");
    if(key & HW_KEY_R1 		) logd("	R1,");
    if(key & HW_KEY_L2 		) logd("	L2,");
    if(key & HW_KEY_R2 		) logd("	R2,");
    if(key & HW_KEY_SELECT 	) logd("	SELECT,");
    if(key & HW_KEY_START 	) logd("	START,");
    if(key & HW_KEY_HOME 	) logd("	HOME,");
    if(key & HW_KEY_L3 		) logd("	L3,");
    if(key & HW_KEY_R3 		) logd("	R3,");
    if(key & HW_KEY_CAPTURE	) logd("	CAPTURE,");	
    if(key & HW_KEY_UP 		) logd("	UP,");
    if(key & HW_KEY_DOWN 	) logd("	DOWN,");
    if(key & HW_KEY_LEFT 	) logd("	LEFT,");
    if(key & HW_KEY_RIGHT 	) logd("	RIGHT,");
    if(key & HW_KEY_FN 		) logd("	FN,");
    if(key & HW_KEY_MODE 	) logd("	MODE,");
    if(key & HW_KEY_MENU 	) logd("	MENU,");
    if(key & HW_KEY_M1 		) logd("	M1,");
    if(key & HW_KEY_M2		) logd("	M2,"); 
    if(key & HW_KEY_M3 		) logd("	M3,");
    if(key & HW_KEY_M4 		) logd("	M4,");
    if(key & HW_KEY_M5 		) logd("	M5,");
    if(key & HW_KEY_M6 		) logd("	M6,");
    if(key & HW_KEY_M7 		) logd("	M7,");
    if(key & HW_KEY_M8 		) logd("	M8,");
    if(key & HW_KEY_POWER 	) logd("	POWER,");
    #endif
    logd("\n");
    #endif
	UNUSED_PARAMETER(key);
}

#if WEAK_ENABLE
__WEAK void app_key_vendor_scan(uint32_t *pkey)
{
	
}

__WEAK void app_key_event(void)
{
    
}
#endif


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_key_init(void)
{
    m_key_power_on = false;
    m_key_scan = 0;
    memset(&m_app_key,0,sizeof(m_app_key));
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_key_deinit(void)
{
	return app_key_init();
}



void app_key_scan_task(void *pa)
{
    uint32_t key;
    
    key = 0;
    key = io_key_scan();
    app_key_vendor_scan(&key);	

    if(m_key_scan != key){
       m_key_scan = key;

       #if !APP_GAMEAPD_ENABLE
       app_key_dump(key);
       #endif
    }
    UNUSED_PARAMETER(pa);
}


void app_key_decode_task(uint32_t key_scan)
{
	bool ret = false;
    uint8_t i;
	uint32_t key,bits;
	static uint16_t key_cnt[32];

    key = key_scan;
    for(i = 0; i < 32; i++){
        bits = BIT(i);

        if(key & bits){						//pressed
            if(!(m_app_key.pressed & bits)){
                m_app_key.pressed |= bits;
                key_cnt[i] = 0;
                ret = true;
            }

            if(key_cnt[i] <= KEY_LONG_LONG_TIME){
                key_cnt[i] ++;

                if(key_cnt[i] >= KEY_LONG_LONG_TIME){
                    if(!(m_app_key.long_long & bits)){
                        m_app_key.long_long |= bits;
                        #if KEY_DUMP_ENABLE
                        logd("press_long_long: 0x%x\n", m_app_key.long_long);
                        #endif
                        ret = true;
                    }                
                }else if(key_cnt[i] >= KEY_LONG_TIME){
                    if(!(m_app_key.press_long & bits)){
                        m_app_key.press_long |= bits;
                        #if KEY_DUMP_ENABLE
                        logd("press_long=%x\n",m_app_key.press_long);
                        #endif
                        ret = true;
                    }
                }else if(key_cnt[i] >= KEY_SHORT_TIME){
                    if(!(m_app_key.press_short & bits)){
                        m_app_key.press_short |= bits;
                        #if KEY_DUMP_ENABLE
                        logd("press_short=%x\n",m_app_key.press_short);
                        #endif
                        ret = true;
                    }
                }
            }
        }else{												    //key up
            if((m_app_key.pressed & bits)){
                if( 0 == ((m_app_key.pressed_b | m_app_key.double_b | m_app_key.three_b) & bits) ){
                    if(key_cnt[i] < KEY_SHORT_TIME){
                        if(m_app_key.pre_double_b & bits){      //第二次快速按下
                            if(m_app_key.pre_three_b & bits){   //第三次快速按键按下
                                m_app_key.three_b |= bits;
                                m_app_key.pre_three_b &= ~bits;
                                m_app_key.pre_double_b &= ~bits;
                                #if KEY_DUMP_ENABLE
                                logd("m_app_key.three_b\n");
                                #endif
                            }else{
                                m_app_key.pre_three_b |= bits;
                            }
                        }else{
                            m_app_key.pre_double_b |= bits;                          
                        }
                    }else{
                        m_app_key.pre_double_b &= ~bits;
                        m_app_key.pre_three_b &= ~bits;
                    }
                }

                m_app_key.pressed &= ~bits;
                m_app_key.press_short &= ~bits;
                m_app_key.press_long &= ~bits;
                m_app_key.long_long &= ~bits;
                ret = true;
                key_cnt[i] = 0;
            }else if((m_app_key.pre_double_b | m_app_key.pre_three_b) & bits){
                key_cnt[i]++;
                if(key_cnt[i] >= KEY_DOUBLE_B_TIME){
                    key_cnt[i] = 0;
                    if(m_app_key.pre_three_b & bits){
                        m_app_key.double_b |= bits;
                        #if KEY_DUMP_ENABLE
                        logd("m_app_key.double_b\n");
                        #endif
                    }else{
                        m_app_key.pressed_b |= bits;
                        #if KEY_DUMP_ENABLE
                        logd("m_app_key.pressed_b\n");
                        #endif
                    }
                    m_app_key.pre_three_b &= ~bits;
                    m_app_key.pre_double_b &= ~bits;
                    ret = true;
                }
            }else if(((m_app_key.pressed_b | m_app_key.double_b | m_app_key.three_b) & bits)){
                key_cnt[i]++;
                if(key_cnt[i] >= KEY_PRESSED_B_DELAY){
                    key_cnt[i] = 0;
                    m_app_key.pressed_b &= ~bits;
                    m_app_key.double_b &= ~bits;
                    m_app_key.three_b &= ~bits;
                    ret = true;
                }
            }
        }
    }

    if(ret){
        app_key_event();
    }

    if(!m_key_power_on){
        if(!KEY_POWER && !((m_app_key.pressed_b|m_app_key.double_b|m_app_key.three_b) & HW_KEY_POWER)){
            m_key_power_on = true;
        }
    }

}




#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_key_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_key_scan_task(NULL);
	}
}

void app_key_decode_handler(uint32_t period_10us,uint32_t key_scan)
{
    static timer_t s_timer;
    if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_key_decode_task(key_scan);
	}
}
#endif






#endif




