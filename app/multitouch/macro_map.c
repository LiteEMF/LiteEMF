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
**	Description:宏定义map 管理,动态管理多套宏定义配置:
api_storage_map_t:
	|map_len , map_crc						 |
	|------------------macro1_info-----------|
	|------------------macro 1---------------|
	|------------------macro2_inf------------|
	|------------------macro 2---------------|
	|------------------macro 2---------------|
	|------------------macro 2---------------|
	|------------------macro3_info-----------|
	|------------------macro 3---------------|
	|------------------macro 3---------------|
	|------------------macro 4---------------|
************************************************************************************************************/
#include "hw_config.h"
#if 1
#include "app/multitouch/macro_map.h"
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



/******************************************************************
** Parameters:macro_id:宏定义
			 index:宏定义运行步骤,0:下标0为宏定义信息,1:宏动作信息	
** Returns:	
** Description:	获取宏定义当前动作map
*******************************************************************/
mt_map_t* find_macro_map_by_id(uint16_t macro_id, uint16_t index, api_storage_map_t* macro_map)
{
	uint16_t id;	
	mt_map_t* mapp = (mt_map_t*)macro_map->map;
	uint16_t map_cnt = macro_map->map_len / sizeof(mt_map_t);
	mt_map_t* mapp_end = mapp + map_cnt;
	do{
		if(MT_MACRO_INOF == mapp->mode ){								//查找宏定义信息头
			id = mapp->t;
			map_cnt = mapp->x/sizeof(mt_map_t);
			// logd("macro_id=%d id=%d cnt=%d map=%x %x",macro_id,id, map_cnt, mapp + map_cnt, mapp_end);
			if((id == macro_id) && (mapp + map_cnt <= mapp_end)){		//匹配id,保证len不溢出
				if(index < map_cnt){						//index 有效
					return mapp + index;
				}else{
					break;
				}
			}else{
				mapp += map_cnt;
			}
		}else{			//防止异常数据
			mapp++;
		}
	}while(mapp < mapp_end);

	return NULL;
}

mt_map_t* get_next_macro(mt_map_t* mapp, api_storage_map_t* macro_map)
{
	uint16_t len = mapp->x;	

	if((uint8_t*)mapp + len < (uint8_t*)macro_map->map + macro_map->map_len){
		return (uint8_t*)mapp + len;
	}else{
		return NULL;
	}
}
/*******************************************************************
** Parameters:macrop:要添加的宏动作, 
			macro_map:宏动作map表
			map_size:map大小
** Returns:	
** Description:		
*******************************************************************/
bool macro_map_add(mt_map_t* macrop, api_storage_map_t* macro_map)
{
	uint8_t i; 
	uint16_t id,len;	
	len = macrop->x;

	if(macro_map->max_len - STORAGE_MAP_HEAD_LEN < (uint8_t*)macro_map->map_len + len){
		return false;
	}
	memcpy((uint8_t*)macro_map->map + macro_map->map_len, macrop, len);
	macro_map->map_len += len;
	macro_map->crc = api_storage_calculate_crc(macro_map);

	return true;
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool macro_map_remove(uint16_t macro_id, api_storage_map_t* macro_map)
{
	uint16_t len,map_offset;
	mt_map_t* mapp = find_macro_map_by_id(macro_id, 0, macro_map);
	if(NULL == mapp){
		return false;
	}

	len = mapp->x;
	map_offset = (uint8_t*)mapp - ((uint8_t*)macro_map->map + len);
	//当前宏定义删除,后面的数据覆盖拷贝到前面
	memmove(mapp, (uint8_t*)mapp + len, macro_map->map_len - map_offset);
	macro_map->map_len -= len;
	macro_map->crc = api_storage_calculate_crc(macro_map);

	return true;
}
#endif
