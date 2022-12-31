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
#include  "pinmap.h"

/*****************************************************************************************************
**  Function
******************************************************************************************************/
uint32_t get_pinmap_peripheral(pin_t pin, const pin_map_t* p_pinmap, uint8_t map_count)
{
    uint8_t i = 0;
    for ( i = 0; i < map_count; i++){
        if (p_pinmap->pin == pin) {
            return p_pinmap->peripheral;
        }
        p_pinmap++;
    }
    return 0;
}

uint32_t get_pinmap_att(pin_t pin, const pin_map_t* p_pinmap, uint8_t map_count) 
{
    uint8_t i = 0;
    for ( i = 0; i < map_count; i++){
        if (p_pinmap->pin == pin) {
            return p_pinmap->att;
        }
        p_pinmap++;
    }
    return (uint32_t)0;
}

uint8_t get_pinmap_index(pin_t pin,const pin_map_t *p_pinmap, uint8_t map_count)          
{
    uint8_t i = 0;
    for ( i = 0; i < map_count; i++){
        if (p_pinmap->pin == pin) {
            break;
        }
        p_pinmap++;
    }
    return i;
}








