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


#ifndef _usbh_socket_h
#define _usbh_socket_h
#include "emf_typedef.h" 
#include "api/api_transport.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern trp_handle_t usbh_socket_trp;
extern bool usbh_socket_configured;


/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool usbh_socket_art_cmd(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len);
bool usbh_socket_arg_decode(trp_handle_t* phandle,uint8_t cmd,uint16_t dev_type,uint8_t* buf,uint16_t len);
void usbh_socket_init(trp_handle_t* phandle, uint16_t dev_type);

#ifdef __cplusplus
}
#endif
#endif





