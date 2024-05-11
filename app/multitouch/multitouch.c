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

android: android手机竖屏模式下原点在左上角, 横屏模式下也是在左上角,坐标如下
ios: ios手机同样竖屏模式下原点在左上角, 横屏模式下也是在左上角,坐标如下
multitouch: multitouch坐标原点在不同系统和手机上表现形式不一样, 有的回跟随旋转有的不会
screen x,y
	0---------------->x
	|				|
	|				|
	|				|
	|				|
	|				|
	|				|
	|				|
	|	  home		|
   \|/--------------|
	Y 

point id: 	触摸点id号,用于层应用产生触摸点，范围0~0XFFFE ID_NULL 无效id
			point id由用户自定义, 可以和按键绑定, 也可以和配置序号绑定,也可以根据触摸事件替增分配
slot id: 	上层应用同时支持多少个点同时按下,下标对应 slot_id,可以大于驱动点个数
contant:	底层驱动一次支持多少个点数据发送，决定数据发送长度，可以分多次发送
identifier: contant中触摸点的id, 使用 slot_id+1 作为identifier
************************************************************************************************************/
#include "hw_config.h"
#if APP_MT_ENABLE
#include "app/multitouch/multitouch.h"
#include "api/api_tick.h"
#include "api/api_log.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
multitouch_info_t multitouch_info;

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
uint8_t m_slot_num;
uint8_t m_contact_num;		//touch slot contaact num
uint8_t m_slot_bottom;		// m_slot_bottom < slot_malloc动态分配slot < m_slot_top
uint8_t m_slot_top;
mt_slot_t mt_slot_buf[MT_SLOT_MAX];
mt_slot_t s_mt_slot_buf[MT_SLOT_MAX];

void multitouch_info_dump(multitouch_info_t* pinfo)
{
	logd("multitouch: turn_xy=%d,switch_xy=%d,hdmi=%d\n",pinfo->turn_xy,pinfo->switch_xy,pinfo->hdmi);
	logd("ios: is_ios=%d,curvet_screen=%d,mouse_vertical=%d,version=V%d.%d\n",pinfo->is_ios,pinfo->ios_curvet_screen,
		pinfo->iphone_mouse_vertical, pinfo-> ios_version>>8,pinfo-> ios_version&0xff);
	logd("screen:%d %d, hdim screen:%d %d\n",pinfo->screen.x,pinfo->screen.y,pinfo->hdmi_screen.x,pinfo->hdmi_screen.y);
}

/*******************************************************************
** Parameters:
** Returns:	
** Description:	检测限制点范围
*******************************************************************/
bool multitouch_is_beyond(int16_t x, int16_t y,int16_t boundx, int16_t boundy)
{
	if((x <= boundx) || (x >= (TOUCH_X_LOGICAL_MAX-boundx))
		|| (y <= boundy) || (y >= (TOUCH_X_LOGICAL_MAX-boundy))){
		return true;
	}
	return false;
}

bool moutitouch_limit_bound(int16_t* px, int16_t* py,int16_t boundx, int16_t boundy)
{
	int16_t x,y;

	x = MAX(boundx,*px);
	x = MIN(TOUCH_X_LOGICAL_MAX-boundx,x);
	y = MAX(boundy,*py);
	y = MIN(TOUCH_Y_LOGICAL_MAX-boundy,y);

	if((*px != x) || (*py != y)){
		*px = x;
		*py = y;
		return true;
	}
	return false;
}
/*******************************************************************
** Parameters:
** Returns:	
** Description:	查找当前屏幕按下的点坐标
*******************************************************************/
bool moutitouch_get_point_by_slot(uint8_t slot,int16_t* px, int16_t* py)
{	
	bool ret = false;

	if((uint8_t)ID_NULL != slot){
		ret = (bool)s_mt_slot_buf[slot].tip_switch;
		if(NULL != px) *px = s_mt_slot_buf[slot].x;
		if(NULL != py) *py = s_mt_slot_buf[slot].y;
	}
	return ret;
}

bool moutitouch_get_point_by_id(uint16_t id,int16_t* px, int16_t* py)
{	
	bool ret = false;
	uint8_t find_slot;

	if((uint8_t)ID_NULL != id){
		find_slot = moutitouch_find_slot(id,0);
		return moutitouch_get_point_by_slot(find_slot, px,  py);
	}
	return ret;
}



/*******************************************************************
** Parameters:	priority:优先级，1 为高， 0 为低优先级	
** Returns:	slot
** Description:	固定分配slot号	
*******************************************************************/
uint8_t moutitouch_malloc_slot(uint16_t id, uint8_t priority)
{
	uint8_t slot = ID_NULL;

	if(priority){
		if(m_slot_bottom < m_slot_top){
			slot = m_slot_bottom;
			moutitouch_fill_slot(m_slot_bottom,0,0,0);
			mt_slot_buf[m_slot_bottom].point_id = id;
			m_slot_bottom++;
		}
	}else{
		if(m_slot_top > m_slot_bottom){
			m_slot_top--;
			slot = m_slot_top;
			moutitouch_fill_slot(m_slot_top,0,0,0);
			mt_slot_buf[m_slot_top].point_id = id;
		}
	}
	return slot;
}


/*******************************************************************
** Parameters:	active:0 不自动分配id,1:如果没有找到会自动分配一个
** Returns:	
** Description:	查找slot
*******************************************************************/
uint8_t  moutitouch_find_slot(uint16_t id,uint8_t active)
{
	uint8_t find_slot,i;

	if((uint16_t)ID_NULL == id) return ID_NULL;

	//find
	find_slot = ID_NULL;
	for(i=m_slot_bottom; i< m_slot_top; i++){
		if(mt_slot_buf[i].point_id == id){
			find_slot  = i;
			break;
		}
	}

	//malloc
	if(((uint8_t)ID_NULL == find_slot) && active){		//auto slot
		for(i=m_slot_bottom; i< m_slot_top; i++){
			if((uint16_t)ID_NULL == mt_slot_buf[i].point_id){
				find_slot  = i;
				mt_slot_buf[i].point_id = id;
				// logd("malloc id%d slot=%d",id,i);
				break;
			}
		}
	}
	return find_slot;
}

/*******************************************************************
** Parameters:	
** Returns:	
** Description: 填充触摸点数据
*******************************************************************/
bool  moutitouch_fill_slot(uint8_t slot,uint8_t active,int16_t x, int16_t y)
{
	bool ret = false;

	if((uint16_t)ID_NULL == slot) return ret;

	moutitouch_limit_bound(&x,&y,0,0);

	if(active){		//second press
		if(mt_slot_buf[slot].tip_switch){		//second press
			if((mt_slot_buf[slot].x != x) || (mt_slot_buf[slot].y != y)){	//move
				ret = true;	
			}
		}else {									//准备按下
			ret = true;
		}
	}else{
		if(mt_slot_buf[slot].tip_switch){		//准备抬起
			ret = true;
		}
	}
	
	if(ret){
		mt_slot_buf[slot].in_range = 1;
		mt_slot_buf[slot].identifier = slot;
		mt_slot_buf[slot].tip_switch = active;
		if(active){
			mt_slot_buf[slot].x = x;
			mt_slot_buf[slot].y = y;
		}
		// logd("fill=");dumpd((uint8_t*)&mt_slot_buf,sizeof(mt_slot_buf));
	}
	return ret;
}

bool  moutitouch_fill_id(uint16_t id,uint8_t active,int16_t x, int16_t y)
{
	uint8_t find_slot;

	find_slot = moutitouch_find_slot(id,active);

	if((uint8_t)ID_NULL != find_slot){
		return  moutitouch_fill_slot(find_slot,active, x, y);
	}

	return false;
}



bool  moutitouch_clear_all(void)
{
	bool ret = false;
	uint8_t i;

	for(i=0; i< m_slot_num; i++){
		if(mt_slot_buf[i].tip_switch){
			mt_slot_buf[i].in_range = 1;
			mt_slot_buf[i].identifier = i;
			mt_slot_buf[i].tip_switch = 0;
			ret = true;
		}
	}
	
	return ret;
}


/*******************************************************************
** Parameters:	driver_x,driver_y:设备驱动分辨率
				display_screen.x,display_screen.y:显示器屏幕分辨率
** Returns:	pcut: 投屏黑边
** Description: 转换屏幕显示
*******************************************************************/
void moutitouch_cut_screen(bool switch_xy,axis2i_t screen,axis2i_t display_screen, axis2i_t* pcut)
{	
	axis2i_t driver_screen;		//手机屏幕

	if(switch_xy){
		driver_screen.x	 = screen.y;
		driver_screen.y	 = screen.x;
	}else{
		driver_screen	 = screen;
	}

	//防止计算溢出和除0
	if(0 == display_screen.x)display_screen.x = HDMI_DEFAULT_SCREEN_X;
	if(0 == display_screen.y)display_screen.y = HDMI_DEFAULT_SCREEN_Y;
	if(0 == driver_screen.x)driver_screen.x = HDMI_DEFAULT_SCREEN_X;
	if(0 == driver_screen.y)driver_screen.y = HDMI_DEFAULT_SCREEN_Y;
	//driver_screen.x /= 10;
	//driver_screen.y /= 10;
	display_screen.x /= 10;
	display_screen.y /= 10;

	
	if( (driver_screen.x*100/driver_screen.y) > (display_screen.x * 100 / display_screen.y)){
		pcut->x = 0;
		pcut->y = (((uint32_t)display_screen.x*TOUCH_Y_LOGICAL_MAX/2*driver_screen.y)/driver_screen.x/display_screen.y);
		pcut->y = (TOUCH_Y_LOGICAL_MAX/2 - pcut->y);
		
	}else{
		pcut->x = (((uint32_t)display_screen.y*TOUCH_X_LOGICAL_MAX*driver_screen.x)/driver_screen.y/display_screen.x);
		pcut->x = (TOUCH_X_LOGICAL_MAX - pcut->x)/2;
		pcut->y = 0;
	}
	logd("pcut->x =%d, y=%d\n",pcut->x,pcut->y);
	logd("switchxy=%d,dev=%d %d,hdim=%d %d\n",(uint16_t)switch_xy, driver_screen.x,driver_screen.y,display_screen.x,display_screen.y);
}

#if WEAK_ENABLE
__WEAK bool multitouch_transport_send(trp_handle_t *phandle,uint8_t* buf, uint8_t len)
{
	if(NULL == phandle) 	return false;
	logd("multitouch:");dumpd(buf,len);
	return true;
}
#endif
/*******************************************************************
** Parameters:		
** Returns:	true: 数据同步成功	
** Description:		
*******************************************************************/
bool multitouch_sync(trp_handle_t *phandle)
{
	bool ret = true;
	uint8_t i,j;
	int16d_t x,y;
	multitouch_t touch;

	memset(&touch,0,sizeof(touch));
	for(i=0,j=0; i< m_slot_num;i++){
		if(memcmp(&s_mt_slot_buf[i],&mt_slot_buf[i],sizeof(mt_slot_buf[i]))){
			#if HIDD_SUPPORT & (BIT_ENUM(HID_TYPE_MT) | BIT_ENUM(HID_TYPE_TOUCH))
			touch.id = TOUCH_REPORT_ID;
			#endif
			touch.contact[j].tip_switch = mt_slot_buf[i].tip_switch;
			touch.contact[j].in_range = mt_slot_buf[i].in_range;
			touch.contact[j].identifier = mt_slot_buf[i].identifier;
			
			x = mt_slot_buf[i].x;
			y = mt_slot_buf[i].y;
			if(multitouch_info.hdmi ^ multitouch_info.switch_xy){
				x = mt_slot_buf[i].y;
				y = TOUCH_Y_LOGICAL_MAX-mt_slot_buf[i].x; 
			}
			if(multitouch_info.turn_xy){
				x = TOUCH_X_LOGICAL_MAX - x;
				y = TOUCH_Y_LOGICAL_MAX - y;
  			}	
			touch.contact[j].x = x;
			touch.contact[j].y = y;		

			touch.contact[j].identifier++;	//just for bt id from 1
			if(++j >= m_contact_num){
				i++;
				break;
			}
		}
	}

	if(j){
		if(multitouch_transport_send(phandle, &touch, 1 + m_contact_num*sizeof(mt_contact_t))){
			while(i--){
				if(!mt_slot_buf[i].tip_switch){
					uint16_t point_id = mt_slot_buf[i].point_id;

					memset(&mt_slot_buf[i],0,sizeof(mt_slot_buf[i]));
					if((i >= m_slot_bottom) &&  (i < m_slot_top)){
						mt_slot_buf[i].point_id = ID_NULL;
					}else{
						mt_slot_buf[i].point_id = point_id;		//分配的清除id
					}
				}
				s_mt_slot_buf[i] = mt_slot_buf[i];		//sync
			}
		}else{
			ret = false;
		}	
	}
	return ret;
}



void  multitouch_reinit(void)
{
	uint8_t i;
	memset(mt_slot_buf,0,sizeof(mt_slot_buf));
	memset(s_mt_slot_buf,0,sizeof(s_mt_slot_buf));
	for(i=0; i<countof(mt_slot_buf); i++){
		mt_slot_buf->point_id = ID_NULL;
	}

	logd("slot=%d,contact=%d\n",(uint16_t)m_slot_num,(uint16_t)m_contact_num);
	multitouch_info_dump(&multitouch_info);
}

void  multitouch_init(uint8_t slot_num, uint8_t contact_num)
{
	m_slot_num = slot_num;
	m_contact_num = contact_num;
	if(m_slot_num > countof(s_mt_slot_buf))  m_slot_num = countof(s_mt_slot_buf);
	if(m_contact_num > MT_CONTACT_MAX)  m_contact_num = MT_CONTACT_MAX;
	m_slot_top = m_slot_num;
	m_slot_bottom = 0;

	memset(&multitouch_info, 0, sizeof(multitouch_info));
	multitouch_info.ios_version = 0XF00;
	multitouch_info.iphone_mouse_vertical = 1;
	multitouch_info.screen.x = MT_DEFAULT_SCREEN_X;
	multitouch_info.screen.y = MT_DEFAULT_SCREEN_Y;
	multitouch_info.hdmi_screen.x = HDMI_DEFAULT_SCREEN_X;
	multitouch_info.hdmi_screen.y = HDMI_DEFAULT_SCREEN_Y;

	multitouch_reinit();
}

void  multitouch_deinit(void)
{
	uint8_t i;
	memset(mt_slot_buf,0,sizeof(mt_slot_buf));
	memset(s_mt_slot_buf,0,sizeof(s_mt_slot_buf));
	for(i=0; i<countof(mt_slot_buf); i++){
		mt_slot_buf->point_id = ID_NULL;
	}
}

#ifdef __cplusplus
}
#endif

#endif

