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
//@ref: https://github.com/Aircoookie/WLED/	
//@ref: https://github.com/FastLED/FastLED	(src\colorutils.cpp,src\hsv2rgb.cpp)
/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "hw_config.h"
#if APP_RGB_ENABLE
#include "color_utils.h"


#include "api/api_log.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/


/// Divide the color wheel into four sections, 64 elements each
/// @todo I believe this is mis-named, and should be HSV_SECTION_4
#define HSV_SECTION_3 (0x40)
#define K255 255
#define K171 171
#define K170 170
#define K85  85
/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

#if APP_RGB_COLOR_PALETTE_ENABLE | 1        //默认调色板, 用户可以自定义调色板
/// Cloudy color palette	七彩云调色板
uint32c_t CloudColors_p[16] =
{
    Color_Blue,
    Color_DarkBlue,
    Color_DarkBlue,
    Color_DarkBlue,

    Color_DarkBlue,
    Color_DarkBlue,
    Color_DarkBlue,
    Color_DarkBlue,

    Color_Blue,
    Color_DarkBlue,
    Color_SkyBlue,
    Color_SkyBlue,

    Color_LightBlue,
    Color_White,
    Color_LightBlue,
    Color_SkyBlue
};

/// Lava color palette	熔岩调色板
uint32c_t LavaColors_p[16] =
{
    Color_Black,
    Color_Maroon,
    Color_Black,
    Color_Maroon,

    Color_DarkRed,
    Color_Maroon,
    Color_DarkRed,

    Color_DarkRed,
    Color_DarkRed,
    Color_Red,
    Color_Orange,

    Color_White,
    Color_Orange,
    Color_Red,
    Color_DarkRed
};


/// Ocean colors, blues and whites	海洋的颜色，蓝色和白色
uint32c_t OceanColors_p[16] =
{
    Color_MidnightBlue,
    Color_DarkBlue,
    Color_MidnightBlue,
    Color_Navy,

    Color_DarkBlue,
    Color_MediumBlue,
    Color_SeaGreen,
    Color_Teal,

    Color_CadetBlue,
    Color_Blue,
    Color_DarkCyan,
    Color_CornflowerBlue,

    Color_Aquamarine,
    Color_SeaGreen,
    Color_Aqua,
    Color_LightSkyBlue
};

/// Forest colors, greens	森林色，绿色
uint32c_t ForestColors_p[16] =
{
    Color_DarkGreen,
    Color_DarkGreen,
    Color_DarkOliveGreen,
    Color_DarkGreen,

    Color_Green,
    Color_ForestGreen,
    Color_OliveDrab,
    Color_Green,

    Color_SeaGreen,
    Color_MediumAquamarine,
    Color_LimeGreen,
    Color_YellowGreen,

    Color_LightGreen,
    Color_LawnGreen,
    Color_MediumAquamarine,
    Color_ForestGreen
};
#endif

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/* Similar to above, but for an 8-bit gamma-correction table.
   Copy & paste this snippet into a Python REPL to regenerate:
import math
gamma=2.6
for x in range(256):
    print("{:3},".format(int(math.pow((x)/255.0,gamma)*255.0+0.5))),
    if x&15 == 15: print
*/
#if COLOR_GAMMA_TABLE
static uint8c_t GammaTable[256] = {
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
    255
};
#endif


//gamma 2.8 lookup table used for color correction
#if 28 == COLOR_GAMMA
static uint8c_t GammaTable[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
};
#endif

/*****************************************************************************************************
**	static Function
******************************************************************************************************/


/*****************************************************************************************************
**  Function
******************************************************************************************************/

uint8_t gamma8(uint8_t b)
{
	#if 10 == COLOR_GAMMA
		return b;
	#elif 26 == COLOR_GAMMA || 28 == COLOR_GAMMA
		return GammaTable[b];
	#else
		return (int)(powf((float)b / 255.0f, COLOR_GAMMA) * 255.0f + 0.5f);
	#endif
}





uint32_t color_rgbc(rgb_t *prgb)
{
	return RGB32(prgb->rgb.r, prgb->rgb.g, prgb->rgb.b);
}

void color_rgb(uint32_t c, rgb_t *prgb)
{
	prgb->rgb.r = R(c);
	prgb->rgb.b = G(c);
	prgb->rgb.b = B(c);
}

void color_hsv_to_rgb(hsv_t *phsv,rgb_t *prgb)
{
    // Convert hue, saturation and brightness ( HSV/HSB ) to RGB
    // "Dimming" is used on saturation and brightness to make
    // the output more visually linear.

    uint8_t value = phsv->hsv.v;
    uint8_t saturation = phsv->hsv.s;

    // The brightness floor is minimum number that all of
    // R, G, and B will be set to.
    uint8_t invsat = 255 - saturation;
    uint8_t brightness_floor = (value * invsat) / 256;

    // The color amplitude is the maximum amount of R, G, and B
    // that will be added on top of the brightness_floor to
    // create the specific hue desired.
    uint8_t color_amplitude = value - brightness_floor;

    // Figure out which section of the hue wheel we're in,
    // and how far offset we are withing that section
    uint8_t section = phsv->hsv.h / HSV_SECTION_3; // 0..2
    uint8_t offset = phsv->hsv.h % HSV_SECTION_3;  // 0..63

    uint8_t rampup = offset; // 0..63
    uint8_t rampdown = (HSV_SECTION_3 - 1) - offset; // 63..0

    // We now scale rampup and rampdown to a 0-255 range -- at least
    // in theory, but here's where architecture-specific decsions
    // come in to play:
    // To scale them up to 0-255, we'd want to multiply by 4.
    // But in the very next step, we multiply the ramps by other
    // values and then divide the resulting product by 256.
    // So which is faster?
    //   ((ramp * 4) * othervalue) / 256
    // or
    //   ((ramp    ) * othervalue) /  64
    // It depends on your processor architecture.
    // On 8-bit AVR, the "/ 256" is just a one-cycle register move,
    // but the "/ 64" might be a multicycle shift process. So on AVR
    // it's faster do multiply the ramp values by four, and then
    // divide by 256.
    // On ARM, the "/ 256" and "/ 64" are one cycle each, so it's
    // faster to NOT multiply the ramp values by four, and just to
    // divide the resulting product by 64 (instead of 256).
    // Moral of the story: trust your profiler, not your insticts.

    // Since there's an AVR assembly version elsewhere, we'll
    // assume what we're on an architecture where any number of
    // bit shifts has roughly the same cost, and we'll remove the
    // redundant math at the source level:

    //  // scale up to 255 range
    //  //rampup *= 4; // 0..252
    //  //rampdown *= 4; // 0..252

    // compute color-amplitude-scaled-down versions of rampup and rampdown
    uint8_t rampup_amp_adj   = (rampup   * color_amplitude) / (256 / 4);
    uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

    // add brightness_floor offset to everything
    uint8_t rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
    uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;


    if( section ) {
        if( section == 1) {
            // section 1: 0x40..0x7F
            prgb->rgb.r = brightness_floor;
            prgb->rgb.g = rampdown_adj_with_floor;
            prgb->rgb.b = rampup_adj_with_floor;
        } else {
            // section 2; 0x80..0xBF
            prgb->rgb.r = rampup_adj_with_floor;
            prgb->rgb.g = brightness_floor;
            prgb->rgb.b = rampdown_adj_with_floor;
        }
    } else {
        // section 0: 0x00..0x3F
        prgb->rgb.r = rampdown_adj_with_floor;
        prgb->rgb.g = rampup_adj_with_floor;
        prgb->rgb.b = brightness_floor;
    }
}

void color_hsv_to_rgb_spectrum(hsv_t *phsv,rgb_t *prgb)
{
    hsv_t hsv2 = *phsv;
    hsv2.hsv.h = SCALE8( hsv2.hsv.h, 191);
    color_hsv_to_rgb(&hsv2, prgb);
}



void color_hsv_to_rgb_rainbow(hsv_t *phsv,rgb_t *prgb)
{
    // Yellow has a higher inherent brightness than
    // any other color; 'pure' yellow is perceived to
    // be 93% as bright as white.  In order to make
    // yellow appear the correct relative brightness,
    // it has to be rendered brighter than all other
    // colors.
    // Level Y1 is a moderate boost, the default.
    // Level Y2 is a strong boost.
    const uint8_t Y1 = 1;
    const uint8_t Y2 = 0;
    
    // G2: Whether to divide all greens by two.
    // Depends GREATLY on your particular LEDs
    const uint8_t G2 = 0;
    
    // Gscale: what to scale green down by.
    // Depends GREATLY on your particular LEDs
    const uint8_t Gscale = 0;
    
    
    uint8_t hue = phsv->hsv.h;
    uint8_t sat = phsv->hsv.s;
    uint8_t val = phsv->hsv.v;
    
    uint8_t offset = hue & 0x1F; // 0..31
    
    // offset8 = offset * 8
    uint8_t offset8 = offset;
    offset8 <<= 3;
    
    uint8_t third = SCALE8( offset8, (256 / 3)); // max = 85
    
    volatile uint8_t r=0, g=0, b=0;		//添加volatiel 防止过渡优化
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = K255 - third;
                g = third;
                b = 0;
            } else {
                // 001
                //case 1: // O -> Y
                if( Y1 ) {
                    r = K171;
                    g = K85 + third ;
                    b = 0;
                }
                if( Y2 ) {
                    r = K170 + third;
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = SCALE8( offset8, ((256 * 2) / 3)); // max=170
                    g = K85 + twothirds;
                    b = 0;
                }
            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                if( Y1 ) {
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = SCALE8( offset8, ((256 * 2) / 3)); // max=170
                    r = K171 - twothirds;
                    g = K170 + third;
                    b = 0;
                }
                if( Y2 ) {
                    r = K255 - offset8;
                    g = K255;
                    b = 0;
                }
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                g = K255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = SCALE8( offset8, ((256 * 2) / 3)); // max=170
                g = K171 - twothirds; //K170?
                b = K85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;
                b = K255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = K85 + third;
                g = 0;
                b = K171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = K170 + third;
                g = 0;
                b = K85 - third;
                
            }
        }
    }
    
    // This is one of the good places to scale the green down,
    // although the client can scale green down as well.
    if( G2 ) g = g >> 1;
    if( Gscale ) g = SCALE8_VIDEO( g, Gscale);
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            uint8_t desat = 255 - sat;
            desat = SCALE8_VIDEO( desat, desat);

            uint8_t satscale = 255 - desat;
            //satscale = sat; // uncomment to revert to pre-2021 saturation behavior

            //nscale8x3_video( r, g, b, sat);

            if( r ) r = SCALE8( r, satscale) + 1;
            if( g ) g = SCALE8( g, satscale) + 1;
            if( b ) b = SCALE8( b, satscale) + 1;

            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    // Now scale everything down if we're at value < 255.
    if( val != 255 ) {
        
        val = SCALE8_VIDEO( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            // nscale8x3_video( r, g, b, val);
            if( r ) r = SCALE8( r, val) + 1;
            if( g ) g = SCALE8( g, val) + 1;
            if( b ) b = SCALE8( b, val) + 1;
        }
    }
    
    prgb->rgb.r = r;
    prgb->rgb.g = g;
    prgb->rgb.b = b;
}

#ifndef SCALE8
#define SCALE8(x, scale) (uint8_t)( ( (uint16_t)(x) * (1+(uint16_t)(scale)) ) >> 8 ) 
#endif


uint32_t scale32(uint32_t color, uint8_t scale) 
{
    uint8_t r,g,b;

    r = SCALE8(R(color), scale);
    g = SCALE8(G(color), scale);
    b = SCALE8(B(color), scale);

    return RGB32(r, g, b);
}
/*******************************************************************
** Parameters:		
** Returns:	
** Description:	
	(  color1*(0XFF-blend) + color1 + color2*(blend) + color2 ) / 256

  result = (  A*(amountOfA) + B*(amountOfB)              )/ 256
…where amountOfA = 255-amountOfB.
This formula will never return 255, which is why the BLEND_FIXED + SCALE8_FIXED version is
  result = (  A*(amountOfA) + A + B*(amountOfB) + B      ) / 256
We can rearrange this formula for some great optimisations.
  result = (  A*(amountOfA) + A + B*(amountOfB) + B      ) / 256
         = (  A*(255-amountOfB) + A + B*(amountOfB) + B  ) / 256
         = (  A*(256-amountOfB) + B*(amountOfB) + B      ) / 256
         = (  A*256 + B + B*(amountOfB) - A*(amountOfB)  ) / 256  // this is the version used in SCALE8_FIXED AVR below
         = (  A*256 + B + (B-A)*(amountOfB)              ) / 256  // this is the version used in SCALE8_FIXED C below
*******************************************************************/
uint8_t blend8(uint8_t color1, uint8_t color2, uint8_t blend) {

	uint16_t partial;
    uint8_t result;

  	if(blend == 0)    return color1;
  	if(blend == 0xFF) return color2;
	partial = (color1 << 8) | color2; // A*256 + B

    // on many platforms this compiles to a single multiply of (B-A) * amountOfB
    partial += (color2 * blend);
    partial -= (color1 * blend);

    result = partial >> 8;

  return result;
}
uint32_t blend32(uint32_t color1, uint32_t color2, uint8_t blend) 
{
    uint8_t r,g,b;

    r = blend8(R(color1), R(color2), blend);
    g = blend8(G(color1), G(color2), blend);
    b = blend8(B(color1), B(color2), blend);

    return RGB32(r, g, b);
}
/*******************************************************************
** Parameters:	prgb1: 输入颜色, 	prgb2: 混合颜色, blend:混合颜色比例
** Returns:		prgb1: 输出混合后的颜色
** Description:		
*******************************************************************/
void color_blend8(rgb_t *prgb1,rgb_t *prgb2, uint8_t blend) {
	prgb1->rgb.r = blend8(prgb1->rgb.r, prgb2->rgb.r, blend);
	prgb1->rgb.g = blend8(prgb1->rgb.r, prgb2->rgb.g, blend);
	prgb1->rgb.b = blend8(prgb1->rgb.r, prgb2->rgb.b, blend);
}

/*******************************************************************
** Parameters:	index: 色板中颜色序号(0~255), 	brightness: 输出亮度
				blend_type:模式, NOBLEND:不混色, LINEARBLEND:循环混色,LINEARBLEND_NOWRAP:不循环混色
				palette,palette_size: 调色板
** Returns:		
** Description:	函数返回指定色板中的RGB颜色,
        注意如果调色板样色数不能被256整除,index一个循环的输入范围应该是 (256/palette_size)*palette_size-1
*******************************************************************/
uint32_t color_from_palette(uint8_t index, uint8_t brightness, blend_type_t blend_type, const uint32_t *palette, uint8_t palette_size)
{
	rgb_t entry;
	uint8_t hi4;
    uint8_t lo4;
	uint8_t need_blend,blend;
    uint8_t step;                       //调色板颜色之间步长

	if(0 == brightness) return 0;

    step = 256/palette_size;
	if ( blend_type == LINEARBLEND_NOWRAP) {
		REMAP( index, 0, 255, 0, step * palette_size -1);  // Blend range is affected by lo4 blend of values, remap to avoid wrapping
	}
    
    hi4 = hi4 = index / step;           //通过调色板颜色数量找到颜色区间,调色板数组下标
    if( hi4 >= palette_size ) hi4 = 0;
    lo4 = index & step;                 //余数用于细分颜色值
    
	color_rgb(palette[hi4], &entry);

    need_blend = lo4 && (blend_type != NOBLEND);
    if( need_blend ) {
		rgb_t entry2;

        blend = lo4 * palette_size;         //blend输入范围是0~255, 将lo4调整到0~255范围内

        if( ++hi4 >= palette_size ) hi4 = 0;  //nest color
        color_rgb(palette[hi4], &entry2);

		color_blend8(&entry,&entry2, blend);
    }

    if( brightness != 255) {
		++brightness; // adjust for rounding
		// Now, since brightness is nonzero, we don't need the full scale8_video logic;
		// we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
		if( entry.rgb.r )   {
			entry.rgb.r = SCALE8( entry.rgb.r, brightness);
		}
		if( entry.rgb.g ) {
			entry.rgb.g = SCALE8( entry.rgb.g, brightness);
		}
		if( entry.rgb.b )  {
			entry.rgb.b = SCALE8( entry.rgb.b, brightness);
		}
    }

    return RGB32( entry.rgb.r, entry.rgb.g, entry.rgb.b);
}

/*******************************************************************
** Parameters:	index: 色板中颜色序号(0~255), 	brightness: 输出亮度
				blend_type:模式, NOBLEND:不混色, LINEARBLEND:循环混色,LINEARBLEND_NOWRAP:不循环混色
				palette,palette_size: 调色板
** Returns:		
** Description:	函数返回指定色板中的HSV颜色
*******************************************************************/
uint32_t color_from_palette_hsv(uint8_t index, uint8_t brightness, blend_type_t blend_type, const uint32_t *palette, uint8_t palette_size)
{
	hsv_t entry;
	uint8_t hi4;
    uint8_t lo4;
	uint8_t blend;
    uint8_t step;      

	uint8_t hue1;
    uint8_t sat1;
    uint8_t val1;

    if(0 == brightness) return 0;

    step = 256/palette_size;

	if ( blend_type == LINEARBLEND_NOWRAP) {
		index = REMAP(index, 0, 255, 0, step * palette_size -1);  // Blend range is affected by lo4 blend of values, remap to avoid wrapping
	}
    
    hi4 = hi4 = index / step;           //通过调色板颜色数量找到颜色区间,调色板数组下标
    if( hi4 >= palette_size ) hi4 = 0;
    lo4 = index & step;                 //余数用于细分颜色值
    color_rgb(palette[hi4], (rgb_t*)&entry);

    hue1   = entry.hsv.h;
    sat1   = entry.hsv.s;
    val1   = entry.hsv.v;

    blend = lo4 && (blend_type != NOBLEND);

    if( blend ) {
		uint8_t hue2;
    	uint8_t sat2;
    	uint8_t val2;
		uint8_t f2;
        uint8_t f1;
        
        if( ++hi4 >= palette_size ) hi4 = 0;  //nest color
        color_rgb(palette[hi4], (rgb_t*)&entry);

        f2 = lo4 * palette_size;
        f1 = 255 - f2;

        hue2  = entry.hsv.h;
        sat2  = entry.hsv.s;
        val2  = entry.hsv.v;

        // Now some special casing for blending to or from
        // either black or white.  Black and white don't have
        // proper 'hue' of their own, so when ramping from
        // something else to/from black/white, we set the 'hue'
        // of the black/white color to be the same as the hue
        // of the other color, so that you get the expected
        // brightness or saturation ramp, with hue staying
        // constant:

        // If we are starting from white (sat=0)
        // or black (val=0), adopt the target hue.
        if( sat1 == 0 || val1 == 0) {
            hue1 = hue2;
        }

        // If we are ending at white (sat=0)
        // or black (val=0), adopt the starting hue.
        if( sat2 == 0 || val2 == 0) {
            hue2 = hue1;
        }


        sat1  = SCALE8( sat1, f1);
        val1  = SCALE8( val1, f1);

        sat2  = SCALE8( sat2, f2);
        val2  = SCALE8( val2, f2);

        //    cleanup_R1();

        // These sums can't overflow, so no qadd8 needed.
        sat1  += sat2;
        val1  += val2;

        uint8_t deltaHue = (uint8_t)(hue2 - hue1);
        if( deltaHue & 0x80 ) {
          // go backwards
          hue1 -= scale8( 256 - deltaHue, f2);
        } else {
          // go forwards
          hue1 += scale8( deltaHue, f2);
        }
    }

    if( brightness != 255) {
        val1 = SCALE8_VIDEO( val1, brightness);
    }

    return RGB32( hue1, sat1, val1);
}


#endif
