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
#include  "api/api_log.h"
#include  "api/api_tick.h"

#if APP_JOYSTICK_ENABLE
#include  "app/app_joystick.h"
#endif
#if APP_IMU_ENABLE
#include  "app/imu/app_imu.h"
#endif
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint8_t  m_app_stick_key;
app_key_t m_app_key;

uint32_t m_key_pressed;
uint32_t m_key_short;
uint32_t m_key_pressed_b;
uint32_t m_key_long;
uint32_t m_key_long_long;
uint32_t m_key_double_b;		    
uint32_t m_key_pre_double_b;			


bool m_key_power_on = false;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
void dump_key(uint32_t key)
{
    logd("key=%x:	",key);
    #if 1
    if(key & HW_KEY_A 		) logd("	A,");
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
    if(key 	) logd("\n");
    #endif
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/

void app_key_trigger_check(app_key_t* keyp)
{
	if((keyp->key & HW_KEY_L2) && (0 == keyp->trigger.x)){
		keyp->trigger.x = 0xffff;
	}
	if((0 == keyp->key & HW_KEY_L2) && keyp->trigger.x){
		keyp->key |= HW_KEY_L2;
	}

	if((keyp->key & HW_KEY_R2) && (0 == keyp->trigger.y)){
		keyp->trigger.y = 0xffff;
	}
	if((0 == keyp->key & HW_KEY_R2) && keyp->trigger.y){
		keyp->key |= HW_KEY_R2;
	}
}

void app_key_swapl(app_key_t* keyp)
{
	keyp->key = SWAP32_L(keyp->key);
	axis2i_swapl(&keyp->stick_l);
	axis2i_swapl(&keyp->stick_r);
	axis2i_swapl(&keyp->trigger);
	axis3i_swapl(&keyp->acc);
	axis3i_swapl(&keyp->gyro);
}

#if WEAK_ENABLE
__WEAK void app_key_vendor_scan(app_key_t *keyp)
{
	
}
#endif
#if WEAK_ENABLE
__WEAK void app_key_event(void)
{
    
}
#endif

void app_key_scan(void)
{
    app_key_t app_key;
    uint32_t hw_key=0;

	memset(&app_key,0,sizeof(app_key));
    app_key.key = io_key_scan();

    #if APP_JOYSTICK_ENABLE
    app_key.stick_l = m_joystick.stick[APP_STICK_L_ID];
    app_key.stick_r = m_joystick.stick[APP_STICK_R_ID];
    app_key.trigger.x = m_joystick.tarigger[APP_TRIGGER_L_ID];
    app_key.trigger.y = m_joystick.tarigger[APP_TRIGGER_R_ID];
    #endif

    app_key_vendor_scan(&app_key);	
	app_key_trigger_check(&app_key);

    #if APP_JOYSTICK_ENABLE
    m_app_stick_key = get_stick_dir(&app_key.stick_l);
    m_app_stick_key |= get_stick_dir(&app_key.stick_r)<<4;
    #endif

    #if APP_IMU_ENABLE
    app_imu_get_val(&app_key.acc,&app_key.gyro);
    #endif

	if(m_app_key.key != app_key.key){
		dump_key(app_key.key);
	}
	m_app_key = app_key;
}


void app_key_decode(void)
{
	bool ret = false;
    uint8_t i;
	uint32_t key,bit;

	static uint16_t key_cnt[32];
    static timer_t key_timer;

    if((m_tick - key_timer) >= KEY_PERIOD_DEFAULT){
        key_timer = m_tick;
		key = m_app_key.key;

        for(i = 0; i < 32; i++){
			bit = BIT(i);

            if(key & bit){						//pressed
                if(!(m_key_pressed & bit)){
                    m_key_pressed |= bit;
                    key_cnt[i] = 0;
                    ret = true;
                }

                if(key_cnt[i] <= KEY_LONG_LONG_TIME){
                    key_cnt[i] ++;

					if(key_cnt[i] >= KEY_LONG_LONG_TIME){
						if(!(m_key_long_long & bit)){
							m_key_long_long |= bit;
							logd("m_key_long_long: 0x%x\n", m_key_long_long);
							ret = true;
						}                
					}else if(key_cnt[i] >= KEY_LONG_TIME){
                        if(!(m_key_long & bit)){
                            m_key_long |= bit;
                            logd("m_key_long=%x\n",m_key_long);
                            ret = true;
                        }
                    }else if(key_cnt[i] >= KEY_SHORT_TIME){
                        if(!(m_key_short & bit)){
                            m_key_short |= bit;
                            ret = true;
                        }
                    }
                }
            }else{												//key up
                if((m_key_pressed & bit)){
                    if( 0 == ((m_key_pressed_b | m_key_double_b) & bit) ){
                        if(key_cnt[i] < KEY_SHORT_TIME){
                            if(m_key_pre_double_b & bit){
                                m_key_double_b |= bit;
                                m_key_pre_double_b &= ~bit;
                            }else{
                                m_key_pre_double_b |= bit;
                            }
                        }else{
                            m_key_pre_double_b &= ~bit;
                        }
                    }

                    m_key_pressed &= ~bit;
                    m_key_short &= ~bit;
                    m_key_long &= ~bit;
                    m_key_long_long &= ~bit;
                    ret = true;
                    key_cnt[i] = 0;
                }else if(m_key_pre_double_b & bit){
                    key_cnt[i]++;
                    if(key_cnt[i] >= KEY_DOUBLE_B_TIME){
                        key_cnt[i] = 0;
                        m_key_pre_double_b &= ~bit;
                        m_key_pressed_b |= bit;
                        //logd("m_key_pressed_b\n");
                        ret = true;
                    }
                }else if(((m_key_pressed_b | m_key_double_b) & bit)){
                    key_cnt[i]++;
                    if(key_cnt[i] >= KEY_PRESSED_B_DELAY){
                        key_cnt[i] = 0;
                        m_key_pressed_b &= ~bit;
                        m_key_double_b &= ~bit;
                        ret = true;
                    }
                }
            }
        }

        if(ret){
            app_key_event();
        }

        if(!m_key_power_on){
            if(!KEY_POWER && !((m_key_pressed_b|m_key_double_b) & HW_KEY_POWER)){
                m_key_power_on = true;
            }
        }
    }
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_key_init(void)
{
    memset(&m_app_key, 0, sizeof(m_app_key));
    m_key_power_on = false;
    m_app_stick_key = 0;
    m_key_pressed = 0;
    m_key_short = 0;
    m_key_pressed_b = 0;
    m_key_long = 0;
    m_key_long_long = 0;
    m_key_double_b = 0;		    
    m_key_pre_double_b = 0;			
	return io_key_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_key_deinit(void)
{
	return io_key_deinit();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_key_handler(void)
{
	app_key_scan();
    app_key_decode();
}








#endif




