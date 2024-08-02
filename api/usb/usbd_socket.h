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


#ifndef _usbd_socket_h
#define _usbd_socket_h
#include "emf_typedef.h" 
#include "api/api_transport.h"
#include "app/app_commander.h"
#include "api/usb/device/usbd_core.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/



/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
extern trp_handle_t usbd_socket_trp;
extern bool usbd_socket_en;

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool usbd_socket_cmd(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len);
bool usbd_socket_decode(trp_handle_t* phandle,uint8_t cmd,uint8_t* buf,uint16_t len);
error_t usbd_socket_control_request_process(uint8_t id, usbd_req_t* const preq);
void usbd_socket_init(void);

#ifdef __cplusplus
}
#endif
#endif





