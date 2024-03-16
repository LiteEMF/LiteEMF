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
#if APP_MT_ENABLE
#include "app/multitouch/mt_function.h"
#include "api/api_tick.h"
#include "key_typedef.h"

#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/
mt_map_t mt_map[]={	//TODO for test
	//mode			att		mac	d	pa			key			x		y
	{MT_SLIDE8,		0x01,	0,	5,	50,	U32(HW_KEY_RIGHT_POS,HW_KEY_LEFT_POS,HW_KEY_DOWN_POS,HW_KEY_UP_POS),2000,2000},
	//joystic dir
	{MT_SLIDE8,		0x01,	0,	5,	50,	U32(HW_KEY_M4_POS,HW_KEY_M3_POS,HW_KEY_M2_POS,HW_KEY_M1_POS),2000,2000},
	{MT_SLIDE8,		0x01,	0,	5,	50,	U32(HW_KEY_M8_POS,HW_KEY_M7_POS,HW_KEY_M6_POS,HW_KEY_M5_POS),2000,2000},
	
	{MT_SLIDE,		0x01,	0,	5,	(int8_t)-50<<8,	HW_KEY_A,		2000,	2000},
	{MT_KEY_AUTO,	0x01,	0,	10,	2,				HW_KEY_Y,		2000,	2000},
	{MT_KEY_AUTO,	0x00,	0,	10,	0,				HW_KEY_B,		2000,	2000},	//按住
	{MT_NONE,		0,		0,	0,	0,				HW_KEY_X,		1000,	1000},
};

mt_msg_t mt_event_msg[countof(mt_map)];
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
** Returns:	true:完成一次从按下到抬起的点击过程
** Description:	连点运行,函数调用频率必须住够大于连点频率, 
	否则采样频率不够会导致连点数据不均匀
*******************************************************************/
bool mt_auto_run(uint16_t id,mt_map_t* mapp)
{
	bool ret = false;
	uint8_t auto_index = 0;
	mt_map_t* mpp = &mapp[id];
	uint16_t d;

	d = mpp->d * MT_AUTO_DT;
	if(0 == d){
		moutitouch_fill_id(id, 1, mpp->x, mpp->y);
		ret = true;
	}else if((m_systick / d) % 2){					//看是奇偶数倍
		moutitouch_fill_id(id, 1, mpp->x, mpp->y);
	}else{
		if(moutitouch_get_point_by_id(id,NULL,NULL)){
			ret = true;
		}
		moutitouch_fill_id(id, 0, mpp->x, mpp->y);
	}
	return ret;
}

/*******************************************************************
** Parameters:	min_step,max_step 最大最小步长可以决定滑动方式
** Returns:		true:到达目标位置并且保持时间计时到. false：发送失败或者没有发送完成
** Description:	滑屏运行, 上层调用频率调整滑动速度
*******************************************************************/
bool mt_slide_run(uint16_t id, mt_map_t* mapp, uint16_t min_step, uint16_t max_step) 
{
	bool ret = false;
	int16_t x,y,dx,dy;
	uint16_t abs_d;
	mt_map_t* mpp = &mapp[id];

	if (!moutitouch_get_point_by_id(id,&x,&y)) {	//down
		moutitouch_fill_id(id,1,mpp->x,mpp->y);
	}else{											//move
		if(mpp->mode == MT_SLIDE8){				//slide8识别方向	
			mt_msg_t* pmsg = &mt_event_msg[id];
			abs_d = abs((int8_t)(mpp->pa & 0xff)*32);
			dx = abs_d * pmsg->slide8.dirx + mpp->x - x;
			dy = abs_d * pmsg->slide8.diry + mpp->y - y;
		}else{
			dx = ((int8_t)(mpp->pa & 0xff)*32 + mpp->x - x);		//获取相对坐标位置
			dy = ((int8_t)(mpp->pa >> 8)*32 + mpp->y - y);
			abs_d = MAX(abs(dx) , abs(dy));
		}

		if(0 == max_step) max_step = 1;
		if(abs_d > max_step){				//限定最大步长, 均匀发送
			dx = dx*(int32_t)max_step/(int32_t)abs_d;
			dy = dy*(int32_t)max_step/(int32_t)abs_d;
		}else if(abs_d > min_step){			//二分发送
			dx = dx/2;
			dy = dy/2;
		}else{								//小于最下步长一步到位
			dx = dx;
			dy = dy;
		}

		ret = !moutitouch_fill_id(id,1, x+dx, y+dy);
	}
	return ret;
}




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void mt_event_scan(uint32_t key)
{
	uint8_t id;
	uint32_t key_change = 0, key_change_down = 0, key_change_up = 0;
	static uint32_t s_key;
	
	key_change = s_key ^ key;
	key_change_down = key_change & key;
	key_change_up = key_change & s_key;

	for(id=0; id<countof(mt_map); id++){
		mt_msg_t* pmsg = &mt_event_msg[id];

		switch(mt_map[id].mode){
			case MT_NONE:
				if((key_change_down & mt_map[id].key) == mt_map[id].key){	
					moutitouch_fill_id(id,1,mt_map[id].x, mt_map[id].y);
					logd("id none down=%x ",mt_map[id].key);
				}
				if((key_change_up & mt_map[id].key) == mt_map[id].key){	
					moutitouch_fill_id(id,0,mt_map[id].x, mt_map[id].y);
				}
				break;
			case MT_KEY_TRI:
				if(0 == (mt_map[id].att & BIT(0))){			//0: 按下触发
					if((key_change_down & mt_map[id].key) == mt_map[id].key){	
						pmsg->mode.event = 1;
						pmsg->mode.pa = mt_map[id].d * MT_TRI_DT;
						moutitouch_fill_id(id, 1,mt_map[id].x, mt_map[id].y);
						logd("tri down=%x t=%d",mt_map[id].key, pmsg->mode.pa);
					}
				}else{										//1: 抬起触发
					if((key_change_up & mt_map[id].key) == mt_map[id].key){	
						pmsg->mode.event = 1;
						pmsg->mode.pa = mt_map[id].d * MT_TRI_DT;
						moutitouch_fill_id(id, 1,mt_map[id].x, mt_map[id].y);
					}
				}
				break;
			case MT_KEY_AUTO:
				if((key_change_down & mt_map[id].key) == mt_map[id].key){	
					if(mt_map[id].att & BIT(0)){			//1:按下连点, 再次按下停止
						if(pmsg->mode.event){
							pmsg->value = 0;
							moutitouch_fill_id(id, 0, 0,0);
						}else{
							pmsg->mode.event = 1;
							pmsg->mode.pa = mt_map[id].pa & 0XFF;//低8位连点次数
							logd("auto down=%x ",pmsg->mode.pa);
							
						}
					}else{
						pmsg->mode.event = 1;
						pmsg->mode.pa = mt_map[id].pa & 0XFF;//低8位连点次数
						logd("auto down=%x ",pmsg->mode.pa);
					}
				}
				if((key_change_up & mt_map[id].key) == mt_map[id].key){	
					if(0 == (mt_map[id].att & BIT(0))){		//0:按住连点, 抬起释放
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0,0);
						logd("auto up=%x ",mt_map[id].key);
					}
				}
				break;
			case MT_SLIDE:
				if((key_change_down & mt_map[id].key) == mt_map[id].key){	
					pmsg->mode.event = 1;
					pmsg->mode.pa = 0;
					logd("slide down=%x ",mt_map[id].key);
				}
				if(0 == (mt_map[id].att & BIT(0))){		//0: 按住滑动, 抬起释放
					if((key_change_up & mt_map[id].key) == mt_map[id].key){	
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0, 0);
					}
				}	// else 1:按下滑动,滑动完成后自动释放事件
				break;	
			case MT_SLIDE8:{		//注意按键格式不一样
				uint32_t up_key = BIT(mt_map[id].key & 0Xff);
				uint32_t down_key = BIT((mt_map[id].key >> 8) & 0Xff);
				uint32_t left_key = BIT((mt_map[id].key >> 16) & 0Xff);
				uint32_t right_key = BIT((mt_map[id].key >> 24) & 0Xff);

				if((key_change & (up_key | down_key | left_key | right_key))){ //识别方向
					pmsg->value = 0;
					if(key & up_key){
						pmsg->slide8.dirx -= 1;
					}
					if(key & down_key){
						pmsg->slide8.dirx += 1;
					}
					if(key & left_key){
						pmsg->slide8.diry -= 1;
					}
					if(key & right_key){
						pmsg->slide8.diry += 1;
					}
					if(pmsg->slide8.dirx || pmsg->slide8.diry){
						pmsg->slide8.event = 1;
						pmsg->slide8.step = 0;
					}else{
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0, 0);
					}
					logd("slide8 dir=%d %d\n",pmsg->slide8.dirx , pmsg->slide8.diry);
				}
				break;
			}	
		}
	}
	s_key = key;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool mt_function_init(void)
{
	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool mt_function_deinit(void)
{
	return true;
}


/*******************************************************************
** Parameters:	pa: 任务周期单位10us	
** Returns:	
** Description:		
*******************************************************************/
void mt_event_task(void* pa)
{
	uint8_t id;
	uint16_t dt_ms = (uint32_t)pa / 100;

	for(id=0; id<countof(mt_map); id++){
		mt_msg_t* pmsg = &mt_event_msg[id];

		switch(mt_map[id].mode){
			case MT_KEY_TRI:
				if(pmsg->mode.event){
					pmsg->mode.pa -= dt_ms;
					if(pmsg->mode.pa < dt_ms){
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0,0);
					}
				}
				break;
			case MT_KEY_AUTO:
				if(pmsg->mode.event){
					if(mt_auto_run(id,mt_map)){
						if(pmsg->mode.pa){
							if(0 == --pmsg->mode.pa){
								pmsg->value = 0;
							}
						}
					}
				}
				break;
			case MT_SLIDE:
				if(pmsg->mode.event){		//slide
					if(0 == pmsg->mode.pa){	//只计算一次
						uint16_t t = MAX(1, mt_map[id].d * MT_SLIDE_DT);
						uint16_t abs_d = 32 * MAX(abs((int8_t)(mt_map[id].pa & 0xff)) , abs((int8_t)(mt_map[id].pa >> 8)));
						uint16_t step = MAX(1, abs_d * dt_ms / (t-dt_ms));	//滑动距离 / ((滑动时间/任务时间) - 1)
						pmsg->mode.pa = step;
						logd("slide d=%d t=%d step=%d", abs_d,t,step);
					}

					if(mt_slide_run(id,mt_map, pmsg->mode.pa, pmsg->mode.pa)){
						pmsg->value = 0;		//滑到终点抬起
						moutitouch_fill_id(id, 0, 0,0);
					}
				}
				break;	
			case MT_SLIDE8:
				if(pmsg->slide8.event){		//slide
					if(0 == pmsg->slide8.step){	//只计算一次
						uint16_t t = MAX(1, mt_map[id].d * MT_SLIDE_DT);
						uint16_t abs_d = 32 * abs((int8_t)(mt_map[id].pa & 0xff));
						uint16_t step = MAX(1, abs_d * dt_ms / (t-dt_ms));	//滑动距离 / ((滑动时间/任务时间) - 1)
						pmsg->slide8.step = step;
						logd("slide8 d=%d t=%d step=%d", abs_d,t,step);
					}

					if(mt_slide_run(id,mt_map, pmsg->slide8.step, pmsg->slide8.step)){
						pmsg->value = 0;		//滑到终点抬起
						moutitouch_fill_id(id, 0, 0,0);
					}
				}
				break;	
		}
	}
}


#if TASK_HANDLER_ENABLE
/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void mt_function_handler(uint32_t period_10us)
{
	static timer_t s_timer;
	if((m_task_tick10us - s_timer) >= period_10us){
		s_timer = m_task_tick10us;
		mt_event_task(period_10us);
	}
}
#endif

#endif
