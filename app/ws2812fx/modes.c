/*
  modes.cpp - WS2812FX animation modes/effects

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

  2022-03-23   Separated from the original WS2812FX.cpp file
*/
#include  "hw_config.h"
#if APP_WS2812FX_ENABLE
#include "WS2812FX.h"

/* #####################################################
#
#  Mode Functions
#
##################################################### */

/*
 * 无闪烁。只是普通的老式静电灯。
 */
uint16_t WS2812FX_mode_static(void)
{
    adafruit_neopixel.fill(ws2812fx._seg->colors[0], ws2812fx._seg->start, ws2812fx._seg_len);
    SET_CYCLE;
    return ws2812fx._seg->speed;
}

/*
 * 正常的闪烁。50%开/关时间。
 */
uint16_t WS2812FX_mode_blink(void)
{
    return ws2812fx_method.blink(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], false);
}

/*
 * 经典的眨眼效果。骑着自行车穿过彩虹。
 */
uint16_t WS2812FX_mode_blink_rainbow(void)
{
    return ws2812fx_method.blink(ws2812fx.color_wheel(ws2812fx._seg_rt->counter_mode_call & 0xFF), ws2812fx._seg->colors[1], false);
}

/*
 * 经典的闪光效果。
 */
uint16_t WS2812FX_mode_strobe(void)
{
    return ws2812fx_method.blink(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], true);
}

/*
 * 经典的闪光效果。骑着自行车穿过彩虹。
 */
uint16_t WS2812FX_mode_strobe_rainbow(void)
{
    return ws2812fx_method.blink(ws2812fx.color_wheel(ws2812fx._seg_rt->counter_mode_call & 0xFF), ws2812fx._seg->colors[1], true);
}

/*
 * 一个接一个地点亮所有LED.
 */
uint16_t WS2812FX_mode_color_wipe(void)
{
    return ws2812fx_method.color_wipe(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], false);
}

uint16_t WS2812FX_mode_color_wipe_inv(void)
{
    return ws2812fx_method.color_wipe(ws2812fx._seg->colors[1], ws2812fx._seg->colors[0], false);
}

uint16_t WS2812FX_mode_color_wipe_rev(void)
{
    return ws2812fx_method.color_wipe(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], true);
}

uint16_t WS2812FX_mode_color_wipe_rev_inv(void)
{
    return ws2812fx_method.color_wipe(ws2812fx._seg->colors[1], ws2812fx._seg->colors[0], true);
}

/*
 * 将所有LED一个接一个地变为随机颜色。
 * 然后用另一种颜色重新开始。
 */
uint16_t WS2812FX_mode_color_wipe_random(void)
{
    if(ws2812fx._seg_rt->counter_mode_step % ws2812fx._seg_len == 0)// AUX_PARAM将存储我们的随机色轮索引
    {
        ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);
    }
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param);
    return ws2812fx_method.color_wipe(color, color, false) * 2;
}

/*
 * 从条带的开始和结束交替引入的随机颜色。
 */
uint16_t WS2812FX_mode_color_sweep_random(void)
{
    if(ws2812fx._seg_rt->counter_mode_step % ws2812fx._seg_len == 0)// AUX_PARAM将存储我们的随机色轮索引
    {
        ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);
    }
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param);
    return ws2812fx_method.color_wipe(color, color, true) * 2;
}

/*
 * 以一种随机颜色点亮所有LED。然后将它们切换到下一个随机颜色。
 */
uint16_t WS2812FX_mode_random_color(void)
{
    ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);// AUX_PARAM将存储我们的随机色轮索引
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param);
    adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);
    SET_CYCLE;
    return ws2812fx._seg->speed;
}

/*
 * 以随机颜色点亮每个LED。将一个随机LED一个接一个地更改为另一个随机颜色。
 */
uint16_t WS2812FX_mode_single_dynamic(void)
{
    if(ws2812fx._seg_rt->counter_mode_call == 0)
    {
        for(uint16_t i=ws2812fx._seg->start; i <= ws2812fx._seg->stop; i++)
        {
            ws2812fx.setPixelColor(i, ws2812fx.color_wheel(ws2812fx.random8()));
        }
    }
    ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len), ws2812fx.color_wheel(ws2812fx.random8()));
    SET_CYCLE;
    return (ws2812fx._seg->speed / 16) ;
}

/*
 * 以随机颜色点亮每个LED。同时将所有LED更改为新的随机颜色。
 */
uint16_t WS2812FX_mode_multi_dynamic(void)
{
    for(uint16_t i=ws2812fx._seg->start; i <= ws2812fx._seg->stop; i++)
    {
        ws2812fx.setPixelColor(i, ws2812fx.color_wheel(ws2812fx.random8()));
    }
    SET_CYCLE;
    return (ws2812fx._seg->speed / 4);
}

/*
 * 人们熟知的I-Devices的“待机呼吸”功能。固定速度。如果你想用不同速度的类似的东西，使用模式“淡入淡出”。
 */
uint16_t WS2812FX_mode_breath(void)
{
    int lum = ws2812fx._seg_rt->counter_mode_step;
    if(lum > 255) lum = 511 - lum; // lum = 15 -> 255 -> 15

    uint16_t delay;
    if(lum == 15) delay = 970; // 970 pause before each breath
    else if(lum <=  25) delay = 38; // 19
    else if(lum <=  50) delay = 36; // 18
    else if(lum <=  75) delay = 28; // 14
    else if(lum <= 100) delay = 20; // 10
    else if(lum <= 125) delay = 14; // 7
    else if(lum <= 150) delay = 11; // 5
    else delay = 10; // 4

    uint32_t color =  ws2812fx_method.color_blend(ws2812fx._seg->colors[1], ws2812fx._seg->colors[0], lum);
    adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);

    ws2812fx._seg_rt->counter_mode_step += 2;
    if(ws2812fx._seg_rt->counter_mode_step > (512-15))
    {
        ws2812fx._seg_rt->counter_mode_step = 15;
        SET_CYCLE;
    }
    return delay;
}

/*
 * 使LED在两种颜色之间褪色
 */
uint16_t WS2812FX_mode_fade(void)
{
    int lum = ws2812fx._seg_rt->counter_mode_step;
    if(lum > 255) lum = 511 - lum; // lum = 0 -> 255 -> 0

    uint32_t color = ws2812fx_method.color_blend(ws2812fx._seg->colors[1], ws2812fx._seg->colors[0], lum);
    adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);

    ws2812fx._seg_rt->counter_mode_step += 4;
    if(ws2812fx._seg_rt->counter_mode_step > 511)
    {
        ws2812fx._seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }
    return (ws2812fx._seg->speed / 128);
}

/*
 * 来回运行像素块。
 */
uint16_t WS2812FX_mode_scan(void)
{
    return ws2812fx_method.scan(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], false);
}

/*
 * 在相反的方向上来回运行两个像素块。
 */
uint16_t WS2812FX_mode_dual_scan(void)
{
    return ws2812fx_method.scan(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], true);
}

/*
 * 一次循环所有LED穿过彩虹。
 */
uint16_t WS2812FX_mode_rainbow(void)
{
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->counter_mode_step);
    adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) & 0xFF;

    if(ws2812fx._seg_rt->counter_mode_step == 0)  SET_CYCLE;

    return (ws2812fx._seg->speed / 256);
}

/*
 * 在整个LED串上循环显示一条彩虹。
 */
uint16_t WS2812FX_mode_rainbow_cycle(void)
{
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->counter_mode_step);
    if(IS_REVERSE)
    {
        ws2812fx.copyPixels(ws2812fx._seg->start, ws2812fx._seg->start + 1, ws2812fx._seg_len - 1);
        ws2812fx.setPixelColor(ws2812fx._seg->stop, color);
    }
    else
    {
        ws2812fx.copyPixels(ws2812fx._seg->start + 1, ws2812fx._seg->start, ws2812fx._seg_len - 1);
        ws2812fx.setPixelColor(ws2812fx._seg->start, color);
    }

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) & 0xFF;
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;

    return (ws2812fx._seg->speed / 256);
}

/*
 * 三色追逐模式
 */
uint16_t WS2812FX_mode_tricolor_chase(void)
{
    return ws2812fx_method.tricolor_chase(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], ws2812fx._seg->colors[2]);
}

/*
 * 交替运行的白/红/黑像素
 */
uint16_t WS2812FX_mode_circus_combustus(void)
{
    return ws2812fx_method.tricolor_chase(RED, WHITE, BLACK);
}

/*
 * 戏剧式的爬行灯光。
 * 灵感来自Adafruit的例子。
 */
uint16_t WS2812FX_mode_theater_chase(void)
{
    return ws2812fx_method.tricolor_chase(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], ws2812fx._seg->colors[1]);
}

/*
 * 剧场风格的爬行灯与彩虹效果。
 * 灵感来自Adafruit的例子。
 */
uint16_t WS2812FX_mode_theater_chase_rainbow(void)
{
    ws2812fx._seg_rt->aux_param = (ws2812fx._seg_rt->aux_param + 1) & 0xFF;
    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param);
    return ws2812fx_method.tricolor_chase(color, ws2812fx._seg->colors[1], ws2812fx._seg->colors[1]);
}

/*
 * 运行灯光效果与平滑的正弦过渡。
 */
uint16_t WS2812FX_mode_running_lights(void)
{
    uint8_t size = 1 << SIZE_OPTION;
    uint8_t sineIncr = MAX(1, (256 / ws2812fx._seg_len) * size);
    for(uint16_t i=0; i < ws2812fx._seg_len; i++)
    {
        int lum = (int)adafruit_neopixel.sine8(((i + ws2812fx._seg_rt->counter_mode_step) * sineIncr));
        uint32_t color = ws2812fx_method.color_blend(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], lum);
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + i, color);
        }
        else
        {
        ws2812fx.setPixelColor(ws2812fx._seg->stop - i,  color);
        }
    }
    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % 256;
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;
    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

/*
 * 使多个LED亮起、重置、重复。
 * 灵感来自www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle(void)
{
    return ws2812fx_method.twinkle(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1]);
}

/*
 * 使几个随机颜色的LED亮起、重置、重复。
 * 灵感来自www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_twinkle_random(void)
{
    return ws2812fx_method.twinkle(ws2812fx.color_wheel(ws2812fx.random8()), ws2812fx._seg->colors[1]);
}

/*
 * 亮起几个LED，然后熄灭。
 */
uint16_t WS2812FX_mode_twinkle_fade(void)
{
    return ws2812fx_method.twinkle_fade(ws2812fx._seg->colors[0]);
}

/*
 * 使几个随机颜色的LED亮起，然后淡出。
 */
uint16_t WS2812FX_mode_twinkle_fade_random(void)
{
    return ws2812fx_method.twinkle_fade(ws2812fx.color_wheel(ws2812fx.random8()));
}

/*
 * 一次闪烁一个LED。
 * 灵感来自www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_sparkle(void)
{
    return ws2812fx_method.sparkle(ws2812fx._seg->colors[1], ws2812fx._seg->colors[0]);
}

/*
 * 亮起该颜色的所有LED。随机闪烁白色像素。
 * 灵感来自www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_flash_sparkle(void)
{
    return ws2812fx_method.sparkle(ws2812fx._seg->colors[0], WHITE);
}

/*
 * 就像闪光灯一样。用更多的闪烁。
 * 灵感来自www.tweaking4all.com/hardware/arduino/arduino-led-strip-effects/
 */
uint16_t WS2812FX_mode_hyper_sparkle(void)
{
    adafruit_neopixel.fill(ws2812fx._seg->colors[0], ws2812fx._seg->start, ws2812fx._seg_len);

    uint8_t size = 1 << SIZE_OPTION;
    for(uint8_t i=0; i<8; i++)
    {
        adafruit_neopixel.fill(WHITE, ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len - size + 1), size);
    }

    SET_CYCLE;
    return (ws2812fx._seg->speed / 32);
}

/*
 * 闪光效果与不同的闪光灯计数和暂停，由速度控制。
 */
uint16_t WS2812FX_mode_multi_strobe(void)
{
    adafruit_neopixel.fill(ws2812fx._seg->colors[1], ws2812fx._seg->start, ws2812fx._seg_len);

    uint16_t delay = 200 + ((9 - (ws2812fx._seg->speed % 10)) * 100);
    uint16_t count = 2 * ((ws2812fx._seg->speed / 100) + 1);
    if(ws2812fx._seg_rt->counter_mode_step < count)
    {
        if((ws2812fx._seg_rt->counter_mode_step & 1) == 0)
        {
            adafruit_neopixel.fill(ws2812fx._seg->colors[0], ws2812fx._seg->start, ws2812fx._seg_len);
            delay = 20;
        }
        else
        {
            delay = 50;
        }
    }

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % (count + 1);
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;
    return delay;
}

/*
 * 双色追逐模式
 */
uint16_t WS2812FX_mode_bicolor_chase(void)
{
    return ws2812fx_method.chase(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1], ws2812fx._seg->colors[2]);
}

/*
 * 白色在奔跑_COLOR。
 */
uint16_t WS2812FX_mode_chase_color(void)
{
    return ws2812fx_method.chase(ws2812fx._seg->colors[0], WHITE, WHITE);
}

/*
 * 黑色在奔跑_COLOR。
 */
uint16_t WS2812FX_mode_chase_blackout(void)
{
    return ws2812fx_method.chase(ws2812fx._seg->colors[0], BLACK, BLACK);
}

/*
 * _白色上运行的颜色。
 */
uint16_t WS2812FX_mode_chase_white(void)
{
    return ws2812fx_method.chase(WHITE, ws2812fx._seg->colors[0], ws2812fx._seg->colors[0]);
}

/*
 * 白色运行，然后是随机的颜色。
 */
uint16_t WS2812FX_mode_chase_random(void)
{
    if(ws2812fx._seg_rt->counter_mode_step == 0)
    {
        ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);
    }
    return ws2812fx_method.chase(ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param), WHITE, WHITE);
}

/*
 * 彩虹在白色上流淌。
 */
uint16_t WS2812FX_mode_chase_rainbow_white(void)
{
    uint16_t n = ws2812fx._seg_rt->counter_mode_step;
    uint16_t m = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
    uint32_t color2 = ws2812fx.color_wheel(((n * 256 / ws2812fx._seg_len) + (ws2812fx._seg_rt->counter_mode_call & 0xFF)) & 0xFF);
    uint32_t color3 = ws2812fx.color_wheel(((m * 256 / ws2812fx._seg_len) + (ws2812fx._seg_rt->counter_mode_call & 0xFF)) & 0xFF);

    return ws2812fx_method.chase(WHITE, color2, color3);
}

/*
 * 白色在彩虹上奔跑。
 */
uint16_t WS2812FX_mode_chase_rainbow(void)
{
    uint8_t color_sep = 256 / ws2812fx._seg_len;
    uint8_t color_index = ws2812fx._seg_rt->counter_mode_call & 0xFF;
    uint32_t color = ws2812fx.color_wheel(((ws2812fx._seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

    return ws2812fx_method.chase(color, WHITE, WHITE);
}

/*
 * 黑色在彩虹上奔跑。
 */
uint16_t WS2812FX_mode_chase_blackout_rainbow(void)
{
    uint8_t color_sep = 256 / ws2812fx._seg_len;
    uint8_t color_index = ws2812fx._seg_rt->counter_mode_call & 0xFF;
    uint32_t color = ws2812fx.color_wheel(((ws2812fx._seg_rt->counter_mode_step * color_sep) + color_index) & 0xFF);

    return ws2812fx_method.chase(color, BLACK, BLACK);
}

/*
 * _COLOR上出现白色闪烁。
 */
uint16_t WS2812FX_mode_chase_flash(void)
{
    return ws2812fx_method.chase_flash(ws2812fx._seg->colors[0], WHITE);
}

/*
 * 白色闪烁，然后是随机的颜色。
 */
uint16_t WS2812FX_mode_chase_flash_random(void)
{
    return ws2812fx_method.chase_flash(ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param), WHITE);
}

/*
 * 彩色/白色像素交替运行。
 */
uint16_t WS2812FX_mode_running_color(void)
{
    return ws2812fx_method.running(ws2812fx._seg->colors[0], ws2812fx._seg->colors[1]);
}

/*
 * 红色/蓝色像素交替运行。
 */
uint16_t WS2812FX_mode_running_red_blue(void)
{
    return ws2812fx_method.running(RED, BLUE);
}

/*
 * 红色/绿色像素交替运行。
 */
uint16_t WS2812FX_mode_merry_christmas(void)
{
    return ws2812fx_method.running(RED, GREEN);
}

/*
 * 交替运行的橙色/紫色像素。
 */
uint16_t WS2812FX_mode_halloween(void)
{
    return ws2812fx_method.running(PURPLE, ORANGE);
}

/*
 * 随机运行的彩色像素。
 */
uint16_t WS2812FX_mode_running_random(void)
{
    uint8_t size = 2 << SIZE_OPTION;
    if((ws2812fx._seg_rt->counter_mode_step) % size == 0)
    {
        ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);
    }

    uint32_t color = ws2812fx.color_wheel(ws2812fx._seg_rt->aux_param);

    return ws2812fx_method.running(color, color);
}

/*
 * K.I.T.T.
 */
uint16_t WS2812FX_mode_larson_scanner(void)
{
    ws2812fx_method.fade_out(ws2812fx._seg->colors[1]);

    if(ws2812fx._seg_rt->counter_mode_step < ws2812fx._seg_len)
    {
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - ws2812fx._seg_rt->counter_mode_step, ws2812fx._seg->colors[0]);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx._seg_rt->counter_mode_step, ws2812fx._seg->colors[0]);
        }
    }
    else
    {
        uint16_t index = (ws2812fx._seg_len * 2) - ws2812fx._seg_rt->counter_mode_step - 2;
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - index, ws2812fx._seg->colors[0]);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + index, ws2812fx._seg->colors[0]);
        }
    }

    ws2812fx._seg_rt->counter_mode_step++;
    if(ws2812fx._seg_rt->counter_mode_step >= (uint16_t)((ws2812fx._seg_len * 2) - 2))
    {
        ws2812fx._seg_rt->counter_mode_step = 0;
        SET_CYCLE;
    }

    return (ws2812fx._seg->speed / (ws2812fx._seg_len * 2));
}

/*
 * 从一端发射彗星。
 */
uint16_t WS2812FX_mode_comet(void)
{
    ws2812fx_method.fade_out(ws2812fx._seg->colors[1]);

    if(IS_REVERSE)
    {
        ws2812fx.setPixelColor(ws2812fx._seg->stop - ws2812fx._seg_rt->counter_mode_step, ws2812fx._seg->colors[0]);
    }
    else
    {
        ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx._seg_rt->counter_mode_step, ws2812fx._seg->colors[0]);
    }

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;

    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

/*
 * 烟火火花四溅。
 */
uint16_t WS2812FX_mode_fireworks(void)
{
    uint32_t color = BLACK;
    do
    { // randomly choose a non-BLACK color from the colors array
        color = ws2812fx._seg->colors[ws2812fx.random8_lim(MAX_NUM_COLORS)];
    }while (color == BLACK);
    return ws2812fx_method.fireworks(color);
}

/*
 * 随机的彩色烟花火花。
 */
uint16_t WS2812FX_mode_fireworks_random(void)
{
    return ws2812fx_method.fireworks(ws2812fx.color_wheel(ws2812fx.random8()));
}

/*
 * 随机闪烁。
 */
uint16_t WS2812FX_mode_fire_flicker(void)
{
    return ws2812fx_method.fire_flicker(3);
}

/*
* 随机闪烁，强度较小。
*/
uint16_t WS2812FX_mode_fire_flicker_soft(void)
{
    return ws2812fx_method.fire_flicker(6);
}

/*
* 随机闪烁，强度更大。
*/
uint16_t WS2812FX_mode_fire_flicker_intense(void)
{
    return ws2812fx_method.fire_flicker(1);
}

// 马克·克里格斯曼的FastLED TinkeFOX效应的改编。
// https://gist.github.com/kriegsman/756ea6dcae8e30845b5a
uint16_t WS2812FX_mode_twinkleFOX(void)
{
    uint16_t mySeed = 0; // 重置随机数生成器种子

    // 获取并转换段的大小选项
    uint8_t size = 1 << ((ws2812fx._seg->options >> 1) & 0x03); // 1,2,4,8

    // 获取线段的颜色数组值
    uint32_t color0 = ws2812fx._seg->colors[0];
    uint32_t color1 = ws2812fx._seg->colors[1];
    uint32_t color2 = ws2812fx._seg->colors[2];
    uint32_t blendedColor;

    for (uint16_t i = ws2812fx._seg->start; i <= ws2812fx._seg->stop; i+=size)
    {
        // 使用Mark Kriegsman的巧妙想法，使用伪随机数来确定每个LED的初始值和增量混合值
        mySeed = (mySeed * 2053) + 13849; // 一个随机但确定的数字
        uint16_t initValue = (mySeed + (mySeed >> 8)) & 0xff; // LED的初始混合指数(0-255)
        mySeed = (mySeed * 2053) + 13849; // 另一个随机但确定的数字
        uint16_t incrValue = (((mySeed + (mySeed >> 8)) & 0x07) + 1) * 2; // 混合指数增量(2，4，6，8，10，12，14，16)

        // 我们将使用正弦函数来混合颜色，而不是使用Mark的三角形函数，因为Adafruit_NeoPixel库中已经内置了正弦查找表。
        // 是的，我很懒。
        // 使用COUNTER_MODE_CALL变量作为时钟“滴答”计数器并计算混合指数
        uint8_t blendIndex = (initValue + (ws2812fx._seg_rt->counter_mode_call * incrValue)) & 0xff; // 0-255
        // 索引到内置的Adafruit_NeoPixel正弦表以查找混合量
        uint8_t blendAmt = adafruit_neopixel.sine8(blendIndex); // 0-255

        // 如果Colors[0]为黑色，则混合随机颜色
        if(color0 == BLACK)
        {
            blendedColor = ws2812fx_method.color_blend(ws2812fx.color_wheel(initValue), color1, blendAmt);

        }
        else if((color2 != BLACK) && (initValue < 128) == 0)// 如果颜色[2]不是黑色，请选择混合颜色[0]/颜色[1]或颜色[1]/颜色[2](随机拾取要混合的颜色对)
        {
            blendedColor = ws2812fx_method.color_blend(color2, color1, blendAmt);
        }
        else
        {
            // 否则，始终混合颜色[0]/颜色[1]
            blendedColor = ws2812fx_method.color_blend(color0, color1, blendAmt);
        }

        // 将新颜色分配给大小选项指定的LED数量
        for(uint8_t j=0; j<size; j++)
        {
            if((i + j) <= ws2812fx._seg->stop)
            {
                ws2812fx.setPixelColor(i + j, blendedColor);
            }
        }
    }
    SET_CYCLE;
    return ws2812fx._seg->speed / 32;
}

// 烟花效果和奔跑效果的组合，以创建看起来像雨的效果。
uint16_t WS2812FX_mode_rain(void)
{
    // 随机选择颜色[0]或颜色[2]
    uint32_t rainColor = (ws2812fx.random8() & 1) == 0 ? ws2812fx._seg->colors[0] : ws2812fx._seg->colors[2];
    // 如果 colors[0] == colors[1], 随机选择一种颜色
    if(ws2812fx._seg->colors[0] == ws2812fx._seg->colors[1]) rainColor = ws2812fx.color_wheel(ws2812fx.random8());

    // 运行烟花效果以创建一个“雨滴”
    ws2812fx_method.fireworks(rainColor);

    // 将所有内容移动两个像素
    if(IS_REVERSE)
    {
        ws2812fx.copyPixels(ws2812fx._seg->start, ws2812fx._seg->start + 2, ws2812fx._seg_len - 2);
    }
    else
    {
        ws2812fx.copyPixels(ws2812fx._seg->start + 2, ws2812fx._seg->start, ws2812fx._seg_len - 2);
    }

    return (ws2812fx._seg->speed / 16);
}

// 块融合效果
uint16_t WS2812FX_mode_block_dissolve(void)
{
    uint32_t color = ws2812fx._seg->colors[ws2812fx._seg_rt->aux_param]; // 获取目标颜色

    // 在setPixelColor()根据亮度设置对其进行破坏后，获取抽取的颜色
    ws2812fx.setPixelColor(ws2812fx._seg->start, color);
    uint32_t desColor = adafruit_neopixel.getPixelColor(ws2812fx._seg->start);

    // 找到不是目标颜色的随机像素并对其进行更新
    for(uint16_t i=0; i<ws2812fx._seg_len; i++)
    {
        int index = ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len);
        if(adafruit_neopixel.getPixelColor(index) != desColor)
        {
            ws2812fx.setPixelColor(index, color);
            return ws2812fx._seg->speed / 64;
        }
    }

    // 如果未找到不是目标颜色的随机像素，则将整个线段设置为目标颜色
    adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);

    // 选择新的目标颜色
    ws2812fx._seg_rt->aux_param = (ws2812fx._seg_rt->aux_param + 1) % MAX_NUM_COLORS;
    if(ws2812fx._seg_rt->aux_param == 0) SET_CYCLE;
    return ws2812fx._seg->speed / 64;
}

// ICU效应
uint16_t WS2812FX_mode_icu(void)
{
    uint16_t pos = ws2812fx._seg_rt->counter_mode_step; // 当前眼睛位置
    uint16_t dest = ws2812fx._seg_rt->aux_param3;       // 眼睛的目的地
    uint16_t index = ws2812fx._seg->start + pos;        // 第一只眼的索引
    uint16_t index2 = index + ws2812fx._seg_len/2;      // 第二只眼的索引

    adafruit_neopixel.clear(); // 擦除当前眼睛

    if(pos != dest)// 如果眼睛还没有到达目的地
    {
        // 根据相对于目的地的位置向右或向左移动眼睛
        int dir = dest > pos ? 1 : -1;
        ws2812fx.setPixelColor(index + dir, ws2812fx._seg->colors[0]); // 画两只眼睛
        ws2812fx.setPixelColor(index2 + dir, ws2812fx._seg->colors[0]);
        ws2812fx._seg_rt->counter_mode_step += dir; // 更新眼睛位置
        return (ws2812fx._seg->speed / ws2812fx._seg_len);
    }
    else// 眼睛已经到达它们的目的地
    {
        if(ws2812fx.random8_lim(6) == 0)//偶尔眨眼
        {
            return 200;
        }
        else
        {
            ws2812fx.setPixelColor(index, ws2812fx._seg->colors[0]);
            ws2812fx.setPixelColor(index2, ws2812fx._seg->colors[0]);
            ws2812fx._seg_rt->aux_param3 = ws2812fx.random16(ws2812fx._seg_len/2); // 设置新的目的地
            SET_CYCLE;
            return 1000 + ws2812fx.random16(2000); // 停顿一两秒钟
        }
    }
}

// 双拉森效应
uint16_t WS2812FX_mode_dual_larson(void)
{
    ws2812fx_method.fade_out(ws2812fx._seg->colors[1]);

    ws2812fx._seg_rt->aux_param3 += ws2812fx._seg_rt->aux_param ? -1 : 1; // 更新LED索引

    ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx._seg_rt->aux_param3, ws2812fx._seg->colors[0]);
    ws2812fx.setPixelColor(ws2812fx._seg->stop  - ws2812fx._seg_rt->aux_param3, ws2812fx._seg->colors[2] ? ws2812fx._seg->colors[2] : ws2812fx._seg->colors[0]);

    if(ws2812fx._seg_rt->aux_param3 == 0 || ws2812fx._seg_rt->aux_param3 >= ws2812fx._seg_len - 1)
    {
        ws2812fx._seg_rt->aux_param = !ws2812fx._seg_rt->aux_param; // change direction
        SET_CYCLE;
    }

    return (ws2812fx._seg->speed / (ws2812fx._seg_len * 2));
}

// 随机擦拭亮度效果(与自定义RandomChase效果相同)
uint16_t WS2812FX_mode_random_wipe_bright(void)
{
    uint32_t color = IS_REVERSE ? adafruit_neopixel.getPixelColor(ws2812fx._seg->stop): adafruit_neopixel.getPixelColor(ws2812fx._seg->start);

    // 定期将每个RGB分量更改为随机值
    uint8_t mask = ws2812fx.random8_lim(7);
    color = mask & 0x1 ? color : (color & 0x00ffff) | (ws2812fx.random8() << 16);
    color = mask & 0x2 ? color : (color & 0xff00ff) | (ws2812fx.random8() <<  8);
    color = mask & 0x4 ? color : (color & 0xffff00) | (ws2812fx.random8());

    return ws2812fx_method.running(color, color);
}

/*
 * 自定义模式
 */
uint16_t customModes_Default(void)
{
    return 1000;
}

// 自定义模式函数指针数组
uint16_t (*customModes[8])(void)=
{
    customModes_Default,
    customModes_Default,
    customModes_Default,
    customModes_Default,
    customModes_Default,
    customModes_Default,
    customModes_Default,
    customModes_Default,
};

uint16_t WS2812FX_mode_custom_0()
{
    return customModes[0]();
}
uint16_t WS2812FX_mode_custom_1()
{
    return customModes[1]();
}
uint16_t WS2812FX_mode_custom_2()
{
    return customModes[2]();
}
uint16_t WS2812FX_mode_custom_3()
{
    return customModes[3]();
}
uint16_t WS2812FX_mode_custom_4()
{
    return customModes[4]();
}
uint16_t WS2812FX_mode_custom_5()
{
    return customModes[5]();
}
uint16_t WS2812FX_mode_custom_6()
{
    return customModes[6]();
}
uint16_t WS2812FX_mode_custom_7()
{
    return customModes[7]();
}

/* #####################################################
#
#  Mode Functions Define
#
##################################################### */
// 显示效果分类
const char cat_simple[]  = "Simple";
const char cat_wipe[]    = "Wipe";
const char cat_sweep[]   = "Sweep";
const char cat_special[] = "Special";
const char cat_custom[]  = "Custom";

// 显示效果名称
const char name_0[] = "Static";
const char name_1[] = "Blink";
const char name_2[] = "Breath";
const char name_3[] = "Color Wipe";
const char name_4[] = "Color Wipe Inverse";
const char name_5[] = "Color Wipe Reverse";
const char name_6[] = "Color Wipe Reverse Inverse";
const char name_7[] = "Color Wipe Random";
const char name_8[] = "Random Color";
const char name_9[] = "Single Dynamic";
const char name_10[] = "Multi Dynamic";
const char name_11[] = "Rainbow";
const char name_12[] = "Rainbow Cycle";
const char name_13[] = "Scan";
const char name_14[] = "Dual Scan";
const char name_15[] = "Fade";
const char name_16[] = "Theater Chase";
const char name_17[] = "Theater Chase Rainbow";
const char name_18[] = "Running Lights";
const char name_19[] = "Twinkle";
const char name_20[] = "Twinkle Random";
const char name_21[] = "Twinkle Fade";
const char name_22[] = "Twinkle Fade Random";
const char name_23[] = "Sparkle";
const char name_24[] = "Flash Sparkle";
const char name_25[] = "Hyper Sparkle";
const char name_26[] = "Strobe";
const char name_27[] = "Strobe Rainbow";
const char name_28[] = "Multi Strobe";
const char name_29[] = "Blink Rainbow";
const char name_30[] = "Chase White";
const char name_31[] = "Chase Color";
const char name_32[] = "Chase Random";
const char name_33[] = "Chase Rainbow";
const char name_34[] = "Chase Flash";
const char name_35[] = "Chase Flash Random";
const char name_36[] = "Chase Rainbow White";
const char name_37[] = "Chase Blackout";
const char name_38[] = "Chase Blackout Rainbow";
const char name_39[] = "Color Sweep Random";
const char name_40[] = "Running Color";
const char name_41[] = "Running Red Blue";
const char name_42[] = "Running Random";
const char name_43[] = "Larson Scanner";
const char name_44[] = "Comet";
const char name_45[] = "Fireworks";
const char name_46[] = "Fireworks Random";
const char name_47[] = "Merry Christmas";
const char name_48[] = "Fire Flicker";
const char name_49[] = "Fire Flicker (soft)";
const char name_50[] = "Fire Flicker (intense)";
const char name_51[] = "Circus Combustus";
const char name_52[] = "Halloween";
const char name_53[] = "Bicolor Chase";
const char name_54[] = "Tricolor Chase";
const char name_55[] = "TwinkleFOX";
const char name_56[] = "Rain";
const char name_57[] = "Custom 0"; // 自定义模式需要放在最后
const char name_58[] = "Custom 1";
const char name_59[] = "Custom 2";
const char name_60[] = "Custom 3";
const char name_61[] = "Custom 4";
const char name_62[] = "Custom 5";
const char name_63[] = "Custom 6";
const char name_64[] = "Custom 7";

/* #####################################################
#
#  Mode Functions Array
#
##################################################### */
// 定义成员函数指针的静态数组。
// 确保_modes数组元素的顺序与Enm_FX_Effect值匹配
__attribute__ ((unused)) const Tst_mode _modes[65] = 
{
    {name_0,  cat_simple,   WS2812FX_mode_static },
    {name_1,  cat_simple,   WS2812FX_mode_blink },
    {name_2,  cat_special,  WS2812FX_mode_breath },
    {name_3,  cat_wipe,     WS2812FX_mode_color_wipe },
    {name_4,  cat_wipe,     WS2812FX_mode_color_wipe_inv },
    {name_5,  cat_sweep,    WS2812FX_mode_color_wipe_rev },
    {name_6,  cat_sweep,    WS2812FX_mode_color_wipe_rev_inv },
    {name_7,  cat_wipe,     WS2812FX_mode_color_wipe_random },
    {name_8,  cat_simple,   WS2812FX_mode_random_color },
    {name_9,  cat_simple,   WS2812FX_mode_single_dynamic },
    {name_10, cat_simple,   WS2812FX_mode_multi_dynamic },
    {name_11, cat_simple,   WS2812FX_mode_rainbow },
    {name_12, cat_wipe,     WS2812FX_mode_rainbow_cycle },
    {name_13, cat_sweep,    WS2812FX_mode_scan },
    {name_14, cat_sweep,    WS2812FX_mode_dual_scan },
    {name_15, cat_sweep,    WS2812FX_mode_fade },
    {name_16, cat_wipe,     WS2812FX_mode_theater_chase },
    {name_17, cat_wipe,     WS2812FX_mode_theater_chase_rainbow },
    {name_18, cat_wipe,     WS2812FX_mode_running_lights },
    {name_19, cat_simple,   WS2812FX_mode_twinkle },
    {name_20, cat_simple,   WS2812FX_mode_twinkle_random },
    {name_21, cat_simple,   WS2812FX_mode_twinkle_fade },
    {name_22, cat_simple,   WS2812FX_mode_twinkle_fade_random },
    {name_23, cat_simple,   WS2812FX_mode_sparkle },
    {name_24, cat_simple,   WS2812FX_mode_flash_sparkle },
    {name_25, cat_simple,   WS2812FX_mode_hyper_sparkle },
    {name_26, cat_simple,   WS2812FX_mode_strobe },
    {name_27, cat_simple,   WS2812FX_mode_strobe_rainbow },
    {name_28, cat_simple,   WS2812FX_mode_multi_strobe },
    {name_29, cat_simple,   WS2812FX_mode_blink_rainbow },
    {name_30, cat_wipe,     WS2812FX_mode_chase_white },
    {name_31, cat_wipe,     WS2812FX_mode_chase_color },
    {name_32, cat_wipe,     WS2812FX_mode_chase_random },
    {name_33, cat_wipe,     WS2812FX_mode_chase_rainbow },
    {name_34, cat_wipe,     WS2812FX_mode_chase_flash },
    {name_35, cat_wipe,     WS2812FX_mode_chase_flash_random },
    {name_36, cat_wipe,     WS2812FX_mode_chase_rainbow_white },
    {name_37, cat_wipe,     WS2812FX_mode_chase_blackout },
    {name_38, cat_wipe,     WS2812FX_mode_chase_blackout_rainbow },
    {name_39, cat_sweep,    WS2812FX_mode_color_sweep_random },
    {name_40, cat_wipe,     WS2812FX_mode_running_color },
    {name_41, cat_wipe,     WS2812FX_mode_running_red_blue },
    {name_42, cat_wipe,     WS2812FX_mode_running_random },
    {name_43, cat_sweep,    WS2812FX_mode_larson_scanner },
    {name_44, cat_wipe,     WS2812FX_mode_comet },
    {name_45, cat_simple,   WS2812FX_mode_fireworks },
    {name_46, cat_simple,   WS2812FX_mode_fireworks_random },
    {name_47, cat_wipe,     WS2812FX_mode_merry_christmas },
    {name_48, cat_simple,   WS2812FX_mode_fire_flicker },
    {name_49, cat_simple,   WS2812FX_mode_fire_flicker_soft },
    {name_50, cat_simple,   WS2812FX_mode_fire_flicker_intense },
    {name_51, cat_wipe,     WS2812FX_mode_circus_combustus },
    {name_52, cat_wipe,     WS2812FX_mode_halloween },
    {name_53, cat_wipe,     WS2812FX_mode_bicolor_chase },
    {name_54, cat_wipe,     WS2812FX_mode_tricolor_chase },
    {name_55, cat_special,  WS2812FX_mode_twinkleFOX },
    {name_56, cat_special,  WS2812FX_mode_rain },
    {name_57, cat_custom,   WS2812FX_mode_custom_0 },
    {name_58, cat_custom,   WS2812FX_mode_custom_1 },
    {name_59, cat_custom,   WS2812FX_mode_custom_2 },
    {name_60, cat_custom,   WS2812FX_mode_custom_3 },
    {name_61, cat_custom,   WS2812FX_mode_custom_4 },
    {name_62, cat_custom,   WS2812FX_mode_custom_5 },
    {name_63, cat_custom,   WS2812FX_mode_custom_6 },
    {name_64, cat_custom,   WS2812FX_mode_custom_7 }
};

#endif
