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
**	Description:	ios 13.4以上系统使用鼠标模拟touch
************************************************************************************************************/
#include "hw_config.h"
#if APP_MT_ENABLE && (HIDD_SUPPORT & BIT_ENUM(HID_TYPE_MOUSE))
#include "ios_simulate_touch.h"
#include "api/api_tick.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/
//#define IOS134_DATA_COLLECTION	//测试用于采集IOS13.4数据

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
axis2_t ios_dir;	//方向	

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
typedef enum {
	IOS134_MOVE,
	IOS134_DOWN,
	IOS134_UP,
	IOS134_RESET,
}ios134_send_sta;

typedef  struct {
	uint8_t id;
	uint8_t down;
	float x;
	float y;
}pt_tr_t;



float pt_to_mouse(float pt_r,uint16_t ios_version)
{
	float mouse_r;
	if(ios_version >= 0xF00){
		if(pt_r <= 1){
			mouse_r = -6.744/pow(pt_r,0.1719)+9.872;
		}else if(pt_r <= 24){
			mouse_r = 0.9993*pt_r + 2.096;
		}else if(pt_r <= 39){
			mouse_r = 0.4007*pt_r + 16.33;
		}else if(pt_r <= 134.5){
			mouse_r = 0.4084*pt_r+16.06;
		}else if(pt_r <= 199){
			mouse_r = 1.035*pt_r-69.24;
		}else{
			mouse_r = 2.746*pt_r-409.9;
		}
	}else{
		if(pt_r <= 10.72){
			mouse_r = 3.726 * pow(pt_r, 0.5558) - 0.888;    // 0 ~ 10;
		}else if(pt_r <= 100.93){
			mouse_r = 4.778 * pow(pt_r, 0.4926) - 2.38;     // 10 ~ 100;
		}else if(pt_r <= 206.94){
			mouse_r = 0.5051 * pow(pt_r, 0.8453) + 19.12;   // 100 ~ 200;
		}else if(pt_r <= 341.52){
			mouse_r = 0.1899 * pow(pt_r, 1.003) + 25.16;     // 200 ~ 340;
		}else{
			mouse_r = 0.0002899 * pow(pt_r, 2) + 57.23;     // > 340;
		}
	}
	return mouse_r;
}

float mouse_to_pt(float mouse_r,uint16_t ios_version)
{
	float pt_r=0;
	if(ios_version >= 0xF00){

		if(mouse_r <= 3.1){
			 pt_r = 0.03576*pow(mouse_r,2.745)+0.1672;		//mouse=0.20296
		}else if(mouse_r <= 26){
			pt_r = (mouse_r - 2.096)/0.9993;
		}else if(mouse_r <= 32){
			pt_r = (mouse_r-16.33)/0.4007;
		}else if(mouse_r <= 71){
			pt_r = (mouse_r-16.06)/0.4084;
		}else if(mouse_r <= 137){
			pt_r = (mouse_r+69.24)/1.035;
		}else{
			pt_r = (mouse_r+409.9)/2.746;
		}
	}else{
		if(mouse_r <= 13.0){
			pt_r = 0.1296 * pow(mouse_r, 1.712) + 0.2165;		//mouse=0.3461
		}else if(mouse_r <= 44){
			pt_r = 0.06234 * pow(mouse_r, 1.948) + 1.589;
		}else if(mouse_r <= 65){
			pt_r = 1.326 * pow(mouse_r, 1.276) - 65.14;
		}else if(mouse_r <= 91){
			pt_r = 5.286 * pow(mouse_r, 0.9961) - 131.2;
		}else if(mouse_r <= 450){
			pt_r = 488.9 * pow(mouse_r, 0.2597) - 1231;
		}else{
			pt_r = 488.9 * pow(mouse_r, 0.261) - 1231;		//微调临时解决位置稍微偏差问题
		}
	}
	return pt_r;
}

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#ifdef IOS134_DATA_COLLECTION
uint8_t ios134_data_collection(mt_contact_t* pmt)
{
	static ios134_send_sta send_sta=IOS134_MOVE;
	static timer_t t,timer_del=0;
	static uint16_t s_d = 0;
	
	static pt_tr_t	spt;		//当前位置

	uint8_t s = ERROR_BUSY;
	pt_tr_t	pt;
	mouse_t	mouse;


	pt.id = pmt->contact[0].identifier;
	pt.down= pmt->contact[0].tip_switch;
	if(0 == pt.id){
		pt.y = 100*ios_dir.y;
	}else{
		pt.y = 0;
	}
	pt.x = ios_dir.x*s_d;

	
	if((m_systick-t) < timer_del)  return ERROR_BUSY;
	t = m_systick; timer_del = 0;

	dumpd(pmt,sizeof(*pmt));
	memset(&mouse,0,sizeof(mouse));
	p_mt_hid_tr->index = TR_MOUSE_INDEX;

	
	switch(send_sta){
	case IOS134_MOVE:			//移动必须一次完成
		if((pt.id != spt.id) && (!pt.down  && spt.down)) break;				//忽略不需要的抬起
		if((pt.down == spt.down) && (5 == pt.id && 5 != spt.id)) break;		//按键按下后摇杆移动无效

		// 使用起方式
		if((!pt.down) || (pt.id != spt.id)){
			if(spt.down){
				app_mouse_report(p_mt_hid_tr,&mouse);
				logd("mouse=%d: x=%d,y=%d\n",mouse.but,mouse.x,mouse.y);
			}
			send_sta = IOS134_RESET;
			s = ERROR_BUSY;
			break;
		}

		if((spt.x != pt.x) || spt.y != pt.y){
			mouse.x = pt.x;
			mouse.y = pt.y;
			if(app_mouse_report(p_mt_hid_tr,&mouse)){
				spt.x = pt.x;
				spt.y = pt.y;
				logd("mouse=%d: x=%d,y=%d\n",mouse.but,mouse.x,mouse.y);
			}
			timer_del = 15;
		}else if (spt.down != pt.down){			//发送down
			mouse.but = pt.down;
			if(app_mouse_report(p_mt_hid_tr,&mouse)){
				spt.down = pt.down;
				logd("mouse=%d: x=%d,y=%d\n",mouse.but,mouse.x,mouse.y);
			}
			timer_del = 15;
		}else{
			s_d += 1;
			if(pt.id == 2) s_d = 0;
			s =  ERROR_SUCCESS;
		}
	break;
	case IOS134_RESET:
		mouse.x = -800*ios_dir.x ;
		mouse.y = 0;
		if(app_mouse_report(p_mt_hid_tr,&mouse)){
			logd("mouse=%d: x=%d,y=%d\n",mouse.but,mouse.x,mouse.y);

			memset(&spt,0,sizeof(spt));
			spt.id = pt.id;
			spt.y = 0;
			spt.x = 0;
			if(!pt.down){
				s = ERROR_SUCCESS;
			}
			timer_del = 15;
			send_sta = IOS134_MOVE;
		}
		break;
	}

	return s;
}
#endif



/*******************************************************************
** Parameters:		
** Returns:	
	ERROR_SUCCESS: 	无数据发送,表示到达目标位置
	ERROR_BUSY:		有数据发送成功,需要设置busy状态, 等待下一次发送
	ERROR_SEND_ERR: 有数据发送失败,需要重新发送
** Description:	发送移动数据
*******************************************************************/
uint8_t ios_touch_send_move(trp_handle_t* phandle, pt_tr_t* pt, pt_tr_t* spt)
{
	uint8_t ret = ERROR_SUCCESS;
	
    static vector2f_t factual_vector;	//实际鼠标运行的pt值
	app_mouse_t	mouse;
	int16_t dx,dy;
    vector2f_t vector_pt;
    float pt_r, mouse_r,max_dp;			//ios15以后鼠标最大步长受到限制

	memset(&mouse,0,sizeof(mouse));
	
	vector_pt.x = pt->x - spt->x;
    vector_pt.y = pt->y - spt->y;
	// logd("dx=%d(%d-%d),dy=%d(%d-%d)\n", (int32_t)(vector_pt.x*10), (int32_t)(pt->x*10), (int32_t)(spt->x*10),
	// 	(int32_t)(vector_pt.y*10), (int32_t)(pt->y*10), (int32_t)(spt->y*10));

	vector2f_normalization(&vector_pt);
	max_dp = (multitouch_info.ios_version >= 0xF00)? 210:2400;
	pt_r = MIN(vector_pt.r,max_dp);

	if(pt_r < 0.35){			//发送down
		if(spt->down != pt->down){
			mouse.but = pt->down;
			if(app_mouse_key_send(phandle,&mouse)){
				spt->down = pt->down;
			}
		}
    }else{						//发送XY
		mouse_r = pt_to_mouse(pt_r,multitouch_info.ios_version);
        dx = round(vector_pt.x*mouse_r);
        dy = round(vector_pt.y*mouse_r);
		// logd("ptr=%d %d mosuetr=%d", (int32_t)vector_pt.r, (int32_t)(pt_r), (int32_t)mouse_r);

		// mouse 反向运算dp
		float r;
		factual_vector.x = dx;           //mouse
		factual_vector.y = dy;
		vector2f_normalization(&factual_vector);
		r = mouse_to_pt(factual_vector.r,multitouch_info.ios_version);
		factual_vector.x = factual_vector.x*r;
		factual_vector.y = factual_vector.y*r;
		// logd("offset r=%d %d,xy=%d %d", (int32_t)(pt_r*10),(int32_t)(r*10),
		// 	(int32_t)(factual_vector.x*10),(int32_t)(factual_vector.y*10));

		mouse.but = spt->down;
		mouse.x = dx;
		mouse.y = dy;
		if(app_mouse_key_send(phandle,&mouse)){
			spt->x += factual_vector.x;
			spt->y += factual_vector.y;
		}
	}

	if(vector_pt.r <= max_dp) {				//最后一个包
		if(spt->down == pt->down){
			 ret = ERROR_SUCCESS;
		}else if(pt->down){
			ret = ERROR_TIMEOUT;		//goto 结束, 标识延时,防止按下粘包
		}else{
			ret = ERROR_BUSY;
		}
	}else{
		ret = ERROR_BUSY;
	}
	//logd("ret=%d,dx=(%d-%d),dy=(%d-%d),mouse=%d,%d\n",ret, (int32_t)(pt->x*10), (int32_t)(spt->x*10),
	//	 (int32_t)(pt->y*10), (int32_t)(spt->y*10),mouse.x,mouse.y);
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	ios 鼠标模拟触屏	
		iphone_mouse_vertical			m_turn_xy	
			1						0			phone鼠标竖屏模式,不旋转屏幕
			1						1			phone鼠标竖屏模式,旋转屏幕
			0						x			ipade 都是横屏模式, 旋转屏幕不区分
*******************************************************************/
uint8_t ios_simulate_touch_tx(trp_handle_t* phandle, multitouch_t* pmt, uint8_t len)
{
	uint8_t s = ERROR_SUCCESS;
	mouse_t	mouse;
	pt_tr_t	pt;					//目标位置
	int16_t x,y;
	axis2i_t screen = multitouch_info.screen;

	static ios134_send_sta send_sta=IOS134_MOVE;
	static pt_tr_t	spt;		//当前位置
	static timer_t t;
	static uint8_t timer_del = 0;

	logd("ios tm id=%d",pmt->contact[0].identifier);
    // if(1 == pmt->contact[0].identifier){   			//左上角固定点
    // 	if(!api_transport_tx(phandle,pmt,len)){
    // 		s = ERROR_UNKNOW;
    // 	}
    //     return s;
    // }

	if((m_systick-t) < timer_del)  return ERROR_BUSY;


	t = m_systick;
	timer_del = 0;

	memset(&mouse,0,sizeof(mouse));
    //logd("down: %d, %d, %d, %d", pt.down, spt.down, pt.id, spt.id);
	switch(send_sta){
		case IOS134_MOVE:			//移动必须一次完成
			logd("move ");
			pt.id = pmt->contact[0].identifier;
			//使用鼠标侧键作为按下, ios模式下鼠标左右键,侧键都可以实现点击效果
			// if(pmt->contact[0].tip_switch == 1){
			// 	pt.down = 16;
			// }else{
			// 	pt.down = 0;
			// }
			pt.down= pmt->contact[0].tip_switch;
			logi_y("down: %d, %d, %d, %d\n", pt.down, spt.down, pt.id, spt.id);
			x = pmt->contact[0].x;
			y = pmt->contact[0].y;

			//这里调整触摸方向到鼠标方向一致
			if(multitouch_info.iphone_mouse_vertical){			//phone  竖屏鼠标模式
				if(multitouch_info.turn_xy){					//旋转屏幕到横屏
					pt.x = x * screen.x / TOUCH_X_LOGICAL_MAX - screen.x;
					pt.y = y * screen.y / TOUCH_Y_LOGICAL_MAX - screen.y;
				}else{						//不旋转屏幕
					pt.x = x * screen.x / TOUCH_X_LOGICAL_MAX;
					pt.y = y * screen.y / TOUCH_Y_LOGICAL_MAX;
				}
			}else{						//ipade  横屏幕鼠标模式
				pt.x = y * screen.y / TOUCH_Y_LOGICAL_MAX;
				pt.y = 0 - x * screen.x / TOUCH_X_LOGICAL_MAX;
			}

			if((pt.id != spt.id) && (!pt.down && spt.down)) break;				//忽略不需要的抬起
			if((pt.down == spt.down) && (5 == pt.id && 5 != spt.id)) break;		//按键按下后.  摇杆移动无效

			// 抬起
			if( (!pt.down) || ((pt.id != spt.id) && (0 != spt.id)) ){
				int16_t max;
				if(multitouch_info.ios_version >= 0xF00){
					max = 2*MAX(fabs(spt.x),fabs(spt.y));
					max = MIN(max, 1600);
				}else{
					max = 1600;
				}	
				spt.x = max*ios_dir.x;
				spt.y = max*ios_dir.y;			//将当前距离设置到最大位置
				timer_del = 5;//20;				//防止不抬起,是否需要?
				send_sta = IOS134_UP;
				s = ERROR_BUSY;
				break;
			}

			spt.id = pt.id;
			s = ios_touch_send_move(phandle,&pt, &spt);
			if(ERROR_BUSY == s){
				timer_del = 5;
			}else if(ERROR_TIMEOUT == s){
				timer_del = 20;
			}
			break;
		case IOS134_UP:
			logd("up\n");
			if(app_mouse_key_send(phandle,&mouse)){
				spt.down = 0;
				timer_del = 5;
				send_sta = IOS134_RESET;
			}
			s = ERROR_BUSY;
			break;
		case IOS134_RESET:					//TODO reset 可以moredata 方式发送
			memset(&pt,0,sizeof(pt));		//reset 目标值的坐标(0,0)
			s = ios_touch_send_move(phandle, &pt, &spt);
			logd("reset=%d\n",s);
			if(ERROR_SUCCESS == s){
				memset(&spt,0,sizeof(spt));
				if(multitouch_info.ios_curvet_screen){
					spt.x=13*ios_dir.x;				//如果是曲面屏做补偿
					spt.y=15*ios_dir.y;
				}

				if(pmt->contact[0].tip_switch){		//有下一个按键按下
					s = ERROR_BUSY;
				}else{
					s = ERROR_SUCCESS;
				}
				timer_del = 20;						//reset 后延时防止点跑不出来,必须添加
				send_sta = IOS134_MOVE;
			}else{									//延时重新发送
				timer_del = 5;
			}
			break;
	}
	return s;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool ios_touch_init(void)
{
	if(multitouch_info.iphone_mouse_vertical){		//phone  竖屏鼠标模式
		if(multitouch_info.turn_xy){				//旋转屏幕
			ios_dir.x = -1;
			ios_dir.y = -1;
		}else{										//不旋转屏幕
			ios_dir.x = 1;
			ios_dir.y = 1;
		}
	}else{											//ipade  横屏幕鼠标模式
		ios_dir.x = 1;
		ios_dir.y = -1;
	}

	logd("ios_dir:%d %d\n",ios_dir.x,ios_dir.y);

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool ios_touch_deinit(void)
{
	return true;
}

#endif
