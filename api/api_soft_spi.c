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
#if API_SOFT_SPI_ENABLE
#include "api/api_soft_spi.h"
#include "api/api_gpio.h"
#include "utils/emf_utils.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
//port select for soft spi
#define A_CLK_BIT           			BIT(1)// set clk
#define A_CLK_PORT(x)       			JL_PORTA->x
#define A_D0_BIT            			BIT(15)// set d0
#define A_D0_PORT(x)        			JL_PORTA->x
#define A_D1_BIT            			BIT(14)// set d1
#define A_D1_PORT(x)        			JL_PORTA->x
#define A_DATA_WIDE_MODE                SOFT_SPI_MODE_UNIDIR_1BIT   //只支持双向或单线


#define A_CS_BIT           			    BIT(13)// set cs
#define A_CS_PORT(x)       			    JL_PORTA->x
#define SOFTSPI_CS_PORT       			IO_PORTA_13
#define softspi_cs_h()                  api_gpio_out(SOFTSPI_CS_PORT, 1)
#define softspi_cs_l()                  api_gpio_out(SOFTSPI_CS_PORT, 0)

#define bit_delay_r()  asm("nop");asm("nop");asm("nop");asm("nop");
#define bit_delay_w()  asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
struct _dat_bit {
    uint8_t bit0 : 1;
    uint8_t bit1 : 1;
    uint8_t bit2 : 1;
    uint8_t bit3 : 1;
    uint8_t bit4 : 1;
    uint8_t bit5 : 1;
    uint8_t bit6 : 1;
    uint8_t bit7 : 1;
};


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
static void soft_spi_io_port_init(void)
{
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    A_CLK_PORT(DIR) &= ~A_CLK_BIT;
    A_CLK_PORT(DIE) |= A_CLK_BIT;
    A_CLK_PORT(PU) &= ~A_CLK_BIT;
    A_CLK_PORT(PD) &= ~A_CLK_BIT;
    A_D0_PORT(OUT) |= A_D0_BIT;
    A_D0_PORT(DIR) &= ~A_D0_BIT;
    A_D0_PORT(DIE) |= A_D0_BIT;
    A_D0_PORT(PU) |= A_D0_BIT;
    A_D0_PORT(PD) &= ~A_D0_BIT;
}
static void soft_spi_io_port_uninit(void)
{
    A_CLK_PORT(DIR) |= A_CLK_BIT;
    A_CLK_PORT(DIE) &= ~A_CLK_BIT;
    A_CLK_PORT(PU) &= ~A_CLK_BIT;
    A_CLK_PORT(PD) &= ~A_CLK_BIT;
    A_D0_PORT(DIR) |= A_D0_BIT;
    A_D0_PORT(DIE) &= ~A_D0_BIT;
    A_D0_PORT(PU) &= ~A_D0_BIT;
    A_D0_PORT(PD) &= ~A_D0_BIT;
}

void soft_spi_set_bit_mode(int data_wide)
{

    switch (data_wide) {
    case SOFT_SPI_MODE_BIDIR_1BIT:
        A_D0_PORT(OUT) |= A_D0_BIT;
        A_D0_PORT(DIR) &= ~A_D0_BIT;
        A_D0_PORT(DIE) |= A_D0_BIT;
        A_D0_PORT(PU) |= A_D0_BIT;
        A_D0_PORT(PD) &= ~A_D0_BIT;
        A_D1_PORT(DIR) |= A_D1_BIT;
        A_D1_PORT(DIE) |= A_D1_BIT;
        A_D1_PORT(PU) &= ~A_D1_BIT;
        A_D1_PORT(PD) &= ~A_D1_BIT;
        break;
    case SOFT_SPI_MODE_UNIDIR_1BIT:
        break;
    case SOFT_SPI_MODE_UNIDIR_2BIT:
        A_D0_PORT(DIR) |= A_D0_BIT;
        A_D0_PORT(DIE) |= A_D0_BIT;
        A_D0_PORT(PU) &= ~A_D0_BIT;
        A_D0_PORT(PD) &= ~A_D0_BIT;
        break;
    }
    
}
static void soft_spi_cs_port_init(void)
{
    A_CS_PORT(OUT) |= A_CS_BIT;
    A_CS_PORT(DIR) &= ~A_CS_BIT;
    A_CS_PORT(DIE) |= A_CS_BIT;
    A_CS_PORT(PU) &= ~A_CS_BIT;
    A_CS_PORT(PD) &= ~A_CS_BIT;
}
int soft_spi_open()
{
    soft_spi_io_port_init();
    soft_spi_set_bit_mode(SOFT_SPI_MODE_BIDIR_1BIT);
    soft_spi_cs_port_init();
    logd("soft spi open");
    return 0;
}

void soft_spi_close()
{
    uint8_t data_wide_mode = 0;
    data_wide_mode = A_DATA_WIDE_MODE;

    switch (data_wide_mode) {
    case SOFT_SPI_MODE_BIDIR_1BIT:
    case SOFT_SPI_MODE_UNIDIR_2BIT:
        A_D1_PORT(DIR) |= A_D1_BIT;
        A_D1_PORT(DIE) &= ~A_D1_BIT;
        A_D1_PORT(PU) &= ~A_D1_BIT;
        A_D1_PORT(PD) &= ~A_D1_BIT;
        break;
    case SOFT_SPI_MODE_UNIDIR_1BIT:
        break;
    }
    soft_spi_io_port_uninit();
}

/* mode0: CPOL = 0, CPHA = 0, MSB first, clk_l, smp_edge_rise*/
/* mode3: CPOL = 1, CPHA = 1, MSB first, clk_h, smp_edge_rise */
static void soft_spi_a_w_mode03(uint8_t write_dat)
{
    struct _dat_bit *_dat = (struct _dat_bit *)&write_dat;

    local_irq_disable();
    _dat->bit7 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit6 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit5 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit4 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit3 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit2 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit1 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    _dat->bit0 ? A_D0_PORT(OUT) |= A_D0_BIT : (A_D0_PORT(OUT) &= ~A_D0_BIT);
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    bit_delay_w();
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    local_irq_enable();
}
/* mode0: CPOL = 0, CPHA = 0, MSB first, clk_l, smp_edge_rise*/
/* mode3: CPOL = 1, CPHA = 1, MSB first, clk_h, smp_edge_rise */
static uint8_t soft_spi_a_r_mode03()
{
    uint8_t read_dat = 0;
    struct _dat_bit *_dat = (struct _dat_bit *)&read_dat;

    local_irq_disable();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit7 = 1 : (_dat->bit7 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit6 = 1 : (_dat->bit6 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit5 = 1 : (_dat->bit5 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit4 = 1 : (_dat->bit4 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit3 = 1 : (_dat->bit3 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit2 = 1 : (_dat->bit2 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit1 = 1 : (_dat->bit1 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit0 = 1 : (_dat->bit0 = 0);
    asm("nop");asm("nop");
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    local_irq_enable();

    return read_dat;
}

static uint8_t soft_spi_a_r_mode2bit()
{
    uint8_t read_dat = 0;
    struct _dat_bit *_dat = (struct _dat_bit *)&read_dat;

    local_irq_disable();     
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D0_PORT(IN)& A_D0_BIT) ? _dat->bit6 = 1 : (_dat->bit6 = 0);
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit7 = 1 : (_dat->bit7 = 0);
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D0_PORT(IN)& A_D0_BIT) ? _dat->bit4 = 1 : (_dat->bit4 = 0);
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit5 = 1 : (_dat->bit5 = 0);
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r();
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D0_PORT(IN)& A_D0_BIT) ? _dat->bit2 = 1 : (_dat->bit2 = 0);
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit3 = 1 : (_dat->bit3 = 0);
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    bit_delay_r(); 
    A_CLK_PORT(OUT) |=  A_CLK_BIT;
    (A_D0_PORT(IN)& A_D0_BIT) ? _dat->bit0 = 1 : (_dat->bit0 = 0);
    (A_D1_PORT(IN)& A_D1_BIT) ? _dat->bit1 = 1 : (_dat->bit1 = 0);
    A_CLK_PORT(OUT) &= ~A_CLK_BIT;
    local_irq_enable();

    return read_dat;
}

int soft_spi_send_byte(uint8_t byte)
{
    
    soft_spi_a_w_mode03(byte);

    return 0;
}

uint8_t soft_spi_recv_byte(int *err)
{
    uint8_t rx_data = 0;

    rx_data = soft_spi_a_r_mode03();

    err != NULL ? *err = 0 : 0;
    return rx_data;
}

/*
 * @brief spi dma
 * @parm spi  spi
 * @parm buf
 * @parm len
 * @return < 0
 */
int soft_spi_dma_recv(void *buf, uint32_t len)
{
    uint8_t *buf_temp = (uint8_t *)buf;

    while (len--) {
        *buf_temp = soft_spi_a_r_mode03();
        buf_temp++;
    }

    return len;
}

int soft_spi_dma_send(const void *buf, uint32_t len)
{
    uint8_t *buf_temp = (uint8_t *)buf;

    while (len--) {
        soft_spi_a_w_mode03(*buf_temp);
        buf_temp++;
    }
    
    return len;
}

int soft_spi_dma_2bitrecv(void *buf, uint32_t len)
{
    uint8_t *buf_temp = (uint8_t *)buf;

    while (len--) {
        *buf_temp = soft_spi_a_r_mode2bit();
        buf_temp++;
    }

    return len;
}

void soft_spi_writeReg(uint8_t addr, uint8_t dat)
{
    softspi_cs_l();
    soft_spi_a_w_mode03(addr);
    soft_spi_a_w_mode03(dat);
    softspi_cs_h();
    
}
uint8_t soft_spi_readReg(uint8_t addr)
{
    uint8_t buf_temp;

    softspi_cs_l();
    soft_spi_a_w_mode03(0x80 | addr);
    buf_temp = soft_spi_a_r_mode03();
    softspi_cs_h();
    
    return buf_temp;
}
void soft_spi_readRegs(uint8_t addr, uint8_t *buf, uint8_t len)
{
    uint8_t i;

    softspi_cs_l();
    soft_spi_a_w_mode03(0x80 | addr);
    for(i=0; i<len; i++)
	{
        buf[i] = soft_spi_a_r_mode03();
    }
    softspi_cs_h();
    
}

#endif
