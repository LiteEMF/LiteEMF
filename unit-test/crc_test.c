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

#include "utils/crc/crc.h"
#include "api/api_log.h"

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void crc_test(void)
{
	char str[]="123456789";
	
	logd("crc8=%x\n",crc8(0,str,strlen(str)));
	logd("crc16=%x\n",crc16(0XFFFF,str,strlen(str)));
	logd("crc32=%x\n",crc32(0,str,strlen(str)));
}



