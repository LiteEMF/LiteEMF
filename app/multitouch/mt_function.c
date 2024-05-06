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
#include "utils/emf_utils.h"
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



/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
//宏定义运行msg
typedef struct {
	uint16_t id;
	mt_msg_t msg;
	mt_map_t* map;
	mt_map_t* map_end;
	uint16_t macro_step_num;
	list_head_t list;
}macro_msg_t;
static list_head_t macro_msg_head = LIST_HEAD_INIT(macro_msg_head);

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
bool mt_function_run(uint16_t id, uint16_t dt_ms, mt_msg_t* pmsg, mt_map_t* mapp);

/*****************************************************************************************************
**  Function
******************************************************************************************************/
api_storage_map_t* macro_map  = NULL;
static mt_map_t* mt_mapp = NULL;
uint16_t mt_map_count = 0;

static mt_msg_t* mt_msgp = NULL;

static macro_msg_t* get_macro_msg(uint16_t id)
{
	macro_msg_t* macro_pmsg = NULL;
	macro_msg_t* pos;

	list_for_each_entry_type(pos, &macro_msg_head, macro_msg_t, list){
		if(pos->id == id){
			macro_pmsg = pos;
			break;
		}
	}
	return macro_pmsg;
}



/*******************************************************************
** Parameters:	id: 	触发按键id
				dt_ms:	运行触发任务的间隔时间
				pmsg: 	消息事件
				mapp: 	事件map列表
** Returns:	true:完成
** Description:	运行按键触发
*******************************************************************/
bool mt_tri_run(uint16_t id, uint16_t dt_ms, mt_msg_t* pmsg, mt_map_t* mapp)
{
	bool ret = false;

	if(pmsg->tri.time_out >= dt_ms){
		pmsg->tri.time_out -= dt_ms;
		moutitouch_fill_id(id, 1,mapp->x, mapp->y);
	}else{
		pmsg->value = 0;
		moutitouch_fill_id(id, 0, 0,0);
		ret = true;
	}
	return ret;
}

/*******************************************************************
** Parameters:	
** Returns:	true:完成一次从按下到抬起的点击过程
** Description:	连点运行,函数调用频率必须住够大于连点频率, 
	否则采样频率不够会导致连点数据不均匀
*******************************************************************/
bool mt_auto_run(uint16_t id, mt_msg_t* pmsg, mt_map_t* mapp)
{
	bool ret = false;
	uint8_t auto_index = 0;
	uint16_t d = mapp->r * MT_AUTO_DT;

	if(0 == d){
		moutitouch_fill_id(id, 1, mapp->x, mapp->y);
		ret = true;
	}else if((m_systick / d) % 2){					//看是奇偶数倍
		moutitouch_fill_id(id, 1, mapp->x, mapp->y);
	}else{											//抬起
		if(moutitouch_get_point_by_id(id,NULL,NULL)){	//完成有一次从按下到抬起的动作
			ret = true;
			if(pmsg->autos.times){
				if(0 == --pmsg->autos.times){
					pmsg->value = 0;
				}
			}
		}
		moutitouch_fill_id(id, 0, mapp->x, mapp->y);
	}

	return ret;
}

/*******************************************************************
** Parameters:	min_step,max_step 最大最小步长可以决定滑动方式
** Returns:		true:到达目标位置并且保持时间计时到. false：发送失败或者没有发送完成
** Description:	滑屏运行, 上层调用频率调整滑动速度
*******************************************************************/
bool mt_slide_run(uint16_t id, uint16_t min_step, uint16_t max_step, mt_msg_t* pmsg, mt_map_t* mapp) 
{
	bool ret = false;
	int16_t x,y,dx,dy;
	uint16_t abs_d;

	if (!moutitouch_get_point_by_id(id,&x,&y)) {	//down
		moutitouch_fill_id(id,1,mapp->x,mapp->y);
	}else{											//move
		if(mapp->mode == MT_SLIDE8){					//slide8识别方向	
			abs_d = abs((int8_t)(mapp->r) * (TOUCH_X_LOGICAL_MAX/128));		//r 0~127
			dx = abs_d * pmsg->slide8.dirx + mapp->x - x;
			dy = abs_d * pmsg->slide8.diry + mapp->y - y;				
		}else{
			dx = ((int8_t)(mapp->t & 0xff)*(TOUCH_X_LOGICAL_MAX/128) + mapp->x - x);		//获取相对坐标位置
			dy = ((int8_t)(mapp->t >> 8)*(TOUCH_X_LOGICAL_MAX/128) + mapp->y - y);
		}
		abs_d = MAX(abs(dx) , abs(dy));
		if(0 == max_step) max_step = 1;
		// logd("slide:%d %d, %d %d %d",dx,dy,abs_d,min_step,max_step);
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
** Returns:	true:运行宏定义完成
** Description:	宏定义运行
*******************************************************************/
bool mt_macro_run(uint16_t id, uint16_t dt_ms, mt_msg_t* pmsg, mt_map_t* mapp)
{
	bool ret = false;
	macro_msg_t* macro_pmsg = get_macro_msg(id);
	uint16_t macro_map_id;

	if(NULL == macro_pmsg){		//第一次运行动态分配空间
		macro_pmsg = emf_malloc(sizeof(macro_msg_t));

		if(NULL != macro_pmsg){
			uint16_t macro_id = mapp->t;
			uint16_t macro_step;
			macro_pmsg->id = id;
			macro_pmsg->map = get_macro_map(macro_id, 0, macro_map);
			if(NULL != macro_pmsg->map){
				macro_step = macro_pmsg->map->x / sizeof(mt_map_t);
				macro_pmsg->map_end = macro_pmsg->map + macro_step;
				list_add(&macro_pmsg->list,&macro_msg_head);
				logd("malloc macro%d step=%d\n",id, macro_step);
			}else{
				emf_free(macro_pmsg);
				logd("get macro map err\n",id);
				return true;					//出错, 宏定义运行完成
			}
		}else{
			logd("malloc macro msg err\n");
			return true;					//出错, 宏定义运行完成
		}
	}

	if(!pmsg->macro.step_run){				//当前步骤运行完,或者第一次进入
		if(pmsg->macro.time_out >= dt_ms){
			pmsg->macro.time_out -= dt_ms;
		}else{
			
			pmsg->macro.step_run = true;

			//更新下一步宏定义步骤map,注意跳过MT_MACRO_INOF从1开始
			macro_pmsg->map++;
			if((macro_pmsg->map < macro_pmsg->map_end) && (macro_pmsg->map->mode != MT_MACRO_INOF)){
				macro_pmsg->msg.value = 0;
				macro_pmsg->msg.macro.event = 1;
				macro_pmsg->msg.macro.run = 1;
			}else{
				list_del(&macro_pmsg->list);
				emf_free(macro_pmsg);
				moutitouch_fill_id(id, 0, 0,0);
				logd("macro%d run end", id);
				return true;		//宏运行结束
			}
		}
	}

	if(pmsg->macro.step_run){
		if(!mt_function_run(id, dt_ms, &macro_pmsg->msg, macro_pmsg->map)){
			pmsg->macro.step_run = false;
			if(NULL != macro_pmsg->map) pmsg->macro.time_out = macro_pmsg->map->k & 0XFFFF;		//获取动作结束空闲时间
		}
	}
	
	return ret;
}

/*******************************************************************
** Parameters:	功能按键运行	
** Returns:	是否运行完
** Description:		
*******************************************************************/
bool mt_function_run(uint16_t id, uint16_t dt_ms, mt_msg_t* pmsg, mt_map_t* mapp)
{
	if(NULL == mapp) return false;

	switch(mapp->mode){
		case MT_KEY_TRI:
			if(pmsg->tri.event){
				pmsg->tri.event = false;
				pmsg->tri.time_out = mapp->t;
			}
			if(pmsg->tri.run){
				mt_tri_run(id, dt_ms, pmsg, mapp);
			}
			break;
		case MT_KEY_AUTO:
			if(pmsg->autos.event){
				pmsg->autos.event = false;
				pmsg->autos.times = mapp->t; //连点次数
			}
			if(pmsg->autos.run){
				mt_auto_run(id, pmsg, mapp);
			}
			break;
		case MT_SLIDE:
			if(pmsg->slide.event){
				uint16_t t = MAX(1, mapp->r * MT_SLIDE_DT);
				uint16_t abs_d = (TOUCH_X_LOGICAL_MAX/128) * MAX(abs((int8_t)(mapp->t & 0xff)) , abs((int8_t)(mapp->t >> 8)));
				uint16_t step = MAX(1, abs_d * dt_ms / (t-dt_ms));	//滑动距离 / ((滑动时间/任务时间) - 1)
				pmsg->slide.step = step;
				pmsg->slide.event = false;
				logd("slide d=%d t=%d step=%d", abs_d,t,step);
			}
			if(pmsg->slide.run){		//slide
				if(mt_slide_run(id, pmsg->slide.step, pmsg->slide.step, pmsg, mapp)){
					pmsg->value = 0;		//滑到终点抬起
					moutitouch_fill_id(id, 0, 0,0);
				}
			}
			break;	
		case MT_SLIDE8:
			if(pmsg->slide.event){
				uint16_t t = MAX(1, mapp->t);
				uint16_t abs_d = (TOUCH_X_LOGICAL_MAX/128) * abs((int8_t)mapp->r);
				uint16_t step = MAX(1, abs_d * dt_ms / (t-dt_ms));	//滑动距离 / ((滑动时间/任务时间) - 1)
				pmsg->slide8.step = step;
				pmsg->slide.event = false;
				logd("slide8 r=%d d=%d dt=%dms t=%d step=%d", mapp->r, abs_d, dt_ms,t,step);
			}
			if(pmsg->slide8.run){		//slide
				if(mt_slide_run(id, pmsg->slide8.step, pmsg->slide8.step, pmsg, mapp)){
					pmsg->value = 0;		//滑到终点抬起
					moutitouch_fill_id(id, 0, 0,0);
				}
			}
			break;
		case MT_MACRO:
			if(pmsg->macro.event){
				pmsg->macro.event = false;
			}
			if(pmsg->macro.run){			//宏运行
				if(mt_macro_run(id, dt_ms, pmsg, mapp)){
					pmsg->value = 0;		//宏运行结束
					moutitouch_fill_id(id, 0, 0, 0);
				}
			}
			break;	
		default:
			break;
	}
	return pmsg->tri.run;
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

	if(NULL == mt_mapp) return;
	
	key_change = s_key ^ key;
	key_change_down = key_change & key;
	key_change_up = key_change & s_key;

	for(id=0; id<mt_map_count; id++){
		mt_msg_t* pmsg = &mt_msgp[id];

		switch(mt_mapp[id].mode){
			case MT_NONE:
				if((key_change_down & mt_mapp[id].k) == mt_mapp[id].k){	
					moutitouch_fill_id(id,1,mt_mapp[id].x, mt_mapp[id].y);
					logd("id none down=%x ",mt_mapp[id].k);
				}
				if((key_change_up & mt_mapp[id].k) == mt_mapp[id].k){	
					moutitouch_fill_id(id,0,mt_mapp[id].x, mt_mapp[id].y);
				}
				break;
			case MT_KEY_TRI:
				if(0 == (mt_mapp[id].att & BIT(0))){			//0: 按下触发
					if((key_change_down & mt_mapp[id].k) == mt_mapp[id].k){	
						pmsg->tri.event = 1;
						pmsg->tri.run = 1;
						logd("tri down=%x",mt_mapp[id].k);
					}
				}else{										//1: 抬起触发
					if((key_change_up & mt_mapp[id].k) == mt_mapp[id].k){	
						pmsg->tri.event = 1;
						pmsg->tri.run = 1;
					}
				}
				break;
			case MT_KEY_AUTO:
				if((key_change_down & mt_mapp[id].k) == mt_mapp[id].k){	
					if(mt_mapp[id].att & BIT(0)){			//1:按下连点, 再次按下停止
						if(pmsg->autos.event){
							pmsg->value = 0;
							moutitouch_fill_id(id, 0, 0,0);
						}else{
							pmsg->autos.event = 1;
							pmsg->autos.run = 1;
						}
					}else{
						pmsg->autos.event = 1;
						pmsg->autos.run = 1;
					}
				}
				if((key_change_up & mt_mapp[id].k) == mt_mapp[id].k){	
					if(0 == (mt_mapp[id].att & BIT(0))){		//0:按住连点, 抬起释放
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0,0);
						logd("auto up=%x ",mt_mapp[id].k);
					}
				}
				break;
			case MT_SLIDE:
				if((key_change_down & mt_mapp[id].k) == mt_mapp[id].k){	
					pmsg->slide.event = 1;
					pmsg->slide.run = 1;
					logd("slide down=%x ",mt_mapp[id].k);
				}
				if(0 == (mt_mapp[id].att & BIT(0))){		//0: 按住滑动, 抬起释放
					if((key_change_up & mt_mapp[id].k) == mt_mapp[id].k){	
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0, 0);
					}
				}	// else 1:按下滑动,滑动完成后自动释放事件
				break;	
			case MT_SLIDE8:{		//注意按键格式不一样
				uint32_t up_key = BIT(mt_mapp[id].k & 0Xff);
				uint32_t down_key = BIT((mt_mapp[id].k >> 8) & 0Xff);
				uint32_t left_key = BIT((mt_mapp[id].k >> 16) & 0Xff);
				uint32_t right_key = BIT((mt_mapp[id].k >> 24) & 0Xff);

				if((key_change & (up_key | down_key | left_key | right_key))){ //识别方向
					pmsg->value = 0;
					if(key & up_key){
						pmsg->slide8.diry -= 1;
					}
					if(key & down_key){
						pmsg->slide8.diry += 1;
					}
					if(key & left_key){
						pmsg->slide8.dirx -= 1;
					}
					if(key & right_key){
						pmsg->slide8.dirx += 1;
					}
					if(pmsg->slide8.dirx || pmsg->slide8.diry){
						pmsg->slide8.event = 1;
						pmsg->slide8.run = 1;
					}else{
						pmsg->value = 0;
						moutitouch_fill_id(id, 0, 0, 0);
					}
					logd("slide8 dir=%d %d\n",(int8_t)pmsg->slide8.dirx, (int8_t)pmsg->slide8.diry);
				}
				break;
			}	
			case MT_MACRO:{		//注意这是宏定义触发按键
				if(0 == (mt_mapp[id].att & BIT(0))){			//0: 按下触发
					if((key_change_down & mt_mapp[id].k) == mt_mapp[id].k){	
						pmsg->value = 0;
						pmsg->macro.event = 1;
						pmsg->macro.run = 1;
						logd("macro down=%x\n",mt_mapp[id].k);
					}
				}else{										//1: 抬起触发
					if((key_change_up & mt_mapp[id].k) == mt_mapp[id].k){	
						pmsg->value = 0;
						pmsg->macro.event = 1;
						pmsg->macro.run = 1;
					}
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
bool mt_function_init(const api_storage_map_t* mapp, const api_storage_map_t* macro_mapp,mt_msg_t* msgp)
{
	mt_mapp = (mt_map_t*)mapp->map;
	mt_map_count = mapp->len/sizeof(mt_map_t);
	macro_map = macro_mapp;

	mt_msgp = msgp;
	memset(mt_msgp, 0, mt_map_count);
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
void mt_function_task(void* pa)
{
	uint8_t id;
	uint16_t dt_ms;

	if(NULL == mt_mapp) return;

	dt_ms = (uint32_t)pa / 100;
	for(id=0; id<mt_map_count; id++){
		mt_msg_t* pmsg = &mt_msgp[id];
		mt_function_run(id, dt_ms, pmsg, &mt_mapp[id]);
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
		mt_function_task(period_10us);
	}
}
#endif

#endif
