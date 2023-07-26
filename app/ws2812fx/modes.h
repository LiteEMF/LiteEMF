/*
  modes_esp.h - WS2812FX header file for ESP8266 and ESP32 microprocessors

  LICENSE

  The MIT License (MIT)

  Copyright (c) 2016  Harm Aldick

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.


  CHANGELOG

  2022-03-23   Separated from the original WS2812FX.h file
*/
#ifndef mode_h
#define mode_h

#include "WS2812FX.h"
#include "emf_typedef.h"

#define MODE_COUNT (sizeof(_modes)/sizeof(_modes[0]))
#define MODE_PTR(x) _modes[x].mode_ptr
#define MODE_NAME(x) _modes[x].name

// 显示效果枚举
typedef enum
{
    FX_MODE_STATIC=0,               //无闪烁
    FX_MODE_BLINK,                  //正常的闪烁
    FX_MODE_BREATH,                 //待机呼吸
    FX_MODE_COLOR_WIPE,             //一个接一个地点亮所有LED
    FX_MODE_COLOR_WIPE_INV,
    FX_MODE_COLOR_WIPE_REV,
    FX_MODE_COLOR_WIPE_REV_INV,
    FX_MODE_COLOR_WIPE_RANDOM,      //将所有LED一个接一个地变为随机颜色,然后用另一种颜色重新开始
    FX_MODE_RANDOM_COLOR,           //以一种随机颜色点亮所有LED,然后将它们切换到下一个随机颜色
    FX_MODE_SINGLE_DYNAMIC,         //以随机颜色点亮每个LED,将一个随机LED一个接一个地更改为另一个随机颜色
    FX_MODE_MULTI_DYNAMIC,          //以随机颜色点亮每个LED,同时将所有LED更改为新的随机颜色
    FX_MODE_RAINBOW,                //一次循环所有LED穿过彩虹
    FX_MODE_RAINBOW_CYCLE,          //在整个LED串上循环显示一条彩虹
    FX_MODE_SCAN,                   //来回运行像素块
    FX_MODE_DUAL_SCAN,              //在相反的方向上来回运行两个像素块
    FX_MODE_FADE,                   //使LED在两种颜色之间褪色
    FX_MODE_THEATER_CHASE,          //三色追逐模式
    FX_MODE_THEATER_CHASE_RAINBOW,  //剧场风格的爬行灯与彩虹效果
    FX_MODE_RUNNING_LIGHTS,         //运行灯光效果与平滑的正弦过渡
    FX_MODE_TWINKLE,                //使多个LED亮起、重置、重复
    FX_MODE_TWINKLE_RANDOM,         //使几个随机颜色的LED亮起、重置、重复
    FX_MODE_TWINKLE_FADE,           //亮起几个LED，然后熄灭
    FX_MODE_TWINKLE_FADE_RANDOM,    //使几个随机颜色的LED亮起，然后淡出
    FX_MODE_SPARKLE,                //一次闪烁一个LED
    FX_MODE_FLASH_SPARKLE,          //亮起该颜色的所有LED,随机闪烁白色像素
    FX_MODE_HYPER_SPARKLE,          //就像闪光灯一样,用更多的闪烁
    FX_MODE_STROBE,                 //经典的闪光效果
    FX_MODE_STROBE_RAINBOW,         //经典的闪光效果。骑着自行车穿过彩虹
    FX_MODE_MULTI_STROBE,           //闪光效果与不同的闪光灯计数和暂停，由速度控制
    FX_MODE_BLINK_RAINBOW,          //经典的眨眼效果。骑着自行车穿过彩虹
    FX_MODE_CHASE_WHITE,            //白色上运行的颜色
    FX_MODE_CHASE_COLOR,            //白色在奔跑_COLOR
    FX_MODE_CHASE_RANDOM,           //白色运行，然后是随机的颜色
    FX_MODE_CHASE_RAINBOW,          //白色在彩虹上奔跑
    FX_MODE_CHASE_FLASH,            //COLOR上出现白色闪烁
    FX_MODE_CHASE_FLASH_RANDOM,     //白色闪烁，然后是随机的颜色
    FX_MODE_CHASE_RAINBOW_WHITE,    //彩虹在白色上流淌
    FX_MODE_CHASE_BLACKOUT,         //黑色在奔跑_COLOR
    FX_MODE_CHASE_BLACKOUT_RAINBOW, //黑色在彩虹上奔跑
    FX_MODE_COLOR_SWEEP_RANDOM,     //从条带的开始和结束交替引入的随机颜色
    FX_MODE_RUNNING_COLOR,          //彩色/白色像素交替运行
    FX_MODE_RUNNING_RED_BLUE,       //红色/蓝色像素交替运行
    FX_MODE_RUNNING_RANDOM,         //随机运行的彩色像素
    FX_MODE_LARSON_SCANNER,         //K.I.T.T.
    FX_MODE_COMET,                  //从一端发射彗星
    FX_MODE_FIREWORKS,              //烟火火花四溅
    FX_MODE_FIREWORKS_RANDOM,       //随机的彩色烟花火花
    FX_MODE_MERRY_CHRISTMAS,        //红色/绿色像素交替运行
    FX_MODE_FIRE_FLICKER,           //随机闪烁
    FX_MODE_FIRE_FLICKER_SOFT,      //随机闪烁，强度较小
    FX_MODE_FIRE_FLICKER_INTENSE,   //随机闪烁，强度更大
    FX_MODE_CIRCUS_COMBUSTUS,       //交替运行的白/红/黑像素
    FX_MODE_HALLOWEEN,              //交替运行的橙色/紫色像素
    FX_MODE_BICOLOR_CHASE,          //双色追逐模式
    FX_MODE_TRICOLOR_CHASE,         //三色追逐函数
    FX_MODE_TWINKLEFOX,             //马克·克里格斯曼的FastLED TinkeFOX效应的改编
    FX_MODE_RAIN,                   //烟花效果和奔跑效果的组合，以创建看起来像雨的效果
    FX_MODE_CUSTOM_0,
    FX_MODE_CUSTOM_1,
    FX_MODE_CUSTOM_2,
    FX_MODE_CUSTOM_3,
    FX_MODE_CUSTOM_4,
    FX_MODE_CUSTOM_5,
    FX_MODE_CUSTOM_6,
    FX_MODE_CUSTOM_7,
}Enm_FX_Effect;

// 显示效果数组元素定义
typedef struct
{
    const char* name;
    const char* category;
    uint16_t (*mode_ptr)(void);
}Tst_mode;

extern const Tst_mode _modes[65];
extern uint16_t (*customModes[8])(void);
#endif
