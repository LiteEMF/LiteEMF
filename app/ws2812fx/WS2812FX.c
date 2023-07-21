#include  "hw_config.h"
#if APP_WS2812FX_ENABLE
#include "WS2812FX.h"
#include "stdlib.h"
#include "string.h"
#include <stdint.h>
#include "api/api_tick.h"

_WEAK_ uint16_t constrain(uint16_t x,uint16_t a,uint16_t b)
{
    x = (x<a)?a:x;
    x = (x>b)?b:x;
    return x;
}

_WEAK_ uint32_t millis(void)
{
    return m_systick;
}    

static void (*customShow)(void) = NULL;//自定义显示函数指针

static Tst_Segment segment[MAX_NUM_SEGMENTS];// 段参数
static Tst_Segment_runtime segment_runtime[MAX_NUM_ACTIVE_SEGMENTS];// 段运行时参数 
static uint8_t active_segments[MAX_NUM_ACTIVE_SEGMENTS];// 记录活动段编号的数组

static bool _running;
static bool _triggered;
uint16_t _rand16seed;
static Tst_Segment* _segments;// 段数组(每个元素20个字节)
static Tst_Segment_runtime* _segment_runtimes; // 段运行参数数组(每个元素16个字节)
static uint8_t* _active_segments;// 活动段数组(每个元素1个字节)
static uint8_t _segments_len;// _Segments数组的大小
static uint8_t _active_segments_len;// _Segments_Runtime和_Active_Segments数组大小
static uint8_t _num_segments;// _Segments数组中配置的段数

static void WS2812FX_structure(uint16_t num_leds, neoPixelType type, uint8_t max_num_segments, uint8_t max_num_active_segments)
{
    adafruit_neopixel.init(num_leds, type);
    adafruit_neopixel.brightness = DEFAULT_BRIGHTNESS + 1; //Adafruit_NeoPixel内部将亮度偏移1
    _running = false;

    if(max_num_segments > MAX_NUM_SEGMENTS)max_num_segments = MAX_NUM_SEGMENTS;
    if(max_num_active_segments > MAX_NUM_ACTIVE_SEGMENTS )max_num_active_segments = MAX_NUM_ACTIVE_SEGMENTS;
    _segments_len = max_num_segments;
    _active_segments_len = max_num_active_segments;

    // 创建所有段数组并将其初始化为零
    _segments = segment;
    _active_segments = active_segments;
    _segment_runtimes = segment_runtime;

    // 初始段指针
    ws2812fx._seg     = _segments;
    ws2812fx._seg_rt  = _segment_runtimes;

    ws2812fx.resetSegments();
    ws2812fx.setSegment(0, 0, num_leds - 1, DEFAULT_MODE, DEFAULT_COLOR, DEFAULT_SPEED, NO_OPTIONS);
};

static void WS2812FX_resetSegmentRuntimes() 
{
    memset(_segment_runtimes, 0, _active_segments_len * sizeof(Tst_Segment_runtime));
}

// 重载show()函数，以便我们可以使用定制show()
void WS2812FX_show(void)
{
    customShow == NULL ? adafruit_neopixel.show() : customShow();
}

static void WS2812FX_init()
{
    WS2812FX_resetSegmentRuntimes();
    adafruit_neopixel.begin();
}

bool WS2812FX_service()
{
    bool doShow = false;
    if(_running || _triggered)
    {
        unsigned long now = millis(); // 请注意，Millis()每49天滚动一次
        for(uint8_t i=0; i < _active_segments_len; i++)
        {
            if(_active_segments[i] != INACTIVE_SEGMENT)
            {
                ws2812fx._seg     = &_segments[_active_segments[i]];
                ws2812fx._seg_len = (uint16_t)(ws2812fx._seg->stop - ws2812fx._seg->start + 1);
                ws2812fx._seg_rt  = &_segment_runtimes[i];
                CLR_FRAME_CYCLE;
                if(now > ws2812fx._seg_rt->next_time || _triggered)
                {
                    SET_FRAME;
                    doShow = true;
                    uint16_t delay = (MODE_PTR(ws2812fx._seg->mode))();
                    ws2812fx._seg_rt->next_time = now + MAX(delay, SPEED_MIN);
                    ws2812fx._seg_rt->counter_mode_call++;
                }
            }
        }
        if(doShow)
        {
            WS2812FX_show();
        }
        _triggered = false;
    }
    return doShow;
}

// 重载setPixelColor()函数，以便我们可以使用Gamma校正
// (see https://learn.adafruit.com/led-tricks-gamma-correction/the-issue)
void WS2812FX_setPixelColor(uint16_t n, uint32_t c)
{
    uint8_t w = (c >> 24) & 0xFF;
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >>  8) & 0xFF;
    uint8_t b =  c        & 0xFF;
    if(IS_GAMMA)
    {
        adafruit_neopixel.setPixelColor_rgbw(n, adafruit_neopixel.gamma8(r), adafruit_neopixel.gamma8(g),\
                                                adafruit_neopixel.gamma8(b), adafruit_neopixel.gamma8(w));
    }
    else
    {
        adafruit_neopixel.setPixelColor_rgbw(n, r, g, b, w);
    }
}

void WS2812FX_setPixelColor_rgb(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    if(IS_GAMMA)
    {
        adafruit_neopixel.setPixelColor_rgb(n, adafruit_neopixel.gamma8(r), adafruit_neopixel.gamma8(g), adafruit_neopixel.gamma8(b));
    }
    else
    {
        adafruit_neopixel.setPixelColor_rgb(n, r, g, b);
    }
}

void WS2812FX_setPixelColor_rgbw(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    if(IS_GAMMA)
    {
        adafruit_neopixel.setPixelColor_rgbw(n, adafruit_neopixel.gamma8(r), adafruit_neopixel.gamma8(g),\
                                                adafruit_neopixel.gamma8(b), adafruit_neopixel.gamma8(w));
    }
    else
    {
        adafruit_neopixel.setPixelColor_rgbw(n, r, g, b, w);
    }
}

static uint8_t WS2812FX_getNumBytesPerPixel(void) 
{
    return (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset) ? 3 : 4; // 3=RGB, 4=RGBW
}

void WS2812FX_copyPixels(uint16_t dest, uint16_t src, uint16_t count)
{
    uint8_t *pixels = adafruit_neopixel.getPixels();
    uint8_t bytesPerPixel = WS2812FX_getNumBytesPerPixel(); // 3=RGB, 4=RGBW

    memmove(pixels + (dest * bytesPerPixel), pixels + (src * bytesPerPixel), count * bytesPerPixel);
}

// 更改基础Adafruit_NeoPixel像素指针(请谨慎使用)
void WS2812FX_setPixels(uint16_t num_leds, uint8_t* ptr)
{
    my_free(adafruit_neopixel.pixels); // free existing data (if any)
    adafruit_neopixel.pixels = ptr;
    adafruit_neopixel.numLEDs = num_leds;
    adafruit_neopixel.numBytes = num_leds * ((adafruit_neopixel.wOffset == adafruit_neopixel.rOffset) ? 3 : 4);
}

static void WS2812FX_resetSegmentRuntime(uint8_t seg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr == NULL) return; // segment not active
    memset(&_segment_runtimes[ptr - _active_segments], 0, sizeof(Tst_Segment_runtime));
}

static void WS2812FX_setMode(uint8_t seg, uint8_t m)
{
    WS2812FX_resetSegmentRuntime(seg);
    _segments[seg].mode = constrain(m, 0, MODE_COUNT - 1);// 归一化到指定区间
}

static void WS2812FX_setOptions(uint8_t seg, uint8_t o)
{
    _segments[seg].options = o;
}

static void WS2812FX_setSpeed(uint8_t seg, uint16_t s)
{
    _segments[seg].speed = constrain(s, SPEED_MIN, SPEED_MAX);// 归一化到指定区间
}

static void WS2812FX_increaseSpeed(uint8_t s)
{
    uint16_t newSpeed = constrain(ws2812fx._seg->speed + s, SPEED_MIN, SPEED_MAX);// 归一化到指定区间
    WS2812FX_setSpeed(0, newSpeed);
}

static void WS2812FX_decreaseSpeed(uint8_t s)
{
    uint16_t newSpeed = constrain(ws2812fx._seg->speed - s, SPEED_MIN, SPEED_MAX);// 归一化到指定区间
    WS2812FX_setSpeed(0, newSpeed);
}

static void WS2812FX_setColor_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    _segments[0].colors[0] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static void WS2812FX_setColor_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    _segments[0].colors[0] = ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b);
}

static void WS2812FX_setColor(uint32_t c)
{
    _segments[0].colors[0] = c;
}

static void WS2812FX_setColor_seg(uint8_t seg, uint32_t c)
{
    _segments[seg].colors[0] = c;
}

static void WS2812FX_setColors(uint8_t seg, uint32_t* c)
{
    for(uint8_t i=0; i<MAX_NUM_COLORS; i++)
    {
        _segments[seg].colors[i] = c[i];
    }
}

static void WS2812FX_setBrightness(uint8_t b)
{
    adafruit_neopixel.setBrightness(b);
    WS2812FX_show();
}

static void WS2812FX_increaseBrightness(uint8_t s)
{
    uint8_t brightness = adafruit_neopixel.getBrightness();
    WS2812FX_setBrightness(brightness + s);
}

static void WS2812FX_decreaseBrightness(uint8_t s)
{
    uint8_t brightness = adafruit_neopixel.getBrightness();
    WS2812FX_setBrightness(brightness - s);
}

static void WS2812FX_setLength(uint16_t b)
{
    WS2812FX_resetSegmentRuntimes();
    if (b < 1) b = 1;

  // 将数字LED减少到最大可用内存
    do
    {
        adafruit_neopixel.updateLength(b);
        b--;
    } while(!adafruit_neopixel.numLEDs && b > 1);

    _segments[0].start = 0;
    _segments[0].stop = adafruit_neopixel.numLEDs - 1;
}

static void WS2812FX_increaseLength(uint16_t s)
{
    uint16_t seglen = _segments[0].stop - _segments[0].start + 1;
    WS2812FX_setLength(seglen + s);
}

static void WS2812FX_decreaseLength(uint16_t s)
{
    uint16_t seglen = _segments[0].stop - _segments[0].start + 1;
    adafruit_neopixel.fill(BLACK, _segments[0].start, seglen);
    WS2812FX_show();

    if (s < seglen) WS2812FX_setLength(seglen - s);
}

bool WS2812FX_isRunning(void)
{
    return _running;
}

bool WS2812FX_isTriggered(void) 
{
    return _triggered;
}

bool WS2812FX_isFrame(uint8_t seg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr == NULL) return false; // segment not active
    return (_segment_runtimes[ptr - _active_segments].aux_param2 & FRAME);
}

bool WS2812FX_isCycle(uint8_t seg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr == NULL) return false; // segment not active
    return (_segment_runtimes[ptr - _active_segments].aux_param2 & CYCLE);
}

static void WS2812FX_setCycle() 
{
    SET_CYCLE;
}

static uint8_t WS2812FX_getMode(uint8_t seg)  
{
    return _segments[seg].mode;
}

static uint16_t WS2812FX_getSpeed(uint8_t seg) 
{
    return _segments[seg].speed;
}

static uint8_t WS2812FX_getOptions(uint8_t seg) 
{
    return _segments[seg].options;
}

static uint16_t WS2812FX_getLength(void) 
{
    return adafruit_neopixel.numPixels();
}

uint16_t WS2812FX_getNumBytes(void) 
{
    return adafruit_neopixel.numBytes;
}

static uint8_t WS2812FX_getModeCount(void) 
{
    return MODE_COUNT;
}

static uint8_t WS2812FX_getNumSegments(void) 
{
    return _num_segments;
}

static void WS2812FX_setNumSegments(uint8_t n) 
{
    _num_segments = n;
}

static uint32_t WS2812FX_getColor(uint8_t seg) 
{
    return _segments[seg].colors[0];
}

static uint32_t* WS2812FX_getColors(uint8_t seg) 
{
    return _segments[seg].colors;
}

static Tst_Segment* WS2812FX_getSegment_active(void) 
{
    return ws2812fx._seg;
}

static Tst_Segment* WS2812FX_getSegment(uint8_t seg) 
{
    return &_segments[seg];
}

static Tst_Segment* WS2812FX_getSegments(void) 
{
    return _segments;
}

static Tst_Segment_runtime* WS2812FX_getSegmentRuntime_active(void) 
{
    return ws2812fx._seg_rt;
}

static Tst_Segment_runtime* WS2812FX_getSegmentRuntime(uint8_t seg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr == NULL) return NULL; // segment not active
    return &_segment_runtimes[ptr - _active_segments];
}

static Tst_Segment_runtime* WS2812FX_getSegmentRuntimes(void) 
{
    return _segment_runtimes;
}

static uint8_t* WS2812FX_getActiveSegments(void) 
{
    return _active_segments;
}

static const char* WS2812FX_getModeName(uint8_t m) 
{
    if(m < MODE_COUNT) 
    {
        return MODE_NAME(m);
    }
    else 
    {
        return NULL;
    }
}

static void WS2812FX_addActiveSegment(uint8_t seg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr != NULL) return; // 数据段已处于活动状态
    for(uint8_t i=0; i<_active_segments_len; i++) 
    {
        if(_active_segments[i] == INACTIVE_SEGMENT) 
        {
            _active_segments[i] = seg;
            WS2812FX_resetSegmentRuntime(seg);
            break;
        }
    }
}

static void WS2812FX_setSegment_colors(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode, uint32_t* colors, uint16_t speed, uint8_t options) 
{
    if(n < _segments_len) 
    {
        if(n + 1 > _num_segments) _num_segments = n + 1;
        _segments[n].start = start;
        _segments[n].stop = stop;
        _segments[n].mode = mode;
        _segments[n].speed = speed;
        _segments[n].options = options;

        WS2812FX_setColors(n, colors);

        if(n < _active_segments_len) WS2812FX_addActiveSegment(n);
    }
}

static void WS2812FX_setSegment(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode, uint32_t color, uint16_t speed, uint8_t options) 
{
    uint32_t colors[] = {color, 0, 0};
    WS2812FX_setSegment_colors(n, start, stop, mode, colors, speed, options);
}

static void WS2812FX_removeActiveSegment(uint8_t seg) 
{
    for(uint8_t i=0; i<_active_segments_len; i++) 
    {
        if(_active_segments[i] == seg) 
        {
            _active_segments[i] = INACTIVE_SEGMENT;
        }
    }
}

static void WS2812FX_setIdleSegment_colors(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode, uint32_t* colors, uint16_t speed, uint8_t options) 
{
    WS2812FX_setSegment_colors(n, start, stop, mode, colors, speed, options);
    if(n < _active_segments_len) WS2812FX_removeActiveSegment(n);;
}

static void WS2812FX_setIdleSegment(uint8_t n, uint16_t start, uint16_t stop, uint8_t mode, uint32_t color, uint16_t speed, uint8_t options) 
{
    uint32_t colors[] = {color, 0, 0};
    WS2812FX_setIdleSegment_colors(n, start, stop, mode, colors, speed, options);
}

static void WS2812FX_swapActiveSegment(uint8_t oldSeg, uint8_t newSeg) 
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, newSeg, _active_segments_len);
    if(ptr != NULL) return; // 如果newSeg已处于活动状态，则不交换
    for(uint8_t i=0; i<_active_segments_len; i++) 
    {
        if(_active_segments[i] == oldSeg) 
        {
            _active_segments[i] = newSeg;

            // 重置除Next_Time之外的所有运行时参数，允许当前动画帧完成
            __attribute__ ((unused)) Tst_Segment_runtime seg_rt = _segment_runtimes[i];
            seg_rt.counter_mode_step = 0;
            seg_rt.counter_mode_call = 0;
            seg_rt.aux_param = 0;
            seg_rt.aux_param2 = 0;
            seg_rt.aux_param3 = 0;
            break;
        }
    }
}

static bool WS2812FX_isActiveSegment(uint8_t seg)
{
    uint8_t* ptr = (uint8_t*)memchr(_active_segments, seg, _active_segments_len);
    if(ptr != NULL) return true;
    return false;
}

static void WS2812FX_resetSegments() 
{
    WS2812FX_resetSegmentRuntimes();
    memset(_segments, 0, _segments_len * sizeof(Tst_Segment));
    memset(_active_segments, INACTIVE_SEGMENT, _active_segments_len);
    _num_segments = 0;
}

/*
 * 关闭所有功能。
 */
static void WS2812FX_strip_off() 
{
    adafruit_neopixel.clear();
    WS2812FX_show();
}

static void WS2812FX_start()
{
    WS2812FX_resetSegmentRuntimes();
    _running = true;
}

static void WS2812FX_stop()
{
    _running = false;
    WS2812FX_strip_off();
}

static void WS2812FX_pause()
{
    _running = false;
}

static void WS2812FX_resume()
{
    _running = true;
}

static void WS2812FX_trigger()
{
    _triggered = true;
}

/*
 * 将值0到255放入即可获得颜色值.
 * 颜色是从r->g->b->到r的过渡，灵感来自Adafruit示例。
 */
static uint32_t WS2812FX_color_wheel(uint8_t pos) 
{
    pos = 255 - pos;
    if(pos < 85) 
    {
        return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) | (pos * 3);
    }
    else if(pos < 170) 
    {
        pos -= 85;
        return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) | (255 - pos * 3);
    }
    else 
    {
        pos -= 170;
        return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) | (0);
    }
}

/*
 * 返回与位置的最小距离为42的新的随机轮子索引。
 */
static uint8_t WS2812FX_get_random_wheel_index(uint8_t pos) 
{
    uint8_t r = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t d = 0;

    while(d < 42) 
    {
        r = ws2812fx.random8();
        x = abs(pos - r);
        y = 255 - x;
        d = MIN(x, y);
    }

    return r;
}

void WS2812FX_setRandomSeed(uint16_t seed) 
{
    _rand16seed = seed;
}

// 无耻地借用FastLED的快速8位随机数生成器
static uint8_t WS2812FX_random8() 
{
    _rand16seed = (_rand16seed * 2053) + 13849;// 线性同余法随机数生成
    return (uint8_t)((_rand16seed + (_rand16seed >> 8)) & 0xFF);
}

// 随机8(LIM)生成0到(LIM-1)范围内的数字
static uint8_t WS2812FX_random8_lim(uint8_t lim) 
{
    uint8_t r;
    _rand16seed = (_rand16seed * 2053) + 13849;// 线性同余法随机数生成
    r = (uint8_t)((_rand16seed + (_rand16seed >> 8)) & 0xFF);;
    r = ((uint16_t)r * lim) >> 8;
    return r;
}

// 随机16(LIM)生成0到(LIM-1)范围内的数字
uint16_t WS2812FX_random(uint16_t lim) 
{
    uint16_t r ;
    r = (uint16_t)ws2812fx.random8() * 256 + ws2812fx.random8();
    r = ((uint32_t)r * lim) >> 16;
    return r;
}

// 返回所有LED强度的总和(可用于基本功率计算)
static uint32_t WS2812FX_intensitySum(void) 
{
    uint8_t *pixels = adafruit_neopixel.getPixels();
    uint32_t sum = 0;
    for(uint16_t i=0; i <adafruit_neopixel.numBytes; i++) 
    { 
        sum+= pixels[i];
    }
    return sum;
}

// 返回每种颜色强度的总和。
// 请注意，返回数组中的亮度顺序取决于您拥有的WS2812 LED的类型。
// NEO_GRB LED将返回条目顺序与NEO_RGB LED不同的数组。
static uint32_t* WS2812FX_intensitySums(void) 
{
    static uint32_t intensities[] = { 0, 0, 0, 0 };
    memset(intensities, 0, sizeof(intensities));

    uint8_t *pixels = adafruit_neopixel.getPixels();
    uint8_t bytesPerPixel = WS2812FX_getNumBytesPerPixel(); // 3=RGB, 4=RGBW
    for(uint16_t i=0; i <adafruit_neopixel.numBytes; i += bytesPerPixel) 
    {
        intensities[0] += pixels[i];
        intensities[1] += pixels[i + 1];
        intensities[2] += pixels[i + 2];
        if(bytesPerPixel == 4) intensities[3] += pixels[i + 3]; // for RGBW LEDs
    }
    return intensities;
}

/*
 * 自定义模式帮助程序
 */
uint8_t WS2812FX_setCustomMode(uint8_t index, uint16_t (*p)())
{
    if((uint8_t)(FX_MODE_CUSTOM_0 + index) < MODE_COUNT) 
    {
        customModes[index] = p; // 存储自定义模式
        return (FX_MODE_CUSTOM_0 + index);
    }
    return 0;
}

/*
 * 自定义模式显示帮助程序
 */
void WS2812FX_setCustomShow(void (*p)(void)) 
{
    customShow = p;
}

Tst_WS2812FX ws2812fx=
{
    WS2812FX_structure,
    WS2812FX_init,
    WS2812FX_start,
    WS2812FX_stop,
    WS2812FX_pause,
    WS2812FX_resume,
    WS2812FX_strip_off,
    WS2812FX_setMode,
    WS2812FX_setOptions,
    WS2812FX_setCustomShow,// 用户自定义的向WS2812的发送程序
    WS2812FX_setSpeed,
    WS2812FX_increaseSpeed,
    WS2812FX_decreaseSpeed,
    WS2812FX_setColor_rgb,
    WS2812FX_setColor_rgbw,
    WS2812FX_setColor,
    WS2812FX_setColor_seg,
    WS2812FX_setColors,
    WS2812FX_setBrightness,
    WS2812FX_increaseBrightness,
    WS2812FX_decreaseBrightness,
    WS2812FX_setLength,
    WS2812FX_increaseLength,
    WS2812FX_decreaseLength,
    WS2812FX_trigger,
    WS2812FX_setCycle,
    WS2812FX_setNumSegments,
    WS2812FX_setSegment,
    WS2812FX_setSegment_colors,
    WS2812FX_setIdleSegment,
    WS2812FX_setIdleSegment_colors,
    WS2812FX_addActiveSegment,
    WS2812FX_removeActiveSegment,
    WS2812FX_swapActiveSegment,
    WS2812FX_resetSegments,
    WS2812FX_resetSegmentRuntimes,
    WS2812FX_resetSegmentRuntime,
    WS2812FX_setPixelColor,
    WS2812FX_setPixelColor_rgb,
    WS2812FX_setPixelColor_rgbw,
    WS2812FX_copyPixels,
    WS2812FX_setPixels,
    WS2812FX_setRandomSeed,
    WS2812FX_show,
    WS2812FX_service,
    WS2812FX_isRunning,
    WS2812FX_isTriggered,
    WS2812FX_isFrame,
    WS2812FX_isCycle,
    WS2812FX_isActiveSegment,
    WS2812FX_random8,
    WS2812FX_random8_lim,
    WS2812FX_random,
    WS2812FX_getMode,
    WS2812FX_getModeCount,
    WS2812FX_setCustomMode,
    WS2812FX_getNumSegments,
    WS2812FX_get_random_wheel_index,
    WS2812FX_getOptions,
    WS2812FX_getNumBytesPerPixel,
    WS2812FX_getSpeed,
    WS2812FX_getLength,
    WS2812FX_getNumBytes,
    WS2812FX_color_wheel,
    WS2812FX_getColor,
    WS2812FX_getColors,
    WS2812FX_intensitySum,
    WS2812FX_intensitySums,
    WS2812FX_getActiveSegments,
    WS2812FX_getModeName,
    WS2812FX_getSegment_active,
    WS2812FX_getSegment,
    WS2812FX_getSegments,
    WS2812FX_getSegmentRuntime_active,
    WS2812FX_getSegmentRuntime,
    WS2812FX_getSegmentRuntimes,
    // 变量
    NULL,
    NULL,
    0,
};

#endif
