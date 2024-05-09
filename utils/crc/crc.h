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


#ifndef _crc_h
#define _crc_h
#include  "emf_typedef.h" 
#include  "hw_config.h"
#include  "crc_poly.h"

#ifdef __cplusplus
extern "C" {
#endif
//ref: https://crccalc.com/%EF%BC%89%EF%BC%9A%E8%BF%99%E6%98%AF%E4%B8%80%E4%B8%AA%E7%AE%80%E5%8D%95%E6%98%93%E7%94%A8%E7%9A%84%E5%9C%A8%E7%BA%BF

/******************************************************************************************************
** Defined
*******************************************************************************************************/
//CRC8
#define CRC8_POLY_DEFAULT 			0x07
#ifndef  CRC8_POLY			
#define  CRC8_POLY 			CRC8_POLY_DEFAULT
#endif
#ifndef  CRC8_EANBLE
#define  CRC8_EANBLE 			1
#endif

//CRC16
#define CRC16_POLY_XMODEM 			0x1021
#define CRC16_POLY_MODBUS 			0x8005
#ifndef  CRC16_POLY			
#define  CRC16_POLY 				CRC16_POLY_MODBUS
#endif
#ifndef  CRC16_EANBLE
#define  CRC16_EANBLE 			1
#endif

//CRC32
#define CRC32_POLY_DEFAULT 		0x04C11DB7		/*和CRC32_POLY_LE结果一样只是初始值不一样*/
#define CRC32_POLY_LE 			0xedb88320
#ifndef  CRC32_POLY			
#define  CRC32_POLY 			CRC32_POLY_LE
#endif
#ifndef  CRC32_EANBLE
#define  CRC32_EANBLE 			1
#endif






/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern uint8_t  crc8(uint8_t crc, const void *buf, uint32_t len); 
extern uint16_t crc16(uint16_t crc,const void *buf, uint32_t len); 
extern uint32_t crc32(uint32_t crc,const void* buf, uint32_t len);


#ifdef __cplusplus
}
#endif
#endif





