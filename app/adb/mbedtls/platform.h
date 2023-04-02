#ifndef BREVENT_PLATFORM_H
#define BREVENT_PLATFORM_H

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG_MEMORY
#define mbedtls_printf      printf

static void *mbedtls_calloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    fprintf(stderr, "calloc %p %zu %zu\n", p, count, size);
    return p;
}

static void mbedtls_free(void *ptr) {
    free(ptr);
    fprintf(stderr, "free %p\n", ptr);
}
#else
#define mbedtls_printf      printf
#define mbedtls_calloc      calloc
#define mbedtls_free        free
#endif

#endif
