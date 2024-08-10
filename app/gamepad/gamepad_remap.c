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
#include "app/gamepad/gamepad_remap.h"
#include "api/api_tick.h"
#if APP_KM_ENABLE
#include  "app/app_km.h"
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

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

#if GAMEAPD_IMU_TO_TICK_ENABLE
static kalman_axis2f_t gyro_kalman;
static axis2f_t gyro_lpf;
static firf_axis2_t *gyro_fir;
static float gyro_fir_buf_xy[32*2];

/*******************************************************************
** Parameters:		
	kalman_smoot = 0.8
	fir_imp_size max 16
** Returns:	
** Description:		
*******************************************************************/
void gamepad_gyro_filter_init(float kalman_smoot, uint8_t fir_imp_size)
{
	gyro_lpf.x = 0; 
	gyro_lpf.y = 0;
	if(fir_imp_size > 32)fir_imp_size = 32;
	fir_axis2_fiter_init(&gyro_fir,NULL, gyro_fir_buf_xy,fir_imp_size);
	kalman_axis2f_filter_init(&gyro_kalman, 1-kalman_smoot, kalman_smoot);
}

void gamepad_gyro_filter(vector2f_t* vectorp, axis2i_t kconst, axis2f_t slopes, float exponent_f)
{
	vector2f_t vector;
	vector = *vectorp;

	if(vector.x > 0) vector.x *= 1.05;		//补偿摇杆正负方向
	if(vector.y > 0) vector.y *= 1.05;

	if(gyro_kalman.x.r || gyro_kalman.y.r){
		lpf_1st_axis2f(&gyro_lpf, &vector, LPF_1ST_FACTOR(0.008, 100));
		AXIS2_COPY(&vector, &gyro_lpf);
		kalman_axis2f_filter(&gyro_kalman, &vector);
		vector.x = gyro_kalman.x.out;
		vector.y = gyro_kalman.y.out;
	}
	fir_axis2l_fiter(&gyro_fir, &vector);

	vector2f_normalization(&vector);
	vector.r = pow(vector.r,exponent_f);
	vector.x *= kconst.x + slopes.x * vector.r;
	vector.y *= kconst.y + slopes.y * vector.r;
	*vectorp = vector;
}

float gamepad_mouse_to_stick(vector2f_t* vectorp, axis2i_t kconst, axis2f_t slopes, float exponent_f)
{
	vector2f_t vector;
	vector2f_normalization(&vector);

	vector.r = pow(vector.r,exponent_f);
	vector.x *= kconst.x + slopes.x * vector.r;
	vector.y *= kconst.y + slopes.y * vector.r;
	vector.r = 1;
	*vectorp = vector;
}

// VECTOR2_ADD
float gamepad_stick_to_mouse(vector2f_t* vectorp, axis2i_t kconst, axis2f_t slopes, float exponent_f)
{
	vector2f_t vector = *vectorp;

	vector.x =  (vector.x - kconst.x) / slopes.x;
	vector.y =  (vector.y - kconst.y) / slopes.y;
	vector2f_normalization(&vector);
	vector.r = pow(vector.r,1 / exponent_f);
}

/*******************************************************************
** Parameters:		
	accp: gsensor raw data
	gyrop: int16 , 需要通过gyro_range_t 转换成 rad/s(dps) 度每秒
** Returns:	
** Description:	获取机身角度		
*******************************************************************/
void gamepad_imu_get_body_angle(angle_t* anglep, axis3f_t* accp, axis3f_t* gyrop, gyro_range_t gyro_range, float dt)
{
	float lsb_dps;
	axis3f_t acc_vector;
	static axis3f_t gyro_lpf={0,0,0};
	//ACC 计算角度
	acc_vector.x = atan2f(0-accp->x, sqrt(accp->y * accp->y + accp->z * accp->z)) * RtA;
	acc_vector.y = atan2f(accp->y, sqrt(accp->x * accp->x + accp->z * accp->z)) * RtA;
	
	//gyro计算角度
	lsb_dps = 32768.0 / gyro_range;	//LSB/dps
	gyro_lpf.x += gyrop->x * dt / lsb_dps;	
	gyro_lpf.y += gyrop->y * dt / lsb_dps;
	gyro_lpf.z += gyrop->z * dt / lsb_dps;
	
	//互补滤波
	anglep->roll = gyro_lpf.x;
	anglep->pitch = gyro_lpf.y;
	anglep->yaw = gyro_lpf.z;
	lpf_1st(&anglep->roll , acc_vector.x, 0.02);
	lpf_1st(&anglep->pitch, acc_vector.y, 0.02);

	anglep->roll = CONSTRAIN(anglep->roll, -180, 180);
	anglep->pitch = CONSTRAIN(anglep->pitch, -90, 90);
	anglep->yaw = CONSTRAIN(anglep->yaw, -180, 180);
}


/*******************************************************************
** Parameters:	
		tick_cfgp: 摇杆死区配置
		anglep: 输入的角度数据
		tick_vectorp: 输出摇杆向量, tick_vectorp->r 范围 -32768~32767
** Returns:	
** Description:	convert pitch and roll angle to axies	
	运算完成后可以通过 app_vector_to_stick(tickp, tick_vectorp); 转换成摇杆数据
*******************************************************************/
void gamepad_angle_to_stick(joystick_cfg_t *tick_cfgp, angle_t* anglep, vector2f_t* tick_vectorp)
{
	vector2f_t vector;
	vector.x = anglep->pitch;
	vector.y = CONSTRAIN(anglep->roll, -90, 90);		//roll 范围限制到-90 ~90度

	vector2f_normalization(&vector);
	vector.r *= 32768 / 90;
	app_stick_deadband(tick_cfgp, &vector);
	*tick_vectorp = vector;
}
#endif

#if GAMEAPD_MACRO_ENABLE
gamepad_macro_sta_t m_macro_sta;
gp_macro_ctb_t  m_macro_ctb;

/*******************************************************************
** Parameters:	headp: 输入宏定义录制属性
				max_step: 输入最大录制步长
				macrop: 输出宏控制数据
** Returns:		ret
** Description:	开启宏录制	
*******************************************************************/
bool gamepade_macro_start_record(gp_macro_ctb_t* macrop, gamepad_macro_map_t *headp, uint16_t max_step)
{
    bool ret = false;
    api_storage_map_t* storage_mapp = NULL;

    if(GP_MACRO_STA_NONE == m_macro_sta){
        uint16_t len = STORAGE_MAP_HEAD_LEN + (max_step + 1) * sizeof(gamepad_macro_map_t);
        storage_mapp = (api_storage_map_t*)emf_malloc(len);
        if(NULL != storage_mapp){
            gamepad_macro_map_t* macro_mapp = (gamepad_macro_map_t*)storage_mapp->map;
            
            memset(macrop, 0, sizeof(gp_macro_ctb_t));
            memset(storage_mapp, 0, len);
            storage_mapp->max_len = len - STORAGE_MAP_HEAD_LEN;	//设置map最大长度
            storage_mapp->map_len = sizeof(gamepad_macro_map_t);
            *macro_mapp = *headp;
            macrop->mapp = storage_mapp;
            m_macro_sta = GP_MACRO_STA_REC;
            ret = true;
        }	
    }
	return ret;
}

bool gamepade_macro_end_record(gp_macro_ctb_t* macrop, api_storage_map_t* out_mapp)
{
	bool ret = false;
    api_storage_map_t* storage_mapp;
    gamepad_macro_map_t* macro_mapp;

    if(NULL == storage_mapp) return false;
    storage_mapp = (api_storage_map_t*)macrop->mapp;
    macro_mapp = (gamepad_macro_map_t*)storage_mapp->map;

	m_macro_sta = GP_MACRO_STA_NONE;

	if(macrop->step <= 1){              //没有录制到按键
		return false;
	}else{                              //保存最后一次动作运行时间
        uint16_t timer;
        timer = ((m_systick - macrop->timer) / GP_MACRG_DT);
        macro_mapp[macrop->step-1].gp.time = timer;
        logd("... step[%d] time=%d\n",macrop->step-1,timer);
    }

    //store
	if(out_mapp->max_len >= storage_mapp->max_len){
		memcpy(out_mapp->map, storage_mapp->map, storage_mapp->map_len);
		api_storage_set_crc(out_mapp, storage_mapp->map_len);
		loge("macro end ok step=%d\n", macrop->step);
		ret = true;
	}else{
		loge("macro end err storage map over!");
	}
	emf_free(storage_mapp);				//must free
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	宏录制
    gamepade_macro_start_record 会对macrop 进行初始化
*******************************************************************/
void gamepade_macro_record_task(gp_macro_ctb_t* macrop, const app_gamepad_key_t* keyp)
{
    uint8_t stick_key, s_stick_key;          //摇杆方向
    axis2i_t stick;
	bool key_change=false;
    uint16_t timer;
    api_storage_map_t* storage_mapp;
    gamepad_macro_map_t* macro_mapp;

    if(NULL == storage_mapp) return;
    storage_mapp = (api_storage_map_t*)macrop->mapp;
    macro_mapp = (gamepad_macro_map_t*)storage_mapp->map;

    stick_key = get_stick_dir(&keyp->stick_l, STICK_DIR_THRESHOLD);
    stick_key |= get_stick_dir(&keyp->stick_r, STICK_DIR_THRESHOLD)<<4;

    if(0 == macrop->step) {                                      //第一帧数据
        if(stick_key || keyp->key){
            key_change = true;
            macrop->step = 1;
            macrop->timer = m_systick;
        }
    }else{                                                      
        if((macro_mapp[macrop->step-1].gp.key != keyp->key)){     //按键改变
            key_change = true;
        }else if(m_systick - macrop->timer >= 50){              //摇杆改变, 防止摇杆录制斜方向，会出现两帧问题
            axis2i_t stickl, stickr;
            stickl.x = macro_mapp[macrop->step-1].gp.lx << 8;
            stickl.y = macro_mapp[macrop->step-1].gp.ly << 8;
            stickr.x = macro_mapp[macrop->step-1].gp.rx << 8;
            stickr.y = macro_mapp[macrop->step-1].gp.ry << 8;
            s_stick_key = get_stick_dir(&stickl, STICK_DIR_THRESHOLD);
            s_stick_key |= get_stick_dir(&stickr, STICK_DIR_THRESHOLD)<<4;
            if(stick_key != s_stick_key){
                key_change = true;
            }
        }
    }

    if(key_change){                                             //有动作改变
        if(storage_mapp->map_len >= storage_mapp->max_len){             
            gamepad_macro_record_stop(macrop);                  //录制满强制停止?
        }else{
            if(macrop->step > 1){                               //保存上一次的时间
                timer = ((m_systick - macrop->timer) / GP_MACRG_DT);
                macro_mapp[macrop->step-1].gp.time = timer;     	//mapp从1开始, mapp[0]是head
                logd("... step[%d] time=%d\n",macrop->step-1,timer);
            }
            macro_mapp[macrop->step].gp.type = GAMEPAD_REMAP_GP_TYEP;
            macro_mapp[macrop->step].gp.key = keyp->key;
            get_stick_val(stick_key, &stick);
            macro_mapp[macrop->step].gp.lx = stick.x >> 8; 
            macro_mapp[macrop->step].gp.ly = stick.y >> 8; 
            get_stick_val(stick_key>>4, &stick);
            macro_mapp[macrop->step].gp.rx = stick.x >> 8; 
            macro_mapp[macrop->step].gp.ry = stick.y >> 8; 

            storage_mapp->map_len += sizeof(gamepad_macro_map_t);

            logd("step[%d] k=%x, stick_key:%x\n", macrop->step, macro_mapp[macrop->step].gp.key, stick_key);
            macrop->timer = m_systick;
            macrop->step++;
        }
    }
}

/*******************************************************************
** Parameters:	macrop: 宏运行控制单块	
** Returns:	
** Description:	手柄宏运行任务	
				必须初始化好 macrop
*******************************************************************/
bool gamepade_macro_run_task(gp_macro_ctb_t* macrop, gamepad_remap_key_t *rekeyp)
{
    bool ret = false;
	uint32_t time;
	uint16_t step_num;
	api_storage_map_t* storage_mapp;
    gamepad_macro_map_t* macro_mapp;

    if(NULL == storage_mapp) return true;				//立即结束
    storage_mapp = (api_storage_map_t*)macrop->mapp;
    macro_mapp = (gamepad_macro_map_t*)storage_mapp->map;

	step_num = storage_mapp->map_len / sizeof(gamepad_macro_map_t);
	if(step_num < 2) return true;						//除去head最小值为2
	step_num--;

	memset(rekeyp, 0, sizeof(gamepad_remap_key_t));
	if(macrop->step <= step_num){
		if(GAMEPAD_REMAP_GP_TYEP == macro_mapp[macrop->step].gp.type){
            rekeyp->gp.key |= macro_mapp[macrop->step].gp.key; 
            rekeyp->gp.stick_r.x = remap(macro_mapp[macrop->step].gp.rx, INT_LEAST8_MIN, INT_LEAST8_MAX, INT16_MIN, INT16_MAX);
            rekeyp->gp.stick_r.y = remap(macro_mapp[macrop->step].gp.ry, INT_LEAST8_MIN, INT_LEAST8_MAX, INT16_MIN, INT16_MAX);
            rekeyp->gp.stick_l.x = remap(macro_mapp[macrop->step].gp.lx, INT_LEAST8_MIN, INT_LEAST8_MAX, INT16_MIN, INT16_MAX);
            rekeyp->gp.stick_l.y = remap(macro_mapp[macrop->step].gp.ly, INT_LEAST8_MIN, INT_LEAST8_MAX, INT16_MIN, INT16_MAX);
		}else if(GAMEPAD_REMAP_KB_TYEP == macro_mapp[macrop->step].gp.type){
            rekeyp->kb = macro_mapp[macrop->step].kb.kb;
        }

		//切换到下一个动作
		time = macro_mapp[macrop->step].gp.time * GP_MACRG_DT;
		if(m_systick - macrop->timer >= time){
			macrop->timer = m_systick;
			logd("macro step[%d] \t %d \t %d \t %x \n", macrop->step, 0, time, rekeyp->gp.key);
			macrop->step++;
		}
	}

	// 0 		0     	//普通按住运行, 抬起结束
    // 1 		0 		//按下触发运行
    // 0 		1		//按下循环运行, 抬起结束运行
    // 1 		1		//点击开始循环, 点击结束循环
	if(macrop->step > step_num){						//运行完全部动作
        if(macro_mapp->head.att & BIT(1)){                       //循环运行    
            if(m_systick - macrop->macro_timer >= macro_mapp->head.time){
                macrop->step = 1;
                macrop->timer = m_systick;
                macrop->macro_timer = m_systick; 
            }
        }else{											//直接退出
            ret = true;
        }
	}
    
    // if(0 == (m_macro_att & BIT(0))){     				//按住运行,如果抬起立即结束
    //     if(0 == (macro_mapp->head.key & key_pressed)){ 
    //         ret = true;
    //     }
    // }
	//触发由外界触发执行和结束?//TODO

    if(ret){
        logd("macro[%d] run end\n",macrop->cfg_index);
    }
	return ret;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:	宏定义任务
*******************************************************************/
void gamepad_macro_task(app_gamepad_key_t* keyp, gamepad_remap_key_t *rekeyp)
{ 
	switch (m_macro_sta){
	case GP_MACRO_STA_RUN:
        if(gamepade_macro_run_task(&m_macro_ctb, rekeyp)){
            m_macro_sta = GP_MACRO_STA_NONE;
        }
		break;
	case GP_MACRO_STA_REC:
		gamepade_macro_record_task(&m_macro_ctb, keyp);
		break;
	default:
		break;
	}

}
#endif



/*******************************************************************
** Parameters:	key_in: 按键输入
				rekeyp: 映射输出键值
				mapp:map表指针,固定map长度为32	
** Returns:	
** Description:	手柄按键转换, 可以映射为手柄按键,键盘和多媒体按键
*******************************************************************/
void gamepad_remap_key(uint32_t key_in, gamepad_remap_map_t* mapp, gamepad_remap_key_t *rekeyp)
{
    uint8_t pos,i,kb_index=0;;

	memset(rekeyp, 0, sizeof(gamepad_remap_key_t));
    for(pos=0; pos<32; pos++){
        if(key_in & BIT(pos)){
			switch(mapp[pos].gp.type){
				case GAMEPAD_REMAP_GP_TYEP:
					for(i=0; i<3; i++){
						if(mapp[pos].gp.key_pos[i] < 32){
							rekeyp->gp.key |= BIT(mapp[pos].gp.key_pos[i]);
						}
					}
					break;
				#if APP_KM_ENABLE
				case GAMEPAD_REMAP_KB_TYEP:
					app_fill_kb(mapp[pos].kb.fn, &rekeyp->kb, 0);
					if(app_fill_kb(mapp[pos].kb.key[0], &rekeyp->kb, kb_index)){
						kb_index++;
					}
					if(app_fill_kb(mapp[pos].kb.key[1], &rekeyp->kb, kb_index)){
						kb_index++;
					}
					break;
				case GAMEPAD_REMAP_CONSUMER_TYEP:
					rekeyp->consumer_key = mapp[pos].consumer.key;
					break;
				#endif
				default:
					rekeyp->gp.key |= BIT(pos);
					break;
			}
        }
    }
}



/*******************************************************************
** Parameters:	pkey:输入要改的按键,输出按键值	
** Returns:	
** Description:	auto turbo功能执行,在按键发送地方调用
	在工程中定义auto 和 turbo控制变量
	gamepad_auto_t m_auto_key;
	gamepad_auto_t m_turbo_key;
*******************************************************************/
void gamepad_auto_key(gamepad_auto_t *pauto, uint32_t *pkey)
{
	if(pauto->en){
		if(pauto->key){
			if(m_systick - pauto->timer >= pauto->period){
				pauto->timer = m_systick;
				pauto->dir = !pauto->dir;
			}
			if(pauto->dir){
				*pkey |= pauto->key;
			}else{
				*pkey &= ~pauto->key;
			}
		}else{							//为了下次触发能立即按下
			pauto->dir = false;
		}
	}
}


/*******************************************************************
** Parameters:	pcurve_shape:0~100	, 
                shape_cnt: 折线点个数, shape_cnt-1必须能被10整除推荐:3, 6, 11
** Returns:	    
** Description:	Curve shaping 多点折线比例转换
@ref: https://github.dev/FreeJoy-Team/FreeJoy ShapeFunc	
*******************************************************************/
int32_t curve_shape_remap(int32_t value, int32_t max_val, uint8_t* pcurve_shape, uint8_t shape_cnt)
{
	int32_t out_min, out_max, step;
	int32_t in_min, in_max;
	int32_t min_index;
	int32_t ret;

    int8_t i, shape_step = 100 / (shape_cnt-1);

    // check if is not linear shape_cnt-1必须能被10整除
	for(i=0; i<shape_cnt; i++){
        if(pcurve_shape[i] != i*shape_step){
            break;
        }
    }
    if(i == shape_cnt) return value;            // is line

    step = max_val / (shape_cnt-1);
    min_index = value / step;

    if (min_index >= shape_cnt-1){
        return ((int32_t)pcurve_shape[shape_cnt-1] * (int32_t)max_val/100);
    }else{
        in_min = min_index*step;
        in_max = (min_index+1)*step;
        
        out_min = ((int32_t)pcurve_shape[min_index] * (int32_t)max_val/100);
        out_max = ((int32_t)pcurve_shape[min_index+1] * (int32_t)max_val/100);
        
        ret = REMAP(value, in_min, in_max, out_min, out_max);
	    return(ret);
    }
}

#endif
