/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/

#ifndef _EMF_ERROR_H__
#define _EMF_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif


//可以自定义ERROR 等级
#define ERR_USB         0x20   
#define ERR_RF          0x40   
#define ERR_VENDOR      0x80

#ifndef ASSER_EN
#define ASSER_EN        0
#endif


typedef enum{
  ERROR_SUCCESS     = 0 ,     //< Successful command
  ERROR_FAILE       = 1 ,     //FAILE
  ERROR_DISCON      = 2 ,     //DISCONNECTED
  ERROR_NOT_FOUND   = 3 ,     //< Not found
  ERROR_STALL       = 4 ,     // ERR STALL
  ERROR_BUSY        = 5 ,     //< Busy
  ERROR_TIMEOUT     = 6 ,     //< Operation timed out
  ERROR_NULL        = 7 ,     //< Null Pointer
  ERROR_NO_MEM      = 8 ,     //< No Memory for operation
  ERROR_PARAM       = 9 ,     //< Invalid Parameter
  ERROR_STATE       = 10 ,     //< Invalid state, operation disallowed in this state
  ERROR_LENGTH      = 11 ,     //< Invalid Length
  ERROR_DATA        = 12 ,     //< Invalid Data
  ERROR_ADDR        = 13 ,     //< Bad Memory Address
  ERROR_NACK        = 14 ,     //< Bad Memory Address
  ERROR_UNSUPPORT   = 15 ,     //< Not supported
  ERROR_UNKNOW      = 16 ,     //< Busy
}error_t;


#if ASSER_EN
#define EMF_ASSERT(expr)                                 \
  do {                                                     \
      if (!(expr)) {                                       \
          emf_assert_internal(#expr, __FILE__, __LINE__);  \
      }                                                    \
  } while (0)
#else
#define EMF_ASSERT(expr)  
#endif

/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern void emf_assert_internal(const char *expr, const char *file, int line);

#ifdef __cplusplus
}
#endif
#endif
