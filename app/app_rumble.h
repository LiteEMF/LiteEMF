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


#ifndef _app_rumble_h
#define _app_rumble_h
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef RUMBLE_MIN_DEFAULT
#define RUMBLE_MIN_DEFAULT			0
#endif

#ifndef APP_RUMBLE_NUM
#define APP_RUMBLE_NUM				4
#endif

#ifndef APP_RUMBLE_SYNC_TIMES
#define APP_RUMBLE_SYNC_TIMES		3
#endif
/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct	
{
    uint8_t mode;
    uint8_t duty[APP_RUMBLE_NUM];
} rumble_t;


typedef struct
{   
    int16_t timeout;         
    uint8_t percent;
    uint8_t min;
} rumble_ctb_t;

extern rumble_t	m_rumble;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool app_rumble_show(void);                      //__WEAK
void app_rumble_set_pa(uint8_t id, uint8_t min,uint8_t percent);
void app_rumble_set_duty(uint8_t id,uint8_t duty,uint16_t timeout_ms);
void app_set_rumble(rumble_t *rumblep,uint16_t timeout_ms);
bool app_rumble_init(void);
bool app_rumble_deinit(void);
void app_rumble_handler(void);

#ifdef __cplusplus
}
#endif
#endif





