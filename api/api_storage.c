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
	|storage flash map:						|
	|------------------page1----------------|
	|m_storage		| storage_map[0]		|
	|storage_map[1]						|
	|------------------page2----------------|
	|storage_map[1] | storage_map[2]	|
	|------------------page3----------------|
	|storage_map[2] | storage_map[3]	|
************************************************************************************************************/
#include "hw_config.h"

#include  "api/api_storage.h"
#include  "api/api_tick.h"
#include  "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static uint8_t s_map_index;					//current m_storage_map index
static bool is_stg_auto_sync;
static timer_t storage_timer;

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
api_storage_t m_storage;
uint8_t m_storage_map[STORAGE_MAP_SIZE];


/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*******************************************************************
** Parameters:	lenp:expectation merge
** Returns:	bool: true:merge, false:no data merge
			lenp: available merge_len
** Description:	merge user buf to page_buf
*******************************************************************/
static bool storage_merge_buf(uint16_t page_addr,uint8_t* page_buf,uint16_t map_addr, uint8_t* buf,uint16_t* lenp)
{
	bool ret = false;
	uint16_t page_end,map_offset,merge_len=0;

	page_end = page_addr + API_FLASH_PAGE_SIZE;
	if((map_addr >= page_addr) && (map_addr < page_end)){	//in page
		map_offset = map_addr - page_addr;
		merge_len = MIN(*lenp, (page_end - map_addr));

		if(memcpy(page_buf+map_offset,buf,merge_len)){
			memcpy(page_buf+map_offset,buf,merge_len);
			ret = true;
		}
	}
	*lenp = merge_len;
	return ret;
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	
*******************************************************************/
bool api_storage_read_map(uint8_t index,uint8_t* map, uint16_t map_len)
{
	return api_flash_read(STORAGE_MAP_ADDR(index),map, MIN(map_len,STORAGE_MAP_SIZE) );
}

bool api_storage_write_map(uint8_t index,uint8_t* map_buf, uint16_t map_len)
{
	bool ret = false,merge;
    uint8_t i;
	uint16_t page_addr,page_end;										//page address

	uint8_t* stg_buf;													//m_storage 
	uint16_t stg_addr,stg_len,stg_write_len;							
	uint16_t map_addr,map_write_len;									//storage map

	uint8_t tmp[MIN(STORAGE_SIZE, API_FLASH_PAGE_SIZE)];				//Optimize memory

	stg_addr = 0;
	stg_buf = (uint8_t*)&m_storage;
	stg_len = sizeof(m_storage);

	map_addr = STORAGE_MAP_ADDR(index);
	map_len = MIN(map_len,STORAGE_MAP_SIZE);							//limit map_len

	for(i=0; i < API_FLASH_PAGE_NUM; i++){
		page_addr = i*API_FLASH_PAGE_SIZE;
		page_end = page_addr + API_FLASH_PAGE_SIZE;
		if(STORAGE_SIZE <= page_addr){
			break;								//flash page not used
		}

		if(((stg_addr >= page_addr) && (stg_addr < page_end))
			|| ((map_addr >= page_addr) && (map_addr < page_end))){		//in page

			ret = api_flash_read(page_addr,tmp,sizeof(tmp));
			if(ret){
				stg_write_len = stg_len;
				merge = storage_merge_buf(page_addr, tmp, stg_addr, stg_buf, &stg_write_len);
				map_write_len = map_len;
				merge = storage_merge_buf(page_addr, tmp, map_addr, map_buf, &map_write_len);
				if(merge){
					ret = api_flash_erase(page_addr);
					ret &= api_flash_write(page_addr,tmp,sizeof(tmp));
					if(ret){
						stg_addr += stg_write_len;
						stg_buf += stg_write_len;
						stg_len -= stg_write_len;

						map_addr += map_write_len;
						map_buf += map_write_len;
						map_len -= map_write_len;
						logd("sync%d...\n",i);
					}
				}else{
					logd("no need sync%d...\n",i);
				}
			}
			if(!ret){
				loge("sync%d err...\n",i);
				break;
			}
		}
	}
	return ret;
}

uint8_t api_storage_map_index(void)
{
	return s_map_index;
}
bool api_storage_set_map(uint8_t index)
{
	bool ret = false;
	ret = api_storage_sync();
	if(ret){
		ret = api_flash_read(STORAGE_MAP_ADDR(index),m_storage_map, sizeof(m_storage_map));
		s_map_index = index;
	}
	
	return ret;
}
bool api_storage_sync(void)
{
	bool ret = false;
    ret = api_storage_write_map(s_map_index, m_storage_map, sizeof(m_storage_map));
	is_stg_auto_sync = !ret;
	storage_timer = m_systick;
	return ret;
}
bool api_storage_sync_complete(void)
{
	return true;
}


void api_storage_auto_sync(void)
{
    storage_timer = m_systick;
	is_stg_auto_sync = true;
}

bool api_storage_init(void)
{
	bool ret;
    s_map_index = 0;
	is_stg_auto_sync = false;
	ret = api_flash_read(0,(uint8_t*)&m_storage,sizeof(m_storage));
	ret &= api_flash_read(STORAGE_MAP_ADDR(0),m_storage_map, sizeof(m_storage_map) );		//default map index 0
	if(!ret){
		loge_r("storage init error!\n");
	}
	return ret;
}
void api_storage_handler(void)
{
    if(is_stg_auto_sync){
        if(m_systick - storage_timer >= API_STORAGE_TIME){
            if(api_storage_sync()){
            	is_stg_auto_sync = false;
			}
        }
    }
}







