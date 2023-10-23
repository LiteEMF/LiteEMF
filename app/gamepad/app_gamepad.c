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
#if APP_GAMEAPD_ENABLE
#include  "api/api_tick.h"
#include  "app/gamepad/app_gamepad.h"

#if APP_JOYSTICK_ENABLE
#include  "app/app_joystick.h"
#endif
#if APP_IMU_ENABLE
#include  "app/imu/app_imu.h"
#endif

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

#if(HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_SWITCH)
static uint32c_t switch_key_map[20][2] =
{
	{HW_KEY_A 			,SWITCH_A		  },
	{HW_KEY_B 			,SWITCH_B		  },
	{HW_KEY_X 			,SWITCH_X		  },
	{HW_KEY_Y 			,SWITCH_Y		  },
	{HW_KEY_L1			,SWITCH_L1		  },
	{HW_KEY_R1			,SWITCH_R1		  },
	{HW_KEY_R2			,SWITCH_R2		  },
	{HW_KEY_L2			,SWITCH_L2		  },
	{HW_KEY_R3			,SWITCH_R3		  },
	{HW_KEY_L3			,SWITCH_L3		  },
	{HW_KEY_SELECT		,SWITCH_MINUS	  },
	{HW_KEY_START		,SWITCH_POSITIVE  },
	{HW_KEY_UP			,SWITCH_UP		  },
	{HW_KEY_DOWN		,SWITCH_DN		  },
	{HW_KEY_LEFT		,SWITCH_LEFT	  },
	{HW_KEY_RIGHT 		,SWITCH_RIGHT	  },
	{HW_KEY_HOME		,SWITCH_HOME	  },
	{HW_KEY_CAPTURE		,SWITCH_CAPTURE   },
};
#endif


#if (HIDD_SUPPORT | HIDH_SUPPORT) & (BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5))
static uint32c_t ps4_key_map[20][2] =
{
	{HW_KEY_A 			,PS4_O			},
	{HW_KEY_B 			,PS4_X			},
	{HW_KEY_X 			,PS4_TRIANGLE	},
	{HW_KEY_Y 			,PS4_SQUARE		},
	{HW_KEY_L1			,PS4_L1			},
	{HW_KEY_R1			,PS4_R1			},
	{HW_KEY_L2			,PS4_L2			},
	{HW_KEY_R2			,PS4_R2			},
	{HW_KEY_R3			,PS4_R3			},
	{HW_KEY_L3			,PS4_L3			},
	{HW_KEY_SELECT 		,PS4_SHARE		},
	{HW_KEY_START		,PS4_OPTION		},
	{HW_KEY_UP			,PS4_UP			},
	{HW_KEY_DOWN		,PS4_DOWN		},
	{HW_KEY_LEFT		,PS4_LEFT		},
	{HW_KEY_RIGHT 		,PS4_RIGHT		},
	{HW_KEY_HOME		,PS4_PS			},
	{HW_KEY_CAPTURE		,PS4_TOUCH_KEY	},
};
#endif
#if((HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_PS3))
static uint32c_t ps3_key_map[20][2] =
{
	{HW_KEY_A 		,PS3_O			},
	{HW_KEY_B 		,PS3_X			},
	{HW_KEY_X 		,PS3_TRIANGLE	},
	{HW_KEY_Y 		,PS3_SQUARE		},
	{HW_KEY_L1		,PS3_L1			},
	{HW_KEY_R1		,PS3_R1			},
	{HW_KEY_L2		,PS3_L2			},
	{HW_KEY_R2		,PS3_R2			},
	{HW_KEY_L3		,PS3_L3			},
	{HW_KEY_R3		,PS3_R3			},
	{HW_KEY_SELECT	,PS3_SELECT		},
	{HW_KEY_START 	,PS3_START		},
	{HW_KEY_UP		,PS3_UP			},
	{HW_KEY_DOWN	,PS3_DOWN		},
	{HW_KEY_LEFT	,PS3_LEFT		},
	{HW_KEY_RIGHT 	,PS3_RIGHT		},
	{HW_KEY_HOME	,PS3_PS			},
};
#endif
#if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_XBOX)
static uint32c_t xbox_key_map[20][2] =
{
	{HW_KEY_A 			,XBOX_A 	},
	{HW_KEY_B 			,XBOX_B 	},
	{HW_KEY_X 			,XBOX_X 	},
	{HW_KEY_Y 			,XBOX_Y 	},
	{HW_KEY_L1			,XBOX_L1	},
	{HW_KEY_R1			,XBOX_R1	},
	{HW_KEY_L2			,XBOX_L2	},
	{HW_KEY_R2			,XBOX_R2	},
	{HW_KEY_L3			,XBOX_L3	},
	{HW_KEY_R3			,XBOX_R3	},
	{HW_KEY_SELECT		,XBOX_MAP	},
	{HW_KEY_START 		,XBOX_MENU 	},
	{HW_KEY_UP			,XBOX_UP	},
	{HW_KEY_DOWN		,XBOX_DOWN	},
	{HW_KEY_LEFT		,XBOX_LEFT	},
	{HW_KEY_RIGHT 		,XBOX_RIGHT },
	{HW_KEY_HOME		,XBOX_HOME	},
	{HW_KEY_CAPTURE		,XBOX_SHARE	},
};
#endif
#if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_X360)
static uint32c_t x360_key_map[20][2] =
{
	{HW_KEY_A 			,X360_A		},
	{HW_KEY_B 			,X360_B		},
	{HW_KEY_X 			,X360_X		},
	{HW_KEY_Y 			,X360_Y		},
	{HW_KEY_L1			,X360_L1	},
	{HW_KEY_R1			,X360_R1	},
	{HW_KEY_L2			,X360_L2	},
	{HW_KEY_R2			,X360_R2	},
	{HW_KEY_L3			,X360_L3	},
	{HW_KEY_R3			,X360_R3	},
	{HW_KEY_SELECT		,X360_BACK	},
	{HW_KEY_START 		,X360_START	},
	{HW_KEY_DOWN		,X360_DOWN	},
	{HW_KEY_UP			,X360_UP	},
	{HW_KEY_LEFT		,X360_LEFT	},
	{HW_KEY_RIGHT 		,X360_RIGHT	},
	{HW_KEY_HOME		,X360_HOME	},
};
#endif

#if (HIDD_SUPPORT | HIDH_SUPPORT) & (BIT_ENUM(HID_TYPE_GAMEPADE) | BIT_ENUM(HID_TYPE_XBOX))
static uint32c_t gamepad_key_map[20][2] =
{
	{HW_KEY_A 			,GAMEPAD_A		},
	{HW_KEY_B 			,GAMEPAD_B		},
	{HW_KEY_X 			,GAMEPAD_X		},
	{HW_KEY_Y 			,GAMEPAD_Y		},
	{HW_KEY_L1			,GAMEPAD_L1		},
	{HW_KEY_R1			,GAMEPAD_R1		},
	{HW_KEY_L2			,GAMEPAD_L2		},
	{HW_KEY_R2			,GAMEPAD_R2		},
	{HW_KEY_L3			,GAMEPAD_L3		},
	{HW_KEY_R3			,GAMEPAD_R3		},
	{HW_KEY_SELECT		,GAMEPAD_SELECT	},
	{HW_KEY_START 		,GAMEPAD_START	},
	{HW_KEY_DOWN		,GAMEPAD_DOWN	},
	{HW_KEY_UP			,GAMEPAD_UP		},
	{HW_KEY_LEFT		,GAMEPAD_LEFT	},
	{HW_KEY_RIGHT 		,GAMEPAD_RIGHT	},
	{HW_KEY_HOME		,GAMEPAD_HOME	},
};
#endif

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if WEAK_ENABLE
__WEAK uint8_t app_gamepad_get_vendor_map(trp_handle_t *phandle,uint32_t(**mapp)[2])
{
	return 0;
}
#endif

hid_type_t app_gamepad_get_hidtype(uint16_t hid_types)
{
	hid_type_t hid_type = HID_TYPE_NONE;

	if(HID_GAMEPAD_MASK & hid_types){
		if(BIT(HID_TYPE_GAMEPADE) & hid_types){
			hid_type = HID_TYPE_GAMEPADE;
		}else if(BIT(HID_TYPE_X360) & hid_types){
			hid_type = HID_TYPE_X360;
		}else if(BIT(HID_TYPE_XBOX) & hid_types){
			hid_type = HID_TYPE_XBOX;
		}else if(BIT(HID_TYPE_SWITCH) & hid_types){
			hid_type = HID_TYPE_SWITCH;
		}else if(BIT(HID_TYPE_PS3) & hid_types){
			hid_type = HID_TYPE_PS3;
		}else if(BIT(HID_TYPE_PS4) & hid_types){
			hid_type = HID_TYPE_PS4;
		}else if(BIT(HID_TYPE_PS5) & hid_types){
			hid_type = HID_TYPE_PS5;
		}
	}
	return hid_type;
}

hid_type_t app_gamepad_get_vid_pid(trp_t trp, uint16_t hid_types, uint16_t* vidp, uint16_t* pidp)
{
	hid_type_t gamepad_type = app_gamepad_get_hidtype(hid_types);

	if(hid_types & HID_SWITCH_MASK){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK
		*vidp = SWITCH_VID;
		*pidp = SWITCH_PID;
		#endif
	}else if(hid_types & BIT_ENUM(HID_TYPE_PS3)){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
		*vidp = PS_VID;
		*pidp = PS3_PID;
		#endif
	}else if(hid_types & BIT_ENUM(HID_TYPE_PS4)){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
		*vidp = PS_VID;
		if(api_trp_is_usb(trp)){
			*pidp = PS4_PART3_PID;
		}else{
			*pidp = PS4_PID;
		}
		#endif
	}else if(hid_types & BIT_ENUM(HID_TYPE_PS5)){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
		*vidp = PS_VID;
		*pidp = PS5_PID;
		#endif
	}else if(hid_types & BIT_ENUM(HID_TYPE_X360)){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK
		*vidp = XBOX_VID;
		if(api_trp_is_usb(trp)){
			*pidp = X360_PID;
		}else{
			*pidp = XBOX_BT_PID;
		}
		#endif
	}else if(hid_types & BIT_ENUM(HID_TYPE_XBOX)){
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK
		*vidp = XBOX_VID;
		if(api_trp_is_usb(trp)){
			*pidp = XBOX_PID;
		}else{
			*pidp = XBOX_BT_PID;
		}
		#endif
	}
	return  gamepad_type;
}

/*******************************************************************
** Function:
** Parameters:
** Returns:		函数返回的是	手柄转换map
*******************************************************************/
uint8_t app_gamepad_get_map(trp_handle_t *phandle,uint32_t(**mapp)[2])
{
	uint8_t len;

	len = app_gamepad_get_vendor_map(phandle,mapp);
	if(len) return len;

	switch(phandle->index&0xff){
		#if(HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_SWITCH)
		case HID_TYPE_SWITCH:
			*mapp = (uint32_t(*)[2])switch_key_map;
			return countof(switch_key_map);
		#endif

		#if (HIDD_SUPPORT | HIDH_SUPPORT) & (BIT_ENUM(HID_TYPE_PS4) | BIT_ENUM(HID_TYPE_PS5))
		case HID_TYPE_PS4:
		case HID_TYPE_PS5:
			*mapp = (uint32_t(*)[2])ps4_key_map;
			return countof(ps4_key_map);
		#endif
		
		#if((HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_PS3))
		case HID_TYPE_PS3:
			*mapp = (uint32_t(*)[2])ps3_key_map;
			return countof(ps3_key_map);
		#endif

		#if((HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_XBOX))
		case HID_TYPE_XBOX:
			if(api_trp_is_usb(phandle->trp)){
				*mapp = (uint32_t(*)[2])xbox_key_map;
				return countof(xbox_key_map);
			}else{												//蓝牙模式下使用gamepad 定义
				*mapp = (uint32_t(*)[2])gamepad_key_map;
				return countof(gamepad_key_map);
			}
		#endif

		#if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_X360)
		case HID_TYPE_X360:
			if(api_trp_is_usb(phandle->trp)){
				*mapp = (uint32_t(*)[2])x360_key_map;
				return countof(x360_key_map);
			}else{												//蓝牙模式下使用gamepad 定义
				*mapp = (uint32_t(*)[2])gamepad_key_map;
				return countof(gamepad_key_map);
			}
		#endif
		#if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_GAMEPADE)
		case HID_TYPE_GAMEPADE:
			*mapp = (uint32_t(*)[2])gamepad_key_map;
			return countof(gamepad_key_map);
		#endif
		default:
			return 0;
	}
}

uint32_t app_gamepad_map_convert(uint32_t key, uint32_t(* map)[2], uint8_t map_count)
{
	uint8_t i;
	uint32_t keyout = 0;

	for(i=0; i<map_count;i++){
		if(map[i][0] & key){
			keyout |= map[i][1];
		}
	}
	return keyout;
}
uint32_t app_gamepad_map_invconvert(uint32_t key, uint32_t(* map)[2], uint8_t map_count)
{
	uint8_t i;
	uint32_t keyout = 0;

	for(i=0; i<map_count;i++){
		if(map[i][1] & key){
			keyout |= map[i][0];
		}
	}
	return keyout;
}
uint32_t app_gamepad_key_convert(trp_handle_t *phandle, uint32_t key_in)
{
	uint32_t key=0;
	uint32_t(*mapp)[2];
	uint8_t map_count;

	map_count = app_gamepad_get_map(phandle,&mapp);
	key = app_gamepad_map_convert(key_in,mapp,map_count);

	return key;
}
uint32_t app_gamepad_key_invconvert(trp_handle_t *phandle,uint32_t key_in)
{
	uint32_t key=0;
	uint32_t(*mapp)[2];
	uint8_t map_count;

	map_count = app_gamepad_get_map(phandle,&mapp);
	key = app_gamepad_map_invconvert(key_in,mapp,map_count);

	return key;
}




/*******************************************************************
** Parameters:	keyp: 标准按键	
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_key_send(trp_handle_t *phandle,app_gamepad_key_t *keyp)
{
	bool ret = false;
	uint16_t len = 0;
	app_gamepad_key_t key;

	#if ((BT_HID_SUPPORT) & HID_SWITCH_MASK)
	uint8_t buf[sizeof(switch_large_report_t)];
	#else
	uint8_t buf[78];
	#endif
	
	key = *keyp;
	key.key = app_gamepad_key_convert(phandle, key.key);			//标准按键转换对应主机按键

	switch(phandle->index & 0xff){
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            len = gamepad_key_pack(phandle, &key, buf, sizeof(buf));
            break;
        #endif
        #if (HIDD_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            len = xbox_key_pack(phandle, &key, buf, sizeof(buf));
            break;
        #endif
        #if HIDD_SUPPORT & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            len = switch_key_pack(phandle, &key, buf, sizeof(buf));
            break;
        #endif
        #if HIDD_SUPPORT & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            len = ps_key_pack(phandle, &key, buf, sizeof(buf));
            break;
        #endif
        default:
            break;
    }

    if(len){
		ret = api_transport_tx(phandle,buf,len);
	}

	return ret;
}


bool app_gamepad_dev_process(trp_handle_t *phandle, uint8_t* buf,uint8_t len)
{
	bool ret = false;

	switch(phandle->index & 0xff){
        #if HIDD_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            ret = gamepad_dev_process(phandle,buf,len);
            break;
        #endif
        #if (HIDD_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
			ret = x360_dev_process(phandle,buf,len);
			break;
        case HID_TYPE_XBOX	:
            ret = xbox_dev_process(phandle,buf,len);
            break;
        #endif
        #if HIDD_SUPPORT & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            ret = switch_dev_process(phandle,buf,len);
            break;
        #endif
        #if HIDD_SUPPORT & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            ret = ps_dev_process(phandle,buf,len);
            break;
        #endif
        default:
            break;
    }
    
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_rumble_send(trp_handle_t *phandle, rumble_t const *prumble)
{
	bool ret = false;

	switch(phandle->index & 0xff){
        #if HIDH_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            ret = gamepad_rumble_send(phandle,prumble);
            break;
        #endif
        #if (HIDH_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            ret = xbox_rumble_send(phandle,prumble);
            break;
        #endif
        #if HIDH_SUPPORT & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            ret = switch_rumble_send(phandle,prumble);
            break;
        #endif
        #if HIDH_SUPPORT & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            ret = ps_rumble_send(phandle,prumble);
            break;
        #endif
        default:
            break;
    }
    
	return ret;
}
/*******************************************************************
** Parameters:		keyp: 标准按键格式
** Returns:	
** Description:		
*******************************************************************/

bool app_gamepad_host_process(trp_handle_t* phandle,app_gamepad_key_t *keyp, uint8_t *buf, uint8_t len)
{
	bool ret = false;
	bool key_decode = false;
	app_gamepad_key_t key;

    switch(phandle->index & 0xff){
        #if HIDH_SUPPORT & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
			key_decode = gamepad_key_decode(phandle,buf,len,&key);
            if(!key_decode){
                ret = gamepad_in_process(phandle, buf, len);
            }
            break;
        #endif
        #if (HIDH_SUPPORT & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
			key_decode = xbox_key_decode(phandle,buf,len,&key);
            if(!key_decode){
                ret = xbox_in_process(phandle, buf, len);
            }
            break;
        #endif
        #if HIDH_SUPPORT & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
			key_decode = switch_key_decode(phandle,buf,len,&key);
            if(!key_decode){
                ret = switch_in_process(phandle, buf, len);
            }
            break;
        #endif
        #if HIDH_SUPPORT & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
			key_decode = ps_key_decode(phandle,buf,len,&key);
            if(!key_decode){
                ret = ps_in_process(phandle, buf, len);
            }
            break;
        #endif
        default:
            break;
    }

	if(key_decode){				//将按键转换为标准按键
		key.key = app_gamepad_key_invconvert(phandle,key.key);
		*keyp = key;
	}
	
	return ret | key_decode;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_init( trp_handle_t *phandle ) 
{
    switch(phandle->index & 0xff){
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            gamepad_controller_init(phandle);
            break;
        #endif
        #if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            xbox_controller_init(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            switch_controller_init(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            ps_controller_init(phandle);
            break;
        #endif
        default:
            break;
    }
    
    return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool app_gamepad_deinit( trp_handle_t *phandle ) 
{
    switch(phandle->index & 0xff){
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            gamepad_controller_deinit(phandle);
            break;
        #endif
        #if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            xbox_controller_deinit(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            switch_controller_deinit(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            ps_controller_deinit(phandle);
            break;
        #endif
        default:
            break;
    }
    
	return true;
}


/*******************************************************************
** Parameters:		
** Returns:	
** Description:
*******************************************************************/
void app_gamepad_task(trp_handle_t *phandle)
{
    switch(phandle->index & 0xff){
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & BIT_ENUM(HID_TYPE_GAMEPADE)
        case HID_TYPE_GAMEPADE:
            gamepad_controller_task(phandle);
            break;
        #endif
        #if ((HIDD_SUPPORT | HIDH_SUPPORT) & HID_XBOX_MASK)
        case HID_TYPE_X360	:
        case HID_TYPE_XBOX	:
            xbox_controller_task(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_SWITCH_MASK
        case HID_TYPE_SWITCH:
            switch_controller_task(phandle);
            break;
        #endif
        #if (HIDD_SUPPORT | HIDH_SUPPORT) & HID_PS_MASK
        case HID_TYPE_PS3	:
        case HID_TYPE_PS4	:
        case HID_TYPE_PS5	:
            ps_controller_task(phandle);
            break;
        #endif
        default:
            break;
    }

}



#endif






