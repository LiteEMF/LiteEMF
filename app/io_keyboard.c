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
#include  "io_keyboard.h"
#include  "api_tick.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/
#if defined(MATRIX_KB_IN_GPIO) && defined(MATRIX_KB_OUT_GPIO)	&& defined(MATRIX_KB)	
//[IN],[OUT]
static uint8c_t matrix_kb_in_gpio[] = MATRIX_KB_IN_GPIO; 
static uint8c_t matrix_kb_out_gpio[] = MATRIX_KB_OUT_GPIO; 
static uint8c_t matrix_kb[countof(matrix_kb_out_gpio)][countof(matrix_kb_in_gpio)] = MATRIX_KB;
 

static void matrix_kb_init(void)
{
	uint8_t out = 0;
	uint8_t in = 0;
	for(out = 0; out < countof(matrix_kb_out_gpio); out++){
		//out 
		api_gpio_dir(matrix_kb_out_gpio[out], PIN_IN, PIN_PULLNONE);

		for(in = 0; in < countof(matrix_kb_in_gpio); in++){
			#if MATRIX_KB_ACTIVE
			api_gpio_dir(matrix_kb_in_gpio[out], PIN_IN, PIN_PULLDOWN);
			#else
			api_gpio_dir(matrix_kb_in_gpio[out], PIN_IN, PIN_PULLUP);
			#endif
		}
		
	}
}

/*******************************************************************
** Parameters:		
** Returns:		keyboard scan key num
** Description:		
*******************************************************************/
static uint8_t matrix_kb_scan(uint8_t* keyp, uint8_t len)
{
	uint8_t out;
	uint8_t in;
	uint8_t key;
	uint8_t k=0;
	for(out = 0; out < countof(matrix_kb_out_gpio); out++){
		//out 
		api_gpio_dir(matrix_kb_out_gpio[out], PIN_OUT, 0);
		api_gpio_out(matrix_kb_out_gpio[out], MATRIX_KB_ACTIVE);
		MATRIX_KB_DELAY();
		for(in = 0; in < countof(matrix_kb_in_gpio); in++){
			if(MATRIX_KB_ACTIVE == BOOL_SET(api_gpio_in( matrix_kb_in_gpio[in])) ){
				key = matrix_kb[out][in];
				if(key){
					if(k < len) {			
						keyp[k++] = key;
					}else{
						break;
					}
				}
			}
		}
		api_gpio_dir(matrix_kb_out_gpio[out], PIN_IN, PIN_PULLNONE);
	}

	return k;
}
#endif







#if defined(IO_KB_GPIO) && defined(IO_KB)

static const_t pin_t io_kb_gpio[] = IO_KB_GPIO; 
static uint8c_t io_kb[] = IO_KB;
 

static void iokb_init(void)
{
	uint8_t i;
	for(i = 0; i < countof(io_kb_gpio); i++){
		#if IO_KB_ACTIVE
		api_gpio_dir(io_kb_gpio[i], PIN_IN, PIN_PULLDOWN);
		#else
		api_gpio_dir(io_kb_gpio[i], PIN_IN, PIN_PULLUP);
		#endif
	}
}

/*******************************************************************
** Parameters:		
** Returns:		keyboard scan key num
** Description:		
*******************************************************************/
static uint8_t iokb_scan(uint8_t* keyp, uint8_t len)
{
	uint8_t key;
	uint8_t i,k=0;
	for(i = 0; i < countof(io_kb_gpio); i++){
		if(IO_KB_ACTIVE == BOOL_SET(api_gpio_in(io_kb_gpio[i])) ){
			key = io_kb[i];
			if(key){
				if(k < len) {			
					keyp[k++] = key;
				}else{
					break;
				}
			}
		}
	}
	return i;
}

#endif

	
/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:		keyboard scan key num
** Description:		
*******************************************************************/
uint8_t io_keyboard_scan(uint8_t* keyp, uint8_t len)
{
	uint8_t i = 0;

	#if defined(IO_KB_GPIO) && defined(IO_KB)
	i = matrix_kb_scan(keyp, len);
	#endif

	#if defined(MATRIX_KB_IN_GPIO) && defined(MATRIX_KB_OUT_GPIO) && defined(MATRIX_KB)	
	i += iokb_scan(keyp+i, len-i);
	#endif
	return i;
}

bool io_keyboard_init(void)
{
	#if defined(MATRIX_KB_IN_GPIO) && defined(MATRIX_KB_OUT_GPIO) && defined(MATRIX_KB)	
	matrix_kb_init();
	#endif

	#if defined(IO_KB_GPIO) && defined(IO_KB)
	iokb_init();
	#endif

	return true;
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool io_keyboard_deinit(void)
{
	return io_keyboard_init();
}

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
void io_keyboard_handler(void)
{

}








