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


#ifndef _colors_utils_h
#define _colors_utils_h
#include "emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
#ifndef COLOR_GAMMA		//26, 28, 10, GAMMA=10 isnone GAMMA
#define	COLOR_GAMMA		10			
#endif
#ifndef APP_RGB_COLOR_PALETTE_ENABLE
#define APP_RGB_COLOR_PALETTE_ENABLE	1
#endif

//y = x * (scale / 0XFF)
#ifndef SCALE8
#define SCALE8(x, scale) (uint8_t)( ( (uint16_t)(x) * (1+(uint16_t)(scale)) ) >> 8 ) 
#endif
#define scale8(x, scale) 	SCALE8(x, scale)

#ifndef SCALE8_VIDEO
#define SCALE8_VIDEO(x, scale) (uint8_t)( (((uint16_t)(x) * (uint16_t)(scale)) >> 8) + ((x&&scale)?1:0) ) 
#endif


#define RGB32(r,g,b) 		U32(0,r,g,b)
#define W(c) 				((uint8_t)((c) >> 24))
#define R(c) 				((uint8_t)((c) >> 16))
#define G(c) 				((uint8_t)((c) >> 8))
#define B(c) 				((uint8_t)(c))




/// Predefined RGB colors
#define Color_AliceBlue    		0xF0F8FF              	///< @htmlcolorblock{F0F8FF}
#define Color_Amethyst    		0x9966CC              	///< @htmlcolorblock{9966CC}
#define Color_AntiqueWhite  	0xFAEBD7           		///< @htmlcolorblock{FAEBD7}
#define Color_Aqua				0x00FFFF                ///< @htmlcolorblock{00FFFF}
#define Color_Aquamarine    	0x7FFFD4             	///< @htmlcolorblock{7FFFD4}
#define Color_Azure    			0xF0FFFF                ///< @htmlcolorblock{F0FFFF}
#define Color_Beige				0xF5F5DC                ///< @htmlcolorblock{F5F5DC}
#define Color_Bisque    		0xFFE4C4                ///< @htmlcolorblock{FFE4C4}
#define Color_Black    			0x000000                ///< @htmlcolorblock{000000}
#define Color_BlanchedAlmond    0xFFEBCD         		///< @htmlcolorblock{FFEBCD}
#define Color_Blue    			0x0000FF                ///< @htmlcolorblock{0000FF}
#define Color_BlueViolet    	0x8A2BE2             	///< @htmlcolorblock{8A2BE2}
#define Color_Brown    			0xA52A2A                ///< @htmlcolorblock{A52A2A}
#define Color_BurlyWood    		0xDEB887              	///< @htmlcolorblock{DEB887}
#define Color_CadetBlue    		0x5F9EA0              	///< @htmlcolorblock{5F9EA0}
#define Color_Chartreuse    	0x7FFF00              	///< @htmlcolorblock{7FFF00}
#define Color_Chocolate    		0xD2691E              	///< @htmlcolorblock{D2691E}
#define Color_Coral    			0xFF7F50                ///< @htmlcolorblock{FF7F50}
#define Color_CornflowerBlue    0x6495ED         		///< @htmlcolorblock{6495ED}
#define Color_Cornsilk    		0xFFF8DC               	///< @htmlcolorblock{FFF8DC}
#define Color_Crimson    		0xDC143C                ///< @htmlcolorblock{DC143C}
#define Color_Cyan    			0x00FFFF                ///< @htmlcolorblock{00FFFF}
#define Color_DarkBlue    		0x00008B                ///< @htmlcolorblock{00008B}
#define Color_DarkCyan    		0x008B8B                ///< @htmlcolorblock{008B8B}
#define Color_DarkGoldenrod    	0xB8860B          		///< @htmlcolorblock{B8860B}
#define Color_DarkGray    		0xA9A9A9                ///< @htmlcolorblock{A9A9A9}
#define Color_DarkGrey    		0xA9A9A9                ///< @htmlcolorblock{A9A9A9}
#define Color_DarkGreen    		0x006400                ///< @htmlcolorblock{006400}
#define Color_DarkKhaki    		0xBDB76B                ///< @htmlcolorblock{BDB76B}
#define Color_DarkMagenta   	0x8B008B                ///< @htmlcolorblock{8B008B}
#define Color_DarkOliveGreen    0x556B2F         		///< @htmlcolorblock{556B2F}
#define Color_DarkOrange    	0xFF8C00             	///< @htmlcolorblock{FF8C00}
#define Color_DarkOrchid    	0x9932CC             	///< @htmlcolorblock{9932CC}
#define Color_DarkRed    		0x8B0000                ///< @htmlcolorblock{8B0000}
#define Color_DarkSalmon    	0xE9967A             	///< @htmlcolorblock{E9967A}
#define Color_DarkSeaGreen    	0x8FBC8F           		///< @htmlcolorblock{8FBC8F}
#define Color_DarkSlateBlue    	0x483D8B          		///< @htmlcolorblock{483D8B}
#define Color_DarkSlateGray    	0x2F4F4F          		///< @htmlcolorblock{2F4F4F}
#define Color_DarkSlateGrey    	0x2F4F4F          		///< @htmlcolorblock{2F4F4F}
#define Color_DarkTurquoise    	0x00CED1          		///< @htmlcolorblock{00CED1}
#define Color_DarkViolet    	0x9400D3             	///< @htmlcolorblock{9400D3}
#define Color_DeepPink    		0xFF1493               	///< @htmlcolorblock{FF1493}
#define Color_DeepSkyBlue    	0x00BFFF           		///< @htmlcolorblock{00BFFF}
#define Color_DimGray    		0x696969                ///< @htmlcolorblock{696969}
#define Color_DimGrey    		0x696969                ///< @htmlcolorblock{696969}
#define Color_DodgerBlue    	0x1E90FF             	///< @htmlcolorblock{1E90FF}
#define Color_FireBrick    		0xB22222              	///< @htmlcolorblock{B22222}
#define Color_FloralWhite    	0xFFFAF0            	///< @htmlcolorblock{FFFAF0}
#define Color_ForestGreen    	0x228B22            	///< @htmlcolorblock{228B22}
#define Color_Fuchsia    		0xFF00FF                ///< @htmlcolorblock{FF00FF}
#define Color_Gainsboro    		0xDCDCDC              	///< @htmlcolorblock{DCDCDC}
#define Color_GhostWhite   		0xF8F8FF             	///< @htmlcolorblock{F8F8FF}
#define Color_Gold    			0xFFD700 				///< @htmlcolorblock{FFD700}
#define Color_Goldenrod    		0xDAA520              	///< @htmlcolorblock{DAA520}
#define Color_Gray   	 		0x808080                ///< @htmlcolorblock{808080}
#define Color_Grey    			0x808080                ///< @htmlcolorblock{808080}
#define Color_Green    			0x008000                ///< @htmlcolorblock{008000}
#define Color_GreenYellow   	0xADFF2F            	///< @htmlcolorblock{ADFF2F}
#define Color_Honeydew    		0xF0FFF0               	///< @htmlcolorblock{F0FFF0}
#define Color_HotPink    		0xFF69B4                ///< @htmlcolorblock{FF69B4}
#define Color_IndianRed    		0xCD5C5C              	///< @htmlcolorblock{CD5C5C}
#define Color_Indigo    		0x4B0082                ///< @htmlcolorblock{4B0082}
#define Color_Ivory    			0xFFFFF0                ///< @htmlcolorblock{FFFFF0}
#define Color_Khaki    			0xF0E68C                ///< @htmlcolorblock{F0E68C}
#define Color_Lavender    		0xE6E6FA                ///< @htmlcolorblock{E6E6FA}
#define Color_LavenderBlush 	0xFFF0F5          		///< @htmlcolorblock{FFF0F5}
#define Color_LawnGreen    		0x7CFC00              	///< @htmlcolorblock{7CFC00}
#define Color_LemonChiffon		0xFFFACD           		///< @htmlcolorblock{FFFACD}
#define Color_LightBlue			0xADD8E6              	///< @htmlcolorblock{ADD8E6}
#define Color_LightCoral    	0xF08080             	///< @htmlcolorblock{F08080}
#define Color_LightCyan			0xE0FFFF              	///< @htmlcolorblock{E0FFFF}
#define Color_LightGoldenrodYellow    0xFAFAD2  		///< @htmlcolorblock{FAFAD2}
#define Color_LightGreen    	0x90EE90             	///< @htmlcolorblock{90EE90}
#define Color_LightGrey    		0xD3D3D3              	///< @htmlcolorblock{D3D3D3}
#define Color_LightPink    		0xFFB6C1              	///< @htmlcolorblock{FFB6C1}
#define Color_LightSalmon   	0xFFA07A           	 	///< @htmlcolorblock{FFA07A}
#define Color_LightSeaGreen		0x20B2AA          		///< @htmlcolorblock{20B2AA}
#define Color_LightSkyBlue		0x87CEFA           		///< @htmlcolorblock{87CEFA}
#define Color_LightSlateGray    0x778899        		///< @htmlcolorblock{778899}
#define Color_LightSlateGrey    0x778899        		///< @htmlcolorblock{778899}
#define Color_LightSteelBlue    0xB0C4DE        		///< @htmlcolorblock{B0C4DE}
#define Color_LightYellow    	0xFFFFE0            	///< @htmlcolorblock{FFFFE0}
#define Color_Lime    			0x00FF00                ///< @htmlcolorblock{00FF00}
#define Color_LimeGreen    		0x32CD32              	///< @htmlcolorblock{32CD32}
#define Color_Linen    			0xFAF0E6                ///< @htmlcolorblock{FAF0E6}
#define Color_Magenta   		0xFF00FF                ///< @htmlcolorblock{FF00FF}
#define Color_Maroon    		0x800000                ///< @htmlcolorblock{800000}
#define Color_MediumAquamarine  0x66CDAA       			///< @htmlcolorblock{66CDAA}
#define Color_MediumBlue    	0x0000CD                ///< @htmlcolorblock{0000CD}
#define Color_MediumOrchid		0xBA55D3               	///< @htmlcolorblock{BA55D3}
#define Color_MediumPurple		0x9370DB           		///< @htmlcolorblock{9370DB}
#define Color_MediumSeaGreen	0x3CB371         		///< @htmlcolorblock{3CB371}
#define Color_MediumSlateBlue	0x7B68EE        		///< @htmlcolorblock{7B68EE}
#define Color_MediumSpringGreen	0x00FA9A      			///< @htmlcolorblock{00FA9A}
#define Color_MediumTurquoise	0x48D1CC        		///< @htmlcolorblock{48D1CC}
#define Color_MediumVioletRed	0xC71585       		 	///< @htmlcolorblock{C71585}
#define Color_MidnightBlue		0x191970           		///< @htmlcolorblock{191970}
#define Color_MintCream			0xF5FFFA              	///< @htmlcolorblock{F5FFFA}
#define Color_MistyRose			0xFFE4E1              	///< @htmlcolorblock{FFE4E1}
#define Color_Moccasin			0xFFE4B5               	///< @htmlcolorblock{FFE4B5}
#define Color_NavajoWhite		0xFFDEAD           		///< @htmlcolorblock{FFDEAD}
#define Color_Navy    			0x000080                ///< @htmlcolorblock{000080}
#define Color_OldLace    		0xFDF5E6                ///< @htmlcolorblock{FDF5E6}
#define Color_Olive    			0x808000                ///< @htmlcolorblock{808000}
#define Color_OliveDrab   	 	0x6B8E23             	///< @htmlcolorblock{6B8E23}
#define Color_Orange    		0xFFA500                ///< @htmlcolorblock{FFA500}
#define Color_OrangeRed    		0xFF4500              	///< @htmlcolorblock{FF4500}
#define Color_Orchid    		0xDA70D6                ///< @htmlcolorblock{DA70D6}
#define Color_PaleGoldenrod    	0xEEE8AA          		//< @htmlcolorblock{EEE8AA}
#define Color_PaleGreen    		0x98FB98          		///< @htmlcolorblock{98FB98}
#define Color_PaleTurquoise    	0xAFEEEE          		//< @htmlcolorblock{AFEEEE}
#define Color_PaleVioletRed    	0xDB7093          		//< @htmlcolorblock{DB7093}
#define Color_PapayaWhip    	0xFFEFD5          		//< @htmlcolorblock{FFEFD5}
#define Color_PeachPuff    		0xFFDAB9          		//< @htmlcolorblock{FFDAB9}
#define Color_Peru   			0xCD853F          		///< @htmlcolorblock{CD853F}
#define Color_Pink   			0xFFC0CB          		///< @htmlcolorblock{FFC0CB}
#define Color_Plaid  			0xCC5533          		///< @htmlcolorblock{CC5533}
#define Color_Plum   			0xDDA0DD          		///< @htmlcolorblock{DDA0DD}
#define Color_PowderBlue    	0xB0E0E6          		///< @htmlcolorblock{B0E0E6}
#define Color_Purple    		0x800080          		///< @htmlcolorblock{800080}
#define Color_Red    			0xFF0000          		///< @htmlcolorblock{FF0000}
#define Color_RosyBrown    		0xBC8F8F          		///< @htmlcolorblock{BC8F8F}
#define Color_RoyalBlue    		0x4169E1          		///< @htmlcolorblock{4169E1}
#define Color_SaddleBrown   	0x8B4513          		///< @htmlcolorblock{8B4513}
#define Color_Salmon    		0xFA8072          		///< @htmlcolorblock{FA8072}
#define Color_SandyBrown    	0xF4A460          		///< @htmlcolorblock{F4A460}
#define Color_SeaGreen    		0x2E8B57          		///< @htmlcolorblock{2E8B57}
#define Color_Seashell    		0xFFF5EE          		///< @htmlcolorblock{FFF5EE}
#define Color_Sienna    		0xA0522D          		///< @htmlcolorblock{A0522D}
#define Color_Silver    		0xC0C0C0          		///< @htmlcolorblock{C0C0C0}
#define Color_SkyBlue    		0x87CEEB          		///< @htmlcolorblock{87CEEB}
#define Color_SlateBlue    		0x6A5ACD          		///< @htmlcolorblock{6A5ACD}
#define Color_SlateGray    		0x708090          		///< @htmlcolorblock{708090}
#define Color_SlateGrey    		0x708090          		///< @htmlcolorblock{708090}
#define Color_Snow    			0xFFFAFA          		///< @htmlcolorblock{FFFAFA}
#define Color_SpringGreen    	0x00FF7F          		///< @htmlcolorblock{00FF7F}
#define Color_SteelBlue    		0x4682B4          		///< @htmlcolorblock{4682B4}
#define Color_Tan    			0xD2B48C          		///< @htmlcolorblock{D2B48C}
#define Color_Teal    			0x008080          		///< @htmlcolorblock{008080}
#define Color_Thistle    		0xD8BFD8          		///< @htmlcolorblock{D8BFD8}
#define Color_Tomato    		0xFF6347          		///< @htmlcolorblock{FF6347}
#define Color_Turquoise    		0x40E0D0          		///< @htmlcolorblock{40E0D0}
#define Color_Violet   			0xEE82EE          		///< @htmlcolorblock{EE82EE}
#define Color_Wheat    			0xF5DEB3          		///< @htmlcolorblock{F5DEB3}
#define Color_White    			0xFFFFFF          		///< @htmlcolorblock{FFFFFF}
#define Color_WhiteSmoke    	0xF5F5F5          		///< @htmlcolorblock{F5F5F5}
#define Color_Yellow    		0xFFFF00          		///< @htmlcolorblock{FFFF00}
#define Color_YellowGreen    	0x9ACD32          		///< @htmlcolorblock{9ACD32}

/// Pre-defined hue values for CHSV objects
typedef enum {
    HUE_RED = 0,       ///< Red (0°)
    HUE_ORANGE = 32,   ///< Orange (45°)
    HUE_YELLOW = 64,   ///< Yellow (90°)
    HUE_GREEN = 96,    ///< Green (135°)
    HUE_AQUA = 128,    ///< Aqua (180°)
    HUE_BLUE = 160,    ///< Blue (225°)
    HUE_PURPLE = 192,  ///< Purple (270°)
    HUE_PINK = 224     ///< Pink (315°)
} HSVHue_t;

/******************************************************************************************************
**	Parameters
*******************************************************************************************************/
/// Color interpolation options for palette
typedef enum {
    NOBLEND=0,            ///< No interpolation between palette entries
    LINEARBLEND=1,        ///< Linear interpolation between palette entries, with wrap-around from end to the beginning again
    LINEARBLEND_NOWRAP=2  ///< Linear interpolation between palette entries, but no wrap-around
} blend_type_t;

/// Representation of an HSV pixel (hue, saturation, value (aka brightness)).
typedef union {
	struct {
		uint8_t h;  ///< @copydoc hue
		uint8_t s;    ///< @copydoc saturation
		uint8_t v;    ///< @copydoc value
	}hsv;
	uint8_t val[3];
}hsv_t;

/// Representation of an RGB pixel (Red, Green, Blue)
typedef union {
	struct {
		uint8_t r;    ///< Red channel value
		uint8_t g;      ///< Green channel value
		uint8_t b;     ///< Blue channel value
	}rgb;
	uint8_t val[3];
}rgb_t;




/// Cloudy color palette	七彩云调色板
extern uint32c_t CloudColors_p[16];
/// Ocean colors, blues and whites	海洋的颜色，蓝色和白色
extern uint32c_t OceanColors_p[16];
/// Forest colors, greens	森林色，绿色
extern uint32c_t ForestColors_p[16];

/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint8_t gamma8(uint8_t b);
uint32_t color_rgbc(rgb_t *prgb);
void color_rgb(uint32_t c, rgb_t *prgb);
void color_hsv_to_rgb (hsv_t *phsv,rgb_t *prgb);
void color_hsv_to_rgb_spectrum(hsv_t *phsv,rgb_t *prgb);
void color_hsv_to_rgb_rainbow(hsv_t *phsv,rgb_t *prgb);
uint32_t scale32(uint32_t color, uint8_t scale);
uint32_t blend32(uint32_t color1, uint32_t color2, uint8_t blend);
void color_blend8(rgb_t *prgb1,rgb_t *prgb2, uint8_t blend);
uint32_t color_from_palette(uint8_t index, uint8_t brightness, blend_type_t blend_type, const uint32_t *palette, uint8_t palette_size);
uint32_t color_from_palette_hsv(uint8_t index, uint8_t brightness, blend_type_t blend_type, const uint32_t *palette, uint8_t palette_size);


#ifdef __cplusplus
}
#endif
#endif





