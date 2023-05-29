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

#include "hw_config.h"
#include "hw_board.h"
#ifdef HW_SPI_HOST_MAP


/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_spi_host.h"

#include  "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static volatile bool os_spi_busy = true;			//used for os
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t spi_map_t m_spi_map[] = HW_SPI_HOST_MAP;          
uint8c_t m_spi_num = countof(m_spi_map);
/*****************************************************************************************************
**	static Function
******************************************************************************************************/

#if SPI_SOFT_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static void spi_write(uint8_t id,uint8_t buf)
{
	uint8_t i;
	for(i = 0; i < 8; i++ )
	{
		SPI_SCLK(id,0);		
		SPI_DELAY(id);
		if((buf)&0x80){
			SPI_MOSI(id,1);
		}else{
			SPI_MOSI(id,0);
		}
		buf <<= 1;
		SPI_SCLK(id,1);	
	    SPI_DELAY(id);
	}
    SPI_SCLK(id,0);
    SPI_DELAY(id);
}

static uint8_t spi_read(uint8_t id)
{
	uint8_t i,buf = 0;
	for( i = 0; i < 8; i++ ){
		SPI_DELAY(id);
		SPI_SCLK(id,1);		
		SPI_DELAY(id);
		buf <<= 1;
		if(SPI_MISO(id)){
			buf |= 0x01;		
		}else{
			buf &= 0xfe;
		}
		SPI_SCLK(id,0);	
	}	
    SPI_SCLK(id,0);
    SPI_DELAY(id);
	return buf;
}

static bool soft_spi_write(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len)
{
    bool ret = false;
	uint8_t i;

	if(addr >> 8){
		spi_write(id,addr >> 8);
	}
	spi_write(id,addr);
	for( i = 0; i < len; i++ ){	
		spi_write(id,*buf);
		buf++;
	}
	ret = true;

	return ret;
}


bool soft_spi_read(uint8_t id,uint16_t addr, uint8_t * buf, uint8_t len)
{
    bool ret = false;
	uint8_t i;

	if(addr >> 8){
		spi_write(id,addr >> 8);
	}
	spi_write(id,addr);
	for(i = 0; i < len; i++ ){
		buf[i] = spi_read(id);
	}
	ret = true;

	return ret; 
}
#endif

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_spi_host_write(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
    bool ret = false;

	if(os_spi_busy) return ret;
	os_spi_busy = true;
	api_gpio_out(m_spi_map[id].cs, 0);			//cs select

	#if !SPI_SOFT_ENABLE
		ret = hal_spi_host_write(id,addr,buf,len);
	#else
		ret = soft_spi_write(id,addr,buf,len);
	#endif
	
    api_gpio_out(m_spi_map[id].cs, 1);			//cs select
	os_spi_busy = false;

	return ret;
}


bool api_spi_host_read(uint8_t id,uint16_t addr, uint8_t * buf, uint16_t len)
{
    bool ret = false;

	if(os_spi_busy) return ret;
	os_spi_busy = true;
    api_gpio_out(m_spi_map[id].cs, 0);			//cs select

	#if !SPI_SOFT_ENABLE
		ret = hal_spi_host_read(id,addr,buf,len);
	#else
		ret = soft_spi_read(id,addr,buf,len);
	#endif

    api_gpio_out(m_spi_map[id].cs, 1);			//cs select
	os_spi_busy = false;

	return ret; 
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	中断/DMA写
*******************************************************************/
bool api_spi_host_isr_write(uint8_t id,uint16_t addr, uint8_t *buf, uint16_t len)
{
    bool ret = false;

	if(os_spi_busy) return ret;
	os_spi_busy = true;
	api_gpio_out(m_spi_map[id].cs, 0);			//cs select

	#if !SPI_SOFT_ENABLE
		ret = hal_spi_host_write(id,addr,buf,len);
	#else
		ret = soft_spi_write(id,addr,buf,len);
		api_gpio_out(m_spi_map[id].cs, 1);			//cs select
		os_spi_busy = false;
	#endif

	return ret;
}


bool api_spi_host_isr_read(uint8_t id,uint16_t addr, uint8_t *buf, uint16_t len)
{
    bool ret = false;

	if(os_spi_busy) return ret;
	os_spi_busy = true;
    api_gpio_out(m_spi_map[id].cs, 0);			//cs select

	#if !SPI_SOFT_ENABLE
		ret = hal_spi_host_read(id,addr,buf,len);
	#else
		ret = soft_spi_read(id,addr,buf,len);
		api_gpio_out(m_spi_map[id].cs, 1);			//cs select
		os_spi_busy = false;
	#endif

	return ret; 
}
/*******************************************************************
** Parameters:	err: 读写成功/失败信息	
** Returns:	
** Description:	中断/DMA 读写中断处理,用户需要在hal 中断中调用该函数
*******************************************************************/
#if WEAK_ENABLE
__WEAK void api_spi_host_isr_hook(uint8_t id,error_t err)
{
	api_gpio_out(m_spi_map[id].cs, 1);			//cs select
	os_spi_busy = false;
	// logd("spi%x=%d\n",id,err);
}
#endif



bool api_spi_host_init(uint8_t id)
{
	bool ret = true;
	api_gpio_out(m_spi_map[id].clk, 0);
	api_gpio_out(m_spi_map[id].mosi, 1);
	api_gpio_out(m_spi_map[id].cs, 1);
	api_gpio_dir(m_spi_map[id].clk, PIN_OUT, PIN_PULLNONE);
	api_gpio_dir(m_spi_map[id].mosi, PIN_OUT, PIN_PULLNONE);
	api_gpio_dir(m_spi_map[id].cs,  PIN_OUT, PIN_PULLNONE);
	api_gpio_dir(m_spi_map[id].miso,  PIN_IN,PIN_PULLUP);
	#if !SPI_SOFT_ENABLE
	ret = hal_spi_host_init(id);
	#endif
	os_spi_busy = false;
	return ret;
}

bool api_spi_host_deinit(uint8_t id)
{
	return hal_spi_host_deinit(id);
}
void api_spis_init(void)
{
	uint8_t id;
	for(id=0; id<m_spi_num; id++){
		api_spi_host_init(id);
	}   
}
void api_spis_deinit(void)
{
	uint8_t id;
	for(id=0; id<m_spi_num; id++){
		api_spi_host_deinit(id);
	}   
}


#endif





