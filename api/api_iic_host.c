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
#ifdef HW_IIC_MAP
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "api/api_iic_host.h"
#include  "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static volatile bool os_iic_busy = true;			//used for os
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
const_t iic_map_t m_iic_map[] = HW_IIC_MAP;          
uint8c_t m_iic_num = countof(m_iic_map);

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
#if IIC_SOFT_ENABLE
static bool start(uint8_t id)
{
    IIC_SCK_OUT(id,1);
	IIC_DELAY(id);
    IIC_SDA_DIR(id,PIN_IN);

    if(!IIC_SDA_IN(id)){
        return 0;
    }
    IIC_SDA_OUT(id,0);
	IIC_SDA_DIR(id,PIN_OUT); //preparation for the first transfer.

	IIC_DELAY(id);
    IIC_SCK_OUT(id,0);

	IIC_DELAY(id);
	return 1;
}
static bool stop(uint8_t id)
{
    IIC_SDA_OUT(id,0);
	IIC_SDA_DIR(id,PIN_OUT);

	IIC_DELAY(id);
	IIC_SCK_OUT(id,1);
	IIC_DELAY(id);
	IIC_SDA_OUT(id,1);
	IIC_DELAY(id);

    IIC_SDA_DIR(id,PIN_IN); 
    if(!IIC_SDA_IN(id)){
        return 0;
    }
    return 1;
}
static void write_bit(uint8_t id,bool bit){
    /* prepare data */
    IIC_SDA_OUT(id,bit);
    IIC_DELAY(id);
    /* set scl high to indicate a new vaild sda bit */
    IIC_SCK_OUT(id,1);
    /* wait for SDA value to be read by slave. */
    IIC_DELAY(id);
    /* clear the scl to low in preparation for the next*/
    IIC_SCK_OUT(id,0);
}
static bool read_bit(uint8_t id){

    bool ret;
    /* wait for sda line to be written by slave */
    IIC_DELAY(id);
    IIC_SCK_OUT(id,1);
    IIC_DELAY(id);
    ret = IIC_SDA_IN(id);
    IIC_SCK_OUT(id,0);
    return ret;
}
static bool write(uint8_t id,uint8_t buf)
{
    bool ack;
	uint8_t i;
    IIC_SDA_DIR(id,PIN_OUT);
    for(i=0;i<8;i++){
        write_bit(id,buf&0x80);
        buf<<=1;
    }
    IIC_SDA_DIR(id,PIN_IN);
	#ifdef IIC_ACK_TIMEOUT
    for(i=0; i<IIC_ACK_TIMEOUT; i++){
        if(!IIC_SDA_IN(id)){
            break;
        }
		delay_us(1);
    }
	#endif
    ack = !read_bit(id);

    return ack;
}

static uint8_t read(uint8_t id,bool nack)
{
    uint8_t i;
    uint8_t buf = 0;
    IIC_SDA_DIR(id,PIN_IN);
    for(i = 0; i < 8; i++){
        buf = (buf<<1) | read_bit(id);
    }
    IIC_SDA_DIR(id,PIN_OUT);

    write_bit(id,nack);

    return buf;
}

static bool write_dev_addr(uint8_t id,uint8_t addr)
{
	bool ret = false;
	uint8_t i;
 	for(i = 0; i < IIC_RETRY; i++){
        if(start(id)){
			ret = write(id,addr);
			if(ret) break;
        }
        IIC_DELAY(id);
        logd("iic retry %d\n", i);
    }
	return ret;
}

static bool soft_iic_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len)
{
	uint8_t i;
	bool ret = false;

	ret = write_dev_addr(id,dev_addr);
	if(!ret) goto _err;

	if(addr & 0xFF00){
		ret = write(id,addr>>8);
		if(!ret) goto _err;
	}
	ret = write(id,addr);
	if(!ret) goto _err;
	
	for(i=0; i<len; i++){
		ret = write(id,buf[i]);
		if(!ret) goto _err;
	}
_err:
	ret &= stop(id);						//write ret & stop ret

	return ret;
}



static bool soft_iic_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len)
{
	uint8_t i;
    bool ret = false;

    if(len == 0) return ret;

	ret = write_dev_addr(id,dev_addr);
	if(!ret) goto _err;

	if(addr & 0xFF00){
		ret = write(id,addr>>8);
		if(!ret) goto _err;
	}
	ret = write(id,addr);
	if(!ret) goto _err;
	ret = write_dev_addr(id,dev_addr|1);
	if(!ret) goto _err;

	for(i=0; i<len -1; i++){
		buf[i] = read(id,0);
	}
	buf[i] = read(id,1); 					//最后一个数据不发送应答信号
_err:
	ret &= stop(id);						//write ret & stop ret

	return ret;
}
#endif
/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool api_iic_host_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len)
{
    bool ret = false;

	if(os_iic_busy) return ret;
	os_iic_busy = true;

	api_gpio_out(m_iic_map[id].cs, 0);			//cs select

    #if IIC_SOFT_ENABLE
		ret = soft_iic_write(id, dev_addr, addr, buf, len);
	#else
    	ret = hal_iic_write(id,dev_addr, addr, buf, len);
    #endif

	api_gpio_out(m_iic_map[id].cs, 1);			//cs select
	os_iic_busy = false;

    return ret;
}

bool api_iic_host_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len)
{
    bool ret = false;

    if(len == 0) return ret;
	if(os_iic_busy) return ret;
	os_iic_busy = true;

	api_gpio_out(m_iic_map[id].cs, 0);			//cs select

    #if IIC_SOFT_ENABLE
		ret = soft_iic_read(id, dev_addr, addr, buf, len);
	#else
    	ret = hal_iic_read(id, dev_addr, addr, buf, len);
    #endif
    
	api_gpio_out(m_iic_map[id].cs, 1);			//cs select
	os_iic_busy = false;
	return ret;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:	中断/DMA写
*******************************************************************/
bool api_iic_host_isr_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len)
{
    bool ret = false;

	if(os_iic_busy) return ret;
	os_iic_busy = true;

	api_gpio_out(m_iic_map[id].cs, 0);				//cs select

    #if IIC_SOFT_ENABLE
		ret = soft_iic_write(id, dev_addr, addr, buf, len);
		api_gpio_out(m_iic_map[id].cs, 1);			//cs select
		os_iic_busy = false;
	#else
    	ret = hal_iic_isr_write(id,dev_addr, addr, buf, len);
    #endif

    return ret;
}


bool api_iic_host_isr_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len)
{
    bool ret = false;

    if(len == 0) return ret;
	if(os_iic_busy) return ret;
	os_iic_busy = true;
	api_gpio_out(m_iic_map[id].cs, 0);			//cs select

    #if IIC_SOFT_ENABLE
		ret = soft_iic_read(id, dev_addr, addr, buf, len);
		api_gpio_out(m_iic_map[id].cs, 1);			//cs select
		os_iic_busy = false;
	#else
    	ret = hal_iic_isr_read(id, dev_addr, addr, buf, len);
    #endif

	return ret;
}

/*******************************************************************
** Parameters:	err: 读写成功/失败信息	
** Returns:	
** Description:	中断/DMA 读写中断处理,用户需要在hal 中断中调用该函数
*******************************************************************/
#if WEAK_ENABLE
__WEAK void api_iic_host_isr_hook(uint8_t id,error_t err)
{
	api_gpio_out(m_iic_map[id].cs, 1);			//cs select
	os_iic_busy = false;
	if(err) logd("iic%x=%d\n",id,err);
}
#endif

bool api_iic_host_scan(uint8_t id) 
{
	uint8_t addr;
	bool ret = false;
  	logi("iic(%d) scan\n",(uint16_t)id);

	for ( addr = 1; addr < 0x80; addr++) {
		ret = api_iic_host_write(id,addr<<1,0, NULL,0);
		if (ret) break;
	}

    if(ret){
		logi("api iic found :0x%x\n",(uint16_t)addr);;
	}else{
		logi("api iic not found\n");
	}
	return ret;
}

bool api_iic_host_init(uint8_t id)
{
	bool ret = true;

	api_gpio_out(m_iic_map[id].cs, 1);
	api_gpio_out(m_iic_map[id].clk, 1);
	api_gpio_out(m_iic_map[id].sda, 1);
	api_gpio_dir(m_iic_map[id].cs,  PIN_OUT, PIN_PULLNONE);
	api_gpio_dir(m_iic_map[id].clk, PIN_OUT, PIN_PULLNONE);
	#if GPIO_OD_EN
	api_gpio_dir(m_iic_map[id].sda, PIN_OUT, PIN_PULL_OD);
	#else
	api_gpio_dir(m_iic_map[id].sda, PIN_OUT, PIN_PULLNONE);
	#endif

	#if !IIC_SOFT_ENABLE
	ret = hal_iic_init(id);
	#endif

	os_iic_busy = false;

	return ret;
}

bool api_iic_host_deinit(uint8_t id)
{
	return hal_iic_deinit(id);
}
void api_iics_init(void)
{
	uint8_t id;
	for(id = 0; id < m_iic_num; id++){
		api_iic_host_init(id);
	}   
}
void api_iics_deinit(void)
{
	uint8_t id;
	for(id = 0; id < m_iic_num; id++){
		api_iic_host_deinit(id);
	}   
}

#endif




