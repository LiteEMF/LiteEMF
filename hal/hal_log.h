/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/


#ifndef _hal_log_h
#define _hal_log_h
#include "emf_typedef.h"
#include "sdk_hal_log.h"


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

#if defined   (__C51__) 		//Note c51 log must be on one line
	#if INFO_LOG_ENABLE && LOG_ENABLE
		#define logi          	HAL_PRINTF
		#define dumpi         	dump
	#else
		#define logi          	/##/
		#define dumpi			/##/
	#endif

	#if DEBUG_LOG_ENABLE && LOG_ENABLE
		#define logd          	HAL_PRINTF
		#define dumpd         	dump
	#else
		#define logd          	/##/
		#define dumpd			/##/
	#endif

	#if ERR_LOG_ENABLE && LOG_ENABLE
		#define loge          	HAL_PRINTF
		#define dumpe         	dump
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
	#define loge_r        logi
	#define loge_g        logi
	#define loge_b        logi
	#define loge_y        logi
#else
	#if INFO_LOG_ENABLE && LOG_ENABLE
		#define logi(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
        #define dumpi                   dump
	#else
		#define logi(fmt,args...)
        #define dumpi(fmt,args...)
	#endif

	#if DEBUG_LOG_ENABLE && LOG_ENABLE
		#define logd(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
		#define dumpd                   dump
	#else
		#define logd(fmt,args...)
        #define dumpd(fmt,args...)
	#endif

	#if ERR_LOG_ENABLE && LOG_ENABLE
		#define loge(format, ...)       HAL_PRINTF(format , ## __VA_ARGS__)
		#define dumpe                   dump
	#else
		#define loge(fmt,args...)
		#define dumpe(fmt,args...)
	#endif

	#if ( __CC_ARM )
		#define logi_r(x, ...)  logi("\x1B[2;31m" x "\x1B[0m", ## __VA_ARGS__)
		#define logi_g(x, ...)  logi("\x1B[2;32m"x"\x1B[0m", ## __VA_ARGS__)
		#define logi_b(x, ...)  logi("\x1B[2;34m"x"\x1B[0m", ## __VA_ARGS__)
		#define logi_y(x, ...)  logi("\x1B[2;33m"x"\x1B[0m", ## __VA_ARGS__)
		#define logd_r(x, ...)  logd("\e[31m\e[1m"x"\x1B[0m", ## __VA_ARGS__)
		#define logd_g(x, ...)  logd("\x1B[2;32m"x"\x1B[0m", ## __VA_ARGS__)
		#define logd_b(x, ...)  logd("\x1B[2;34m"x"\x1B[0m", ## __VA_ARGS__)
		#define logd_y(x, ...)  logd("\x1B[2;33m"x"\x1B[0m", ## __VA_ARGS__)
		#define loge_r(x, ...)  loge("\e[31m\e[1m"x"\x1B[0m", ## __VA_ARGS__)
		#define loge_g(x, ...)  loge("\x1B[2;32m"x"\x1B[0m", ## __VA_ARGS__)
		#define loge_b(x, ...)  loge("\x1B[2;34m"x"\x1B[0m", ## __VA_ARGS__)
		#define loge_y(x, ...)  loge("\x1B[2;33m"x"\x1B[0m", ## __VA_ARGS__)
	#else
		#define logi_r(x, ...)  logi("\e[31m\e[1m" x "\e[0m", ## __VA_ARGS__)
		#define logi_g(x, ...)  logi("\e[32m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logi_b(x, ...)  logi("\e[34m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logi_y(x, ...)  logi("\e[33m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logd_r(x, ...)  logd("\e[31m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logd_g(x, ...)  logd("\e[32m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logd_b(x, ...)  logd("\e[34m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define logd_y(x, ...)  logd("\e[33m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define loge_r(x, ...)  loge("\e[31m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define loge_g(x, ...)  loge("\e[32m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define loge_b(x, ...)  loge("\e[34m\e[1m"x"\e[0m", ## __VA_ARGS__)
		#define loge_y(x, ...)  loge("\e[33m\e[1m"x"\e[0m", ## __VA_ARGS__)
	#endif
#endif

#ifdef  ASSERT_ENABLE
	#define EMF_ASSERT(expr) ((expr) ? 	(void)0U : assert_failed((void const *)__FILE__, __LINE__))
#else
	#if defined   (__C51__)
		#define EMF_ASSERT(expr)        /##/
	#else
		#define EMF_ASSERT(expr) 		((void)0U)
	#endif
#endif


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void dump(void const *buf, uint16_t len);				//WEAK
void assert_failed(void const* file, uint32_t line);	//WEAK


#ifdef __cplusplus
}
#endif
#endif





