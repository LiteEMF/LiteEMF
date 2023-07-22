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


#ifndef _rgb_driver_h
#define _rgb_driver_h
#include "emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef WS2811_HIGH				//demo
#define	WS2811_HIGH  0xFE	
#endif
#ifndef WS2811_LOW
#define WS2811_LOW 	 0xE0
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/



/*****************************************************************************************************
**  Function
******************************************************************************************************/
void spi_rgb_set_buf(uint8_t* frame, uint8_t len, uint8_t* spi_tx_buf, uint8_t spi_size);

bool rgb_driver_show(uint8_t* frame, uint8_t len);	//__WEAK 	
bool rgb_driver_init(void);							//__WEAK 	
bool rgb_driver_deinit(void);						//__WEAK 		



#ifdef __cplusplus
}
#endif
#endif





