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


#ifndef _hw_config_h
#define _hw_config_h

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define LOG_ENABLE                      1

#define PROJECT_KM                  0           //keyboard and mouse project
#define PROJECT_DEMO                1           //keyboard and mouse

#if PROJECT_KM

#elif PROJECT_DEMO
	#define DEMO1					1
	#if DEMO1
		#define SW_VERSION                     	0x01
        #define DEFAULT_NAME			       	"demo01"
        #define DEFAULT_MODEL					"DEMO01"
	#else

	#endif
#endif

#include "emf_config.h"
#include "hw_board.h"
#ifdef __cplusplus
}
#endif
#endif





