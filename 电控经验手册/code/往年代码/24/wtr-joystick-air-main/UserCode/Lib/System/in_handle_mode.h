#pragma once
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Determine whether we are in thread mode or handler mode. */
static inline int InHandlerMode(void)
{
    int result;
    __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
    return result != 0;
}

#ifdef __cplusplus
}
#endif
