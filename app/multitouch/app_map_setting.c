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
#include "hw_config.h"
#if 1
#include "app/multitouch/app_map_setting.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


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
void app_map_swap_l(mt_map_t* mapp, uint16_t size)
{
	#ifdef __BIG_ENDIAN
	uint16_t i;

	for(i=0; i<size/sizeof(mt_map_t); i++){
		mapp->t = SWAP16_L(mapp->t);
		mapp->x = SWAP16_L(mapp->x);
		mapp->y = SWAP16_L(mapp->y);
		mapp->k = SWAP32_L(mapp->k);
	}
	#endif
}

void app_map_dump(mt_map_t* mapp, uint16_t size)
{
	uint16_t i,count = size/sizeof(mt_map_t);

	logd("mt map len=%d\n",count);
	for(i=0; i<count; i++){
		logd("mode=%x,att=%x,r=%4x,t=%4x,k=%8x,x=%3x,y=%3x\n",
			mapp[i].mode,mapp[i].att,mapp[i].r,mapp[i].t,mapp[i].k,mapp[i].x,mapp[i].y);
	}
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
mt_map_t* find_map_by_id(uint16_t id, api_storage_map_t* app_map)
{
	if(id >= app_map->map_len / sizeof(mt_map_t)){
		return NULL;
	}
	return (mt_map_t*)app_map->map + id;
}
/*******************************************************************
** Parameters:	att:0xff 匹配任意属性	
** Returns:	
** Description:		
*******************************************************************/
uint16_t find_map_by_key(uint32_t key, uint8_t att, api_storage_map_t* app_map)
{
	uint16_t id = ID_NULL;
	mt_map_t* mapp;

	for(id=0; id<app_map->map_len/sizeof(mt_map_t); id++){
		mapp = (mt_map_t*)app_map->map + id;

		if((mapp->k == key) &&( (0xff == att) || (mapp->att == att))){
			return id;
		}
	}
	return id;
}


bool app_map_add(mt_map_t* mapp, api_storage_map_t* app_map)
{
	if(app_map->max_len - STORAGE_MAP_HEAD_LEN < (uint8_t*)app_map->map_len + sizeof(mt_map_t)){
		return false;
	}
	memcpy((uint8_t*)app_map->map + app_map->map_len, mapp, sizeof(mt_map_t));
	app_map->map_len += sizeof(mt_map_t);
	app_map->crc = api_storage_calculate_crc(app_map);

	return true;
}



bool app_map_remove(mt_map_t* mapp, api_storage_map_t* app_map)
{
	uint16_t len,map_offset;
	if(NULL == mapp){
		return false;
	}

	len = sizeof(mt_map_t);
	map_offset = (uint8_t*)mapp - (app_map->map + len);
	//当前宏定义删除,后面的数据覆盖拷贝到前面
	memmvoe(mapp, (uint8_t*)mapp + len, app_map->map_len - map_offset);
	app_map->map_len -= len;
	app_map->crc = api_storage_calculate_crc(app_map);
}

bool app_map_remove_by_id(uint16_t id, api_storage_map_t* app_map)
{
	mt_map_t* mapp = find_map_by_id(id, app_map);
	if(NULL == mapp){
		return false;
	}
	return app_map_remove(mapp, app_map);
}

bool app_map_replace_by_id(uint16_t id, mt_map_t* mapp, api_storage_map_t* app_map)
{
	mt_map_t* old_mapp = find_map_by_id(id, app_map);
	if(NULL == mapp){
		return false;
	}

	memcpy(old_mapp, mapp, sizeof(mt_map_t));
	app_map->crc = api_storage_calculate_crc(app_map);
}



#endif
