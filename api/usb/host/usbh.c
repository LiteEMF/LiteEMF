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
#if APP_USBH_ENABLE
#include "api/usb/host/usbh.h"
#include "api/api_tick.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
usbh_dev_t m_usbh_dev[USBH_MAX_PORTS][HUB_MAX_PORTS+1];

uint16_t m_usbh_types;			//TODO 是否需要使用
uint16_t m_usbh_hid_types;
uint16_t usbh_iap_hid_types;
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
	usbh_disconnect(id);
	if(attached){
		//设置 USB_STA_ATTACHED
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
	if(id & 0X0F == 0){  				//直接接的root端口
		err = hal_usbh_port_disable(id);
		if(ERROR_SUCCESS == err){			
			for(i = 0; i < HUB_MAX_PORTS+1; i++){		//disconnect hub por also
				usbh_set_status(id | i, USB_STA_DETACHED, 0);
			}
		}
	}else{
		#if USBH_HUB_ENABLE
		err =  usbh_hub_port_disable(id);
		if(ERROR_SUCCESS = er){
			usbh_set_status(id | i, USB_STA_DETACHED, 0);
		}
		#endif
	}
	return err;
}


error_t usbh_reset(uint8_t id)
{
	uint8_t i;
	error_t err = ERROR_STATE;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev ) return err;
	if(pdev->state >= USB_STA_POWERED){
		if(id & 0X0F == 0){
			err = hal_usbh_port_reset(id);

			for(i = 0; i < HUB_MAX_PORTS+1; i++){		//disconnect hub por also
				usbh_set_status(id | i, USB_STA_DETACHED, 0);
			}
		}else{
			#if USBH_HUB_ENABLE
			err =  usbh_hub_port_reset(id);
			#endif
		}
		usbh_set_status(id, USB_STA_DEFAULT, 0);  
	}
	return err;
}

error_t usbh_set_address(uint8_t id,uint8_t addr)
{
	error_t err = ERROR_STATE;
	usbh_dev_t* pdev = get_usbh_dev(id);

	if(NULL == pdev ) return err;
	if(pdev->state == USB_STA_DEFAULT){
		err = usbh_set_addr(id,addr);
		if(ERROR_SUCCESS == err){
			if(id & 0X0F == 0){
				hal_usbh_set_addr(id,addr);
			}
			usbh_set_status(id, USB_STA_ADDRESSING, addr);  
		}
	}
	return err;
}
/*******************************************************************
** Parameters:	id: usb id
                preq: request 请求
                buf: 发送/接收数据buf
                plen:   输入:发送数据长度/ 接收数据长度 
                        输出:实际发送/接收的数据长度
** Returns:	
** Description:	执行控制传输
*******************************************************************/
error_t usbh_ctrl_transfer( uint8_t id, usb_control_request_t* preq,uint8_t* buf, uint16_t* plen)
{
    return hal_usbh_ctrl_transfer(id, preq,buf, plen);
}
error_t usbh_endp_unregister(uint8_t id,usb_endp_t *endpp)
{
    return hal_usbh_endp_unregister(id, endpp);
}
error_t usbh_endp_register(uint8_t id,usb_endp_t *endpp)
{
    return hal_usbh_endp_register(id,endpp);
}
//timeout_ms: 0不等待
error_t usbh_in(uint8_t id,usb_endp_t *endpp, uint8_t* buf,uint16_t* plen, uint16_t timeout_ms)
{
    return hal_usbh_in(id,endpp,buf,plen,timeout_ms);
}
error_t usbh_out(uint8_t id, usb_endp_t *endpp,uint8_t* buf, uint16_t len)
{
    return hal_usbh_out(id,endpp,buf,len);
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

    	if( buf[i+1] == USB_DESC_INTERFACE){
    		inf_num++;
    		if(inf_num > 1) break;
    	}

        pdesc = (usb_desc_endpoint_t*)(buf+i);
        if((pdesc->bDescriptorType == USB_DESC_ENDPOINT) && (dir == BOOL_SET(pdesc->bEndpointAddress & 0X80))){
            endp->addr = pdesc->bEndpointAddress & 0x0F;
			endp->type = pdesc->bmAttributes.xfer;
            endp->dir = dir;
			endp->mtu = SWAP16_L(pdesc->wMaxPacketSize);
            endp->interval = pdesc->bInterval;
			if(0 ==  endp->interval)	 endp->interval = 1;		//cesra add for default
			err = ERROR_SUCCESS;
            break;
        }
    }
    return( err );
}

usbh_dev_t* get_usbh_dev(uint8_t id)   
{
    usbh_dev_t *pdev = NULL;

	if (USBH_MAX_PORTS <= id>>4) return NULL;
	if (HUB_MAX_PORTS < id&0x0f) return NULL;

	pdev = &m_usbh_dev[id>>4][id&0x0f];

	return pdev;
}

uint8_t usbh_find_by_status(uint8_t usb_stas)   
{
    uint8_t i,j;
    usbh_dev_t *pdev;

    for(i = 0; i < USBH_MAX_PORTS; i++){
        for(j = 0; j < HUB_MAX_PORTS+1; j++){
            pdev = &m_usbh_dev[i][j];
            
            if(BIT(pdev->state) & usb_stas) {
				return (i<<4) | j;
            }
        }
    }
    
    return USBH_NULL;
}



error_t usbh_set_status(uint8_t id, usb_state_t usb_sta, uint8_t addr)   
{
    usbh_dev_t *pdev;
	

	if (USBH_MAX_PORTS <= id>>4) return ERROR_PARAM;
	if (HUB_MAX_PORTS < id&0x0f) return ERROR_PARAM;

	pdev = & m_usbh_dev[id>>4][(id&0x0f)+1];

	switch (usb_sta){
	case USB_STA_DETACHED:
	case USB_STA_ATTACHED:
	case USB_STA_POWERED:
		usbh_class_deinit(id);
		memset(pdev,0,sizeof(usbh_dev_t));
		INIT_LIST_HEAD(&pdev->class_list);
		break;
	case USB_STA_DEFAULT:
		pdev->addr = 0;
		pdev->endp0_mtu = DEFAULT_ENDP0_SIZE;
		break;
	case USB_STA_ADDRESSING:
		pdev->addr = addr;
		break;
	case USB_STA_CONFIGURED:
		break;
	case USB_STA_SUSPENDED:
		break;
	default:
		break;
	}	
	pdev->state = usb_sta;

    return ERROR_SUCCESS;
}



static error_t usbh_parse_configuration_desc(uint8_t id,uint8_t cfg,uint8_t *buf,uint16_t len)
{
	uint8_t err = ERROR_UNSUPPORT;
    uint16d_t   i, l;
	usbh_dev_t* pdev = get_usbh_dev(id);
	usb_desc_interface_t *pitf;
	usb_endp_t endp;
	usbh_class_t *pclass, *pos;

	err = usbh_set_configuration(id,cfg);			//set configuration
	if(err) return err;

	for ( i = 0; i < len; i += l ){            // 搜索中断端点描述符,跳过配置描述符和接口描述符
		l = buf[i];
		if(0 == l) break;

        if( buf[i+1] == USB_DESC_INTERFACE ){
			pitf = (usb_desc_interface_t*)&buf[i];
			if(pitf->bNumEndpoints){
				pclass = malloc_usbh_class();
				if(NULL == pclass) break;

				pclass->pdata = NULL;				//must null
				pclass->itf.if_num = pitf->bInterfaceNumber;
				pclass->itf.if_alt = pitf->bAlternateSetting;
				pclass->itf.num_endp = pitf->bNumEndpoints;
				pclass->itf.if_cls = pitf->bInterfaceClass;
				pclass->itf.if_sub_cls = pitf->bInterfaceSubClass;
				pclass->itf.if_pro = pitf->bInterfaceProtocol;

				if(ERROR_SUCCESS == usbh_get_endp(&endp, buf + i, len - i, USB_DIR_IN)){
					pclass->endpin = endp;
				}
				if(ERROR_SUCCESS == usbh_get_endp(&endp, buf + i, len - i, USB_DIR_OUT)){
					pclass->endpout = endp;
				}

				pclass->dev_type = usbh_match_class(id,pclass);
				if(pclass->itf.if_alt){				//only support one intfact alternate setting
					list_for_each_entry(pos,&pdev->class_list, usbh_class_t, list){
						if(pos->itf.if_num == pclass->itf.if_num){
							free_usbh_class(pclass);		//free pclass
							continue;				
						}
					}
				}

				if(DEV_TYPE_NONE != pclass->dev_type){
					err = usbh_class_init(id, pclass, buf + i, len - i);
					if(ERROR_SUCCESS == err){
						list_add(&pclass->list, &pdev->class_list);
					}
				}else{
					free_usbh_class(pclass);		//free pclass
				}
			}
		}
	}

	if(!list_empty(&pdev->class_list)){
		err = usbh_class_open(id);
		if(err){					//释放资源
			usbh_class_deinit(id);
		}
	}
	return err;
}

static error_t usbh_enum_device( uint8_t id )
{
	error_t err = ERROR_UNKNOW;
    uint8_t i,dev_indx;
	uint8_t tmp_buf[20];
    uint16_t len;
	usbh_dev_t* pdev = get_usbh_dev(id);
	usb_desc_device_t *pdev_desc = (usb_desc_device_t *)tmp_buf;
	usb_desc_configuration_t *pcfg_desc = (usb_desc_configuration_t *)tmp_buf;
	uint8_t *pcfg_buf;


	logd("\nusbh%d enum start...\n",(uint16_t)id);
	err = usbh_reset(id);
	if(ERROR_SUCCESS != err) return err;

	err = usbh_set_address(id, id | 0x80);
	if(ERROR_SUCCESS != err) return err;

	
	len = 8;
	err = usbh_get_device_desc(id,tmp_buf,&len);
	if(ERROR_SUCCESS != err) return err;
	pdev->endp0_mtu = pdev_desc->bMaxPacketSize0;				//set endp0 mtu

	len = sizeof(usb_desc_device_t);
	err = usbh_get_device_desc(id,tmp_buf,&len);
	if(ERROR_SUCCESS != err) return err;
	logd("get device desc:");dumpd(pdev_desc,sizeof(usb_desc_device_t));

	pdev->vid = SWAP16_L(pdev_desc->idVendor);
	pdev->pid = SWAP16_L(pdev_desc->idProduct);

	for(i=0; i < pdev_desc->bNumConfigurations; i++){			//支持多配置描述符

		len = sizeof(usb_desc_configuration_t);
		err = usbh_get_configuration_desc(id, i, tmp_buf, &len);
		if(ERROR_SUCCESS != err) return err;

		len =  SWAP16_L(pcfg_desc->wTotalLength);
		pcfg_buf = emf_malloc(len);
		err = usbh_get_configuration_desc(id, i, pcfg_buf, &len);
		if(ERROR_SUCCESS != err) return err;
		logd("get config desc:");dumpd(pcfg_buf,len);

		err = usbh_parse_configuration_desc(id,i,pcfg_buf,len);
		emf_free(pcfg_buf);
		
		if( ERROR_SUCCESS == err ){			//Configuration只能设置有一个, 成功枚举到一个后就退出(LG激活过程如果有枚举另外一个cfg会出错)
			break;
		}
	}
	return err;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
static void usbh_enum_all_device( void )
{
	error_t err;
	uint8_t id;
    static timer_t s_enumtimer = 0;
    static uint8d_t  s_retry = 0;        //失败时重试次数
	
	if(m_systick - s_enumtimer >= 100){
		s_enumtimer = m_systick;

		id = usbh_find_by_status(BIT(USB_STA_POWERED) | BIT(USB_STA_DEFAULT));

		if(USBH_NULL == id){
			id = usbh_find_by_status(BIT(USB_STA_ATTACHED));
			if(USBH_NULL != id){  //查找到刚插入的设备  初始化枚举参数
				s_enumtimer = m_systick;		//等待设备稳定
				s_retry = 0;
				usbh_set_status(id, USB_STA_POWERED, 0);
			}
		}else{
			if(s_retry++ < 3){
				err = usbh_enum_device( id );
				if (( err != ERROR_SUCCESS ) && ( err != ERROR_UNSUPPORT )){
					logd( "usbh enum err = %X\n\n", (uint16_t)(err) );
					usbh_set_status(id, USB_STA_POWERED, 0);		//枚举还未达到最大次数 重新枚举
				}else{
					logd("root enum suc...\n\n");
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
** Description:		
*******************************************************************/
error_t usbh_init( uint8_t id )
{
	uint8_t i;
	usbh_dev_t *pdev = (usbh_dev_t *)m_usbh_dev;
	
	usbh_class_buf_init();
	#if USBH_TYPE_SUPPORT & (BIT_ENUM(DEV_TYPE_HID) | BIT_ENUM(DEV_TYPE_AOA))
	hid_km_items_init();		//TODO 放一个合理的位置
	#endif

	memset(&m_usbh_dev, 0 ,sizeof(m_usbh_dev));
	for(i = 0; i < USBH_MAX_PORTS * (HUB_MAX_PORTS+1); i++,pdev++){
		INIT_LIST_HEAD(&pdev->class_list);
	}

	m_usbh_types = 0;
	m_usbh_hid_types = 0;
	aoa_hid_types = 0;
	usbh_iap_hid_types = 0;

	return hal_usbh_driver_init(id);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
error_t usbh_deinit( uint8_t id )
{
	return hal_usbh_driver_deinit(id);
}


void usbh_handler(void)
{
	hal_usbh_driver_handler( );
	usbh_class_handler( );
	usbh_enum_all_device( );
}



#endif






