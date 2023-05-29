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
#include  "utils/check_sum.h"

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
uint32_t check_sum(const void *buf, uint32_t len)
{
	uint32_t i;
	uint32_t sum = 0;
	uint8_t *p = (uint8_t*)buf;

	for(i=0; i<len; i++){
		sum += *(p+i);
	}
	return sum;
}



uint32_t check_sum_negative(const void *buf, uint32_t len)
{
	uint32_t sum;
	sum = check_sum(buf, len);
	sum = (~sum) + 1;		//取反+1
	return sum;
}





