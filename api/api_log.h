/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/


#ifndef _api_log_h
#define _api_log_h
#include "utils/emf_typedef.h" 
#include "hw_config.h"
#include "hal_log.h"


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef LOG_ENABLE
#define LOG_ENABLE      		1
#endif
#ifndef DEBUG_LOG_ENABLE
#define DEBUG_LOG_ENABLE     	1
#endif
#ifndef INFO_LOG_ENABLE
#define INFO_LOG_ENABLE      	1
#endif
#ifndef ERR_LOG_ENABLE
#define ERR_LOG_ENABLE			1
#endif

#ifndef HAL_PRINTF
#define HAL_PRINTF				printf
#endif

#if defined   (__C51__) 		//Note c51 log must be on one line
	#if INFO_LOG_ENABLE && LOG_ENABLE
		#define logi          	HAL_PRINTF
		#define dumpi         	api_dump
	#else
		#define logi          	/##/
		#define dumpi			/##/
	#endif

	#if DEBUG_LOG_ENABLE && LOG_ENABLE
		#define logd          	HAL_PRINTF
		#define dumpd         	api_dump
	#else
		#define logd          	/##/
		#define dumpd			/##/
	#endif

	#if ERR_LOG_ENABLE && LOG_ENABLE
		#define loge          	HAL_PRINTF
		#define dumpe         	api_dump
	#else
		#define loge          	/##/
		#define dumpe			/##/
	#endif
	#define logi_r        logi
	#define logi_g        logi
	#define logi_b        logi
	#define logi_y        logi
	#define logd_r        logd
	#define logd_g        logd
	#define logd_b        logd
	#define logd_y        logd
	#define loge_r        loge
	#define loge_g        loge
	#define loge_b        loge
	#define loge_y        loge
#else
	#if INFO_LOG_ENABLE && LOG_ENABLE
		#define logi(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
        #define dumpi                   api_dump
	#else
		#define logi(fmt,args...)
        #define dumpi(fmt,args...)
	#endif

	#if DEBUG_LOG_ENABLE && LOG_ENABLE
		#define logd(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
		#define dumpd                   api_dump
	#else
		#define logd(fmt,args...)
        #define dumpd(fmt,args...)
	#endif

	#if ERR_LOG_ENABLE && LOG_ENABLE
		#define loge(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
		#define dumpe                   api_dump
	#else
		#define loge(fmt,args...)
		#define dumpe(fmt,args...)
	#endif

	#if defined(__CC_ARM) && (__CC_ARM)
		// ANSI控制字符定义
		#define ANSI_RESET          "\x1b[0m"
		#define ANSI_BOLD           "\x1b[1m"
		#define ANSI_RED            "\x1b[31m"
		#define ANSI_GREEN          "\x1b[32m"
		#define ANSI_YELLOW         "\x1b[33m"
		#define ANSI_BLUE           "\x1b[34m"
		#define ANSI_MAGENTA        "\x1b[35m"
		#define ANSI_CYAN           "\x1b[36m"
		#define ANSI_WHITE          "\x1b[37m"
	#else
		// ANSI控制字符定义
		#define ANSI_RESET          "\e[0m"
		#define ANSI_BOLD           "\e[30m\e[1m"
		#define ANSI_RED            "\e[31m\e[1m"
		#define ANSI_GREEN          "\e[32m\e[1m"
		#define ANSI_YELLOW         "\e[33m\e[1m"
		#define ANSI_BLUE           "\e[34m\e[1m"
		#define ANSI_MAGENTA        "\e[35m\e[1m"
		#define ANSI_CYAN           "\e[36m\e[1m"
		#define ANSI_WHITE          "\e[37m\e[1m"
	#endif

	#define logi_r(x, ...)  logi(ANSI_RED x ANSI_RESET, 	## __VA_ARGS__)
	#define logi_g(x, ...)  logi(ANSI_GREEN x ANSI_RESET, 	## __VA_ARGS__)
	#define logi_b(x, ...)  logi(ANSI_BLUE x ANSI_RESET, 	## __VA_ARGS__)
	#define logi_y(x, ...)  logi(ANSI_YELLOW x ANSI_RESET, 	## __VA_ARGS__)
	#define logd_r(x, ...)  logd(ANSI_RED x ANSI_RESET, 	## __VA_ARGS__)
	#define logd_g(x, ...)  logd(ANSI_GREEN x ANSI_RESET, 	## __VA_ARGS__)
	#define logd_b(x, ...)  logd(ANSI_BLUE x ANSI_RESET, 	## __VA_ARGS__)
	#define logd_y(x, ...)  logd(ANSI_YELLOW x ANSI_RESET, 	## __VA_ARGS__)
	#define loge_r(x, ...)  loge(ANSI_RED x ANSI_RESET, 	## __VA_ARGS__)
	#define loge_g(x, ...)  loge(ANSI_GREEN x ANSI_RESET, 	## __VA_ARGS__)
	#define loge_b(x, ...)  loge(ANSI_BLUE x ANSI_RESET, 	## __VA_ARGS__)
	#define loge_y(x, ...)  loge(ANSI_YELLOW x ANSI_RESET, 	## __VA_ARGS__)

#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern void api_dump(void const *buf, uint16_t len);				//WEAK

//hal
extern bool hal_dump(void const *buf, uint16_t len);


#ifdef __cplusplus
}
#endif
#endif





