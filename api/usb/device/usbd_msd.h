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


#ifndef _usbd_msd_h
#define _usbd_msd_h
#include "utils/emf_typedef.h" 
#include "api/usb/usb_msd_typedef.h"
#include "api/usb/device/usbd_core.h"
#include "api/usb/device/usbd_msd_scsi.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef USBD_RAM_DISK_ENABLE
#define USBD_RAM_DISK_ENABLE	0
#endif

#ifndef USBD_DISK_BLOCK_SIZE
#define USBD_DISK_BLOCK_SIZE	512
#endif
#ifndef USBD_DISK_BLOCK_NUM				//FAT12 min 8 , FAT32 min 64
#define USBD_DISK_BLOCK_NUM	    64
#endif

#ifndef USBD_MSC_BLOCK_SIZE             //MIN endp mtu, moust 512
#define USBD_MSC_BLOCK_SIZE		64
#endif

#ifndef CONFIG_USBDEV_MSC_MANUFACTURER_STRING
#define CONFIG_USBDEV_MSC_MANUFACTURER_STRING ""
#endif

#ifndef CONFIG_USBDEV_MSC_PRODUCT_STRING
#define CONFIG_USBDEV_MSC_PRODUCT_STRING ""
#endif

#ifndef CONFIG_USBDEV_MSC_VERSION_STRING
#define CONFIG_USBDEV_MSC_VERSION_STRING "0.01"
#endif

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
/* MSC Bulk-only Stage */
typedef enum {
    MSC_READ_CBW = 0, /* Command Block Wrapper */
    MSC_DATA_OUT = 1, /* Data Out Phase */
    MSC_DATA_IN = 2,  /* Data In Phase */
    MSC_SEND_CSW = 3, /* Command Status Wrapper */
    MSC_WAIT_CSW = 4, /* Command Status Wrapper */
}msc_stage_t;

/* Device data structure */
typedef struct _msc_cfg {
    /* state of the bulk-only state machine */
    msc_stage_t stage;
    msc_cbw_t cbw;
    msc_csw_t csw;

    uint8_t readonly;
    uint8_t sKey; /* Sense key */
    uint8_t ASC;  /* Additional Sense Code */
    uint8_t ASQ;  /* Additional Sense Qualifier */
    uint8_t max_lun;
    uint32_t start_sector;
    uint32_t nsectors;
    uint16_t sector_offset;
    uint16_t scsi_blk_size;	//flash block size must
    uint32_t scsi_blk_nbr;	//flash block num

    uint16_t block_index;
    uint8_t block_buffer[USBD_MSC_BLOCK_SIZE];
} msc_cfg_t;


extern msc_cfg_t usbd_msc_cfg;
/*****************************************************************************************************
**  Function
******************************************************************************************************/
error_t usbd_msd_reset(uint8_t id);
error_t usbd_msd_suspend(uint8_t id);
uint16_t usbd_msd_get_itf_desc(uint8_t id, itf_ep_index_t *pindex,uint8_t *pdesc, uint16_t desc_len, uint16_t *pdesc_index);
error_t usbd_msd_control_request_process(uint8_t id, usbd_class_t *pclass, usbd_req_t* const preq);
error_t usbd_msd_out_process(uint8_t id, usbd_class_t* pclass);
void 	usbd_msd_process(uint8_t id, usbd_class_t *pclass, usbd_event_t evt, uint32_t val);
error_t usbd_msd_init(uint8_t id);
error_t usbd_msd_deinit(uint8_t id);


#ifdef __cplusplus
}
#endif
#endif





