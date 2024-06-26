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
//@ref: https://github.com/MicrochipTech/mla_usb

#include  "hw_config.h"
#if 1 || HIDD_SUPPORT || HIDH_SUPPORT
#include  "api/hid/hid_desc_parser.h"
#include  "utils/emf_utils.h"

#include  "api/api_log.h"

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

static void hid_desc_parse_collection(hid_desc_info_t *pinfo,hid_item_info_t* item)
{
    hid_collection_t *lcollection;
    uint16_t i;

//  Initialize the new Collection Structure

    i = pinfo->collections++;
    lcollection = &pinfo->item_list.collectionList[i];
    // lcollection->raw = item->Data.uItemData;
    lcollection->firstUsageItem = pinfo->firstUsageItem;
    lcollection->usageItems = pinfo->usageItems - pinfo->firstUsageItem;
    
    /*lcollection->nextSibling = pinfo->sibling;
    lcollection->firstChild = 0;*/
    lcollection->usagePage = pinfo->globals.usagePage;
    lcollection->firstReportItem = pinfo->reportItems;

    pinfo->firstUsageItem = pinfo->usageItems;
    /*pinfo->sibling = 0;*/

//  Set up the relationship with the Parent Collection
    lcollection->parent = pinfo->parent;
    /*pinfo->item_list.collectionList[pinfo->parent].firstChild = i;*/

//  Save the Parent Collection Information on the stack
    pinfo->item_list.collectionStack[pinfo->collectionNesting++] = pinfo->parent;
    pinfo->parent = i+1;       //emf fix  = i to i+1 ,区分顶级集合和子集合
    UNUSED_PARAMETER(item);
}

static void hid_desc_parse_endcollection(hid_desc_info_t *pinfo,hid_item_info_t* item)
{
    hid_collection_t *lcollection;
    uint8_t i;

//  Remember the number of reportItem MainItems in this Collection

    lcollection = &pinfo->item_list.collectionList[pinfo->parent-1];    //emf fix
    lcollection->reportItems = pinfo->reportItems - lcollection->firstReportItem;

//  Restore the Parent Collection Data

    i = pinfo->item_list.collectionStack[--pinfo->collectionNesting];
    /*pinfo->sibling = pinfo->parent;*/
    pinfo->parent = i;
	UNUSED_PARAMETER(item);
}

/****************************************************************************
  Description:
    This function is parse input, output & report item.
***************************************************************************/
static error_t hid_desc_parse_report_type(hid_desc_info_t *pinfo,hid_item_info_t* item)
{
    hid_report_item_t *lreportItem = NULL;
    hid_report_t *lreport = NULL;
    uint16_t bits = 0;

    if(item == NULL) return(ERROR_NULL);

    //  Reality Check on the Report Main Item
    if (pinfo->globals.logicalMinimum >= ((int32_t)1<<pinfo->globals.reportsize)) {
        logd("hid_desc_parse err%d\n",__LINE__);
        return(ERROR_DATA) ;
    }
    // 0x25, 0x7F Logical Maximum (127)     >    0x75, 0x06 Report Size (6) 可以忽略禁告
    // if (pinfo->globals.logicalMaximum >= ((int32_t)1<<pinfo->globals.reportsize)){
    //     logd("hid_desc_parse err%d %d %d\n",__LINE__,pinfo->globals.logicalMaximum,pinfo->globals.reportsize);
    //     return(ERROR_DATA);
    // }

    // The barcode scanner has this issue.  We'll ignore it.
	// if (pinfo->globals.logicalMinimum > pinfo->globals.logicalMaximum)return(ERROR_DATA);

    //部分设备09 xx, 29 xx的组合
    //if (pinfo->haveUsageMin || pinfo->haveUsageMax)return(ERROR_UNKNOW);
    pinfo->haveUsageMax = false;
    pinfo->haveUsageMin = false;

    //  Initialize the new Report Item structure
    lreportItem = &pinfo->item_list.reportItemList[pinfo->reportItems++];
    lreportItem->dataModes = item->Data.uItemData;
    lreportItem->globals = pinfo->globals;
    /*lreportItem->parent = pinfo->parent;*/
    lreportItem->firstUsageItem = pinfo->firstUsageItem;
    pinfo->firstUsageItem = pinfo->usageItems;
    lreportItem->usageItems = pinfo->usageItems - lreportItem->firstUsageItem;

    //  Update the Report by the size of this item
    lreport = &pinfo->item_list.reportList[pinfo->globals.reportIndex];
    bits = pinfo->globals.reportsize * pinfo->globals.reportCount;
    switch (item->ItemDetails.bits.ItemTag)
    {
        case HIDTag_Feature:
            lreportItem->reportType = HID_REPORT_TYPE_FEATURE;
            lreportItem->startBit = lreport->featureBits;
            lreport->featureBits += bits;
            break;
        case HIDTag_Output:
            lreportItem->reportType = HID_REPORT_TYPE_OUTPUT;
            lreportItem->startBit = lreport->outputBits;
            lreport->outputBits += bits;
            break;
        case HIDTag_Input:
            lreportItem->reportType = HID_REPORT_TYPE_INPUT;
            lreportItem->startBit = lreport->inputBits;
            lreport->inputBits += bits;
            break;
        default:
            lreportItem->reportType = HID_REPORT_TYPE_INVALID;
            break;
    }

    return ERROR_SUCCESS;
}

/****************************************************************************
  Description:
    This function is called by _USBHostHID_Parse_Report() convert data
    to signed whenever required
***************************************************************************/
static void hid_desc_convert_to_signed(hid_item_info_t* item)
{
    uint8_t    dataByte=0;
    uint8_t    index;

    index = item->ItemDetails.bits.ItemSize;

    if(index) {
       if(item->ItemDetails.bits.ItemSize == 3){
           index = 4;
       }

       dataByte = item->Data.bItemData[index-1];
       if ((dataByte & 0x80) != 0){
           while (index < sizeof(int32_t)){
                item->Data.sItemData |= (0xFF << ((index++)*8)); /* extend one */
           }
       }
    }
}


/*****************************************************************************************************
**  Function
******************************************************************************************************/
void hid_items_dump(hid_items_t *pitems)
{
    logd("\npitems id=%d,len=%d\n",(uint16_t)pitems->report_id,(uint16_t)pitems->report_length);
    logd("\tboffset=%d,bzise=%d,count=%d\n",(uint16_t)pitems->bit_offset,(uint16_t)pitems->bit_size,(uint16_t)pitems->bit_count);
    logd("\tusageMinimum=%d\n",(uint16_t)pitems->usageMinimum);
}
void hid_desc_dump(hid_desc_info_t *pinfo)
{
    hid_report_t *lreport;
    hid_report_item_t *lreportItem;
    hid_collection_t *lcollection;
    // hid_usage_item_t *lusageItem;
    uint8_t i;

    logd("\n ======================= Report Descriptor Dump ======================= \n");
    logd("Collections:%x\n",(uint16_t)pinfo->collections );
    logd("maxCollectionNesting:%x\n",(uint16_t)pinfo->maxCollectionNesting );
    logd("Reports:%x\n",(uint16_t)pinfo->reports );
    logd("ReportItems:%x\n",(uint16_t)pinfo->reportItems );
    logd("UsageItems:%x\n",(uint16_t)pinfo->usageItems );

    
    for (i=0; i<pinfo->collections; i++){
        logd("------------------------\n");
        logd("Collection   :%x\n",(uint16_t)i);
        lcollection = &pinfo->item_list.collectionList[i];
        logd("\tusagePage :%x\n",(uint16_t)lcollection->usagePage);
        logd("\tfirstUsageItem :%x\n",(uint16_t)lcollection->firstUsageItem);
        logd("\tusageItems :%x\n",(uint16_t)lcollection->usageItems);
        logd("\tfirstReportItem :%x\n",(uint16_t)lcollection->firstReportItem);
        logd("\treportItems :%x\n",(uint16_t)lcollection->reportItems);
        logd("\tparent :%x\n",(uint16_t)lcollection->parent);
    }

    for (i=0; i<pinfo->reports; i++){
        logd("------------------------\n");
        logd("Report   :%x\n",(uint16_t)i);
        lreport = &pinfo->item_list.reportList[i];
        logd("\tReportID :%x\n",lreport->report_id);
        if (lreport->inputBits > 0){
            logd("\tInbits   :%x\n",lreport->inputBits);
		}
        if (lreport->outputBits > 0){
            logd("\tOutbits  :%x\n",lreport->outputBits);
		}
        if (lreport->featureBits > 0){
            logd("\tFeatbits :%x\n",lreport->featureBits);
		}
    }
    for (i=0; i<pinfo->reportItems; i++) {
        logd("------------------------\n");
        logd("ReportItem  :%x\n",i);
        lreportItem = &pinfo->item_list.reportItemList[i];
        logd("\tReportType  :%x\n",(uint16_t)lreportItem->reportType);

        logd("\tStart Bit   :%x\n",(uint16_t)lreportItem->startBit);
        logd("\tBits        :%x\n",(uint16_t)lreportItem->globals.reportsize);
        logd("\tDataModes   :%x\n",(uint16_t)lreportItem->dataModes);
        logd("\t1st Usage   :%x\n",(uint16_t)lreportItem->firstUsageItem);
        logd("\tUsage Items :%x\n",(uint16_t)lreportItem->usageItems);
        logd("\tusagePage   :%x\n",(uint16_t)lreportItem->globals.usagePage);
        logd("\tUsage       :%x\n",lreportItem->globals.logicalMinimum);
        logd("\treportsize  :%x\n",(uint16_t)lreportItem->globals.reportsize);
        logd("\treportID    :%x\n",(uint16_t)lreportItem->globals.report_id);
        logd("\treportCount :%x\n",(uint16_t)lreportItem->globals.reportCount);
    }

    logd("------------------------\n");
    logd("UsageItem :%x\n",(uint16_t)i);
    for (i = 0; i < pinfo->usageItems; i++){
        logd("Usage:    %02x      %02x\n",pinfo->item_list.usageItemList[i].usageMinimum,pinfo->item_list.usageItemList[i].usageMaximum);
    }
}


void hid_desc_info_free(hid_desc_info_t *pinfo)
{
    // logd("hid_desc_info_free=%x\n", (uint32_t)pinfo->item_list.globalsStack);
    emf_free(pinfo->item_list.globalsStack);
}
/*******************************************************************
** Parameters:	pinfo: return  desc parse info	
** Returns:	
** Description: 如果解析成功必须释放 pinfo->pitem_list 资源
*******************************************************************/
error_t hid_desc_parse_report(hid_desc_info_t *pinfo, uint8_t* pdesc , uint16_t desc_len)
{
    error_t err = ERROR_SUCCESS;
    uint16_t  sizeRequired = 0;
    uint16_t  len_to_be_parsed =0;
    uint8_t* currentRptDescPtr = NULL;
    uint8_t* assignMem = NULL;
    uint8_t tmp_maxCollectionNesting;
    /* Global Item Vars */
    hid_report_t *lreport = NULL;
    uint8_t lreportIndex = (uint8_t)0;

    /* Local Item Vars */
    hid_usage_item_t *lusageItem = NULL;
    hid_item_t hid_item;
    hid_item_info_t item;

    uint8_t  i=0;
    uint8_t  dataByte=0 ;
    uint8_t  ldataSize=0;

    if((pdesc == NULL) ||(desc_len == 0)){
        /* set error flag */
        return(ERROR_NULL);
    }

    memset(pinfo, 0x00, sizeof( hid_desc_info_t ) );
    pinfo->reports = 1;

    len_to_be_parsed = desc_len;
    currentRptDescPtr = pdesc;

    while(len_to_be_parsed > 0){    /* First parse to calculate the space required for all the items */
       item.ItemDetails.val = *currentRptDescPtr;
       /* Data need not be parsed at this point */
       ldataSize = item.ItemDetails.bits.ItemSize ;
       if(item.ItemDetails.bits.ItemSize == 3)
           ldataSize = 4;

       currentRptDescPtr += (ldataSize+1) ; /* point to next item i.e size of item data + 1(item detail) */
       len_to_be_parsed -= (ldataSize+1);   /* remaining bytes = current - (length of data + 1)*/

        switch (item.ItemDetails.bits.ItemType){
            case HIDType_Main:           /* Main Items */
                switch (item.ItemDetails.bits.ItemTag){
                    case HIDTag_Collection:
                        pinfo->collections++;
                        pinfo->collectionNesting++;
                        if (pinfo->collectionNesting > pinfo->maxCollectionNesting)
                            pinfo->maxCollectionNesting = pinfo->collectionNesting;
                        break;
                    case HIDTag_EndCollection:
                        if (pinfo->collectionNesting-- == 0){
                            err = ERROR_UNKNOW ;
                        }
                        break;
                    case HIDTag_Input:
                    case HIDTag_Output:
                    case HIDTag_Feature:
                        pinfo->reportItems++;
                        break;
                    default :
                        break;
                }
                break;
            case HIDType_Global:         /* Global Items */
                switch (item.ItemDetails.bits.ItemTag){
                    case HIDTag_ReportID:
                        pinfo->reports++;
                        break;
                    case HIDTag_Push:
                        pinfo->globalsNesting++;
                        if (pinfo->globalsNesting > pinfo->maxGlobalsNesting)
                            pinfo->maxGlobalsNesting = pinfo->globalsNesting;
                        break;
                    case HIDTag_Pop:
                        pinfo->globalsNesting--;
                        if (pinfo->globalsNesting > pinfo->maxGlobalsNesting)
                            err = ERROR_UNKNOW ;/* Error: global nesting rolled to negative ... */
                        break;
                    default :
                        break;
                }
                break;
            case HIDType_Local:          /* Local Item */
                switch (item.ItemDetails.bits.ItemTag){
                    case HIDTag_Usage:
                        pinfo->usages++;
                        break;
                    case HIDTag_UsageMinimum:
                    case HIDTag_UsageMaximum:
                        pinfo->usageRanges++;
                        break;
                    case HIDTag_StringIndex:
                        break;
                    case HIDTag_StringMinimum:
                    case HIDTag_StringMaximum:
                        break;
                    case HIDTag_DesignatorIndex:
                        break;
                    case HIDTag_DesignatorMinimum:
                    case HIDTag_DesignatorMaximum:
                        break;
                    default :
                        break;
                }
                break;
            default :
                break;

        }
    }

    if(err) return(err);
    if (pinfo->reportItems == 0) return (ERROR_UNKNOW)/* HID_RPT_DESC_FORMAT_IMPROPER */;
    if ((pinfo->usageRanges & 1) == 1) return (ERROR_UNKNOW)/* HID_RPT_DESC_FORMAT_IMPROPER */;

   /* usages , strings & descriptors are in pair */
    pinfo->usages += (pinfo->usageRanges/2);
   /* Calculate space required */
    sizeRequired = (sizeof(hid_report_item_t) * pinfo->reportItems)
                   + (sizeof(hid_collection_t) * pinfo->collections)
                   + (sizeof(uint8_t) * pinfo->maxCollectionNesting)
                   + (sizeof(hid_report_t) * pinfo->reports)
                   + (sizeof(hid_usage_item_t) * pinfo->usages)
                   + (sizeof(hid_globals_item_t) * pinfo->maxGlobalsNesting);
    assignMem = (uint8_t*) emf_malloc(sizeRequired);
    logd( "HID: Memory for Report Descriptor len=%d add=%x\n", sizeRequired, (uint32_t)assignMem);
    if (assignMem == NULL) 
    {
        emf_mem_stats();
        return(ERROR_NO_MEM); /* Error: Not enough memory */
    }
    memset(assignMem,0,sizeRequired);

    /* Allocate Space */
    hid_item.globalsStack = (hid_globals_item_t *) assignMem;
    assignMem += (sizeof(hid_globals_item_t) * pinfo->maxGlobalsNesting);
    hid_item.collectionList = (hid_collection_t *) assignMem;
    assignMem += (sizeof(hid_collection_t) * pinfo->collections);
    hid_item.collectionStack = (uint8_t *) assignMem;
    assignMem += (sizeof(uint8_t) * pinfo->maxCollectionNesting);
    hid_item.reportItemList = (hid_report_item_t *) assignMem;
    assignMem += (sizeof(hid_report_item_t) * pinfo->reportItems);
    hid_item.reportList = (hid_report_t *) assignMem;
    assignMem += (sizeof(hid_report_t) * pinfo->reports);
    hid_item.usageItemList = (hid_usage_item_t *) assignMem;
    assignMem += (sizeof(hid_usage_item_t) * pinfo->usages);

    tmp_maxCollectionNesting = pinfo->maxCollectionNesting;
    memset(pinfo, 0x00, sizeof( hid_desc_info_t ) );
    pinfo->reports = 1;
    pinfo->item_list = hid_item;
    pinfo->item_list_size = sizeRequired;
    pinfo->maxCollectionNesting = tmp_maxCollectionNesting; //used for debug

   /* re-init ptr to Rpt Descp & Length of Descp */
    len_to_be_parsed = desc_len;
    currentRptDescPtr = pdesc;

    //logd( "HID-HOST: ... 2nd Parse \n" );
    while(len_to_be_parsed > 0){     /* Second parse to fill the tables with each item detail */
       item.ItemDetails.val = *currentRptDescPtr;
       item.Data.uItemData = 0;

       ldataSize = item.ItemDetails.bits.ItemSize ;
       if(item.ItemDetails.bits.ItemSize == 3)
           ldataSize = 4;

       currentRptDescPtr++; /* ptr points to data */
       for (i = 0; i < ldataSize; i++){
            dataByte = *currentRptDescPtr++;  /* signed data will be taken care in ItemTag it is expected */
            item.Data.uItemData |= ((uint32_t)dataByte << (i*8));
        }
        //logd("%x %x",item.ItemDetails.val,item.Data.uItemData);

       len_to_be_parsed -= (ldataSize+1);   /* remaining bytes = current - (length of current item + 1)*/

       switch(item.ItemDetails.bits.ItemType){
           case HIDType_Main:   /* look for Main Items*/
                switch(item.ItemDetails.bits.ItemTag){
                    case HIDTag_Input :
                    case HIDTag_Output :
                    case HIDTag_Feature :
                        //logd_r( " haveUsageMin=%d haveUsageMax=%d\n", pinfo->haveUsageMin , pinfo->haveUsageMax);
                        err = hid_desc_parse_report_type(pinfo, &item);
                    break;
                    case HIDTag_Collection :
                        hid_desc_parse_collection(pinfo, &item);
                    break;
                    case HIDTag_EndCollection :
                        hid_desc_parse_endcollection(pinfo, &item);
                    break;
                }
                break;

           case HIDType_Global:   /* look for Global Items*/
                switch(item.ItemDetails.bits.ItemTag){
                    case HIDTag_UsagePage :
                         pinfo->globals.usagePage = item.Data.uItemData;
                         break;

                    case HIDTag_LogicalMinimum : /* convert to signed val */
                        //  Sign extend one value
                        hid_desc_convert_to_signed(&item);
                        pinfo->globals.logicalMinimum = item.Data.sItemData;
                        break;

                    case HIDTag_LogicalMaximum :/* convert to signed val */
                        //  Sign extend one value
                        hid_desc_convert_to_signed(&item);
                        pinfo->globals.logicalMaximum = item.Data.uItemData;
                        break;

                    case HIDTag_PhysicalMinimum :/* convert to signed val */
                        //  Sign extend one value
                        //  hid_desc_convert_to_signed(&item);
                        //  pinfo->globals.physicalMinimum = item.Data.uItemData;
                        break;
                    case HIDTag_PhysicalMaximum :/* convert to signed val */
                        //  Sign extend one value
                        //  hid_desc_convert_to_signed(&item);
                        //  pinfo->globals.physicalMaximum = item.Data.uItemData;
                        break;
                    case HIDTag_UnitExponent :
                        //pinfo->globals.unitExponent = item.Data.uItemData;
                        break;
                    case HIDTag_ReportSize :
                        pinfo->globals.reportsize = item.Data.uItemData;
                        if (pinfo->globals.reportsize == 0){
                              err = ERROR_LENGTH;
                        }
                        break;

                    case HIDTag_ReportID :
                        if (item.Data.uItemData){
                            //Look for the Report ID in the table
                            lreportIndex = 0;
                            while ((lreportIndex < pinfo->reports)
                                && (pinfo->item_list.reportList[lreportIndex].report_id != item.Data.uItemData)){
                                lreportIndex++;
                            }
                            // initialize the entry if it's new and there's room for it
                            // Start with 8 bits for the Report ID
                            if (lreportIndex == pinfo->reports){
                                lreport = &pinfo->item_list.reportList[pinfo->reports++];
                                lreport->report_id = item.Data.uItemData;
                                lreport->inputBits = 8;
                                lreport->outputBits = 8;
                                lreport->featureBits = 8;
                            }
                            //remember which report is being processed
                            pinfo->globals.report_id = item.Data.uItemData;
                            pinfo->globals.reportIndex = lreportIndex;
                        }else{
                            err = ERROR_DATA; 
                            logd("e%d\n",__LINE__);
                        }
                        break;
                    case HIDTag_ReportCount :
                        if (item.Data.uItemData){
                            pinfo->globals.reportCount = item.Data.uItemData;
                        }else{
                            err = ERROR_LENGTH;
                        }
                        break;
                    case HIDTag_Push :
                        pinfo->item_list.globalsStack[pinfo->globalsNesting++] = pinfo->globals;
                        break;
                    case HIDTag_Pop :
                        pinfo->globals = pinfo->item_list.globalsStack[--pinfo->globalsNesting] ;
                    break;
                }
                break;

           case HIDType_Local:  /* look for Local Items*/
                switch(item.ItemDetails.bits.ItemTag){
                    case HIDTag_Usage :
                        lusageItem = &pinfo->item_list.usageItemList[pinfo->usageItems++];
                        if (item.ItemDetails.bits.ItemSize == 3){ /* 4 data bytes */
                            lusageItem->usagePage = item.Data.uItemData >> 16;
                            lusageItem->usageMaximum = item.Data.uItemData & 0x00FF;
                        }else{
                            lusageItem->usagePage = pinfo->globals.usagePage;
                            lusageItem->usageMaximum = item.Data.uItemData;
                        }
                        lusageItem->usageMinimum = lusageItem->usageMaximum;
                        break;

                    case HIDTag_UsageMinimum :
                        if(pinfo->haveUsageMax){
                            lusageItem = &pinfo->item_list.usageItemList[pinfo->usageItems++];
                            if(item.ItemDetails.bits.ItemSize == 3){
                                lusageItem->usagePage = item.Data.uItemData >> 16;
                                lusageItem->usageMinimum = item.Data.uItemData & 0x00FFL;
                            }else{
                                lusageItem->usagePage = pinfo->globals.usagePage;
                                lusageItem->usageMinimum = item.Data.uItemData;
                            }

                            if (lusageItem->usagePage != pinfo->rangeUsagePage){
                                err = ERROR_DATA; /* Error: BadUsageRangePage */
                                logd("e%d\n",__LINE__);
                            }

                            lusageItem->usageMaximum = pinfo->usageMaximum;
                            if (lusageItem->usageMaximum < lusageItem->usageMinimum){
                                err = ERROR_DATA; /* Error: BadUsageRange */
                                logd("e%d\n",__LINE__);
                            }

                            pinfo->haveUsageMax = false;
                            pinfo->haveUsageMin = false;
                        }else{
                            if(item.ItemDetails.bits.ItemSize == 3){
                                pinfo->rangeUsagePage = item.Data.uItemData >> 16;
                                pinfo->usageMinimum = item.Data.uItemData & 0x00FFL;
                            }else{
                                pinfo->rangeUsagePage = pinfo->globals.usagePage;
                                pinfo->usageMinimum = item.Data.uItemData;
                            }
                            pinfo->haveUsageMin = true;
                        }
                        //logd("HIDTag_UsageMinimum min=%d max=%d",pinfo->haveUsageMin,pinfo->haveUsageMax);
                        break;

                    case HIDTag_UsageMaximum :
                        if(pinfo->haveUsageMin){
                            lusageItem = &pinfo->item_list.usageItemList[pinfo->usageItems++];
                            if(item.ItemDetails.bits.ItemSize == 3){
                                lusageItem->usagePage = item.Data.uItemData >> 16;
                                lusageItem->usageMaximum = item.Data.uItemData & 0x00FFL;
                            }else{
                                lusageItem->usagePage = pinfo->globals.usagePage;
                                lusageItem->usageMaximum = item.Data.uItemData;
                            }

                            if (lusageItem->usagePage != pinfo->rangeUsagePage){
                                err = ERROR_DATA; /* Error: BadUsageRangePage */
                                logd("e%d\n",__LINE__);
                            }

                            lusageItem->usageMinimum = pinfo->usageMinimum;

                            if (lusageItem->usageMaximum < lusageItem->usageMinimum){
                                err = ERROR_DATA; /* Error: BadUsageRange */
                                logd("e%d\n",__LINE__);
                            }

                            pinfo->haveUsageMax = false;
                            pinfo->haveUsageMin = false;
                        }else{
                            if(item.ItemDetails.bits.ItemSize == 3){
                                pinfo->rangeUsagePage = item.Data.uItemData >> 16;
                                pinfo->usageMaximum = item.Data.uItemData & 0x00FFL;
                            } else {
                                pinfo->rangeUsagePage = pinfo->globals.usagePage;
                                pinfo->usageMaximum = item.Data.uItemData;
                            }
                            pinfo->haveUsageMax = true;
                        }
                        //logd("HIDTag_UsageMaximum min=%d max=%d",pinfo->haveUsageMin,pinfo->haveUsageMax);
                        break;

                    case HIDTag_DesignatorIndex :
                         break;
                    case HIDTag_DesignatorMinimum :
                        break;
                    case HIDTag_DesignatorMaximum :
                        break;
                    case HIDTag_StringIndex :
                        break;
                    case HIDTag_StringMinimum :
                        break;
                    case HIDTag_StringMaximum :
                        break;
                    case HIDTag_SetDelimiter :
						break;
                }
                break;

           default:
                break;
        }
        if(err) break;
    }

    // Remove reports that have just the report id
    for (i=1; i<pinfo->reports; i++) {
        if (pinfo->item_list.reportList[i].inputBits == 8) pinfo->item_list.reportList[i].inputBits = 0;
        if (pinfo->item_list.reportList[i].outputBits == 8) pinfo->item_list.reportList[i].outputBits = 0;
        if (pinfo->item_list.reportList[i].featureBits == 8) pinfo->item_list.reportList[i].featureBits = 0;
    }

    if(err){            //free item buf
        hid_desc_info_free(pinfo);
    }
    return(err);
}


bool hid_match_collection(hid_desc_info_t *pinfo, uint8_t collection_index, uint16_t usagePage, uint16_t usage)
{
    bool ret = false;
    hid_collection_t* pcollection;

    pcollection = &pinfo->item_list.collectionList[collection_index];
	// logd("index%d parent=%d, usag:%x %x",(uint16_t)collection_index,(uint16_t)pcollection->parent,pcollection->usagePage,pinfo->item_list.usageItemList[pcollection->firstUsageItem].usageMinimum);
    if(0 == pcollection->parent){     //顶层集合确定设备类型
        if((usagePage == pcollection->usagePage)
            && (usage == pinfo->item_list.usageItemList[pcollection->firstUsageItem].usageMinimum)){

            ret = true;
        }
    }
    return ret;
}


bool hid_collection_find_items(hid_desc_info_t *pinfo, hid_collection_t* pcollection, hid_report_type_t type, uint16_t usagePage, uint16_t usage, hid_items_t *pitems)
{
    bool ret = false;
    uint8_t i;

    for(i=0; i<pcollection->reportItems; i++){
        ret = hid_reportlist_find_items(pinfo, pcollection->firstReportItem + i, type, usagePage, usage, pitems);
        if(ret) break;
    }
    return ret;
}

        

bool hid_reportlist_find_items(hid_desc_info_t *pinfo, uint8_t item_index, hid_report_type_t type, uint16_t usagePage, uint16_t usage, hid_items_t *pitems)
{
    bool ret = false;
    hid_report_item_t *reportItem;
    hid_usage_item_t *hidUsageItem;
    hid_report_t *preport; 
    uint8_t usageIndex,i;           //item_list.reportList[] index
    uint8_t reportIndex;            //item_list.usageItemList[] index

    reportItem = &pinfo->item_list.reportItemList[item_index];
    usageIndex = reportItem->firstUsageItem;
    if((reportItem->reportType == type) && (reportItem->globals.usagePage == usagePage)){
        
        for(i = 0; i < reportItem->usageItems; i++){
            hidUsageItem = &pinfo->item_list.usageItemList[usageIndex++];
            if ((usage >= hidUsageItem->usageMinimum) && (usage <= hidUsageItem->usageMaximum)){
                
                reportIndex = reportItem->globals.reportIndex;
                preport = &pinfo->item_list.reportList[reportIndex];
                switch(type){
                    case HID_REPORT_TYPE_INPUT:
                        pitems->report_length = (preport->inputBits + 7)/8;
                        break;
                    case HID_REPORT_TYPE_OUTPUT:
                        pitems->report_length = (preport->outputBits + 7)/8;
                        break;
                    case HID_REPORT_TYPE_FEATURE:
                        pitems->report_length = (preport->featureBits + 7)/8;
                        break;
                    default:
                        break;
                }

                if(hidUsageItem->usageMinimum == hidUsageItem->usageMaximum){
                    pitems->report_id = (uint8_t)reportItem->globals.report_id;
                    pitems->bit_offset = (uint8_t)reportItem->startBit + i * reportItem->globals.reportsize;
                    pitems->bit_size = (uint8_t)reportItem->globals.reportsize;
                    pitems->bit_count = (uint8_t)1;
                    pitems->usageMinimum = (uint8_t)usage;
                }else{          //is in range
                    pitems->report_id = (uint8_t)reportItem->globals.report_id;
                    pitems->bit_offset = (uint8_t)reportItem->startBit;
                    pitems->bit_size = (uint8_t)reportItem->globals.reportsize;
                    pitems->bit_count = (uint8_t)reportItem->globals.reportCount;
                    pitems->usageMinimum = (uint8_t)hidUsageItem->usageMinimum;
                }
                ret = true;
                break;
            }
        }
    }

    return(ret);
}

bool hid_items_match_id(hid_items_t *pitem, uint8_t* buf, uint16_t len)
{
    if((pitem->report_length == len) && ((pitem->report_id == 0) || (buf[0] == pitem->report_id))){
        return true;
    }
    return false;
}

#endif
