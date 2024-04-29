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
	|storage_map[1]							|
	|------------------page2----------------|
	|storage_map[1] | storage_map[2]		|
	|------------------page3----------------|
	|storage_map[2] | storage_map[3]		|
************************************************************************************************************/
#include "hw_config.h"
#if API_STORAGE_ENABLE
#include  "api/api_storage.h"
#include  "api/api_tick.h"
#include  "utils/emf_utils.h"
#include  "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static bool is_stg_auto_sync;
static timer_t storage_timer;

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
api_storage_t m_storage;
api_storage_map_t m_storage_map;

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

	if(0 == *lenp) return false;

	page_end = page_addr + API_FLASH_PAGE_SIZE;
	if((map_addr >= page_addr) && (map_addr < page_end)){	//in page
		map_offset = map_addr - page_addr;
		merge_len = MIN(*lenp, (page_end - map_addr));

		if(memcmp(page_buf+map_offset,buf,merge_len)){
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
** Description:	检测map中crc值
*******************************************************************/
bool api_storage_check_map(api_storage_map_t* mapp)
{
	bool ret = false;
    uint16_t crc, map_len, map_crc;

	map_len = (mapp->len);
	map_crc = (mapp->crc);
	if( (map_len < 4) || (map_len > STORAGE_MAP_SIZE-4 )) {
        loge("map len err\n");
        return ret;
    }
	#if  CRC16_EANBLE
	crc = crc16(0XFFFF,mapp->map, map_len);
	if(crc == map_crc){
		ret = true;
	}else{
        loge("map crc err:%x,%x but %x\n",map_len, map_crc, crc);
    }
	#endif
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	
*******************************************************************/
bool api_storage_read_map(uint8_t index,uint8_t* map, uint16_t map_len)
{
	if(STORAGE_MAP_NUM <= index) return false;

	return api_flash_read(STORAGE_MAP_ADDR(index),map, MIN(map_len,STORAGE_MAP_SIZE) );
}

bool api_storage_write_map(uint8_t index, uint8_t* map_buf, uint16_t map_len)
{
	bool ret = false,merge;
	
    uint8_t i;
	uint16_t page_addr,page_end;										//page address

	uint8_t* stg_buf;													//m_storage 
	uint16_t stg_addr,stg_len,stg_write_len;							
	uint16_t map_addr,map_write_len;									//storage map

	uint8_t* tmp = emf_malloc(MIN(STORAGE_SIZE, API_FLASH_PAGE_SIZE));

	if(NULL == tmp) return false;
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
				merge |= storage_merge_buf(page_addr, tmp, map_addr, map_buf, &map_write_len);
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

	emf_free(tmp);
	return ret;
}

/*******************************************************************
** Parameters:	sync:是否保存当前配置, 1:先保存当前配置再切换新配置
							0:直接覆盖新配, 如果当前map有修改会丢失数据
** Returns:	
** Description:	切换map表配置
*******************************************************************/
bool api_storage_set_map(uint8_t index, bool sync)
{
	bool ret = !sync;

	if(STORAGE_MAP_NUM <= index) return false;

	if(sync){
		ret = api_storage_sync();
	}

	if(ret){
		ret = api_flash_read(STORAGE_MAP_ADDR(index),&m_storage_map, sizeof(m_storage_map));
		m_storage.map_index = index;
	}
	
	return ret;
}

bool api_storage_sync_complete(void)
{
	return true;
}

bool api_storage_sync(void)
{
	bool ret = false;

	logd("api_storage_sync,map=%d\n",m_storage.map_index);
    ret = api_storage_write_map(m_storage.map_index, &m_storage_map, sizeof(m_storage_map));
	is_stg_auto_sync = !ret;
	storage_timer = m_systick;
	return ret;
}

void api_storage_auto_sync(void)
{
	logd("api_storage_auto_sync\n");
    storage_timer = m_systick;
	is_stg_auto_sync = true;
}

bool api_storage_init(void)
{
	bool ret;

	is_stg_auto_sync = false;
	ret = api_flash_read(0,(uint8_t*)&m_storage,sizeof(m_storage));

	if(STORAGE_MAP_NUM <= m_storage.map_index){
		m_storage.map_index = 0;
	}
	ret &= api_flash_read(STORAGE_MAP_ADDR(m_storage.map_index),&m_storage_map, sizeof(m_storage_map) );
	if(!ret){
		loge_r("storage init error!\n");
	}
	return ret;
}



void api_storage_sync_task(void* pa)
{
	if(is_stg_auto_sync){
		logd("is_stg_auto_sync=%d\n",(uint16_t)is_stg_auto_sync);
		api_storage_sync();
	}
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_storage_handler(uint32_t period_10us)
{
	if((m_systick - storage_timer) >= period_10us/100){
		storage_timer = m_systick;
		api_storage_sync_task(NULL);
	}
}
#endif


#endif



