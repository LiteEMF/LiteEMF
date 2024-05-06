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
bool app_map_add(mt_map_t* mapp, api_storage_map_t* app_map, uint16_t app_map_size)
{

}
bool app_map_remove(uint16_t id, api_storage_map_t* app_map, uint16_t app_map_size)
{

}

bool app_map_remove_by_key(uint32_t key, api_storage_map_t* app_map, uint16_t app_map_size)
{

}



#endif
