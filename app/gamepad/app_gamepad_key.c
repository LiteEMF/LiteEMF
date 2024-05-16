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
#if APP_GAMEAPD_ENABLE

#include  "app/gamepad/app_gamepad_key.h"

#include  "api/api_tick.h"

#if APP_JOYSTICK_ENABLE
#include  "app/app_joystick.h"
#endif
#if APP_IMU_ENABLE
#include  "app/imu/app_imu.h"
#endif

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
uint8_t  m_app_stick_key;
app_gamepad_key_t m_gamepad_key;


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
uint8_t get_stick_dir(axis2i_t* stickp, int16_t threshold)
{
    uint8_t dir=0;
 
    if(stickp->x > threshold){
        dir |= KEY_DIR_RIGHT;
    }else if(stickp->x < -threshold){
        dir |= KEY_DIR_LEFT;
    }
    if(stickp->y > threshold){
        dir |= KEY_DIR_UP;
    }else if(stickp->y < -threshold){
        dir |= KEY_DIR_DOWN;
    }
    return dir;
}

void get_stick_val(uint8_t dir, axis2i_t* stickp)
{   
    switch (dir & 0x0f){
        case KEY_DIR_UP:
            stickp->y = INT16_MAX;
            break;
        case KEY_DIR_DOWN:
            stickp->y = INT16_MIN;
            break;
        case KEY_DIR_RIGHT:
            stickp->x = INT16_MAX;
            break;
        case KEY_DIR_LEFT:
            stickp->x = INT16_MIN;
            break;
        case (KEY_DIR_UP | KEY_DIR_RIGHT):
            stickp->y = 23170;
            stickp->x = 23170;
            break;
        case (KEY_DIR_UP | KEY_DIR_LEFT):
            stickp->y = 23170;
            stickp->x = -23170;
            break;
        case (KEY_DIR_DOWN | KEY_DIR_RIGHT):
            stickp->y = -23170;
            stickp->x = 23170;
            break;
        case (KEY_DIR_DOWN | KEY_DIR_LEFT):
            stickp->y = -23170;
            stickp->x = -23170;
            break;
        default:
            stickp->y = 0;
            stickp->x = 0;
            break;
    }
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_gamepad_trigger_check(app_gamepad_key_t* keyp)
{
	if((keyp->key & HW_KEY_L2) && (0 == keyp->l2)){
		keyp->l2 = 0x7fff;
	}
	if((0 == (keyp->key & HW_KEY_L2)) && keyp->l2){
		keyp->key |= HW_KEY_L2;
	}

	if((keyp->key & HW_KEY_R2) && (0 == keyp->r2)){
		keyp->r2 = 0x7fff;
	}
	if((0 == (keyp->key & HW_KEY_R2)) && keyp->r2){
		keyp->key |= HW_KEY_R2;
	}
}

void app_gamepad_key_swapl(app_gamepad_key_t* keyp)
{
	keyp->key = SWAP32_L(keyp->key);
    keyp->l2 = SWAP16_L(keyp->l2);
    keyp->r2 = SWAP16_L(keyp->r2);
	axis2i_swapl(&keyp->stick_l);
	axis2i_swapl(&keyp->stick_r);
	axis3i_swapl(&keyp->acc);
	axis3i_swapl(&keyp->gyro);
}

#if WEAK_ENABLE
__WEAK void app_gamepad_key_vendor_scan(app_gamepad_key_t *keyp)
{
	
}
#endif

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_key_init(void)
{
    memset(&m_gamepad_key, 0, sizeof(m_gamepad_key));
    m_app_stick_key = 0;	
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_key_deinit(void)
{
	return true;
}


void app_gamepad_key_scan_task(app_gamepad_key_t *pkey)
{
    app_gamepad_key_t app_key;
    uint32_t hw_key=0;

    memset(&app_key,0,sizeof(app_key));
    #if APP_KEY_ENABLE
    app_key.key = m_key_scan;
    #endif

    #if APP_JOYSTICK_ENABLE
    app_key.stick_l = m_joystick.stick[APP_STICK_L_ID];
    app_key.stick_r = m_joystick.stick[APP_STICK_R_ID];
    app_key.l2 = m_joystick.tarigger[APP_TRIGGER_L_ID];
    app_key.r2 = m_joystick.tarigger[APP_TRIGGER_R_ID];
    #endif

    app_gamepad_key_vendor_scan(&app_key);	
    app_gamepad_trigger_check(&app_key);

    #if APP_JOYSTICK_ENABLE
    m_app_stick_key = get_stick_dir(&app_key.stick_l, STICK_DIR_THRESHOLD);
    m_app_stick_key |= get_stick_dir(&app_key.stick_r, STICK_DIR_THRESHOLD)<<4;
    #endif

    #if APP_IMU_ENABLE
    app_imu_get_val(&app_key.acc,&app_key.gyro);
    #else
    app_key.acc.z = 0XFFFF/8;         //default 8G acc
    #endif

    #if APP_KEY_ENABLE
    if(pkey->key != app_key.key){
        app_key_dump(app_key.key);
    }
    #endif
    
    *pkey = app_key;
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void app_gamepad_key_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		app_gamepad_key_scan_task(&m_gamepad_key);
	}
}
#endif






#endif




