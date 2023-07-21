/*
  modes_funcs.cpp - WS2812FX effects helper functions

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
#include <stdlib.h>
#include <string.h>
/*
 * 闪烁/闪光功能。
 * 在Color1和Color2之间交替。
 * 如果(STROBE==TRUE)，则创建闪光灯效果
 */
static uint16_t WS2812FX_blink(uint32_t color1, uint32_t color2, bool strobe)
{
    if(ws2812fx._seg_rt->counter_mode_call & 1)
    {
        uint32_t color = (IS_REVERSE) ? color1 : color2; // off
        adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);
        SET_CYCLE;
        return strobe ? ws2812fx._seg->speed - 20 : (ws2812fx._seg->speed / 2);
    }
    else
    {
        uint32_t color = (IS_REVERSE) ? color2 : color1; // on
        adafruit_neopixel.fill(color, ws2812fx._seg->start, ws2812fx._seg_len);
        return strobe ? 20 : (ws2812fx._seg->speed / 2);
    }
}

/*
 * 彩色擦除功能。
 * LED按顺序点亮(Color1)，然后按顺序熄灭(Color2)。
 * 如果(bool rev==true)，则LED按相反顺序熄灭
 */
static uint16_t WS2812FX_color_wipe(uint32_t color1, uint32_t color2, bool rev)
{
    if(ws2812fx._seg_rt->counter_mode_step < ws2812fx._seg_len)
    {
        uint32_t led_offset = ws2812fx._seg_rt->counter_mode_step;
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - led_offset, color1);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + led_offset, color1);
        }
    }
    else
    {
        uint32_t led_offset = ws2812fx._seg_rt->counter_mode_step - ws2812fx._seg_len;
        if((IS_REVERSE && !rev) || (!IS_REVERSE && rev))
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - led_offset, color2);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + led_offset, color2);
        }
    }

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % (ws2812fx._seg_len * 2);
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;
    return (ws2812fx._seg->speed / (ws2812fx._seg_len * 2));
}


/*
 * 扫描功能-来回运行像素块。
 */
static uint16_t WS2812FX_scan(uint32_t color1, uint32_t color2, bool dual)
{
    int8_t dir = ws2812fx._seg_rt->aux_param ? -1 : 1;
    uint8_t size = 1 << SIZE_OPTION;

    adafruit_neopixel.fill(color2, ws2812fx._seg->start, ws2812fx._seg_len);

    for(uint8_t i = 0; i < size; i++)
    {
        if(IS_REVERSE || dual)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - ws2812fx._seg_rt->counter_mode_step - i, color1);
        }
        if(!IS_REVERSE || dual)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx._seg_rt->counter_mode_step + i, color1);
        }
    }

    ws2812fx._seg_rt->counter_mode_step += dir;
    if(ws2812fx._seg_rt->counter_mode_step == 0)
    {
        ws2812fx._seg_rt->aux_param = 0;
        SET_CYCLE;
    }
    if(ws2812fx._seg_rt->counter_mode_step >= (uint16_t)(ws2812fx._seg_len - size)) ws2812fx._seg_rt->aux_param = 1;
    return (ws2812fx._seg->speed / (ws2812fx._seg_len * 2));
}

/*
 * 三色追逐函数
 */
static uint16_t WS2812FX_tricolor_chase(uint32_t color1, uint32_t color2, uint32_t color3)
{
    uint8_t sizeCnt = 1 << SIZE_OPTION;
    uint8_t sizeCnt2 = sizeCnt + sizeCnt;
    uint8_t sizeCnt3 = sizeCnt2 + sizeCnt;
    uint16_t index = ws2812fx._seg_rt->counter_mode_step % sizeCnt3;

    for(uint16_t i=0; i < ws2812fx._seg_len; i++, index++)
    {
        index = index % sizeCnt3;

        uint32_t color = color3;
        if(index < sizeCnt) color = color1;
        else if(index < sizeCnt2) color = color2;

        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + i, color);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - i, color);
        }
    }

    ws2812fx._seg_rt->counter_mode_step++;
    if(ws2812fx._seg_rt->counter_mode_step % ws2812fx._seg_len == 0) SET_CYCLE;

    return (ws2812fx._seg->speed / 16);
}

/*
 * 闪烁功能
 */
static uint16_t WS2812FX_twinkle(uint32_t color1, uint32_t color2)
{
    if(ws2812fx._seg_rt->counter_mode_step == 0)
    {
        adafruit_neopixel.fill(color2, ws2812fx._seg->start, ws2812fx._seg_len);
        uint16_t min_leds = (ws2812fx._seg_len / 4) + 1; // 确保至少有一个LED亮起
        //ws2812fx._seg_rt->counter_mode_step = random(min_leds, min_leds * 2);
        ws2812fx._seg_rt->counter_mode_step = min_leds * (ws2812fx.random16(2) + 1);
        SET_CYCLE;
    }

    ws2812fx.setPixelColor(ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len), color1);

    ws2812fx._seg_rt->counter_mode_step--;
    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

/*
 * 淡出功能
 */
static void WS2812FX_fade_out(uint32_t targetColor)
{
    static const uint8_t rateMapH[] = {0, 1, 1, 1, 2, 3, 4, 6};
    static const uint8_t rateMapL[] = {0, 2, 3, 8, 8, 8, 8, 8};

    uint8_t rate  = FADE_RATE;
    uint8_t rateH = rateMapH[rate];
    uint8_t rateL = rateMapL[rate];

    uint32_t color = targetColor;
    int w2 = (color >> 24) & 0xff;
    int r2 = (color >> 16) & 0xff;
    int g2 = (color >>  8) & 0xff;
    int b2 =  color        & 0xff;

    for(uint16_t i=ws2812fx._seg->start; i <= ws2812fx._seg->stop; i++)
    {
        color = adafruit_neopixel.getPixelColor(i); // 当前颜色
        if(rate == 0)// 旧的淡入淡出算法
        {
            ws2812fx.setPixelColor(i, (color >> 1) & 0x7F7F7F7F);
        }
        else// 一种新的淡入彩色算法
        {
            int w1 = (color >> 24) & 0xff;
            int r1 = (color >> 16) & 0xff;
            int g1 = (color >>  8) & 0xff;
            int b1 =  color        & 0xff;

            // 计算当前颜色和目标颜色之间的色差
            int wdelta = w2 - w1;
            int rdelta = r2 - r1;
            int gdelta = g2 - g1;
            int bdelta = b2 - b1;

            // 如果当前颜色和目标颜色几乎相同，则直接跳到目标颜色，否则计算中间颜色。(修复了舍入问题)
            wdelta = abs(wdelta) < 3 ? wdelta : (wdelta >> rateH) + (wdelta >> rateL);
            rdelta = abs(rdelta) < 3 ? rdelta : (rdelta >> rateH) + (rdelta >> rateL);
            gdelta = abs(gdelta) < 3 ? gdelta : (gdelta >> rateH) + (gdelta >> rateL);
            bdelta = abs(bdelta) < 3 ? bdelta : (bdelta >> rateH) + (bdelta >> rateL);

            ws2812fx.setPixelColor_rgbw(i, r1 + rdelta, g1 + gdelta, b1 + bdelta, w1 + wdelta);
        }
    }
}

/*
 * 颜色混合功能
 */
static uint32_t WS2812FX_color_blend(uint32_t color1, uint32_t color2, uint8_t blendAmt)
{
    uint32_t blendedColor;
    ws2812fx_method.blend((uint8_t*)&blendedColor, (uint8_t*)&color1, (uint8_t*)&color2, sizeof(uint32_t), blendAmt);
    return blendedColor;
}

static uint8_t* WS2812FX_blend(uint8_t *dest, uint8_t *src1, uint8_t *src2, uint16_t cnt, uint8_t blendAmt)
{
    if(blendAmt == 0)
    {
        memmove(dest, src1, cnt);
    }
    else if(blendAmt == 255)
    {
        memmove(dest, src2, cnt);
    }
    else
    {
        for(uint16_t i=0; i<cnt; i++)
        {
            dest[i] =  blendAmt * ((int)src2[i] - (int)src1[i]) / 256 + src1[i]; // map() function
        }
    }
    return dest;
}

/*
 * 闪烁淡出功能
 */
static uint16_t WS2812FX_twinkle_fade(uint32_t color)
{
    WS2812FX_fade_out(ws2812fx._seg->colors[1]);

    if(ws2812fx.random8_lim(3) == 0)
    {
        uint8_t size = 1 << SIZE_OPTION;
        uint16_t index = ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len - size + 1);
        adafruit_neopixel.fill(color, index, size);
        SET_CYCLE;
    }
    return (ws2812fx._seg->speed / 16);
}

/*
 * 闪光功能
 * color1 = 背景颜色
 * color2 = 闪亮的颜色
 */
static uint16_t WS2812FX_sparkle(uint32_t color1, uint32_t color2)
{
    if(ws2812fx._seg_rt->counter_mode_step == 0)
    {
        adafruit_neopixel.fill(color1, ws2812fx._seg->start, ws2812fx._seg_len);
    }

    uint8_t size = 1 << SIZE_OPTION;
    adafruit_neopixel.fill(color1, ws2812fx._seg->start + ws2812fx._seg_rt->aux_param3, size);

    ws2812fx._seg_rt->aux_param3 = ws2812fx.random16(ws2812fx._seg_len - size + 1); // AUX_PARAME3存储随机的LED索引
    adafruit_neopixel.fill(color2, ws2812fx._seg->start + ws2812fx._seg_rt->aux_param3, size);

    SET_CYCLE;
    return (ws2812fx._seg->speed / 32);
}

/*
 * 色彩追逐功能
 * color1 = 背景颜色
 * color2 and color3 = 两个相邻LED的颜色
 */
static uint16_t WS2812FX_chase(uint32_t color1, uint32_t color2, uint32_t color3)
{
    uint8_t size = 1 << SIZE_OPTION;

    for(uint8_t i=0; i<size; i++)
    {
        uint16_t a = (ws2812fx._seg_rt->counter_mode_step + i) % ws2812fx._seg_len;
        uint16_t b = (a + size) % ws2812fx._seg_len;
        uint16_t c = (b + size) % ws2812fx._seg_len;
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - a, color1);
            ws2812fx.setPixelColor(ws2812fx._seg->stop - b, color2);
            ws2812fx.setPixelColor(ws2812fx._seg->stop - c, color3);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + a, color1);
            ws2812fx.setPixelColor(ws2812fx._seg->start + b, color2);
            ws2812fx.setPixelColor(ws2812fx._seg->start + c, color3);
        }
    }

    if(ws2812fx._seg_rt->counter_mode_step + (size * 3) == ws2812fx._seg_len) SET_CYCLE;

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

/*
 * 运行白色闪烁功能。
 * color1 = 背景颜色
 * color2 = 闪光颜色
 */
static uint16_t WS2812FX_chase_flash(uint32_t color1, uint32_t color2)
{
    const static uint8_t flash_count = 4;
    uint8_t flash_step = ws2812fx._seg_rt->counter_mode_call % ((flash_count * 2) + 1);

    if(flash_step < (flash_count * 2))
    {
        uint32_t color = (flash_step % 2 == 0) ? color2 : color1;
        uint16_t n = ws2812fx._seg_rt->counter_mode_step;
        uint16_t m = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
        if(IS_REVERSE)
        {
            ws2812fx.setPixelColor(ws2812fx._seg->stop - n, color);
            ws2812fx.setPixelColor(ws2812fx._seg->stop - m, color);
        }
        else
        {
            ws2812fx.setPixelColor(ws2812fx._seg->start + n, color);
            ws2812fx.setPixelColor(ws2812fx._seg->start + m, color);
        }
        return 30;
    }
    else
    {
        ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
        if(ws2812fx._seg_rt->counter_mode_step == 0)
        {
            // 更新AUX_PARAM，以便MODE_CHASE_FLASH_RANDOM()选择下一种颜色
            ws2812fx._seg_rt->aux_param = ws2812fx.get_random_wheel_index(ws2812fx._seg_rt->aux_param);
            SET_CYCLE;
        }
    }
    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

/*
 * 交替像素运行功能
 */
static uint16_t WS2812FX_running(uint32_t color1, uint32_t color2)
{
    uint8_t size = 2 << SIZE_OPTION;
    uint32_t color = (ws2812fx._seg_rt->counter_mode_step & size) ? color1 : color2;

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

    ws2812fx._seg_rt->counter_mode_step = (ws2812fx._seg_rt->counter_mode_step + 1) % ws2812fx._seg_len;
    if(ws2812fx._seg_rt->counter_mode_step == 0) SET_CYCLE;
    return (ws2812fx._seg->speed / 16);
}

/*
 * 烟花功能
 */
static uint16_t WS2812FX_fireworks(uint32_t color)
{
    WS2812FX_fade_out(ws2812fx._seg->colors[1]);

    // 为获得更好的性能，请直接操作Adafruit_NeoPixels Pixels[]数组
    uint8_t *pixels = adafruit_neopixel.getPixels();
    uint8_t bytesPerPixel = ws2812fx.getNumBytesPerPixel(); // 3=RGB, 4=RGBW
    uint16_t startPixel = ws2812fx._seg->start * bytesPerPixel + bytesPerPixel;
    uint16_t stopPixel = ws2812fx._seg->stop * bytesPerPixel;
    for(uint16_t i=startPixel; i <stopPixel; i++)
    {
        uint16_t tmpPixel = (pixels[i - bytesPerPixel] >> 2) +
        pixels[i] +
        (pixels[i + bytesPerPixel] >> 2);
        pixels[i] =  tmpPixel > 255 ? 255 : tmpPixel;
    }

    uint8_t size = 2 << SIZE_OPTION;
    if(!ws2812fx.isTriggered())
    {
        for(uint16_t i=0; i<MAX(1, ws2812fx._seg_len/20); i++)
        {
            if(ws2812fx.random8_lim(10) == 0)
            {
                uint16_t index = ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len - size + 1);
                adafruit_neopixel.fill(color, index, size);
                SET_CYCLE;
            }
        }
    }
    else
    {
        for(uint16_t i=0; i<MAX(1, ws2812fx._seg_len/10); i++)
        {
            uint16_t index = ws2812fx._seg->start + ws2812fx.random16(ws2812fx._seg_len - size + 1);
            adafruit_neopixel.fill(color, index, size);
            SET_CYCLE;
        }
    }

    return (ws2812fx._seg->speed / 16);
}

/*
 * 火焰闪烁功能
 */
static uint16_t WS2812FX_fire_flicker(int rev_intensity)
{
    uint8_t w = (ws2812fx._seg->colors[0] >> 24) & 0xFF;
    uint8_t r = (ws2812fx._seg->colors[0] >> 16) & 0xFF;
    uint8_t g = (ws2812fx._seg->colors[0] >>  8) & 0xFF;
    uint8_t b = (ws2812fx._seg->colors[0]        & 0xFF);
    uint8_t lum = MAX(w, MAX(r, MAX(g, b))) / rev_intensity;
    for(uint16_t i=ws2812fx._seg->start; i <= ws2812fx._seg->stop; i++)
    {
        int flicker = ws2812fx.random8_lim(lum);
        ws2812fx.setPixelColor_rgbw(i, MAX(r - flicker, 0), MAX(g - flicker, 0), MAX(b - flicker, 0), MAX(w - flicker, 0));
    }

    SET_CYCLE;
    return (ws2812fx._seg->speed / ws2812fx._seg_len);
}

Tst_Method ws2812fx_method =
{
    WS2812FX_blend,
    WS2812FX_blink,
    WS2812FX_chase,
    WS2812FX_chase_flash,
    WS2812FX_color_blend,
    WS2812FX_color_wipe,
    WS2812FX_fade_out,
    WS2812FX_fire_flicker,
    WS2812FX_fireworks,
    WS2812FX_running,
    WS2812FX_scan,
    WS2812FX_sparkle,
    WS2812FX_tricolor_chase,
    WS2812FX_twinkle,
    WS2812FX_twinkle_fade,
};

#endif
