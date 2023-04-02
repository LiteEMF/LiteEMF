#ifndef BREVENT_CONFIG_H
#define BREVENT_CONFIG_H

#define MBEDTLS_BIGNUM_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_MPI_WINDOW_SIZE 5


#ifdef __ARMCC_VERSION

#if __ARMCC_VERSION < 6000000

#ifdef MBEDTLS_HAVE_ASM
#undef MBEDTLS_HAVE_ASM

#warning "asm support on armcc5 is experimental"

#define MULADDC_INIT                        \
{                                           \
    mbedtls_mpi_uint r;                     \
    __asm(

#define MULADDC_CORE                        \
            "mov    r, #0           \n\t"   \
            "umlal  c, r, b, *s     \n\t"   \
            "adds   *d, *d, c       \n\t"   \
            "adc    c, r, #0        \n\t"   \
            "adds   s, s, #4        \n\t"   \
            "adds   d, d, #4        \n\t"

#define MULADDC_STOP                        \
    );                                      \
}

#endif

#elif !defined(__GNUC__)

#define __GNUC__

#endif

#endif

#if defined(__arm__) && (!defined(__ARMCC_VERSION) || __ARMCC_VERSION >= 6000000)

#ifndef __GNUC__
#warning "use gnu to use asm"
#endif

#ifndef __OPTIMIZE__
#warning "use -Ox to turn on optimize"
#endif

#if defined(__thumb__) && !defined(__thumb2__)
#warning "turn off thumb or use thumb2 to use quick asm"
#endif

#endif

#endif
