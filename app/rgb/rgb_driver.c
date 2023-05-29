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
#if APP_RGB_ENABLE
#include "app/rgb/rgb_driver.h"
#include "app/rgb/app_rgb.h"

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
** Parameters:	frame, size: rgb 显示缓存
			spi_tx_buf,spi_size: spi 发送串行控制显示存, spi_buf是 frame的8倍
** Returns:	
** Description:		
*******************************************************************/
void spi_rgb_set_buf(uint8_t* frame, uint8_t size, uint8_t* spi_tx_buf, uint8_t spi_size)
{
    uint8_t id, i;
	
	for(id=0; id<size; id++){
		for(id=0, i=0; i<8; i++){
			if(frame[i] & (0x80>>i)){
				spi_tx_buf[id*8 + i] = WS2811_HIGH;
			}else{
				spi_tx_buf[id*8 + i] = WS2811_LOW;
			}
		}
	}
}

#if WEAK_ENABLE
__WEAK bool rgb_driver_show(uint8_t* frame, uint8_t size)
{
	bool ret = false;
	uint8_t i;
	
	for(i=0; i<size; i++){
		//show frame[i];
		//ret = true;
	}

	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
__WEAK bool rgb_driver_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
__WEAK bool rgb_driver_deinit(void)
{
	return true;
}
#endif

#endif
