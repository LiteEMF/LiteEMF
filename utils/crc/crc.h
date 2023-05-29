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
#include  "utils/emf_typedef.h" 
#include  "hw_config.h"
#include  "crc_poly.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef  CRC8_TABLE_EANBLE
#define  CRC8_TABLE_EANBLE 		1
#endif
#ifndef  CRC16_EANBLE
#define  CRC16_EANBLE 			1
#endif
#ifndef  CRC16_TABLE16_EANBLE			//small flash size
#define  CRC16_TABLE16_EANBLE 	0
#endif
#ifndef  CRC32_TABLE_EANBLE
#define  CRC32_TABLE_EANBLE 	1
#endif



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern uint8_t  crc8(const void *buf, uint32_t len); 
extern uint16_t crc16(const void *buf, uint32_t len); 
extern uint32_t crc32(const void* buf, uint32_t len);


#ifdef __cplusplus
}
#endif
#endif





