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

#ifndef	_API_SOFT_SPI_H_
#define _API_SOFT_SPI_H_
#include "emf_typedef.h" 
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
enum soft_spi_mode {
    SOFT_SPI_MODE_BIDIR_1BIT,    //支持SPIx(x=0,1,2)，全双工，di接收，do发送
    SOFT_SPI_MODE_UNIDIR_1BIT,   //支持SPIx(x=0,1,2)，半双工，do分时发送/接收
    SOFT_SPI_MODE_UNIDIR_2BIT,   //支持SPIx(x=0)，半双工，di & do共2bit分时发送/接收
    SOFT_SPI_MODE_UNIDIR_4BIT,   //支持SPIx(x=0)，半双工，di & do & wp & hold 共4bit分时发送/接收
};


/*****************************************************************************************************
**  Function
******************************************************************************************************/
int soft_spi_open();
void soft_spi_close();
int soft_spi_send_byte(uint8_t byte);
uint8_t soft_spi_recv_byte(int *err);
uint8_t soft_spi_send_recv_byte( uint8_t byte, int *err);
int soft_spi_dma_recv(void *buf, uint32_t len);
int soft_spi_dma_send(const void *buf, uint32_t len);

void soft_spi_set_bit_mode(int data_wide);
int soft_spi_dma_2bitrecv(void *buf, uint32_t len);

void soft_spi_writeReg(uint8_t addr, uint8_t dat);
uint8_t soft_spi_readReg(uint8_t addr);
void soft_spi_readRegs(uint8_t addr, uint8_t *buf, uint8_t len);


#ifdef __cplusplus
}
#endif
#endif
