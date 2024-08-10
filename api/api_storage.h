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

#ifndef STORAGE_MAP_SIZE
#define STORAGE_MAP_SIZE			8
#endif	
#ifndef STORAGE_LIST_MAP_NUM
#define STORAGE_LIST_MAP_NUM		1
#endif	
#ifndef STORAGE_LIST_MAP_SIZE
#define STORAGE_LIST_MAP_SIZE		8
#endif	
#define STORAGE_LIST_MAP_ADDR(i)	(sizeof(m_storage) + (i)*(STORAGE_LIST_MAP_SIZE))
#define STORAGE_TOTAL_SIZE			(sizeof(m_storage) + (STORAGE_LIST_MAP_NUM)*(STORAGE_LIST_MAP_SIZE))



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
#define STORAGE_MAP_HEAD_LEN		8
typedef  struct {	
	uint16_t crc;					//crc 包括len 和 map
	uint16_t res;					//res 0
	uint16_t max_len;				//map_max len, 需要在map初始化的时候设置正确的值后不要修改这个值
	uint16_t map_len;				//map[] len, 当前map中数据有效长度
	uint8_t map[4];					//注意这里只做为占位使用,具体大小根据len和实际空间分配为准	
}api_storage_map_t;

typedef struct {
	uint8_t head;
	uint8_t reset_reson;	
	uint8_t list_map_index;		//storage map index
	uint8_t flash_res[5];	//8
	uint16_t trps;
	uint16_t dev_mode;
	uint16_t hid_mode;
	uint8_t res[10];		//24

	#if APP_JOYSTICK_ENABLE
	int16_t joystick_cal[19];	
	int16_t res1[1];			//arm 不支持__ALIGN(4)修饰符?
	#endif

	#if APP_IMU_ENABLE
	int16_t imu_cal[6];
	#endif

	#if BT_MODIFY_NAME_ENABLE
	uint8_t device_name_len;
	char device_name[31];
	#endif

	uint8_t map[STORAGE_MAP_SIZE];

	#ifdef HW_STORAGE
	HW_STORAGE;
	#endif
}api_storage_t;



extern api_storage_t m_storage;
extern api_storage_map_t* const m_storage_list_map;
extern api_storage_map_t* const m_storage_map;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_storage_check_map(api_storage_map_t* mapp, uint16_t len);
bool api_storage_set_crc(api_storage_map_t* mapp, uint16_t map_len);
uint16_t api_storage_calculate_crc(api_storage_map_t* mapp);
bool api_storage_read_map(uint8_t index,uint8_t* map, uint16_t map_len);
bool api_storage_write_map(uint8_t index,uint8_t* map_buf, uint16_t map_len);
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





