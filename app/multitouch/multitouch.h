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


#ifndef _multitouch_h
#define _multitouch_h

#include "hw_config.h"
#include "utils/emf_utils.h"
#include "api/hid/hid_dev_desc.h"

#ifdef __cplusplus
extern "C" {
#endif




/******************************************************************************************************
** Defined
*******************************************************************************************************/


//for hdmi 投屏
#define HDMI_DEFAULT_SCREEN_X	1920
#define HDMI_DEFAULT_SCREEN_Y	1080
#define MT_DEFAULT_SCREEN_X	1080
#define MT_DEFAULT_SCREEN_Y	1920
#define MT_SLOT_MAX			5


/******************************************************************************************************
**	Parameters
*******************************************************************************************************/

/*******************************************************************
** Description:		
	ios 锁屏的情况下,鼠标一直是竖屏模式不跟随屏幕旋转
	ios 不锁屏鼠标根据屏幕方向会选择调整
*******************************************************************/
typedef  struct{
	uint8_t turn_xy:1;					//xy选择, 横竖屏旋转
	uint8_t switch_xy:1;				//xy交互, 镜像
	uint8_t hdmi:1;
	uint8_t is_ios:1;
	uint8_t curvet_screen:1;			//曲面屏坐标位置有一些偏差
	uint8_t screen_dir:1;				//0:竖屏, 1:横屏
	uint8_t simulate_touch:1;			//鼠标模拟touch
	uint8_t ret:1;

	uint16_t ios_version;				//unused
	axis2i_t screen;
	axis2i_t hdmi_screen;
}multitouch_info_t;


typedef  struct {
	uint16_t point_id;			//point id
	uint8_t tip_switch:1;		//0x42
	uint8_t in_range:1;			//0x32
	uint8_t res:2;
	uint8_t identifier:4;		//0x51, slot_id
	int16_t x;					//0x30
	int16_t y;					//0x31
}mt_slot_t;			//6


#ifndef pack
#pragma pack(1)
#endif

typedef  struct{
	uint8_t tip_switch:1;		//0x42
	uint8_t in_range:1;			//0x32
	uint8_t res:2;
	uint8_t identifier:4;		//0x51
	int16_t x;					//0x30
	int16_t y;					//0x31
}mt_contact_t;		//5

typedef  struct{
	uint8_t id;					//report id
	mt_contact_t contact[MT_CONTACT_MAX];
}multitouch_t;


#ifndef pack
#pragma pack()
#endif



extern multitouch_info_t multitouch_info;



/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern bool		multitouch_transport_send(trp_handle_t *phandle,uint8_t* buf, uint8_t len);	//__WEAK 
bool 			multitouch_is_beyond(int16_t x, int16_t y,int16_t boundx, int16_t boundy);
bool 			moutitouch_limit_bound(int16_t* px, int16_t* py,int16_t boundx, int16_t boundy);
extern bool  	moutitouch_get_point_by_slot(uint8_t slot,int16_t* px, int16_t* py);
extern bool  	moutitouch_get_point_by_id(uint16_t id,int16_t* px, int16_t* py);

extern uint8_t moutitouch_malloc_slot(uint16_t id, uint8_t priority);
extern uint8_t  moutitouch_find_slot(uint16_t id,uint8_t active);
extern bool 	moutitouch_fill_slot(uint8_t slot,uint8_t active,int16_t x, int16_t y);
extern bool  	moutitouch_fill_id(uint16_t id,uint8_t active,int16_t x, int16_t y);
extern bool  	moutitouch_clear_all(void);
extern void 	moutitouch_cut_screen(bool switch_xy, axis2i_t screen,axis2i_t display_screen, axis2i_t* pcut);
extern bool  	multitouch_sync(trp_handle_t *phandle);
extern void  	multitouch_reinit(multitouch_info_t* pinfo);
extern void     multitouch_init(uint8_t slot_num, uint8_t contact_num, multitouch_info_t* pinfo);
extern void     multitouch_deinit(void);


#ifdef __cplusplus
}
#endif


#endif
