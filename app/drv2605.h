
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

#ifndef __DRV2605_H__
#define __DRV2605_H__

#include "emf_typedef.h" 

/******************************************************************************************************
** Defined
*******************************************************************************************************/
#define IIC_DRV2605_ID      0
#define DRV2605_ADDRESS 	(0x5A<<1)

#define STATUS_Reg          0x00
#define MODE_Reg            0x01
#define RTP_INPUT_Reg       0x02
#define LIB_SEL_Reg         0x03
#define WAV_SEQ1_Reg        0x04
#define WAV_SEQ2_Reg        0x05
#define WAV_SEQ3_Reg        0x06
#define WAV_SEQ4_Reg        0x07
#define WAV_SEQ5_Reg        0x08
#define WAV_SEQ6_Reg        0x09
#define WAV_SEQ7_Reg        0x0A
#define WAV_SEQ8_Reg        0x0B
#define GO_Reg              0x0C
#define ODT_OFFSET_Reg      0x0D
#define SPT_Reg             0x0E
#define SNT_Reg             0x0F
#define BRT_Reg             0x10
#define ATV_CON_Reg         0x11
#define ATV_MIN_IN_Reg      0x12
#define ATV_MAX_IN_Reg      0x13
#define ATV_MIN_OUT_Reg     0x14
#define ATV_MAX_OUT_Reg     0x15
#define RATED_VOLTAGE_Reg   0x16
#define OD_CLAMP_Reg        0x17
#define A_CAL_COMP_Reg      0x18
#define A_CAL_BEMF_Reg      0x19
#define FB_CON_Reg          0x1A
#define CONTRL1_Reg         0x1B
#define CONTRL2_Reg         0x1C
#define CONTRL3_Reg         0x1D
#define CONTRL4_Reg         0x1E
#define VBAT_MON_Reg        0x21
#define LRA_RESON_Reg       0x22


/*** Register Bits ***/
/*
#define Dev_Reset           (0x80)
#define STANDBY             (0x40)
#define ACTIVE              (0x00)
*/

/*****************************************************************************************************
**  Function
******************************************************************************************************/
bool drv2605_init(void);
bool drv2605_deinit(void);
bool drv2605_AutoCal(void);
bool drv2605_Play_Waveform(uint8_t ucEffect);


#endif /*  __DRV2605_H__ */
