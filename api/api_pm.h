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


#ifndef _api_pm_h
#define _api_pm_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "hal/hal_pm.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef enum{
	PM_RESON_POR=0,		//power on reset
	PM_RESON_SOFT,		//soft reset
	PM_RESON_PIN,		//pin reset
	PM_RESON_VCM,		//exten isr weakup reset
	PM_RESON_WDT,
	PM_RESON_SYS,
}pm_reson_t;

extern pm_reson_t m_reset_reson;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
void api_weakup_init(void);
void api_boot(uint8_t index);
void api_reset(void);
void api_stop(void);
void api_pm_init(void);

//hal
void hal_weakup_init(void);
pm_reson_t hal_get_reset_reson(void);
void hal_boot(uint8_t index);
void hal_reset(void);
void hal_stop(void);

#ifdef __cplusplus
}
#endif
#endif





