/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
*/

#ifndef _COMPILER_ABSTRACTION_H
#define _COMPILER_ABSTRACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#define	WEAK_ENABLE         1



#if defined ( __CC_ARM )

    #ifndef __ASM
        #define __ASM               __asm
    #endif

    #ifndef __INLINE
        #define __INLINE            __inline
    #endif

    #ifndef __WEAK
        #define __WEAK              __weak
    #endif

    #ifndef __ALIGN
        #define __ALIGN(n)          __align(n)
    #endif

    #ifndef __PACKED
        #define __PACKED            __packed
    #endif

    #define GET_SP()                __current_sp()

#elif defined ( __ICCARM__ )

    #ifndef __ASM
        #define __ASM               __asm
    #endif

    #ifndef __INLINE
        #define __INLINE            inline
    #endif

    #ifndef __WEAK
        #define __WEAK              __weak
    #endif

    #ifndef __ALIGN
        #define STRING_PRAGMA(x) _Pragma(#x)
        #define __ALIGN(n) STRING_PRAGMA(data_alignment = n)
    #endif

    #ifndef __PACKED
        #define __PACKED            __packed
    #endif
    
    #define GET_SP()                __get_SP()

#elif defined   ( __GNUC__ )

    #ifndef __ASM
        #define __ASM               __asm
    #endif

    #ifndef __INLINE
        #define __INLINE            inline
    #endif

    #ifndef __WEAK
        #define __WEAK              __attribute__((weak))
    #endif

    #ifndef __ALIGN
        #define __ALIGN(n)          __attribute__((aligned(n)))
    #endif

    #ifndef __PACKED
        #define __PACKED           __attribute__((packed)) 
    #endif

    #define GET_SP()                gcc_current_sp()

    // static inline unsigned int gcc_current_sp(void)
    // {
    //     register unsigned sp __ASM("sp");
    //     return sp;
    // }

#elif defined   ( __TASKING__ )

    #ifndef __ASM
        #define __ASM               __asm
    #endif

    #ifndef __INLINE
        #define __INLINE            inline
    #endif

    #ifndef __WEAK
        #define __WEAK              __attribute__((weak))
    #endif

    #ifndef __ALIGN
        #define __ALIGN(n)          __align(n)
    #endif

    #ifndef __PACKED
        #define __PACKED
    #endif

    #define GET_SP()                __get_MSP()

#elif defined(__C51__)

    #ifndef __ASM
        #define __ASM               __asm
    #endif

    #ifndef __INLINE
        #define __INLINE            
    #endif

    #undef  WEAK_ENABLE
	#define	WEAK_ENABLE             0			//C51 not support

    #ifndef __WEAK 
        #define  __WEAK              
    #endif

    #ifndef __ALIGN
        #define __ALIGN(n)         
    #endif

    #ifndef __PACKED
        #define __PACKED            /*== #pragma pack(1)*/ 
    #endif

    #define pack(x)                  /*for #pragma pack(n) used*/

    #define GET_SP(x)  

    #if defined( __MODEL__ ) && ( 2 == __MODEL__ )     //TODO
	    #define __BIG_ENDIAN
    #endif

#elif defined (__clang_version__)           //add clang compiler support
    #ifndef __ASM
        #define __ASM               __asm__
    #endif

    #ifndef __INLINE
        #define __INLINE           __inline__
    #endif

    #ifndef __WEAK
        #define __WEAK              __attribute__((weak))
    #endif

    #ifndef __ALIGN
        #define __ALIGN(n)          __attribute__((aligned(n)))
    #endif

    #ifndef __PACKED
        #define __PACKED           __attribute__((packed)) 
    #endif

    #define GET_SP()        
#else
	#error undefined c complier
#endif

#ifdef __cplusplus
}
#endif
#endif
