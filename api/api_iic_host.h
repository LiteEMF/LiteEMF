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

#ifndef _api_iic_host_h
#define _api_iic_host_h
#include "emf_typedef.h" 
#include "hw_config.h"
#include "hal_iic_host.h"
#include "api/api_gpio.h"
#include "api/api_tick.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef IIC_SOFT_ENABLE
#define  IIC_SOFT_ENABLE  		0
#endif

#define IIC_BADU_POS		(0)
#define IIC_BADU_MASK		0X0000FFFF		//kHZ
#define IIC_RES_POS			(16)
#define IIC_RES_MASK		0XFFFF0000

#ifndef IIC_BADU_ATT			//api layout fix do not fix in hal_pwm
#define IIC_BADU_ATT(id)  	(FLD2VAL(IIC_BADU, m_iic_map[id].att))
#endif


#ifndef IIC_DELAY			//TODO check 是否优化代码
#define  IIC_DELAY(id)  	delay_ns(1000000/2/IIC_BADU_ATT(id)) 	//ns, 约400KHZ, use offset for code run delay
#endif

#ifndef IIC_RETRY
#define  IIC_RETRY  		3
#endif
#define IIC_ACK_TIMEOUT		5	//应答超时,us


#ifndef IIC_SDA_DIR
	#if GPIO_OD_EN
	#define IIC_SDA_DIR(id,dir)		if(PIN_IN==dir) api_gpio_out(m_iic_map[id].sda,1)
	#else
	#define IIC_SDA_DIR(id,dir)     api_gpio_dir(m_iic_map[id].sda,dir,PIN_PULLUP) 
	#endif
#endif

#ifndef IIC_SDA_IN
#define IIC_SDA_IN(id)        !!api_gpio_in(m_iic_map[id].sda)
#endif
#ifndef IIC_SDA_OUT
#define IIC_SDA_OUT(id,val)    api_gpio_out(m_iic_map[id].sda,val)
#endif
#ifndef IIC_SCK_OUT
#define IIC_SCK_OUT(id,val)  	api_gpio_out(m_iic_map[id].clk,val)
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
typedef struct{
	pin_t clk;
	pin_t sda;
	pin_t cs;
	uint32_t peripheral;
	uint32_t att;
} iic_map_t;

extern const_t iic_map_t m_iic_map[];          
extern uint8c_t m_iic_num;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool api_iic_host_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len);
bool api_iic_host_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len);
bool api_iic_host_isr_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len);
bool api_iic_host_isr_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len);
void api_iic_host_isr_hook(uint8_t id,error_t err);			//__WEAK

bool api_iic_host_scan(uint8_t id);
bool api_iic_host_init(uint8_t id);
bool api_iic_host_deinit(uint8_t id);
void api_iics_init(void);
void api_iics_deinit(void);



//hal
bool hal_iic_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len);
bool hal_iic_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len);
bool hal_iic_isr_write(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t const *buf, uint16_t len);
bool hal_iic_isr_read(uint8_t id,uint8_t dev_addr,uint16_t addr, uint8_t* buf, uint16_t len);
bool hal_iic_init(uint8_t id);
bool hal_iic_deinit(uint8_t id);

#ifdef __cplusplus
}
#endif
#endif





