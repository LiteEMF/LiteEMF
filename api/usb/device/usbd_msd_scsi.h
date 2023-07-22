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


#ifndef _usbd_msd_scsi_h
#define _usbd_msd_scsi_h
#include "emf_typedef.h" 
#include "api/usb/usb_msd_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
error_t usbd_msc_sector_read(uint8_t lun, uint32_t sector, uint32_t offset, uint8_t *buffer, uint32_t length);	//__WEAK
error_t usbd_msc_sector_write(uint8_t lun, uint32_t sector, uint32_t offset, uint8_t *buffer, uint32_t length);	//__WEAK
void mass_storage_bulk_out(uint8_t id, uint8_t mtu, uint8_t* buf, uint16_t len);
void mass_storage_bulk_in(uint8_t id,uint8_t mtu );


/*****************************************************************************************************
**  Function
******************************************************************************************************/


#ifdef __cplusplus
}
#endif
#endif





