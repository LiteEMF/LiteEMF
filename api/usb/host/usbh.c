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
#if API_USBH_BIT_ENABLE
#include "api/usb/host/usbh.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

uint16_t m_usbh_types = USBH_TYPE_SUPPORT;			//TODO 可用于动态调整usbh 枚举的设备类型
uint16_t m_usbh_hid_types = USBH_TYPE_SUPPORT;		//TODO 可用于动态调整usbh 枚举的设备类型
/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/


/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/


/*******************************************************************
** Parameters:	
** Returns:	
** Description:	usb拔插检测事件, 在SDK中检测到USB拔插后调用该函数
*******************************************************************/
void usbh_det_event(uint8_t id, uint8_t attached )
{
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev) return;

	logd_g("\nusbh%x det=%x\n",(uint16_t)id, (uint16_t)attached);
	if(attached){
		if(TUSB_STA_DETACHED == pdev->state){
			logd("usbh%x TUSB_STA_ATTACHED\n",(uint16_t)id);
			usbh_set_status(id, TUSB_STA_ATTACHED, 0);
		}else{
			// in enuming...
		}
	}else if(TUSB_STA_DETACHED != pdev->state){
		uint8_t i;
		if((id & 0X0F) == 0){
			for(i = 0; i < HUB_MAX_PORTS+1; i++){		//disconnect hub por also
				usbh_set_status(id | i, TUSB_STA_DETACHED, 0);
			}
		}else{
			usbh_set_status(id, TUSB_STA_DETACHED, 0);
		}
	}
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_disconnect(uint8_t id)
{
	error_t err = ERROR_SUCCESS;
	uint8_t i;

	logd("usbh%x disconnect\n",(uint16_t)id);
	if((id & 0X0F) == 0){  				//直接接的root端口
		err = usbh_port_en(id,0,NULL);
		if(ERROR_SUCCESS == err){			
			for(i = 0; i < HUB_MAX_PORTS+1; i++){		//disconnect hub por also
				usbh_set_status(id | i, TUSB_STA_DETACHED, 0);
			}
		}
	}else{
		#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
		err = usbh_hub_port_disable(id);
		if(ERROR_SUCCESS == err){
			usbh_set_status(id, TUSB_STA_DETACHED, 0);
		}
		#endif
	}
	return err;
}


error_t usbh_reset(uint8_t id,uint8_t reset_ms)
{
	error_t err = ERROR_STATE;
	usb_speed_t speed = TUSB_SPEED_FULL;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev ) return err;
	if(pdev->state >= TUSB_STA_POWERED){
		if((id & 0X0F) == 0){
			err = usbh_port_reset(id,reset_ms);
			if(err) return err;
			err = usbh_port_en(id, 1, &speed );
			if(err) return err;

			usbh_set_status(id, TUSB_STA_DEFAULT, 0);
		}else{
			#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_HUB)
			err =  usbh_hub_port_reset(id);
			//hub reset state set in usbh_hub.c
			#endif
		}
		
	}
	return err;
}

error_t usbh_set_address(uint8_t id,uint8_t addr)
{
	error_t err = ERROR_STATE;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev ) return err;

	if(pdev->state == TUSB_STA_DEFAULT){
		err = usbh_req_set_addr(id,addr);
		if(ERROR_SUCCESS == err){
			usbh_set_status(id, TUSB_STA_ADDRESSING, addr);  
		}
	}
	return err;
}


/*******************************************************************
** Parameters:	buf,len: desc buf and len
                dir: endp dir
** Returns:	
** Description:  
*******************************************************************/
error_t usbh_get_endp( usb_endp_t* endp,uint8_t *buf ,uint16_t len,usb_dir_t dir) 
{
    error_t err = ERROR_UNKNOW;
    uint8_t inf_num=0;
    uint16_t i, l;
    usb_desc_endpoint_t *pdesc;

	memset(endp,0,sizeof(usb_endp_t));

    for ( i = 0; i < len; i += l ){
        l = buf[i];
        if(0 == l) break;

    	if( buf[i+1] == TUSB_DESC_INTERFACE){
    		inf_num++;
    		if(inf_num > 1) break;
    	}

        pdesc = (usb_desc_endpoint_t*)(buf+i);
        if((pdesc->bDescriptorType == TUSB_DESC_ENDPOINT) && (dir == BOOL_SET(pdesc->bEndpointAddress & 0X80))){
            endp->addr = pdesc->bEndpointAddress & 0x0F;
			endp->type = pdesc->bmAttributes.xfer;
            endp->dir = dir;
			endp->mtu = SWAP16_L(pdesc->wMaxPacketSize) & 0X3FF;
            endp->interval = pdesc->bInterval;
			if(endp->interval > USBH_ENDPOINT_MAX_INTERVAL){
				endp->interval = USBH_ENDPOINT_MAX_INTERVAL;
			}
			err = ERROR_SUCCESS;
            break;
        }
    }
    return( err );
}


uint8_t usbh_find_by_status(uint8_t usb_stas)   
{
    uint8_t i,j;
    usbh_dev_t *pdev;

    for(i = 0; i < USBH_NUM; i++){
        for(j = 0; j < HUB_MAX_PORTS+1; j++){
            pdev = &m_usbh_dev[i][j];

            if(BIT(pdev->state) & usb_stas) {
				return (i<<4) | j;
            }
        }
    }
    
    return USBH_NULL;
}

#if WEAK_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description: usbh状态通知用户处理
*******************************************************************/
__WEAK void usbh_user_event_weak(uint8_t id, usb_state_t usb_sta)
{

}
#endif

error_t usbh_set_status(uint8_t id, usb_state_t usb_sta, uint8_t addr)   
{
    usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev ) return ERROR_PARAM;

	switch (usb_sta){
	case TUSB_STA_DETACHED:
	case TUSB_STA_ATTACHED:
	case TUSB_STA_POWERED:
		usbh_class_deinit(id);
		memset(pdev,0,sizeof(usbh_dev_t));
		INIT_LIST_HEAD(&pdev->class_list);
		break;
	case TUSB_STA_DEFAULT:
		pdev->addr = 0;
		pdev->endp0_mtu = DEFAULT_ENDP0_SIZE;
		break;
	case TUSB_STA_ADDRESSING:
		pdev->addr = addr;
		break;
	case TUSB_STA_CONFIGURED:
		break;
	case TUSB_STA_SUSPENDED:
		break;
	default:
		break;
	}	
	pdev->state = usb_sta;
	usbh_user_event_weak(id, usb_sta);		//用于自定义处理

	
	logd("usbh%x set status=%d\n",(uint16_t)id,(uint16_t)pdev->state);
    return ERROR_SUCCESS;
}


static error_t usbh_parse_configuration_desc(uint8_t id,uint8_t cfg,uint8_t *buf,uint16_t len)
{
	error_t err = ERROR_UNSUPPORT;
    uint16d_t   i, l;
	usbh_dev_t* pdev = get_usbh_dev(id);
	usb_desc_interface_t *pitf;
	usb_endp_t endp;
	usbh_class_t *pclass;

	if(NULL == pdev ) return err;
	
	err = usbh_req_set_configuration(id,cfg);			//set configuration
	if(err) return err;

	for ( i = 0; i < len; i += l ){            		// 搜索中断端点描述符,跳过配置描述符和接口描述符
		l = buf[i];
		if(0 == l) break;

        if( buf[i+1] == TUSB_DESC_INTERFACE ){
			pitf = (usb_desc_interface_t*)&buf[i];
			pclass = malloc_usbh_class(id);
			if(NULL == pclass){
				logd_r("err usbh class is full!");
				break;
			}

			pclass->pdat = NULL;				//must null
			pclass->itf.if_num = pitf->bInterfaceNumber;
			pclass->itf.if_alt = pitf->bAlternateSetting;
			pclass->itf.num_endp = pitf->bNumEndpoints;
			pclass->itf.if_cls = pitf->bInterfaceClass;
			pclass->itf.if_sub_cls = pitf->bInterfaceSubClass;
			pclass->itf.if_pro = pitf->bInterfaceProtocol;

			if(ERROR_SUCCESS == usbh_get_endp(&endp, buf + i, len - i, TUSB_DIR_IN)){
				pclass->endpin = endp;
			}
			if(ERROR_SUCCESS == usbh_get_endp(&endp, buf + i, len - i, TUSB_DIR_OUT)){
				pclass->endpout = endp;
			}

			//这里区分usb class
			pclass->dev_type = usbh_match_class(id,pclass);
			logd("usbh match itft%d dev_type=%d\n",(uint16_t)pclass->itf.if_num, (uint16_t)pclass->dev_type);
			usbh_class_itf_alt_select(id,pclass);			//user select

			if((USBH_NULL != pclass->id) && (DEV_TYPE_NONE != pclass->dev_type)){
				err = usbh_class_init(id, pclass, buf + i, len - i);
				if(ERROR_SUCCESS == err){
					logd("usbh match dev_type=%d, hid_types=%x\n",(uint16_t)pclass->dev_type, pclass->hid_types);
					list_add(&pclass->list, &pdev->class_list);
				}
			}else{
				free_usbh_class(pclass);		//free pclass
			}
		}
	}

	if(!list_empty(&pdev->class_list)){
		err = usbh_class_open(id);
		if(err){					//释放资源
			usbh_class_deinit(id);
		}
	}else{
		err = ERROR_UNSUPPORT;
	}
	return err;
}

static error_t usbh_enum_device( uint8_t id, uint8_t reset_ms )
{
	error_t err = ERROR_UNKNOW;
    uint8_t i;
	uint8_t tmp_buf[20];
    uint16_t len;
	usbh_dev_t* pdev = get_usbh_dev(id);
	usb_desc_device_t *pdev_desc = (usb_desc_device_t *)tmp_buf;
	usb_desc_configuration_t *pcfg_desc = (usb_desc_configuration_t *)tmp_buf;
	uint8_t *pcfg_buf;

	if(NULL == pdev ) return err;
	logd("\nusbh%x state=%d, enum start...\n",(uint16_t)id,(uint16_t)pdev->state);

	switch(pdev->state){
	case TUSB_STA_POWERED:
		err = usbh_reset(id,reset_ms);
		if(ERROR_SUCCESS != err) return err;
		logd("usbh%x powered=%d t=%d\n",(uint16_t)id, (uint16_t)err, reset_ms);
		break;
	case TUSB_STA_DEFAULT:
		err = usbh_set_address(id, id | 0x20);
		if(ERROR_SUCCESS != err) return err;
		logd("usbh%x address=%d\n",(uint16_t)id, (uint16_t)err);
		len = 8;
		err = usbh_req_get_device_desc(id,tmp_buf,&len);
		if(ERROR_SUCCESS != err) return err;
		pdev->endp0_mtu = pdev_desc->bMaxPacketSize0;				//set endp0 mtu

		len = sizeof(usb_desc_device_t);
		err = usbh_req_get_device_desc(id,tmp_buf,&len);
		if(ERROR_SUCCESS != err) return err;

		logd("usbh get device desc:");dumpd(pdev_desc,sizeof(usb_desc_device_t));
		pdev->vid = SWAP16_L(pdev_desc->idVendor);
		pdev->pid = SWAP16_L(pdev_desc->idProduct);

		for(i=0; i < pdev_desc->bNumConfigurations; i++){			//支持多配置描述符

			len = sizeof(usb_desc_configuration_t);
			err = usbh_req_configuration_desc(id, i, tmp_buf, &len);
			if(ERROR_SUCCESS != err) return err;

			len =  SWAP16_L(pcfg_desc->wTotalLength);
			pcfg_buf = emf_malloc(len);
			err = usbh_req_configuration_desc(id, i, pcfg_buf, &len);
			if(ERROR_SUCCESS != err) return err;
			logd("usbh get config desc:");dumpd(pcfg_buf, len);

			err = usbh_parse_configuration_desc(id, i+1 ,pcfg_buf, len);
			emf_free(pcfg_buf);
			
			if( ERROR_SUCCESS == err ){			//Configuration只能设置有一个, 成功枚举到一个后就退出(LG激活过程如果有枚举另外一个cfg会出错)
				break;
			}
		}
		break;
	default:
		break;
	}
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static void usbh_enum_all_device( uint32_t period_10us )
{
	error_t err;
	uint8_t id;
    static timer_t s_enumtimer = 0;
    static uint8_t  s_retry = 0;        //失败时重试次数
	
	if(m_task_tick10us - s_enumtimer >= period_10us){
		s_enumtimer = m_task_tick10us;

		id = usbh_find_by_status(BIT(TUSB_STA_DEFAULT));		//enum default dev first 
		if(USBH_NULL == id){
			id = usbh_find_by_status(BIT(TUSB_STA_POWERED));	//enum powered dev second
		}
		if(USBH_NULL == id){								// enum attached dev alse
			id = usbh_find_by_status(BIT(TUSB_STA_ATTACHED));
			if(USBH_NULL != id){  //查找到刚插入的设备  初始化枚举参数
				s_enumtimer = m_task_tick10us;		//等待设备稳定
				s_retry = 0;
				usbh_set_status(id, TUSB_STA_POWERED, 0);
			}
		}

		if(USBH_NULL != id){
			usbh_dev_t* pdev = get_usbh_dev(id);
			if((NULL != pdev ) && s_retry <= USBH_ENUM_RETRY){
				err = usbh_enum_device( id , MIN(60,(s_retry+1)*10));

				if (( err != ERROR_SUCCESS ) && ( err != ERROR_UNSUPPORT )){
					logd( "usbh enum err = %d\n\n", (uint16_t)(err) );
					s_retry++;
					usbh_set_status(id, TUSB_STA_POWERED, 0);		//枚举还未达到最大次数 重新枚举
				}else if(ERROR_UNSUPPORT == err){
		    		usbh_disconnect( id );
					logd_r("root enum unsupport \n\n");
				}else if(pdev->state > TUSB_STA_ADDRESSING){
					logd_g("root enum success...\n\n");
				}
			}else{
				usbh_disconnect( id );
				logd("root enum err \n\n");
			}
		}
    }
}



/*******************************************************************
** Parameters:		
** Returns:	
** Description:	usb 事件, 在usb 中断事件产生后调用	
	用户可消息多种处理方式,用户可以自定义修改:
	1. 轮训方式	(默认)
	2. 任务消息推送方式
	3. 中断直接处理方式
*******************************************************************/
__WEAK void usbh_reset_event(uint8_t id)
{
	UNUSED_PARAMETER(id);
}
__WEAK void usbh_endp_in_event(uint8_t id, uint8_t ep)
{
	#if !USBH_LOOP_ENABLE
	error_t err;
	usbh_class_t *pclass = usbh_class_find_by_ep(id,ep);
	uint8_t buf[64];
	uint16_t len;

	if(NULL != pclass){
		uint8_t buf[64];
		uint16_t len = pclass->endpin.mtu;

		err = usbh_in(id, &pclass->endpin, buf, &len,0);
		if((ERROR_SUCCESS == err) && len){
			usbh_class_in_process(id, pclass, buf, len);
		}
	}else{
		logd("usbh%x ep%x null class!\n",(uint16_t)id, (uint16_t)ep);
	}
	#endif
	
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(ep);
}

__WEAK void usbh_endp_out_event(uint8_t id, uint8_t ep)
{
	UNUSED_PARAMETER(id);
	UNUSED_PARAMETER(ep);
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description: usbh硬件初始化,包括部分参数		
*******************************************************************/
error_t usbh_init( uint8_t id )
{
	if((id>>4) >= USBH_NUM) return ERROR_FAILE;

	usbh_class_buf_init(id);
	#if (USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))) && (USBH_HID_SUPPORT & (BIT_ENUM(HID_TYPE_KB) | BIT_ENUM(HID_TYPE_MOUSE)))
	usbh_hid_km_pa_init(id);		//TODO 放一个合理的位置
	#endif

	m_usbh_types = 0;
	m_usbh_hid_types = 0;
	#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_AOA)
	m_aoa_hid_types = 0;
	#endif
	#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_IAP2)
	m_iap_hid_types = 0;
	#endif

	return usbh_core_init(id);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_deinit( uint8_t id )
{
	if((id>>4) >= USBH_NUM) return ERROR_FAILE;
	
	return usbh_core_deinit(id);
}


void usbhs_init(void)
{
	uint8_t id;

	for(id=0; id<USBH_NUM; id++){
		#if API_USBH_BIT_ENABLE
		usbh_core_pa_init(id<<4);
		#endif
		usbh_init(id<<4);
	}
}
void usbhs_deinit(void)
{
	uint8_t id;

	for(id=0; id<USBH_NUM; id++){
		usbh_deinit(id<<4);
	}
}

void usbh_task(uint32_t dt_ms)
{
	hal_usbh_driver_task( dt_ms );
	usbh_class_task( dt_ms );
	usbh_enum_all_device( 200*100 );
}

#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void usbh_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		usbh_task(period_10us/100);
	}
}
#endif


#endif






