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

#include "app/emf.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
int  main( int  argc,  char  *argv[])  
{
	static timer_t timer=0;
	uint32_t i;
	logd("main...\n");

	emf_init();
	
	// crc_test();
	// soft_timer_test();
	// hid_desc_parser_test();
	// mem_test();
	// api_commander_test();
	// app_rgb_test();

	while(0){
		i=0x400000;
		while(i--);
		m_systick++;
		if(m_systick - timer >= 1000){
			timer = m_systick;
			logd("t=%d\n", m_systick);
		}
		if(m_systick > 10 * 1000){
			break;
		}
		emf_handler(0);
	}

	return 0;
}