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


#ifndef _api_spi_h
#define _api_spi_h
#include "emf_typedef.h"
#include "hw_config.h"
#include "hal/hal_spi.h"
#include "api/api_gpio.h"
#include "api/api_tick.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef SPI_SOFT_ENABLE
#define SPI_SOFT_ENABLE    1
#endif


#define SPI_BADU_POS		(0)
#define SPI_BADU_MSK		0X0000FFFF		//kHZ
#define SPI_READ_BIT_POS	(16)
#define SPI_READ_BIT_MSK	0X00FF0000		//0~7, spi read addr bit 
#define SPI_RES_POS			(24)
#define SPI_RES_MSK			0XFF000000


#ifndef SPI_DELAY
#define SPI_DELAY(id)  		(1000000/2/FLD2VAL(SPI_BADU, m_spi_map[id].att) - 100) 	//ns, 约400KHZ, use offset for code run delay
#endif

#ifndef SPI_READ_ADDR
#define SPI_READ_ADDR(id)  	(BIT(FLD2VAL(SPI_READ_BIT, m_spi_map[id].att)))
#endif


#ifndef SPI_SCLK
#define SPI_SCLK(id,x)    api_gpio_out(m_spi_map[id].clk ,x) 	//output
#endif
#ifndef SPI_MOSI
#define SPI_MOSI(id,x)    api_gpio_out(m_spi_map[id].mosi,x)	//output
#endif
#ifndef SPI_MISO
#define SPI_MISO(id)      api_gpio_in(m_spi_map[id].miso)
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	pin_t clk;
	pin_t mosi;
	pin_t miso;
	pin_t cs;
	uint32_t peripheral;
	uint32_t att;
} spi_map_t;

extern const_t spi_map_t m_spi_map[];          
extern uint8c_t m_spi_num;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_spi_write(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len);
bool api_spi_read(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len);
bool api_spi_init(uint8_t id);
bool api_spi_deinit(uint8_t id);
void api_spis_init(void);
void api_spis_deinit(void);

//hal
bool hal_spi_write(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len);
bool hal_spi_read(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len);
bool hal_spi_init(uint8_t id);
bool hal_spi_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





