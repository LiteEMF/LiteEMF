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
#include "emf_typedef.h" 
#include "emf_math.h"

#if SIN8_ENABLE
#include "api/api_log.h"

//@ref: https://github.com/Aircoookie/WLED/ file .pio\libdeps\esp01_1m_full\FastLED\src\lib8tion\trig8.h
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/

/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/
/* A PROGMEM (flash mem) table containing 8-bit unsigned sine wave (0-255).
   Copy & paste this snippet into a Python REPL to regenerate:
import math
for x in range(256):
    print("{:3},".format(int((math.sin(x/128.0*math.pi))*127.5+0.5))),
    if x&15 == 15: print
*/


#if SIN8_TABLE_ENABLE
static uint8c_t Sine8Table[256] = {
    0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46,
    49, 52, 55, 57, 60, 63, 66, 68, 71, 73, 76, 78, 81, 83, 86, 88,
    90, 92, 94, 97, 99, 101, 102, 104, 106, 108, 109, 111, 112, 114, 115, 117,
    118, 119, 120, 121, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 126, 126, 125, 124, 124, 123, 122, 121, 120, 119,
    118, 117, 115, 114, 112, 111, 109, 108, 106, 104, 102, 101, 99, 97, 94, 92,
    90, 88, 86, 83, 81, 78, 76, 73, 71, 68, 66, 63, 60, 57, 55, 52,
    49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 12, 9, 6, 3,
    0, 254, 251, 248, 245, 241, 238, 235, 232, 229, 226, 223, 220, 217, 214, 211,
    208, 205, 202, 200, 197, 194, 191, 189, 186, 184, 181, 179, 176, 174, 171, 169,
    167, 165, 163, 160, 158, 156, 155, 153, 151, 149, 148, 146, 145, 143, 142, 140,
    139, 138, 137, 136, 135, 134, 133, 133, 132, 131, 131, 130, 130, 130, 130, 130, 
    130, 130, 130, 130, 130, 130, 131, 131, 132, 133, 133, 134, 135, 136, 137, 138,
    139, 140, 142, 143, 145, 146, 148, 149, 151, 153, 155, 156, 158, 160, 163, 165,
    167, 169, 171, 174, 176, 179, 181, 184, 186, 189, 191, 194, 197, 200, 202, 205,
    208, 211, 214, 217, 220, 223, 226, 229, 232, 235, 238, 241, 245, 248, 251, 254,
};
#endif

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/// Fast 8-bit approximation of sin(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (int8_t)(sin(i/128.0*pi) * 128.0));
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between -128 and 127
int8_t sin8( uint8_t theta)
{
	#if SIN8_TABLE_ENABLE
	return Sine8Table[theta];
	#else
	int8_t y;
    uint8_t offset = theta;
	uint8_t secoffset,section,s2,b,m16,mx;
	const uint8_t* p;
	static uint8c_t interleave[] = { 0, 49, 49, 41, 90, 27, 117, 10 };

    if( theta & 0x40 ) {
        offset = (uint8_t)255 - offset;
    }
    offset &= 0x3F; // 0..63

    secoffset  = offset & 0x0F; // 0..15
    if( theta & 0x40) ++secoffset;

    section = offset >> 4; // 0..3
    s2 = section * 2;
    p = (const uint8_t*)interleave;
    p += s2;
    b = *p;
    ++p;
    m16 = *p;

    mx = (m16 * secoffset) >> 4;

    y = mx + b;
    if( theta & 0x80 ) y = -y;

    return y;
	#endif
}

/// Fast 8-bit approximation of cos(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (int8_t)(cos(i/128.0*pi) * 128.0));
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between -128 and 127
int8_t cos8( uint8_t theta)
{
    return sin8( theta + 64);
}

#endif


#if SIN16_ENABLE
/// Fast 16-bit approximation of sin(x). This approximation never varies more than
/// 0.69% from the floating point value you'd get by doing
///
///     float s = sin(x/32768.0*pi) * 32767.0;
///
/// @param theta input angle from 0-65535
/// @returns sin of theta, value between -32767 to 32767.
int16_t sin16( uint16_t theta )
{
	int16_t y;
	uint8_t section,secoffset8,m;
	uint16_t offset,b, mx;
    static uint16c_t base[] = { 0, 6393, 12539, 18204, 23170, 27245, 30273, 32137 };
    static uint8c_t slope[] = { 49, 48, 44, 38, 31, 23, 14, 4 };

    offset = (theta & 0x3FFF) >> 3; // 0..2047
    if( theta & 0x4000 ) offset = 2047 - offset;

    section = offset / 256; // 0..7
    b   = base[section];
    m   = slope[section];

    secoffset8 = (uint8_t)(offset) / 2;

    mx = m * secoffset8;
    y  = mx + b;

    if( theta & 0x8000 ) y = -y;

    return y;
}

/// Fast 16-bit approximation of cos(x). This approximation never varies more than
/// 0.69% from the floating point value you'd get by doing
///
///     float s = cos(x) * 32767.0;
///
/// @param theta input angle from 0-65535
/// @returns sin of theta, value between -32767 to 32767.
int16_t cos16( uint16_t theta)
{
    return sin16( theta + 16384);
}
#endif

