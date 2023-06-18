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


#ifndef _km_typedef_h
#define _km_typedef_h
#include "utils/emf_typedef.h" 

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************************************
** Defined
*******************************************************************************************************/
//keyboard page 0X04~0X81
#define    KB_NONE      0x00
#define    KB_A			0x04
#define    KB_B			0x05
#define    KB_C			0x06
#define    KB_D			0x07
#define    KB_E			0x08
#define    KB_F			0x09
#define    KB_G			0x0A
#define    KB_H			0x0B
#define    KB_I			0x0C
#define    KB_J			0x0D
#define    KB_K			0x0E
#define    KB_L			0x0F
#define    KB_M			0X10
#define    KB_N			0X11
#define    KB_O			0X12
#define    KB_P			0X13
#define    KB_Q			0X14
#define    KB_R			0X15
#define    KB_S			0X16
#define    KB_T			0X17
#define    KB_U			0X18
#define    KB_V			0X19
#define    KB_W			0X1A
#define    KB_X			0X1B
#define    KB_Y			0X1C
#define    KB_Z			0X1D
#define    KB_1			0X1E
#define    KB_2			0X1F
#define    KB_3			0X20
#define    KB_4			0X21
#define    KB_5			0X22
#define    KB_6			0X23
#define    KB_7			0X24
#define    KB_8			0X25
#define    KB_9			0X26
#define    KB_0			0X27
#define    KB_RETURN	0X28
#define    KB_ESC		0X29		
#define    KB_BACKSPACE	0X2A
#define    KB_TAB		0X2B
#define    KB_SPACE		0X2C
#define	   KB_SUB		0X2D				/*  - _ */
#define    KB_PLUS		0X2E				/*  = + */
#define    KB_LBRACE	0X2F				/*  [   */
#define    KB_RBRACE	0X30  		        /*  ]   */
#define    KB_BSLASH	0X31			    /*  \ | */
#define    KB_NON_US	0x32
#define    KB_COLON		0X33				/*  : ;  */
#define    KB_DSPOT		0X34				/*  '   */
#define    KB_USPOT		0X35				/*  `   */
#define    KB_SPOT		0X36				/*  ,   */
#define    KB_DOT		0X37				/*  .   */
#define    KB_SLASH		0X38				/*  / ? */
#define    KB_CAP_LOCK	0X39
#define    KB_F1		0X3A
#define    KB_F2		0X3B
#define    KB_F3		0X3C
#define    KB_F4		0X3D
#define    KB_F5		0X3E
#define    KB_F6		0X3F
#define    KB_F7		0X40
#define    KB_F8		0X41
#define    KB_F9		0X42
#define    KB_F10		0X43
#define    KB_F11		0X44
#define    KB_F12		0X45

#define    KB_PRINT_SCR	0X46
#define    KB_SCROLL	0X47
#define    KB_PAUSE		0X48
#define    KB_INSERT	0X49
#define    KB_HOME		0X4A
#define    KB_PAGE_UP	0x4b
#define    KB_DEL		0x4c
#define    KB_END		0x4d
#define	   KB_PAGE_DOWN	0x4e
#define	   KB_RIGHT		0x4f
#define	   KB_LEFT		0x50
#define	   KB_DOWN		0x51
#define	   KB_UP		0x52
#define	   KB_NUM		0x53
#define	   KB_NUMSLASH	0x54
#define	   KB_NUMSTAR	0x55
#define	   KB_NUMPLUS	0x56
#define	   KB_NUMSUB	0x57
#define	   KB_NUMENTER	0x58
#define	   KB_NUM1		0x59
#define	   KB_NUM2		0x5A
#define	   KB_NUM3		0x5B
#define	   KB_NUM4		0x5C
#define	   KB_NUM5		0x5D
#define	   KB_NUM6		0X5E
#define	   KB_NUM7		0X5F
#define	   KB_NUM8		0X60
#define	   KB_NUM9		0X61
#define	   KB_NUM0		0X62
#define    KB_NUMDEL 	0X63

#define    KB_APP 		0X65
#define    KB_POWER 	0X66
#define    KB_EQU 		0X67		/*  =   */
#define    KB_F13 		0X68
#define    KB_F14 		0X69
#define    KB_F15 		0X6A
#define    KB_F16 		0X6B
#define    KB_F17 		0X6C
#define    KB_F18 		0X6D
#define    KB_F19 		0X6E		/*hide mouse key*/
#define    KB_F20 		0X6F
#define    KB_F21 		0X70
#define    KB_F22 		0X71
#define    KB_F23 		0X72
#define    KB_F24 		0X73

#define    KB_EXECUTE 	0X74
#define    KB_HELP 		0X75
#define    KB_MENU 		0X76
#define    KB_SELECT 	0X77
#define    KB_STOP 		0X78
#define    KB_AGAIN 	0X79
#define    KB_UNDO 		0X7A
#define    KB_CUT 		0X7B
#define    KB_COPY 		0X7C
#define    KB_PASTE 	0X7D
#define    KB_FIND 		0X7E
#define    KB_MUTE 		0X7F
#define    KB_VOL_UP 	0X80
#define    KB_VOL_DOWN 	0X81
// #define    KB_LOCK_CAPS 	0X82
// #define    KB_LOCK_NUM 	   0X83
// #define    KB_LOCK_SCROLL   0X84



//0X84 ~ 0XDF  used for user vendor 92
// ctrl + key	0x80+KEY	range: 0x84~0xAF
// alt +  key	0xB0+KEY	range: 0XB4~0XDF
#define KB_COMB_MIN    	0X84
#define KB_COMB_MAX    	0XDF
#define KB_CTRL_COMB	0X80
#define KB_ALT_COMB		0XB0

//E0~EF
#define    KB_LCTR_BIT		BIT(0)
#define    KB_LSHIFT_BIT	BIT(1)
#define    KB_LALT_BIT		BIT(2)
#define    KB_LWIN_BIT		BIT(3)
#define    KB_RCTR_BIT		BIT(4)
#define    KB_RSHIFT_BIT	BIT(5)
#define    KB_RALT_BIT		BIT(6)
#define    KB_RWIN_BIT		BIT(7)
#define    KB_LCTR			0XE0
#define    KB_LSHIFT		0XE1
#define    KB_LALT			0XE2
#define    KB_LWIN			0XE3
#define    KB_RCTR			0XE4
#define    KB_RSHIFT		0XE5
#define    KB_RALT			0XE6
#define    KB_RWIN			0XE7

#define    KB_FN 	  		0XE8		/*vendor*/
#define    KB_M1	  		0XE9		/*vendor*/
#define    KB_M2 			0XEA		/*vendor*/
#define    KB_M3			0XEB		/*vendor*/
#define    KB_M4			0XEC		/*vendor*/
#define    KB_M5			0XED		/*vendor*/


//0XF0~FF
#define   MOUSE_LEFT_BIT		BIT(0)
#define   MOUSE_RIGHT_BIT		BIT(1)
#define   MOUSE_MID_BIT			BIT(2)
#define   MOUSE_SIDDOWN_BIT		BIT(3)
#define   MOUSE_SIDUP_BIT		BIT(4)
#define   MOUSE_KEY				(0XF0)
#define   MOUSE_LEFT			(0XF0)
#define   MOUSE_RIGHT			(0XF1)
#define   MOUSE_MID				(0XF2)
#define   MOUSE_SIDDOWN			(0XF3)
#define   MOUSE_SIDUP			(0XF4)
#define   MOUSE_WHILE_D			(0XF5)
#define   MOUSE_WHILE_U			(0XF6)
#define   MOUSE_WHILE			(0XF7)
#define   MOUSE_XY				(0XFE)

#define   KB_DEFAULT			(0XFF)


// consumer page 
#define   CONSUMER_MENU			    0X40
#define   CONSUMER_NEXT			    0xB5
#define   CONSUMER_PREVIOUS		  	0xB6
#define   CONSUMER_STOP		  		0xB7
#define   CONSUMER_KEYBOARD		  	0XB8
#define   CONSUMER_PLAYPAUSE	  	0xCD
#define   CONSUMER_VOL_MUTE 	 	0xE2
#define   CONSUMER_VOL_INC	  		0xE9
#define   CONSUMER_VOL_DEC	  		0xEA
#define   CONSUMER_HOME			    0X223
#define   CONSUMER_BACK			    0X224



#ifndef pack
#pragma pack(1)
#endif
typedef struct 
{
    uint8_t id;
	uint8_t but;
	int16_t x;	
	int16_t y;	
	int8_t w;	
} mouse_t;
#ifndef PRAGMA_PACK_IGNORED
#pragma pack()
#endif

typedef struct 
{
    uint8_t id;
    uint8_t fn;
    uint8_t res;
	uint8_t key[6];
} kb_t;

typedef struct 
{
    uint8_t id;
    uint8_t fn;
	uint8_t key[16];     /*0~128*/
} kb_bit_t;

typedef union{
    struct{
        uint8_t numLock       : 1;
        uint8_t capsLock      : 1;
        uint8_t scrollLock    : 1;
        uint8_t res : 5;
    }bits;
    uint8_t val;
}kb_ledd_t;


#ifdef __cplusplus
}
#endif
#endif





