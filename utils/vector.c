/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*   opensource.org/licenses/BSD-2-Clause
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include "vector.h"

/******************************************************************************************************
** Defined
*******************************************************************************************************/




/*******************************************************************
** Parameters:		
** Returns:	
** Description:		vector swap
*******************************************************************/
void  axis3i_swapl( axis3i_t* p)
{
    p->x = SWAP16_L(p->x);
    p->y = SWAP16_L(p->y);
    p->z = SWAP16_L(p->z);
}
void  axis3i_swaph( axis3i_t* p)
{
    p->x = SWAP16_H(p->x);
    p->y = SWAP16_H(p->y);
    p->z = SWAP16_H(p->z);
}
void  axis2i_swapl( axis2i_t* p)
{
    p->x = SWAP16_L(p->x);
    p->y = SWAP16_L(p->y);
}
void  axis2i_swaph( axis2i_t* p)
{
    p->x = SWAP16_H(p->x);
    p->y = SWAP16_H(p->y);
}
void  axis3l_swapl( axis3l_t* p)
{
    p->x = SWAP32_H(p->x);
    p->y = SWAP32_H(p->y);
    p->z = SWAP32_H(p->z);
}
void  axis3l_swaph( axis3l_t* p)
{
    p->x = SWAP32_H(p->x);
    p->y = SWAP32_H(p->y);
    p->z = SWAP32_H(p->z);
}
void  axis2l_swapl( axis2l_t* p)
{
    p->x = SWAP32_H(p->x);
    p->y = SWAP32_H(p->y);
}
void  axis2l_swaph( axis2l_t* p)
{
    p->x = SWAP32_H(p->x);
    p->y = SWAP32_H(p->y);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		vector normalization
*******************************************************************/
void vector2f_normalization(vector2f_t* vectorp)
{
	vectorp->r = sqrt(vectorp->x*vectorp->x + vectorp->y*vectorp->y);
    if(vectorp->r < 0.0001){
        vectorp->x = 0;
        vectorp->y = 0;
    }else{
        vectorp->x = vectorp->x / vectorp->r;
        vectorp->y = vectorp->y / vectorp->r;
    }
}
void vector3f_normalization(vector3f_t* vectorp)
{
	vectorp->r = sqrt(vectorp->x*vectorp->x + vectorp->y*vectorp->y + vectorp->z*vectorp->z);
    if(vectorp->r < 0.0001){
        vectorp->x = 0;
        vectorp->y = 0;
		vectorp->z = 0;
    }else{
        vectorp->x = vectorp->x / vectorp->r;
        vectorp->y = vectorp->y / vectorp->r;
		vectorp->z = vectorp->z / vectorp->r;
    }
}
