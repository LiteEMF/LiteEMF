/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "hw_config.h"
#include  "api/api_log.h"


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void api_dump(void const *buf, uint16_t len)
{
	#if LOG_ENABLE
	if(!hal_dump(buf,len)){
		uint16_t i;
		uint8_t* p = (uint8_t* )buf;

		if(0 == len) return;

		for(i=0; i<len; i++){
			#if defined   (__C51__)
			HAL_PRINTF("%02bx ",p[i]);
			#else
			HAL_PRINTF("%02x ",p[i]);
			#endif

		}
		HAL_PRINTF("\r\n");
	}
	#endif
}









