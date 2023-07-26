#ifndef WS2812FX_h
#define WS2812FX_h

#include "emf_typedef.h"
#include "Adafruit_NeoPixel.h"
#include "modes.h"

// #define MAX(x, y) (((x)>(y))?(x):(y))
// #define MIN(x, y) (((x)<(y))?(x):(y))

// 默认参数
#define DEFAULT_BRIGHTNESS (uint8_t)50 // 默认亮度
#define DEFAULT_MODE       (uint8_t)0 // 默认模式
#define DEFAULT_SPEED      (uint16_t)1000 // 默认速度
#define DEFAULT_COLOR      (uint32_t)0xFF0000 //默认颜色
#define DEFAULT_COLORS     { RED, GREEN, BLUE } //默认颜色组

// 速度上下限
#define SPEED_MIN   (uint16_t)2
#define SPEED_MAX   (uint16_t)65535

// 亮度上下限
#define BRIGHTNESS_MIN  (uint8_t)0
#define BRIGHTNESS_MAX  (uint8_t)255

/* 每个段使用36字节SRAM内存，因此如果由于内存不足而导致编译失败，减少MAX_NUM_SEGMENTS可能会有所帮助 */
#define MAX_NUM_SEGMENTS            10 // 最大段数
#define MAX_NUM_ACTIVE_SEGMENTS     10 // 最大活动段数
#define INACTIVE_SEGMENT            255 /* max uint_8 */
#define MAX_NUM_COLORS              3 // 每段的最大原色数

// 一些常见的颜色
#define RED         (uint32_t)0xFF0000
#define GREEN       (uint32_t)0x00FF00
#define BLUE        (uint32_t)0x0000FF
#define WHITE       (uint32_t)0xFFFFFF
#define BLACK       (uint32_t)0x000000
#define YELLOW      (uint32_t)0xFFFF00
#define CYAN        (uint32_t)0x00FFFF
#define MAGENTA     (uint32_t)0xFF00FF
#define PURPLE      (uint32_t)0x400080
#define ORANGE      (uint32_t)0xFF3000
#define PINK        (uint32_t)0xFF1493
#define GRAY        (uint32_t)0x101010
#define ULTRAWHITE  (uint32_t)0xFFFFFFFF
#define DIM(c)      (uint32_t)((c >> 2) & 0x3f3f3f3f) // 强度为25%的颜色
#define DARK(c)     (uint32_t)((c >> 4) & 0x0f0f0f0f) // 强度为6%的颜色

// 段选项
// bit    7: 反转动画
// bits 4-6: 淡入率 (0-7)
// bit    3: 伽马校正
// bits 1-2: 大小
// bits   0: 待定
#define NO_OPTIONS      (uint8_t)0x00 // 无段选项

#define REVERSE         (uint8_t)0x80 // 反转动画
#define IS_REVERSE      ((ws2812fx._seg->options & REVERSE) == REVERSE) // 是否反转

#define FADE_XFAST      (uint8_t)0x10 // 超快淡入率
#define FADE_FAST       (uint8_t)0x20 // 快淡入率
#define FADE_MEDIUM     (uint8_t)0x30 // 中等淡入率
#define FADE_SLOW       (uint8_t)0x40 // 慢淡入率
#define FADE_XSLOW      (uint8_t)0x50 // 超慢淡入率
#define FADE_XXSLOW     (uint8_t)0x60 // 极慢淡入率
#define FADE_GLACIAL    (uint8_t)0x70 // 冰川一样慢
#define FADE_RATE       ((ws2812fx._seg->options >> 4) & 7) // 提取淡入率

#define GAMMA           (uint8_t)0x08 // 伽马校正
#define IS_GAMMA        ((ws2812fx._seg->options & GAMMA) == GAMMA)// 是否伽马校正

#define SIZE_SMALL      (uint8_t)0x00
#define SIZE_MEDIUM     (uint8_t)0x02
#define SIZE_LARGE      (uint8_t)0x04
#define SIZE_XLARGE     (uint8_t)0x06
#define SIZE_OPTION     ((ws2812fx._seg->options >> 1) & 3)// 提取大小

// 段运行时选项(AUX_PARAM2)
#define FRAME           (uint8_t)0x80
#define SET_FRAME       (ws2812fx._seg_rt->aux_param2 |=  FRAME)
#define CLR_FRAME       (ws2812fx._seg_rt->aux_param2 &= ~FRAME)

#define CYCLE           (uint8_t)0x40
#define SET_CYCLE       (ws2812fx._seg_rt->aux_param2 |=  CYCLE)
#define CLR_CYCLE       (ws2812fx._seg_rt->aux_param2 &= ~CYCLE)
#define CLR_FRAME_CYCLE (ws2812fx._seg_rt->aux_param2 &= ~(FRAME | CYCLE))

// 段参数 20 bytes
typedef struct
{
    uint16_t start;
    uint16_t stop;
    uint16_t speed;
    uint8_t  mode;
    uint8_t  options;
    uint32_t colors[MAX_NUM_COLORS];
}Tst_Segment;

// 段运行参数 16 bytes
typedef struct
{
    uint32_t next_time;
    uint32_t counter_mode_step;
    uint32_t counter_mode_call;
    uint8_t aux_param;   // 辅助参数(通常存储COLOR_SHEAR索引)
    uint8_t aux_param2;  // 辅助参数(通常存储按位选项)
    uint16_t aux_param3; // 辅助参数(通常存储段索引)
}Tst_Segment_runtime;

// 类定义
typedef struct
{
    // 函数
    void (*structure)(uint16_t, neoPixelType, uint8_t, uint8_t);
    void (*init)(void);
    void (*start)(void);
    void (*stop)(void);
    void (*pause)(void);
    void (*resume)(void);
    void (*strip_off)(void);
    void (*setMode)(uint8_t, uint8_t);
    void (*setOptions)(uint8_t, uint8_t);
    void (*setCustomShow)(void (*p)(void));// 用户自定义的向WS2812的发送程序
    void (*setSpeed)(uint8_t, uint16_t);
    void (*increaseSpeed)(uint8_t);
    void (*decreaseSpeed)(uint8_t);
    void (*setColor_rgb)(uint8_t, uint8_t, uint8_t);
    void (*setColor_rgbw)(uint8_t, uint8_t, uint8_t, uint8_t);
    void (*setColor)(uint32_t);
    void (*setColor_seg)(uint8_t, uint32_t);
    void (*setColors)(uint8_t, uint32_t*);
    void (*setBrightness)(uint8_t);
    void (*increaseBrightness)(uint8_t);
    void (*decreaseBrightness)(uint8_t);
    void (*setLength)(uint16_t);
    void (*increaseLength)(uint16_t);
    void (*decreaseLength)(uint16_t);
    void (*trigger)(void);
    void (*setCycle)(void);
    void (*setNumSegments)(uint8_t);
    void (*setSegment)(uint8_t, uint16_t, uint16_t, uint8_t, uint32_t, uint16_t, uint8_t);
    void (*setSegment_colors)(uint8_t, uint16_t, uint16_t, uint8_t, uint32_t*, uint16_t, uint8_t);
    void (*setIdleSegment)(uint8_t, uint16_t, uint16_t, uint8_t, uint32_t, uint16_t, uint8_t);
    void (*setIdleSegment_colors)(uint8_t, uint16_t, uint16_t, uint8_t, uint32_t*, uint16_t, uint8_t);
    void (*addActiveSegment)(uint8_t);
    void (*removeActiveSegment)(uint8_t);
    void (*swapActiveSegment)(uint8_t, uint8_t);
    void (*resetSegments)(void);
    void (*resetSegmentRuntimes)(void);
    void (*resetSegmentRuntime)(uint8_t);
    void (*setPixelColor)(uint16_t, uint32_t);
    void (*setPixelColor_rgb)(uint16_t, uint8_t, uint8_t, uint8_t);
    void (*setPixelColor_rgbw)(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t);
    void (*copyPixels)(uint16_t, uint16_t, uint16_t);
    void (*setPixels)(uint16_t, uint8_t*);
    void (*setRandomSeed)(uint16_t);
    void (*show)(void);
    bool (*service)(void);
    bool (*isRunning)(void);
    bool (*isTriggered)(void);
    bool (*isFrame)(uint8_t);
    bool (*isCycle)(uint8_t);
    bool (*isActiveSegment)(uint8_t);
    uint8_t (*random8)(void);
    uint8_t (*random8_lim)(uint8_t);
    uint16_t (*random16)(uint16_t);
    uint8_t (*getMode)(uint8_t);
    uint8_t (*getModeCount)(void);
    uint8_t (*setCustomMode)(uint8_t, uint16_t (*p)(void));
    uint8_t (*getNumSegments)(void);
    uint8_t (*get_random_wheel_index)(uint8_t);
    uint8_t (*getOptions)(uint8_t);
    uint8_t (*getNumBytesPerPixel)(void);
    uint16_t (*getSpeed)(uint8_t);
    uint16_t (*getLength)(void);
    uint16_t (*getNumBytes)(void);
    uint32_t (*color_wheel)(uint8_t);
    uint32_t (*getColor)(uint8_t);
    uint32_t* (*getColors)(uint8_t);
    uint32_t (*intensitySum)(void);
    uint32_t* (*intensitySums)(void);
    uint8_t* (*getActiveSegments)(void);
    const char* (*getModeName)(uint8_t);
    Tst_Segment* (*getSegment_ative)(void);
    Tst_Segment* (*getSegment)(uint8_t);
    Tst_Segment* (*getSegments)(void);
    Tst_Segment_runtime* (*getSegmentRuntime_active)(void);
    Tst_Segment_runtime* (*getSegmentRuntime)(uint8_t);
    Tst_Segment_runtime* (*getSegmentRuntimes)(void);
    
    // 变量
    Tst_Segment* _seg;// 当前活动的段(20字节)
    Tst_Segment_runtime* _seg_rt;// 当前活动的段运行参数(16字节)
    uint16_t _seg_len;// 当前活动的段中的LED数量
}Tst_WS2812FX;

// WS2812FX 内置效果功能函数
typedef struct
{
    uint8_t* (*blend)(uint8_t*, uint8_t*, uint8_t*, uint16_t, uint8_t);
    uint16_t (*blink)(uint32_t, uint32_t, bool);
    uint16_t (*chase)(uint32_t, uint32_t, uint32_t);
    uint16_t (*chase_flash)(uint32_t, uint32_t);
    uint32_t (*color_blend)(uint32_t, uint32_t, uint8_t);
    uint16_t (*color_wipe)(uint32_t, uint32_t, bool);
    void (*fade_out)(uint32_t);
    uint16_t (*fire_flicker)(int);
    uint16_t (*fireworks)(uint32_t);
    uint16_t (*running)(uint32_t, uint32_t);
    uint16_t (*scan)(uint32_t, uint32_t, bool);
    uint16_t (*sparkle)(uint32_t, uint32_t);
    uint16_t (*tricolor_chase)(uint32_t, uint32_t, uint32_t);
    uint16_t (*twinkle)(uint32_t, uint32_t);
    uint16_t (*twinkle_fade)(uint32_t);
}Tst_Method;

extern Tst_WS2812FX ws2812fx;
extern Tst_Method ws2812fx_method;
#endif
