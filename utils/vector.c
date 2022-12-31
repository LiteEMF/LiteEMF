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
** Description:		vector operation
*******************************************************************/
void constrain_axis2(axis2i_t* axisp)
{
    CONSTRAIN_AXIS2(axisp);
}
void constrain_axis2i(axis2l_t* axisp)
{
    CONSTRAIN_AXIS2i(axisp);
}
void constrain_axis3(axis3i_t* axisp)
{
    CONSTRAIN_AXIS2(axisp);
}
void constrain_axis3i(axis3l_t* axisp)
{
    CONSTRAIN_AXIS3i(axisp);
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		vector operation
*******************************************************************/
void  axis2i_add( axis2i_t* axisp, axis2i_t* a)
{
    AXIS2_ADD(axisp,a);
}
void  axis3i_add( axis3i_t* axisp, axis3i_t* a)
{
    AXIS3_ADD(axisp,a);
}
void  axis2l_add( axis2l_t* axisp, axis2l_t* a)
{
    AXIS2_ADD(axisp,a);
}
void  axis3l_add( axis3l_t* axisp, axis3l_t* a)
{
    AXIS3_ADD(axisp,a);
}
void  axis2f_add( axis2f_t* axisp, axis2f_t* a)
{
    AXIS2_ADD(axisp,a);
}
void  axis3f_add( axis3f_t* axisp, axis3f_t* a)
{
    AXIS3_ADD(axisp,a);
}

void  axis2i_sub( axis2i_t* axisp, axis2i_t* a)
{
    AXIS2_SUB(axisp,a);
}
void  axis3i_sub( axis3i_t* axisp, axis3i_t* a)
{
    AXIS3_SUB(axisp,a);
}
void  axis2l_sub( axis2l_t* axisp, axis2l_t* a)
{
    AXIS2_SUB(axisp,a);
}
void  axis3l_sub( axis3l_t* axisp, axis3l_t* a)
{
    AXIS3_SUB(axisp,a);
}
void  axis2f_sub( axis2f_t* axisp, axis2f_t* a)
{
    AXIS2_SUB(axisp,a);
}
void  axis3f_sub( axis3f_t* axisp, axis3f_t* a)
{
    AXIS3_SUB(axisp,a);
}

void  axis2i_mul( axis2i_t* axisp,int32_t k)
{
   AXIS2_MUL(axisp,k);
}
void  axis3i_mul( axis3i_t* axisp,int32_t k)
{
    AXIS3_MUL(axisp,k);
}
void  axis2l_mul( axis2l_t* axisp,int32_t k)
{
    AXIS2_MUL(axisp,k);
}
void  axis3l_mul( axis3l_t* axisp,int32_t k)
{
    AXIS3_MUL(axisp,k);
}
void  axis2f_mul( axis2f_t* axisp,float k)
{
    AXIS2_MUL(axisp,k);
}
void  axis3f_mul( axis3f_t* axisp,float k)
{
    AXIS3_MUL(axisp,k);
}


void  axis2i_div( axis2i_t* axisp,int32_t k)
{
    AXIS2_DIV(axisp,k);
}
void  axis3i_div( axis3i_t* axisp,int32_t k)
{
    AXIS3_DIV(axisp,k);
}
void  axis2l_div( axis2l_t* axisp,int32_t k)
{
    AXIS2_DIV(axisp,k);
}
void  axis3l_div( axis3l_t* axisp,int32_t k)
{
    AXIS3_DIV(axisp,k);
}
void  axis2f_div( axis2f_t* axisp,float k)
{
    AXIS2_DIV(axisp,k);
}
void  axis3f_div( axis3f_t* axisp,float k)
{
    AXIS3_DIV(axisp,k);
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
