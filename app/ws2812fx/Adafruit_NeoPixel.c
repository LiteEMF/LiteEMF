#include  "hw_config.h"
#if APP_WS2812FX_ENABLE
#include "Adafruit_NeoPixel.h"
#include "utils/mem/emf_mem.h"


// 这两个表是在Adafruit_NeoPixel类之外声明的，因为一些主板可能需要不处理C++11 conexpr关键字的老式编译器。
#if EN_sin8==1
/* 包含8位无符号正弦波(0-255)的PROGMEM(闪存内存)表.
   将此代码段复制并粘贴到Python REPL中以重新生成:
    import math
    for x in range(256):
        print("{:3},".format(int((math.sin(x/128.0*math.pi)+1.0)*127.5+0.5))),
        if x&15 == 15: print
*/
static const uint8_t _NeoPixelSineTable[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170,
    173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211,
    213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240,
    241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254,
    254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251,
    250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237, 235, 234, 232,
    230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 208, 206, 203, 201, 198,
    196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165, 162, 158, 155,
    152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109,
    106, 103, 100, 97,  93,  90,  88,  85,  82,  79,  76,  73,  70,  67,  65,
    62,  59,  57,  54,  52,  49,  47,  44,  42,  40,  37,  35,  33,  31,  29,
    27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,  10,  9,   7,   6,
    5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,   0,   0,   0,
    0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,   10,  11,
    12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,  37,
    40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,  73,  76,
    79,  82,  85,  88,  90,  93,  97,  100, 103, 106, 109, 112, 115, 118, 121,
    124};
#endif

#if (EN_gamma8==1) || (EN_gamma32==1)
/* 8位伽马校正表
   将此代码段复制并粘贴到Python REPL中以重新生成:
    import math
    gamma=2.6
    for x in range(256):
        print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
        if x&15 == 15: print
*/
static const uint8_t _NeoPixelGammaTable[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,
    3,   3,   3,   3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,
    6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,
    11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,  17,
    17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  34,  34,  35,
    36,  37,  38,  38,  39,  40,  41,  42,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  68,  69,  70,  71,  72,  73,  75,  76,  77,  78,  80,  81,
    82,  84,  85,  86,  88,  89,  90,  92,  93,  94,  96,  97,  99,  100, 102,
    103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 124, 125,
    127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148, 150, 152,
    154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182,
    184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252,
    255};
#endif

/*!
  @brief   配置用于输出的NeoPixel管脚.
*/
static void Adafruit_NeoPixel_begin(void)
{
    adafruit_neopixel.begun = true;
}

/*!
  @brief   更改先前声明的Adafruit_NeoPixel条带对象的长度。
           旧数据被释放，新数据被清除。
           引脚编号和像素格式不变。
  @param   n  新的条带长度，以像素为单位。
  @note    此函数已弃用，此处仅适用于可能仍在调用它的旧项目。
           新项目应该改用带有第一个构造函数语法(length, pin,type)的‘new’关键字。
*/
static void Adafruit_NeoPixel_updateLength(uint16_t n)
{
    my_free(adafruit_neopixel.pixels); // 释放现有数据 (if any)

    // 分配新数据--注意：所有像素都已清除
    adafruit_neopixel.numBytes = n * ((adafruit_neopixel.wOffset == adafruit_neopixel.rOffset) ? 3 : 4);
    adafruit_neopixel.pixels = (uint8_t *)emf_malloc(adafruit_neopixel.numBytes);
    if (adafruit_neopixel.pixels)
    {
        memset(adafruit_neopixel.pixels, 0, adafruit_neopixel.numBytes);
        adafruit_neopixel.numLEDs = n;
    }
    else
    {
        adafruit_neopixel.numLEDs = adafruit_neopixel.numBytes = 0;
    }
}

/*!
  @brief   更改先前声明的Adafruit_NeoPixel条带对象的像素格式。
           如果格式从RGB变量之一更改为RGBW变量(或RGBW到RGB)，则旧数据将被释放，新数据将被清除。
           否则，旧数据将保留在RAM中，并且不会重新排序为新格式，因此建议使用Clear()进行后续处理。
  @param   t  Pixel type -- 将Adafruit_NeoPixel.h中定义的neo_*常量加在一起，
           例如，对于NeoPixels，NEO_GRB+NEO_KHZ800需要800 kHz(Vs 400 KHz)数据流，
           颜色字节以每像素的绿色、红色、蓝色顺序表示。
  @note    此函数已弃用，此处仅适用于可能仍在调用它的旧项目。
           新项目应该改用带有第一个构造函数语法(length, pin, type)的‘new’关键字。
*/
static void Adafruit_NeoPixel_updateType(neoPixelType t)
{
    bool oldThreeBytesPerPixel = (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset); // false if RGBW

    adafruit_neopixel.wOffset = (t >> 6) & 0x03; // See notes in header file
    adafruit_neopixel.rOffset = (t >> 4) & 0x03; // regarding R/G/B/W offsets
    adafruit_neopixel.gOffset = (t >> 2) & 0x03;
    adafruit_neopixel.bOffset = t & 0x03;

    // 如果每像素字节数已更改(并且先前已分配像素数据)，则重新分配到新的大小。将清除所有数据。
    if (adafruit_neopixel.pixels)
    {
        bool newThreeBytesPerPixel = (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset);
        if (newThreeBytesPerPixel != oldThreeBytesPerPixel)
            Adafruit_NeoPixel_updateLength(adafruit_neopixel.numLEDs);
    }
}

/*!
  @brief   将RAM中的像素数据传输到NeoPixels。
  @note    在大多数架构上，中断被暂时禁用，以实现正确的NeoPixel信号时序。
           这意味着需要中断的Arduino Millis()和micros()函数在每次调用该函数时
           都会丢失很小的时间间隔(每RGB像素大约30微秒，RGBW像素大约40微秒)。
           这个问题没有简单的解决办法，但存在一些专门的替代或配套的库，
           它们使用非常特定于设备的外围设备来解决它。
*/
static void Adafruit_NeoPixel_show(void)
{

    if ((!adafruit_neopixel.pixels) || (!adafruit_neopixel.NeoPixel_op))
        return;

    adafruit_neopixel.NeoPixel_op(adafruit_neopixel.pixels,adafruit_neopixel.numBytes);
}

#if EN_setPixelColor_rgb==1
/*!
  @brief   使用单独的红色、绿色和蓝色分量设置像素的颜色。如果使用RGBW像素，则白色将设置为0。
  @param   n  Pixel index, starting from 0.
  @param   r  Red brightness, 0 = minimum (off), 255 = maximum.
  @param   g  Green brightness, 0 = minimum (off), 255 = maximum.
  @param   b  Blue brightness, 0 = minimum (off), 255 = maximum.
*/
static void Adafruit_NeoPixel_setPixelColor_rgb(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t *p;
    
    if (n < adafruit_neopixel.numLEDs)
    {
        if (adafruit_neopixel.brightness)// 请参阅setBright()中的注释
        {
            r = (r * adafruit_neopixel.brightness) >> 8;
            g = (g * adafruit_neopixel.brightness) >> 8;
            b = (b * adafruit_neopixel.brightness) >> 8;
        }
        
        if (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset)// 是RGB类型的条带
        {
            p = &adafruit_neopixel.pixels[n * 3];// 每像素3个字节
        }
        else// 是WRGB类型的条带
        {
            p = &adafruit_neopixel.pixels[n * 4];// 每像素4字节
            p[adafruit_neopixel.wOffset] = 0;// But only R,G,B passed -- set W to 0
        }
        p[adafruit_neopixel.rOffset] = r; // R,G,B 始终存储
        p[adafruit_neopixel.gOffset] = g;
        p[adafruit_neopixel.bOffset] = b;
    }
}
#endif

#if EN_setPixelColor_rgbw==1
/*!
  @brief   使用单独的红、绿、蓝、白分量设置像素的颜色(仅适用于RGBW NeoPixels)。
  @param   n  Pixel index, starting from 0.
  @param   r  Red brightness, 0 = minimum (off), 255 = maximum.
  @param   g  Green brightness, 0 = minimum (off), 255 = maximum.
  @param   b  Blue brightness, 0 = minimum (off), 255 = maximum.
  @param   w  White brightness, 0 = minimum (off), 255 = maximum, ignored
              if using RGB pixels.
*/
static void Adafruit_NeoPixel_setPixelColor_rgbw(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    uint8_t *p;
    
    if (n < adafruit_neopixel.numLEDs)
    {
        if (adafruit_neopixel.brightness)// 请参阅setBright()中的注释
        {
            r = (r * adafruit_neopixel.brightness) >> 8;
            g = (g * adafruit_neopixel.brightness) >> 8;
            b = (b * adafruit_neopixel.brightness) >> 8;
            w = (w * adafruit_neopixel.brightness) >> 8;
        }
        
        if (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset)// 是RGB类型的条带
        {
            p = &adafruit_neopixel.pixels[n * 3];// 每像素3个字节
        }
        else// 是WRGB类型的条带
        {
            p = &adafruit_neopixel.pixels[n * 4];// 每像素4字节
            p[adafruit_neopixel.wOffset] = w; // Store W
        }
        p[adafruit_neopixel.rOffset] = r; // Store R,G,B
        p[adafruit_neopixel.gOffset] = g;
        p[adafruit_neopixel.bOffset] = b;
    }
}
#endif

#if EN_setPixelColor==1
/*!
  @brief   使用32位RGB或RGBW值设置像素的颜色。
  @param   n  Pixel index, starting from 0.
  @param   c  32-bit color value. Most significant byte is white (for RGBW
              pixels) or ignored (for RGB pixels), next is red, then green,
              and least significant byte is blue.
*/
static void Adafruit_NeoPixel_setPixelColor(uint16_t n, uint32_t c)
{
    uint8_t w;
    uint8_t *p, r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
    
    if (n < adafruit_neopixel.numLEDs)
    {
        if (adafruit_neopixel.brightness)// 请参阅setBright()中的注释
        {
            r = (r * adafruit_neopixel.brightness) >> 8;
            g = (g * adafruit_neopixel.brightness) >> 8;
            b = (b * adafruit_neopixel.brightness) >> 8;
        }
        if (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset)
        {
            p = &adafruit_neopixel.pixels[n * 3];
        }
        else
        {
            p = &adafruit_neopixel.pixels[n * 4];
            w = (uint8_t)(c >> 24);
            p[adafruit_neopixel.wOffset] = adafruit_neopixel.brightness ? ((w * adafruit_neopixel.brightness) >> 8) : w;
        }
        p[adafruit_neopixel.rOffset] = r;
        p[adafruit_neopixel.gOffset] = g;
        p[adafruit_neopixel.bOffset] = b;
    }
}
#endif

#if EN_fill==1
/*!
  @brief   用颜色填充全部或部分NeoPixel条带。
  @param   c      32-bit color value. Most significant byte is white (for
                  RGBW pixels) or ignored (for RGB pixels), next is red,
                  then green, and least significant byte is blue. If all
                  arguments are unspecified, this will be 0 (off).
  @param   first  Index of first pixel to fill, starting from 0. Must be
                  in-bounds, no clipping is performed. 0 if unspecified.
  @param   count  Number of pixels to fill, as a positive value. Passing
                  0 or leaving unspecified will fill to end of strip.
*/
static void Adafruit_NeoPixel_fill(uint32_t c, uint16_t first, uint16_t count)
{
    uint16_t i, end;

    if (first >= adafruit_neopixel.numLEDs)
    {
        return; // 如果第一个LED超过条带末端，则无需执行任何操作
    }

    // 计算要填充的最后一个像素之后的索引
    if (count == 0)
    {
        end = adafruit_neopixel.numLEDs;//填充至条带末尾
    }
    else
    {
        end = first + count;// 确保循环不会超过最后一个像素
        if (end > adafruit_neopixel.numLEDs)
            end = adafruit_neopixel.numLEDs;
    }

    for (i = first; i < end; i++)
    {
        Adafruit_NeoPixel_setPixelColor(i, c);
    }
}
#endif

#if EN_ColorHSV==1
/*!
  @brief   将色调、饱和度和值转换为打包的32位RGB颜色，该颜色可以传递给setPixelColor()或其他与RGB兼容的函数。
  @param   hue  An unsigned 16-bit value, 0 to 65535, representing one full
                loop of the color wheel, which allows 16-bit hues to "roll
                over" while still doing the expected thing (and allowing
                more precision than the wheel() function that was common to
                prior NeoPixel examples).
  @param   sat  Saturation, 8-bit value, 0 (min or pure grayscale) to 255
                (max or pure hue). Default of 255 if unspecified.
  @param   val  Value (brightness), 8-bit value, 0 (min / black / off) to
                255 (max or full brightness). Default of 255 if unspecified.
  @return  Packed 32-bit RGB with the most significant byte set to 0 -- the
           white element of WRGB pixels is NOT utilized. Result is linearly
           but not perceptually correct, so you may want to pass the result
           through the gamma32() function (or your own gamma-correction
           operation) else colors may appear washed out. This is not done
           automatically by this function because coders may desire a more
           refined gamma-correction function than the simplified
           one-size-fits-all operation of gamma32(). Diffusing the LEDs also
           really seems to help when using low-saturation colors.
*/
static uint32_t Adafruit_NeoPixel_ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
    uint8_t r, g, b;
    uint32_t v1;
    uint16_t s1;
    uint8_t s2;
  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;
  // Because red is centered on the rollover point (the +32768 above,
  // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
  // where 0 and 1530 would yield the same thing. Rather than apply a
  // costly modulo operator, 1530 is handled as a special case below.

  // So you'd think that the color "hexcone" (the thing that ramps from
  // pure red, to pure yellow, to pure green and so forth back to red,
  // yielding six slices), and with each color component having 256
  // possible values (0-255), might have 1536 possible items (6*256),
  // but in reality there's 1530. This is because the last element in
  // each 256-element slice is equal to the first element of the next
  // slice, and keeping those in there this would create small
  // discontinuities in the color wheel. So the last element of each
  // slice is dropped...we regard only elements 0-254, with item 255
  // being picked up as element 0 of the next slice. Like this:
  // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
  // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
  // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
  // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
  // the constants below are not the multiples of 256 you might expect.

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510)// Red to Green-1
    {
        b = 0;
        if (hue < 255)//  Red to Yellow-1
        {
            r = 255;
            g = hue;// g = 0 to 254
        }
        else//  Yellow to Green-1
        {
            r = 510 - hue; // r = 255 to 1
            g = 255;
        }
    }
    else if (hue < 1020)// Green to Blue-1
    {
        r = 0;
        if (hue < 765)// Green to Cyan-1
        {
            g = 255;
            b = hue - 510;  // b = 0 to 254
        }
        else// Cyan to Blue-1
        {
            g = 1020 - hue; // g = 255 to 1
            b = 255;
        }
    }
    else if (hue < 1530)// Blue to Red-1
    {
        g = 0;
        if (hue < 1275)// Blue to Magenta-1
        {
            r = hue - 1020; // r = 0 to 254
            b = 255;
        }
        else//   Magenta to Red-1
        {
            r = 255;
            b = 1530 - hue; // b = 255 to 1
        }
    }
    else// Last 0.5 Red (quicker than % operator)
    {
        r = 255;
        g = b = 0;
    }

    // 将饱和度和值应用于R、G、B，打包为32位结果：
    v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    s1 = 1 + sat;  // 1 to 256; same reason
    s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
           (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
           (((((b * s1) >> 8) + s2) * v1) >> 8);
}
#endif

#if EN_getPixelColor==1
/*!
  @brief   查询先前设置的像素的颜色。
  @param   n  Index of pixel to read (0 = first).
  @return  'Packed' 32-bit RGB or WRGB value. Most significant byte is white
           (for RGBW pixels) or 0 (for RGB pixels), next is red, then green,
           and least significant byte is blue.
  @note    If the strip brightness has been changed from the default value
           of 255, the color read from a pixel may not exactly match what
           was previously written with one of the setPixelColor() functions.
           This gets more pronounced at lower brightness levels.
*/
static uint32_t Adafruit_NeoPixel_getPixelColor(uint16_t n)
{
    uint8_t *p;
    
    if (n >= adafruit_neopixel.numLEDs)
        return 0; // Out of bounds, return no color.
    
    if (adafruit_neopixel.wOffset == adafruit_neopixel.rOffset)// Is RGB-type device
    {
        p = &adafruit_neopixel.pixels[n * 3];
        if (adafruit_neopixel.brightness)
        {
      // 存储的颜色由setBright()抽取。
      // 返回值试图缩小到设置像素颜色时使用的原始24位值的近似值，但总是会出现一些错误--这些位完全消失了。
      // 问题在低亮度级别时最为明显。
        return (((uint32_t)(p[adafruit_neopixel.rOffset] << 8) / adafruit_neopixel.brightness) << 16) |
               (((uint32_t)(p[adafruit_neopixel.gOffset] << 8) / adafruit_neopixel.brightness) << 8) |
               ((uint32_t)(p[adafruit_neopixel.bOffset] << 8) / adafruit_neopixel.brightness);
        }
        else// 未进行亮度调整--返回‘RAW’颜色
        {
            return ((uint32_t)p[adafruit_neopixel.rOffset] << 16) | ((uint32_t)p[adafruit_neopixel.gOffset] << 8) |
                    (uint32_t)p[adafruit_neopixel.bOffset];
        }
    }
    else// Is RGBW-type device
    {
        p = &adafruit_neopixel.pixels[n * 4];
        if (adafruit_neopixel.brightness)// 返回缩放颜色
        {
            return (((uint32_t)(p[adafruit_neopixel.wOffset] << 8) / adafruit_neopixel.brightness) << 24) |
                    (((uint32_t)(p[adafruit_neopixel.rOffset] << 8) / adafruit_neopixel.brightness) << 16) |
                    (((uint32_t)(p[adafruit_neopixel.gOffset] << 8) / adafruit_neopixel.brightness) << 8) |
                    ((uint32_t)(p[adafruit_neopixel.bOffset] << 8) / adafruit_neopixel.brightness);
        }
        else// 返回原始颜色
        {
            return ((uint32_t)p[adafruit_neopixel.wOffset] << 24) | ((uint32_t)p[adafruit_neopixel.rOffset] << 16) |
                   ((uint32_t)p[adafruit_neopixel.gOffset] << 8) | (uint32_t)p[adafruit_neopixel.bOffset];
        }
    }
}
#endif

/*!
  @brief   Adjust output brightness. Does not immediately affect what's
           currently displayed on the LEDs. The next call to show() will
           refresh the LEDs at this level.
  @param   b  Brightness setting, 0=minimum (off), 255=brightest.
  @note    This was intended for one-time use in one's setup() function,
           not as an animation effect in itself. Because of the way this
           library "pre-multiplies" LED colors in RAM, changing the
           brightness is often a "lossy" operation -- what you write to
           pixels isn't necessary the same as what you'll read back.
           Repeated brightness changes using this function exacerbate the
           problem. Smart programs therefore treat the strip as a
           write-only resource, maintaining their own state to render each
           frame of an animation, not relying on read-modify-write.
*/
static void Adafruit_NeoPixel_setBrightness(uint8_t b)
{
    uint8_t c, *ptr = adafruit_neopixel.pixels, oldBrightness = adafruit_neopixel.brightness - 1; // 对旧亮度值进行解包
    uint16_t scale;
    uint16_t i;
    
    // 存储的亮度值与传递的值不同。
    // 这简化了以后的实际缩放计算，允许进行快速的8x8位乘法并取MSB。
    // ‘Brightness’是uint8_t，在这里加1可能会(故意)翻转...
    // 所以0=最大亮度(颜色值按字面意思解释；没有缩放比例)，1=最小亮度(关)，255=略低于最大亮度。
    uint8_t newBrightness = b + 1;
    if (newBrightness != adafruit_neopixel.brightness)
    {
        // 与之前的值相比，亮度发生了变化--重新调整RAM中现有数据的比例，这一过程可能会有“损耗”，特别是在增加亮度时。
        // WS2811/WS2812代码中的时间紧意味着没有足够的空闲周期来在数据发布时动态执行这种缩放。
        // 因此，我们对RAM中现有的颜色数据进行遍历并对其进行缩放(后续图形命令也可在此亮度级别下工作)。
        // 如果亮度有了显著的提高，那么旧数据中有限的步数(量化)在重新缩放的版本中将非常明显。
        // 对于非破坏性更改，需要重新呈现完整的条带数据。 C'est la vie.
        if (oldBrightness == 0)
            scale = 0; // 避免/0
        else if (b == 255)
            scale = 65535 / oldBrightness;
        else
            scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;

        for (i = 0; i < adafruit_neopixel.numBytes; i++)
        {
            c = *ptr;
            *ptr++ = (c * scale) >> 8;
        }
        adafruit_neopixel.brightness = newBrightness;
    }
}

#if EN_getBrightness==1
/*!
  @brief   检索上一次设置的条带亮度值。
  @return  Brightness value: 0 = minimum (off), 255 = maximum.
*/
static uint8_t Adafruit_NeoPixel_getBrightness(void)
{
    return adafruit_neopixel.brightness - 1;
}
#endif

#if EN_clear==1
/*!
  @brief   使用0/黑色/禁用填充整个新像素条带。
*/
static void Adafruit_NeoPixel_clear(void)
{
    memset(adafruit_neopixel.pixels, 0, adafruit_neopixel.numBytes);
}
#endif

#if (EN_gamma8==1) || (EN_gamma32==1)
/*!
@brief   用于基本像素亮度调整的8位伽马校正功能。使颜色过渡在视觉上看起来更正确。
@param   X输入亮度，0(最小或关/黑)到255(最大)。
@return  然后可以将Gamma调整后的亮度传递给其中一个setPixelColor()函数。
         这使用固定的Gamma校正指数2.6，这对于平均任务中的平均新像素似乎是合理的。
         如果你需要更精细的控制，你需要提供你自己的伽马校正功能。
*/
static uint8_t Adafruit_NeoPixel_gamma8(uint8_t x)
{
    return (_NeoPixelGammaTable[x]); // 0-255 in, 0-255 out
}
#endif

#if EN_gamma32==1
/*!
@brief   适用于32位打包RGB或WRGB颜色的Gamma校正函数。使颜色过渡在视觉上看起来更正确。
@param   x  32-bit packed RGB or WRGB color.
@return  Gamma调整后的填充颜色，然后可以在setPixelColor()函数之一中传递。
         与Gamma8()一样，它使用固定的Gamma校正指数2.6，对于平均任务中的平均NeoPixels似乎还可以。
         如果你需要更精细的控制，你需要提供你自己的伽马校正功能。
*/
static uint32_t Adafruit_NeoPixel_gamma32(uint32_t x)
{
    uint8_t *y = (uint8_t *)&x;
    uint8_t i
    // All four bytes of a 32-bit value are filtered even if RGB (not WRGB),
    // to avoid a bunch of shifting and masking that would be necessary for
    // properly handling different endianisms (and each byte is a fairly
    // trivial operation, so it might not even be wasting cycles vs a check
    // and branch for the RGB case). In theory this might cause trouble *if*
    // someone's storing information in the unused most significant byte
    // of an RGB value, but this seems exceedingly rare and if it's
    // encountered in reality they can mask values going in or coming out.
    for (i = 0; i < 4; i++)
        y[i] = Adafruit_NeoPixel_gamma8(y[i]);
    return x; // Packed 32-bit return
}
#endif

#if EN_rainbow==1
/*!
  @brief    用一个或多个色调循环填充NeoPixel条带。
           Everyone loves the rainbow swirl so much, now it's canon!
  @param   first_hue   第一个像素的色调，0-65535，表示色轮的一个完整周期。
                       每个后续像素将被偏移以在条带的长度上完成一个或多个循环。
  @param   reps        色轮在条带长度上的循环次数。默认值为1。负值可用于反转色调顺序。
  @param   saturation  饱和度(可选)，0-255=灰色到纯色调，默认为255。
  @param   brightness  Brightness/value (optional), 0-255 = off to max,
                       default = 255. This is distinct and in combination
                       with any configured global strip brightness.
  @param   gammify     If true (default), apply gamma correction to colors
                       for better appearance.
*/
static void Adafruit_NeoPixel_rainbow(uint16_t first_hue, int8_t reps, uint8_t saturation, uint8_t brightness, bool gammify)
{
    uint16_t i;
    for (i=0; i<adafruit_neopixel.numLEDs; i++)
    {
        uint16_t hue = first_hue + (i * reps * 65536) / adafruit_neopixel.numLEDs;
        uint32_t color = Adafruit_NeoPixel_ColorHSV(hue, saturation, brightness);
        if (gammify) color = Adafruit_NeoPixel_gamma32(color);
        Adafruit_NeoPixel_setPixelColor(i, color);
    }
}
#endif

#if EN_numPixels==1
/*!
@brief   返回Adafruit_NeoPixel条带对象中的像素数。
@return  像素计数(如果未设置，则为0)。
*/
uint16_t Adafruit_NeoPixel_numPixels(void)
{
    return adafruit_neopixel.numLEDs;
}
#endif

#if EN_getPixels==1
/*!
@brief   获取一个直接指向RAM中的NeoPixel数据缓冲区的指针。
         像素数据以设备本机格式存储(就像neo_*常量一样)，这里不进行转换。
         访问此缓冲区的应用程序需要知道特定的数据格式并适当地处理颜色。
@return  指向NeoPixel缓冲区(uint8_t*数组)的指针。
@note    这适用于对每个像素调用setPixelColor()太慢的高性能应用程序(例如POV或灯光绘制项目)。
         在数组上没有边界检查，如果写入超过缓冲区的末端，则可能会造成巨大的混乱。
         强大的力量，伟大的责任，以及所有这些。
*/
uint8_t *Adafruit_NeoPixel_getPixels(void)
{
    return adafruit_neopixel.pixels;
}
#endif

/*!
  @brief   类构造函数，在编译时已知长度、管脚和像素类型的NeoPixel构造函数。
  @param   n  NeoPixel 链长度
  @param   t  像素类型--将Adafruit_NeoPixel.h中定义的neo_*常量相加，
              例如，对于NeoPixels，NEO_GRB+NEO_KHZ800需要800 kHz(Vs 400 KHz)数据流，
              其颜色字节以每像素的绿色、红色、蓝色顺序表示。
  @return  Adafruit_NeoPixel对象。在使用之前调用Begin()函数。
*/
static void Adafruit_NeoPixel_init(uint16_t n, neoPixelType t)
{
    adafruit_neopixel.begun = false;
    adafruit_neopixel.brightness = 0;
    adafruit_neopixel.pixels = NULL;
    Adafruit_NeoPixel_updateType(t);
    Adafruit_NeoPixel_updateLength(n);
}

/*!
  @brief   空的类构造函数，当编译时不知道长度、管脚和/或像素类型时，
           必须在以后使用updatType()、updateLength()和setPin()进行初始化。
  @return  Adafruit_NeoPixel对象。在使用之前调用Begin()函数。
  @note    此函数已弃用，此处仅适用于可能仍在调用它的旧项目。
           新项目应该改用带有第一个构造函数语法(length, type)的‘new’关键字。
*/
static void Adafruit_NeoPixel_init_empty(void)
{
    adafruit_neopixel.begun = false;
    adafruit_neopixel.numLEDs = 0;
    adafruit_neopixel.numBytes = 0;
    adafruit_neopixel.brightness = 0;
    adafruit_neopixel.pixels = NULL;
    adafruit_neopixel.rOffset = 1;
    adafruit_neopixel.gOffset = 0;
    adafruit_neopixel.bOffset = 2;
    adafruit_neopixel.wOffset = 1;
}
/*!
  @brief   类析构函数，取消分配Adafruit_NeoPixel对象。
*/
static void Adafruit_NeoPixel_deinit(void)
{
    free(adafruit_neopixel.pixels);
}

#if EN_sin8==1
/*!
@brief   8位整数正弦波函数，与弧度或度数等标准三角单位不直接兼容。
@param   X输入角，0-255；256将循环回到零，完成圆(相当于360度或2圆周率弧度)。
         因此，可以使用无符号8位变量并简单地进行加法或减法，
         从而允许它溢出/下溢，并且它仍然执行预期的连续操作。
@return  正弦结果为0到255，如果类型转换为有符号的int8_t，则为-128到+127，
         但您很可能希望无符号，因为此输出通常用于动画效果中的像素亮度。
*/
static uint8_t Adafruit_NeoPixel_sine8(uint8_t x)
{
    return _NeoPixelSineTable[x]; // 0-255 in, 0-255 out
}
#endif

#if EN_Color_rgb==1
/*!
@brief   将单独的红、绿和蓝值转换为单个打包的“32位RGB颜色”。
@param   r  Red 亮度, 0 to 255.
@param   g  Green 亮度, 0 to 255.
@param   b  Blue 亮度, 0 to 255.
@return  32位打包的RGB值，然后可以将其赋给变量以供以后使用或传递给setPixelColor()函数。
         压缩RGB格式是可预测的，与LED线束颜色顺序无关。
*/
static uint32_t Adafruit_NeoPixel_Color_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
#endif

#if EN_Color_rgbw==1
/*!
@brief   将红、绿、蓝、白四个单独的值转换为一个单独的“打包”32位WRGB颜色。
@param   r  Red 亮度, 0 to 255.
@param   g  Green 亮度, 0 to 255.
@param   b  Blue 亮度, 0 to 255.
@param   w  White 亮度, 0 to 255.
@return  32位打包的WRGB值，然后可以将其赋给变量以供以后使用或传递给setPixelColor()函数。
         打包的WRGB格式是可预测的，无论LED线束的颜色顺序如何。
*/
static uint32_t Adafruit_NeoPixel_Color_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
#endif

Tst_Adafruit_NeoPixel  adafruit_neopixel=
{
    // 功能函数
    Adafruit_NeoPixel_begin,
    Adafruit_NeoPixel_updateLength,
    Adafruit_NeoPixel_updateType,
    Adafruit_NeoPixel_show,

    #if EN_setPixelColor_rgb==1
    Adafruit_NeoPixel_setPixelColor_rgb,
    #else
    NULL,
    #endif

    #if EN_setPixelColor_rgbw==1
    Adafruit_NeoPixel_setPixelColor_rgbw,
    #else
    NULL,
    #endif

    #if EN_setPixelColor==1
    Adafruit_NeoPixel_setPixelColor,
    #else
    NULL,
    #endif

    #if EN_fill==1
    Adafruit_NeoPixel_fill,
    #else
    NULL,
    #endif

    #if EN_ColorHSV==1
    Adafruit_NeoPixel_ColorHSV,
    #else
    NULL,
    #endif

    #if EN_getPixelColor==1
    Adafruit_NeoPixel_getPixelColor,
    #else
    NULL,
    #endif

    Adafruit_NeoPixel_setBrightness,

    #if EN_getBrightness==1
    Adafruit_NeoPixel_getBrightness,
    #else
    NULL,
    #endif

    #if EN_clear==1
    Adafruit_NeoPixel_clear,
    #else
    NULL,
    #endif

    #if EN_rainbow==1
    Adafruit_NeoPixel_rainbow,
    #else
    NULL,
    #endif

    #if EN_numPixels==1
    Adafruit_NeoPixel_numPixels,
    #else
    NULL,
    #endif

    #if EN_getPixels==1
    Adafruit_NeoPixel_getPixels,
    #else
    NULL,
    #endif

    #if EN_Color_rgb==1
    Adafruit_NeoPixel_Color_rgb,
    #else
    NULL,
    #endif

    #if EN_Color_rgbw==1
    Adafruit_NeoPixel_Color_rgbw,
    #else
    NULL,
    #endif

    Adafruit_NeoPixel_init,
    Adafruit_NeoPixel_init_empty,
    Adafruit_NeoPixel_deinit,
    #if (EN_gamma8==1) || (EN_gamma32==1)
    Adafruit_NeoPixel_gamma8,
    #else
    NULL,
    #endif
    
    #if EN_gamma32==1
    Adafruit_NeoPixel_gamma32,
    #else
    NULL,
    #endif
    
    #if EN_sin8==1
    Adafruit_NeoPixel_sine8,
    #else
    NULL,
    #endif
    
    // 硬件接口
    NULL,

    // 变量
    false,
    0,
    0,
    0,
    NULL,
    0,
    0,
    0,
    0
};

#undef EN_setPixelColor_rgb
#undef EN_setPixelColor_rgbw
#undef EN_setPixelColor
#undef EN_fill
#undef EN_ColorHSV
#undef EN_getPixelColor
#undef EN_getBrightness
#undef EN_clear
#undef EN_rainbow
#undef EN_numPixels
#undef EN_getPixels
#undef EN_Color_rgb
#undef EN_Color_rgbw
#undef EN_gamma8
#undef EN_gamma32
#undef EN_sin8

#endif
