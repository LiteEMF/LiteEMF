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

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "hw_config.h"
#if API_USBD_BIT_ENABLE && (USBD_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_MSD))
#include "api/usb/device/usbd.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

static void usbd_msc_bot_abort(uint8_t id)
{
    usbd_class_t* pclass;

	pclass = usbd_class_find_by_type(id, DEV_TYPE_MSD, DEV_TYPE_NONE);
	if(NULL == pclass) {
        loge("usbd msc abort error!\n");
        return;
    }
    
    if ((usbd_msc_cfg.cbw.bmFlags == 0) && (usbd_msc_cfg.cbw.dDataLength != 0)) {
        usbd_endp_stall(id,pclass->endpout.addr);
    }
    usbd_endp_stall(id,USB_DIR_IN_MASK | pclass->endpin.addr);

    usbd_msc_cfg.stage = MSC_READ_CBW;
}

static void usbd_msc_send_csw(uint8_t id,uint8_t CSW_Status)
{
    usbd_msc_cfg.csw.dSignature = MSC_CSW_Signature;
    usbd_msc_cfg.csw.bStatus = CSW_Status;

    /* updating the State Machine , so that we wait CSW when this
	 * transfer is complete, ie when we get a bulk in callback
	 */
    usbd_msc_cfg.stage = MSC_WAIT_CSW;
    
    logd("Send csw\n");
    usbd_class_in(id, DEV_TYPE_MSD, DEV_TYPE_NONE, (uint8_t *)&usbd_msc_cfg.csw,sizeof(msc_csw_t));
    
    usbd_msc_cfg.stage = MSC_READ_CBW;      //cesar add 
}

static void usbd_msc_send_info(uint8_t id, uint8_t *buffer, uint8_t size)
{
    size = MIN(size, usbd_msc_cfg.cbw.dDataLength);

    /* updating the State Machine , so that we send CSW when this
	 * transfer is complete, ie when we get a bulk in callback
	 */
    usbd_msc_cfg.stage = MSC_SEND_CSW;
    usbd_class_in(id, DEV_TYPE_MSD, DEV_TYPE_NONE, buffer,size);

    usbd_msc_cfg.csw.dDataResidue -= size;
    usbd_msc_cfg.csw.bStatus = CSW_STATUS_CMD_PASSED;
}
static bool SCSI_processWrite(uint8_t id,uint32_t nbytes);
static bool SCSI_processRead(uint8_t id);

/**
* @brief  SCSI_SetSenseData
*         Load the last error code in the error list
* @param  sKey: Sense Key
* @param  ASC: Additional Sense Code
* @retval none

*/
static void SCSI_SetSenseData(uint32_t KCQ)
{
    usbd_msc_cfg.sKey = (uint8_t)(KCQ >> 16);
    usbd_msc_cfg.ASC = (uint8_t)(KCQ >> 8);
    usbd_msc_cfg.ASQ = (uint8_t)(KCQ);
}

/**
 * @brief SCSI Command list
 *
 */

static bool SCSI_testUnitReady(uint8_t **ppdata, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }
    *ppdata = NULL;
    *len = 0;
    return true;
}

static bool SCSI_requestSense(uint8_t **ppdata, uint32_t *len)
{
    uint8_t data_len = SCSIRESP_FIXEDSENSEDATA_SIZEOF;
    uint8_t request_sense[SCSIRESP_FIXEDSENSEDATA_SIZEOF] = {
        0x70,
        0x00,
        0x00, /* Sense Key */
        0x00,
        0x00,
        0x00,
        0x00,
        SCSIRESP_FIXEDSENSEDATA_SIZEOF - 8,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, /* Additional Sense Code */
        0x00, /* Additional Sense Request */
        0x00,
        0x00,
        0x00,
        0x00,
    };

    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    if (usbd_msc_cfg.cbw.CB[4] < SCSIRESP_FIXEDSENSEDATA_SIZEOF) {
        data_len = usbd_msc_cfg.cbw.CB[4];
    }

    request_sense[2] = usbd_msc_cfg.sKey;
    request_sense[12] = usbd_msc_cfg.ASC;
    request_sense[13] = usbd_msc_cfg.ASQ;
#if 0
    request_sense[ 2] = 0x06;           /* UNIT ATTENTION */
    request_sense[12] = 0x28;           /* Additional Sense Code: Not ready to ready transition */
    request_sense[13] = 0x00;           /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x02;           /* NOT READY */
    request_sense[12] = 0x3A;           /* Additional Sense Code: Medium not present */
    request_sense[13] = 0x00;           /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x05;         /* ILLEGAL REQUEST */
    request_sense[12] = 0x20;         /* Additional Sense Code: Invalid command */
    request_sense[13] = 0x00;         /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x00;         /* NO SENSE */
    request_sense[12] = 0x00;         /* Additional Sense Code: No additional code */
    request_sense[13] = 0x00;         /* Additional Sense Code Qualifier */
#endif

    memcpy(*ppdata, (uint8_t *)request_sense, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_inquiry(uint8_t **ppdata, uint32_t *len)
{
    uint8_t data_len = SCSIRESP_INQUIRY_SIZEOF;

    uint8_t inquiry00[6] = {
        0x00,
        0x00,
        0x00,
        (0x06 - 4U),
        0x00,
        0x80
    };

    /* USB Mass storage VPD Page 0x80 Inquiry Data for Unit Serial Number */
    uint8_t inquiry80[8] = {
        0x00,
        0x80,
        0x00,
        0x08,
        0x20, /* Put Product Serial number */
        0x20,
        0x20,
        0x20
    };

    uint8_t inquiry[SCSIRESP_INQUIRY_SIZEOF] = {
        /* 36 */

        /* LUN 0 */
        0x00,
        0x80,
        0x02,
        0x02,
        (SCSIRESP_INQUIRY_SIZEOF - 5),
        0x00,
        0x00,
        0x00,
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', /* Product      : 16 Bytes */
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ' /* Version      : 4 Bytes */
    };

    memcpy(&inquiry[8], CONFIG_USBDEV_MSC_MANUFACTURER_STRING, strlen(CONFIG_USBDEV_MSC_MANUFACTURER_STRING));
    memcpy(&inquiry[16], CONFIG_USBDEV_MSC_PRODUCT_STRING, strlen(CONFIG_USBDEV_MSC_PRODUCT_STRING));
    memcpy(&inquiry[32], CONFIG_USBDEV_MSC_VERSION_STRING, strlen(CONFIG_USBDEV_MSC_VERSION_STRING));

    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[1] & 0x01U) != 0U) { /* Evpd is set */
        if (usbd_msc_cfg.cbw.CB[2] == 0U) {       /* Request for Supported Vital Product Data Pages*/
            data_len = 0x06;
            memcpy(*ppdata, (uint8_t *)inquiry00, data_len);
        } else if (usbd_msc_cfg.cbw.CB[2] == 0x80U) { /* Request for VPD page 0x80 Unit Serial Number */
            data_len = 0x08;
            memcpy(*ppdata, (uint8_t *)inquiry80, data_len);
        } else { /* Request Not supported */
            SCSI_SetSenseData(SCSI_KCQIR_INVALIDFIELDINCBA);
            return false;
        }
    } else {
        if (usbd_msc_cfg.cbw.CB[4] < SCSIRESP_INQUIRY_SIZEOF) {
            data_len = usbd_msc_cfg.cbw.CB[4];
        }
        memcpy(*ppdata, (uint8_t *)inquiry, data_len);
    }

    *len = data_len;
    return true;
}

static bool SCSI_startStopUnit(uint8_t **ppdata, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x1U) /* START=1 */
    {
        //SCSI_MEDIUM_UNLOCKED;
    } else if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x2U) /* START=0 and LOEJ Load Eject=1 */
    {
        //SCSI_MEDIUM_EJECTED;
    } else if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x3U) /* START=1 and LOEJ Load Eject=1 */
    {
        //SCSI_MEDIUM_UNLOCKED;
    } else {
    }

    *ppdata = NULL;
    *len = 0;
    return true;
}

static bool SCSI_preventAllowMediaRemoval(uint8_t **ppdata, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }
    if (usbd_msc_cfg.cbw.CB[4] == 0U) {
        //SCSI_MEDIUM_UNLOCKED;
    } else {
        //SCSI_MEDIUM_LOCKED;
    }
    *ppdata = NULL;
    *len = 0;
    return true;
}

static bool SCSI_modeSense6(uint8_t **ppdata, uint32_t *len)
{
    uint8_t data_len = 4;
	struct scsiresp_modeparameterhdr6_s sense6 = { 0x03, 0x00, 0x00, 0x00 };

    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }
    if (usbd_msc_cfg.cbw.CB[4] < SCSIRESP_MODEPARAMETERHDR6_SIZEOF) {
        data_len = usbd_msc_cfg.cbw.CB[4];
    }

    if (usbd_msc_cfg.readonly) {
        sense6.param = 0x80;
    }
    memcpy(*ppdata, &sense6, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_modeSense10(uint8_t **ppdata, uint32_t *len)
{
    uint8_t data_len = 27;

    uint8_t sense10[27] = {
        0x00,
        0x26,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x12,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };
	
	if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    if (usbd_msc_cfg.cbw.CB[8] < 27) {
        data_len = usbd_msc_cfg.cbw.CB[8];
    }
    memcpy(*ppdata, (uint8_t *)sense10, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_readFormatCapacity(uint8_t **ppdata, uint32_t *len)
{
    struct scsiresp_readformatcapacities_s format_capacity;

	if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }
    
    memset(&format_capacity, 0, sizeof(format_capacity));
    format_capacity.listlen = 0x08;
    format_capacity.nblocks[0] = (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 24) & 0xff);
    format_capacity.nblocks[1] = (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 16) & 0xff);
    format_capacity.nblocks[2] = (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 8) & 0xff);
    format_capacity.nblocks[3] = (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 0) & 0xff);
    format_capacity.type = 0x02; /* Descriptor Code: Formatted Media */
    format_capacity.blocklen[0] = 0;
    format_capacity.blocklen[1] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 8) & 0xff);
    format_capacity.blocklen[2] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 0) & 0xff);

    memcpy(*ppdata, &format_capacity, SCSIRESP_READFORMATCAPACITIES_SIZEOF);
    *len = SCSIRESP_READFORMATCAPACITIES_SIZEOF;
    return true;
}

static bool SCSI_readCapacity10(uint8_t **ppdata, uint32_t *len)
{
	struct scsiresp_readcapacity10_s capacity10;
	
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    capacity10.lba[0] = (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 24) & 0xff),
    capacity10.lba[1] = (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 16) & 0xff),
    capacity10.lba[2] = (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 8) & 0xff),
    capacity10.lba[3] = (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 0) & 0xff),

    capacity10.blklen[0] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 24) & 0xff),
    capacity10.blklen[1] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 16) & 0xff),
    capacity10.blklen[2] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 8) & 0xff),
    capacity10.blklen[3] = (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 0) & 0xff),

    memcpy(*ppdata, &capacity10, SCSIRESP_READCAPACITY10_SIZEOF);
    *len = SCSIRESP_READCAPACITY10_SIZEOF;
    return true;
}

static bool SCSI_read10(uint8_t id,uint8_t **ppdata, uint32_t *len)
{
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x80U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    // scsicmd_read10_s
    usbd_msc_cfg.start_sector = U32_H(&usbd_msc_cfg.cbw.CB[2]); /* Logical Block Address of First Block */
    logd("lba: 0x%04lx\n", usbd_msc_cfg.start_sector);

    usbd_msc_cfg.nsectors = U16_H(&usbd_msc_cfg.cbw.CB[7]); /* Number of Blocks to transfer */
    logd("nsectors: 0x%02lx\n", usbd_msc_cfg.nsectors);

    if ((usbd_msc_cfg.start_sector + usbd_msc_cfg.nsectors) > usbd_msc_cfg.scsi_blk_nbr) {
        SCSI_SetSenseData(SCSI_KCQIR_LBAOUTOFRANGE);
        loge("LBA out of range\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != (usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size)) {
        loge("scsi_blk_len does not match with dDataLength=%ld, blk size=%d\n",usbd_msc_cfg.cbw.dDataLength,usbd_msc_cfg.scsi_blk_size );
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_IN;
    return SCSI_processRead(id);
}

static bool SCSI_read12(uint8_t id,uint8_t **ppdata, uint32_t *len)
{
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x80U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    usbd_msc_cfg.start_sector = U32_H(&usbd_msc_cfg.cbw.CB[2]); /* Logical Block Address of First Block */
    logd("lba: 0x%04lx\n", usbd_msc_cfg.start_sector);

    usbd_msc_cfg.nsectors = U32_H(&usbd_msc_cfg.cbw.CB[6]); /* Number of Blocks to transfer */
    logd("nsectors: 0x%02lx\n", usbd_msc_cfg.nsectors);

    if ((usbd_msc_cfg.start_sector + usbd_msc_cfg.nsectors) > usbd_msc_cfg.scsi_blk_nbr) {
        SCSI_SetSenseData(SCSI_KCQIR_LBAOUTOFRANGE);
        loge("LBA out of range\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != (usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size)) {
        loge("scsi_blk_len does not match with dDataLength=%ld, blk size=%d\n",usbd_msc_cfg.cbw.dDataLength,usbd_msc_cfg.scsi_blk_size );
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_IN;
    return SCSI_processRead(id);
}

static bool SCSI_write10(uint8_t id,uint8_t **ppdata, uint32_t *len)
{
    uint32_t data_len = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    usbd_msc_cfg.start_sector = U32_H(&usbd_msc_cfg.cbw.CB[2]); /* Logical Block Address of First Block */
    logd("lba: 0x%04lx\n", usbd_msc_cfg.start_sector);

    usbd_msc_cfg.nsectors = U16_H(&usbd_msc_cfg.cbw.CB[7]); /* Number of Blocks to transfer */
    logd("nsectors: 0x%02lx\n", usbd_msc_cfg.nsectors);

    data_len = usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size;
    if ((usbd_msc_cfg.start_sector + usbd_msc_cfg.nsectors) > usbd_msc_cfg.scsi_blk_nbr) {
        loge("LBA out of range\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != data_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_OUT;
    data_len = MIN(data_len, USBD_MSC_BLOCK_SIZE);
    
    // usbd_ep_start_read(mass_ep_data[MSD_OUT_EP_IDX].ep_addr, usbd_msc_cfg.block_buffer, data_len);
    return true;
}

static bool SCSI_write12(uint8_t id,uint8_t **ppdata, uint32_t *len)
{
    uint32_t data_len = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    usbd_msc_cfg.start_sector = U32_H(&usbd_msc_cfg.cbw.CB[2]); /* Logical Block Address of First Block */
    logd("lba: 0x%04lx\n", usbd_msc_cfg.start_sector);

    usbd_msc_cfg.nsectors = U32_H(&usbd_msc_cfg.cbw.CB[6]); /* Number of Blocks to transfer */
    logd("nsectors: 0x%02lx\n", usbd_msc_cfg.nsectors);

    data_len = usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size;
    if ((usbd_msc_cfg.start_sector + usbd_msc_cfg.nsectors) > usbd_msc_cfg.scsi_blk_nbr) {
        loge("LBA out of range\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != data_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_OUT;
    data_len = MIN(data_len, USBD_MSC_BLOCK_SIZE);
    // usbd_ep_start_read(mass_ep_data[MSD_OUT_EP_IDX].ep_addr, usbd_msc_cfg.block_buffer, data_len);
    return true;
}
/* do not use verify to reduce code size */
#if 0
static bool SCSI_verify10(uint8_t **ppdata, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;

    if ((usbd_msc_cfg.cbw.CB[1] & 0x02U) == 0x00U) {
        return true;
    }

    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[1] & 0x02U) == 0x02U) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDFIELDINCBA);
        return false; /* Error, Verify Mode Not supported*/
    }

    lba = U32_H(&usbd_msc_cfg.cbw.CB[2]);
    logd("lba: 0x%x\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = U16_H(&usbd_msc_cfg.cbw.CB[7]);

    logd("num (block) : 0x%x\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        loge("LBA out of range\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }

    usbd_msc_cfg.stage = MSC_DATA_OUT;
    return true;
}
#endif

static bool SCSI_processRead(uint8_t id)
{
    uint32_t transfer_len;

    transfer_len = MIN(usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size, USBD_MSC_BLOCK_SIZE);
    //logd("read lba:%ld,offset=%d,transfer_len=%ld\n", usbd_msc_cfg.start_sector,usbd_msc_cfg.sector_offset,transfer_len);

    if (usbd_msc_sector_read(0, usbd_msc_cfg.start_sector, usbd_msc_cfg.sector_offset, usbd_msc_cfg.block_buffer, transfer_len) != 0) {
        SCSI_SetSenseData(SCSI_KCQHE_UREINRESERVEDAREA);
        return false;
    }
    usbd_class_in(id, DEV_TYPE_MSD, DEV_TYPE_NONE, usbd_msc_cfg.block_buffer,transfer_len);

    usbd_msc_cfg.sector_offset += transfer_len;
    usbd_msc_cfg.start_sector += (usbd_msc_cfg.sector_offset / usbd_msc_cfg.scsi_blk_size);
    usbd_msc_cfg.nsectors -= (usbd_msc_cfg.sector_offset / usbd_msc_cfg.scsi_blk_size);
    usbd_msc_cfg.sector_offset %= usbd_msc_cfg.scsi_blk_size;
    usbd_msc_cfg.csw.dDataResidue -= transfer_len;


    if (usbd_msc_cfg.nsectors == 0) {
        usbd_msc_cfg.stage = MSC_SEND_CSW;
    }

    return true;
}

static bool SCSI_processWrite(uint8_t id,uint32_t nbytes)
{
    //uint32_t data_len = 0;

    //logd("write lba:%ld,offset=%d,transfer_len=%ld\n", usbd_msc_cfg.start_sector,usbd_msc_cfg.sector_offset,nbytes);
    if (usbd_msc_sector_write(0, usbd_msc_cfg.start_sector, usbd_msc_cfg.sector_offset, usbd_msc_cfg.block_buffer, nbytes) != 0) {
        SCSI_SetSenseData(SCSI_KCQHE_WRITEFAULT);
        return false;
    }

    usbd_msc_cfg.sector_offset += nbytes;
    usbd_msc_cfg.start_sector += (usbd_msc_cfg.sector_offset / usbd_msc_cfg.scsi_blk_size);
    usbd_msc_cfg.nsectors -= (usbd_msc_cfg.sector_offset / usbd_msc_cfg.scsi_blk_size);
    usbd_msc_cfg.sector_offset %= usbd_msc_cfg.scsi_blk_size;
    usbd_msc_cfg.csw.dDataResidue -= nbytes;

    if (usbd_msc_cfg.nsectors == 0) {
        usbd_msc_send_csw(id, CSW_STATUS_CMD_PASSED);
    } else {
        //data_len = MIN(usbd_msc_cfg.nsectors * usbd_msc_cfg.scsi_blk_size, USBD_MSC_BLOCK_SIZE);
        // usbd_ep_start_read(mass_ep_data[MSD_OUT_EP_IDX].ep_addr, usbd_msc_cfg.block_buffer, data_len);
    }

    return true;
}

static bool SCSI_CBWDecode(uint8_t id,uint32_t nbytes)
{
    uint8_t *buf2send = usbd_msc_cfg.block_buffer;
    uint32_t len2send = 0;
    bool ret = false;

    if (nbytes != sizeof(struct CBW)) {
        loge("size != sizeof(cbw)\n");
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    }

    usbd_msc_cfg.cbw.dSignature = SWAP32_L( usbd_msc_cfg.cbw.dSignature );
    usbd_msc_cfg.cbw.dTag = SWAP32_L( usbd_msc_cfg.cbw.dTag );
    usbd_msc_cfg.cbw.dDataLength = SWAP32_L( usbd_msc_cfg.cbw.dDataLength );

    usbd_msc_cfg.csw.dTag = usbd_msc_cfg.cbw.dTag;
    usbd_msc_cfg.csw.dDataResidue = usbd_msc_cfg.cbw.dDataLength;
    usbd_msc_cfg.sector_offset = 0;

    if ((usbd_msc_cfg.cbw.bLUN > 1) || (usbd_msc_cfg.cbw.dSignature != MSC_CBW_Signature) || (usbd_msc_cfg.cbw.bCBLength < 1) || (usbd_msc_cfg.cbw.bCBLength > 16)) {
        SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
        return false;
    } else {
        logd("usbd decode CB:0x%02x\n", (uint16_t)usbd_msc_cfg.cbw.CB[0]);
        switch (usbd_msc_cfg.cbw.CB[0]) {
            case SCSI_CMD_TESTUNITREADY:
                ret = SCSI_testUnitReady(&buf2send, &len2send);
                break;
            case SCSI_CMD_REQUESTSENSE:
                ret = SCSI_requestSense(&buf2send, &len2send);
                break;
            case SCSI_CMD_INQUIRY:
                ret = SCSI_inquiry(&buf2send, &len2send);
                break;
            case SCSI_CMD_STARTSTOPUNIT:
                ret = SCSI_startStopUnit(&buf2send, &len2send);
                break;
            case SCSI_CMD_PREVENTMEDIAREMOVAL:
                ret = SCSI_preventAllowMediaRemoval(&buf2send, &len2send);
                break;
            case SCSI_CMD_MODESENSE6:
                ret = SCSI_modeSense6(&buf2send, &len2send);
                break;
            case SCSI_CMD_MODESENSE10:
                ret = SCSI_modeSense10(&buf2send, &len2send);
                break;
            case SCSI_CMD_READFORMATCAPACITIES:
                ret = SCSI_readFormatCapacity(&buf2send, &len2send);
                break;
            case SCSI_CMD_READCAPACITY10:
                ret = SCSI_readCapacity10(&buf2send, &len2send);
                break;
            case SCSI_CMD_READ10:
                ret = SCSI_read10(id, NULL, 0);
                break;
            case SCSI_CMD_READ12:
                ret = SCSI_read12(id, NULL, 0);
                break;
            case SCSI_CMD_WRITE10:
                ret = SCSI_write10(id, NULL, 0);
                break;
            case SCSI_CMD_WRITE12:
                ret = SCSI_write12(id, NULL, 0);
                break;
            case SCSI_CMD_VERIFY10:
                //ret = SCSI_verify10(NULL, 0);
                ret = false;
                break;

            default:
                SCSI_SetSenseData(SCSI_KCQIR_INVALIDCOMMAND);
                loge("unsupported cmd:0x%02x\n", (uint16_t)usbd_msc_cfg.cbw.CB[0]);
                ret = false;
                break;
        }
    }
    if (ret) {
        if (usbd_msc_cfg.stage == MSC_READ_CBW) {
            if (len2send) {
                logd("Send info len:%ld\n", len2send);
                usbd_msc_send_info(id,buf2send, len2send);
            } else {
                usbd_msc_send_csw(id,CSW_STATUS_CMD_PASSED);
            }
        }
    }
    return ret;
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
#if WEAK_ENABLE
__WEAK error_t usbd_msc_sector_read(uint8_t lun, uint32_t sector, uint32_t offset, uint8_t *buffer, uint32_t length)
{
    // out of ramdisk
  	if ( sector >= USBD_DISK_BLOCK_NUM ) return ERROR_NO_MEM;

    // memcpy(buffer, msc_disk[sector] + offset, length);
    return ERROR_SUCCESS;

}

__WEAK error_t usbd_msc_sector_write(uint8_t lun, uint32_t sector, uint32_t offset, uint8_t *buffer, uint32_t length)
{
    // out of ramdisk
	if ( sector >= USBD_DISK_BLOCK_NUM ) return ERROR_NO_MEM;

    // memcpy(msc_disk[sector] + offset, buffer, length);

    return ERROR_SUCCESS;
}
#endif

void mass_storage_bulk_out(uint8_t id, uint8_t* buf, uint16_t len)
{
    switch (usbd_msc_cfg.stage) {
        case MSC_READ_CBW:
            memcpy(&usbd_msc_cfg.cbw, buf, MIN(len, sizeof(msc_cbw_t)));
            if (SCSI_CBWDecode(id,len) == false) {
                loge("usbd mass command:0x%02x decode err\n", (uint16_t)usbd_msc_cfg.cbw.CB[0]);
                usbd_msc_bot_abort(id);
                return;
            }
            break;
        case MSC_DATA_OUT:
            memcpy(&usbd_msc_cfg.block_buffer, buf, MIN(len, USBD_MSC_BLOCK_SIZE));
            switch (usbd_msc_cfg.cbw.CB[0]) {
                case SCSI_CMD_WRITE10:
                case SCSI_CMD_WRITE12:
                    if (SCSI_processWrite(id,len) == false) {
                        usbd_msc_send_csw(id, CSW_STATUS_CMD_FAILED); /* send fail status to host,and the host will retry*/
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}




void mass_storage_bulk_in(uint8_t id)
{
    switch (usbd_msc_cfg.stage) {
        case MSC_DATA_IN:
            switch (usbd_msc_cfg.cbw.CB[0]) {
                case SCSI_CMD_READ10:
                case SCSI_CMD_READ12:
                    if (SCSI_processRead(id) == false) {
                        usbd_msc_send_csw(id, CSW_STATUS_CMD_FAILED); /* send fail status to host,and the host will retry*/
                        return;
                    }
                    break;
                default:
                    break;
            }
            break;
        /*the device has to send a CSW*/
        case MSC_SEND_CSW:
            usbd_msc_send_csw(id, CSW_STATUS_CMD_PASSED);
            break;

        /*the host has received the CSW*/
        case MSC_WAIT_CSW:
            usbd_msc_cfg.stage = MSC_READ_CBW;
            logd("Start reading cbw\n");
            break;

        default:
            break;
    }
}




#endif
