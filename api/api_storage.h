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


#ifndef _api_storage_h
#define _api_storage_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "api/api_flash.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef API_STORAGE_TIME			//ms
#define API_STORAGE_TIME   (100*100)
#endif

#ifndef STORAGE_MAP_NUM
#define STORAGE_MAP_NUM		1
#endif	
#ifndef STORAGE_MAP_SIZE
#define STORAGE_MAP_SIZE	0X04
#endif	
#define STORAGE_SIZE		(sizeof(m_storage) + STORAGE_MAP_NUM*STORAGE_MAP_SIZE)
#define STORAGE_MAP_ADDR(i)	(sizeof(m_storage) + i*STORAGE_MAP_SIZE)



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct {
	uint8_t head;
	uint8_t reset_reson;	
	uint8_t map_index;		//storage map index
	uint8_t flash_res[5];	//8
	uint16_t trps;
	uint16_t dev_mode;
	uint16_t hid_mode;

	uint8_t res[10];		//24

	#if APP_JOYSTICK_ENABLE
	int16_t joystick_cal[19];
	#endif

	#if APP_IMU_ENABLE
	int16_t imu_cal[6];
	#endif
	
	#ifdef HW_STORAGE
	HW_STORAGE;
	#endif
}api_storage_t;


typedef  struct {	
	uint16_t len;				//map len
	uint16_t crc;				//map crc
	uint8_t map[STORAGE_MAP_SIZE-4];
}api_storage_map_t;




extern api_storage_t m_storage;
extern api_storage_map_t m_storage_map;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_storage_check_map(api_storage_map_t* mapp);
bool api_storage_read_map(uint8_t index,uint8_t* map, uint16_t map_len);
bool api_storage_write_map(uint8_t index,uint8_t* map_buf, uint16_t map_len);
uint8_t api_storage_map_index(void);
bool api_storage_set_map(uint8_t index, bool sync);
void api_storage_auto_sync(void);
bool api_storage_sync(void);
bool api_storage_sync_complete(void);
bool api_storage_init(void);
void api_storage_sync_task(void* pa);
void api_storage_handler(uint32_t period_10us);

#ifdef __cplusplus
}
#endif
#endif





