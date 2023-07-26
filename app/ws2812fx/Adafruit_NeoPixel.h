#ifndef ADAFRUIT_NEOPIXEL_H
#define ADAFRUIT_NEOPIXEL_H

#include "api/api_gpio.h"
#include "emf_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

// NeoPixel数据流中的原色顺序可能会因设备类型、制造商甚至同一产品的不同版本而有所不同。
// Adafruit_NeoPixel构造函数的第三个参数编码数据流中红色、绿色和蓝色基色(如果存在，还包括白色)的
// 每像素字节偏移量--以下#define为每个排列提供了一个更易于使用的命名版本。
// 例如，NEO_GRB表示与NeoPixel兼容的设备期望每个像素三个字节，传输的第一个字节包含绿色值，第二个包含红色，第三个包含蓝色。
// NeoPixels链的内存表示与数据流顺序相同；向链发布数据时不需要重新排序字节。
// 这些值中的大多数不会存在于现实世界的设备中，但它是以这种方式完成的，
// 所以我们已经做好了准备(此外，如果使用WS2811驱动程序IC，可能会以任何奇怪的排列设置它们的像素)。

// 位5、4是从像素的第一个字节到红色字节位置的偏移量(0-3)。
// 位3，2是绿色偏移量，1，0是蓝色偏移量。
// 如果是RGBW型器件(除R、G、B外还支持白色原色)，则位7、6为白字节的偏移量。
// 否则，位7、6被设置为与5、4(红色)相同的值，以指示RGB(非RGBW)器件。
// 即二进制表示法：
// 0bWWRRGGBB for RGBW devices
// 0bRRRRGGBB for RGB

// RGB NeoPixel 排列; 白色和红色的偏移量始终相同
// Offset:        W          R          G          B
#define NEO_RGB ((0 << 6) | (0 << 4) | (1 << 2) | (2)) ///< Transmit as R,G,B
#define NEO_RBG ((0 << 6) | (0 << 4) | (2 << 2) | (1)) ///< Transmit as R,B,G
#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2)) ///< Transmit as G,R,B
#define NEO_GBR ((2 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,R
#define NEO_BRG ((1 << 6) | (1 << 4) | (2 << 2) | (0)) ///< Transmit as B,R,G
#define NEO_BGR ((2 << 6) | (2 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,R

// RGBW NeoPixel 排列; 所有4个偏移量都是不同的
// Offset:         W          R          G          B
#define NEO_WRGB ((0 << 6) | (1 << 4) | (2 << 2) | (3)) ///< Transmit as W,R,G,B
#define NEO_WRBG ((0 << 6) | (1 << 4) | (3 << 2) | (2)) ///< Transmit as W,R,B,G
#define NEO_WGRB ((0 << 6) | (2 << 4) | (1 << 2) | (3)) ///< Transmit as W,G,R,B
#define NEO_WGBR ((0 << 6) | (3 << 4) | (1 << 2) | (2)) ///< Transmit as W,G,B,R
#define NEO_WBRG ((0 << 6) | (2 << 4) | (3 << 2) | (1)) ///< Transmit as W,B,R,G
#define NEO_WBGR ((0 << 6) | (3 << 4) | (2 << 2) | (1)) ///< Transmit as W,B,G,R

#define NEO_RWGB ((1 << 6) | (0 << 4) | (2 << 2) | (3)) ///< Transmit as R,W,G,B
#define NEO_RWBG ((1 << 6) | (0 << 4) | (3 << 2) | (2)) ///< Transmit as R,W,B,G
#define NEO_RGWB ((2 << 6) | (0 << 4) | (1 << 2) | (3)) ///< Transmit as R,G,W,B
#define NEO_RGBW ((3 << 6) | (0 << 4) | (1 << 2) | (2)) ///< Transmit as R,G,B,W
#define NEO_RBWG ((2 << 6) | (0 << 4) | (3 << 2) | (1)) ///< Transmit as R,B,W,G
#define NEO_RBGW ((3 << 6) | (0 << 4) | (2 << 2) | (1)) ///< Transmit as R,B,G,W

#define NEO_GWRB ((1 << 6) | (2 << 4) | (0 << 2) | (3)) ///< Transmit as G,W,R,B
#define NEO_GWBR ((1 << 6) | (3 << 4) | (0 << 2) | (2)) ///< Transmit as G,W,B,R
#define NEO_GRWB ((2 << 6) | (1 << 4) | (0 << 2) | (3)) ///< Transmit as G,R,W,B
#define NEO_GRBW ((3 << 6) | (1 << 4) | (0 << 2) | (2)) ///< Transmit as G,R,B,W
#define NEO_GBWR ((2 << 6) | (3 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,W,R
#define NEO_GBRW ((3 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,R,W

#define NEO_BWRG ((1 << 6) | (2 << 4) | (3 << 2) | (0)) ///< Transmit as B,W,R,G
#define NEO_BWGR ((1 << 6) | (3 << 4) | (2 << 2) | (0)) ///< Transmit as B,W,G,R
#define NEO_BRWG ((2 << 6) | (1 << 4) | (3 << 2) | (0)) ///< Transmit as B,R,W,G
#define NEO_BRGW ((3 << 6) | (1 << 4) | (2 << 2) | (0)) ///< Transmit as B,R,G,W
#define NEO_BGWR ((2 << 6) | (3 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,W,R
#define NEO_BGRW ((3 << 6) | (2 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,R,W

// 不支持400KHz通信速率的设备
#define NEO_KHZ800 0x0000 ///< 800 KHz data transmission

typedef uint8_t neoPixelType; ///< 3rd arg to Adafruit_NeoPixel constructor

/*!
    @brief  Adafruit NeoPixels功能函数使能开关
*/
#define EN_setPixelColor_rgb    0
#define EN_setPixelColor_rgbw   1
#define EN_setPixelColor        1
#define EN_fill                 1
#define EN_ColorHSV             1
#define EN_getPixelColor        1
#define EN_getBrightness        0
#define EN_clear                0
#define EN_rainbow              0
#define EN_numPixels            1
#define EN_getPixels            1
#define EN_Color_rgb            1
#define EN_Color_rgbw           0
/*!
    @brief  Adafruit NeoPixels内部功能函数使能开关
*/
#define EN_gamma8               1
#define EN_gamma32              0
#define EN_sin8                 1

/*!
    @brief  类型定义存储与Adafruit NeoPixels和兼容设备交互的状态和函数的结构体
*/
typedef struct
{
    // 功能函数
    void (*begin)(void);// 配置用于输出的NeoPixel管脚
    void (*updateLength)(uint16_t);
    void (*updateType)(neoPixelType);
    void (*show)(void);
    void (*setPixelColor_rgb)(uint16_t, uint8_t, uint8_t, uint8_t);
    void (*setPixelColor_rgbw)(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t);
    void (*setPixelColor)(uint16_t, uint32_t);
    void (*fill)(uint32_t, uint16_t, uint16_t);
    uint32_t (*ColorHSV)(uint16_t, uint8_t, uint8_t);
    uint32_t (*getPixelColor)(uint16_t);
    void (*setBrightness)(uint8_t);
    uint8_t (*getBrightness)(void);
    void (*clear)(void);
    void (*rainbow)(uint16_t, int8_t, uint8_t, uint8_t, bool);
    uint16_t (*numPixels)(void);
    uint8_t *(*getPixels)(void);
    uint32_t (*Color_rgb)(uint8_t, uint8_t, uint8_t);
    uint32_t (*Color_rgbw)(uint8_t, uint8_t, uint8_t, uint8_t);
    void (*init)(uint16_t, neoPixelType);
    void (*init_empty)(void);
    void (*deinit)(void);
    uint8_t (*gamma8)(uint8_t);
    uint32_t (*gamma32)(uint32_t);
    uint8_t (*sine8)(uint8_t);
    
    // 硬件接口
    void (*NeoPixel_op)(uint8_t *,uint16_t);
    
    // 变量
    bool begun;         ///< 如果先前调用了Begin()，则为True
    uint16_t numLEDs;   ///< 条带中的RGB LED数量
    uint16_t numBytes;  ///< 下面‘Pixels’缓冲区的大小
    uint8_t brightness; ///< 条带亮度0-255(存储为+1)
    uint8_t *pixels;    ///< 保存LED颜色值(每个值3或4个字节)
    uint8_t rOffset;    ///< 每个3字节或4字节像素内的红色字节的索引
    uint8_t gOffset;    ///< 绿色字节的索引
    uint8_t bOffset;    ///< 蓝色字节的索引
    uint8_t wOffset;    ///< 蓝色字节的索引 (如果没有白色，则等于rOffset)
}Tst_Adafruit_NeoPixel;

extern Tst_Adafruit_NeoPixel adafruit_neopixel;

#ifdef __cplusplus
}
#endif

#endif // ADAFRUIT_NEOPIXEL_H
