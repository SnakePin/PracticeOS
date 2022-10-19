#include <stddef.h>
#include <stdint.h>
#include "utils.h"

void* memcpy(void* dst, const void* src, size_t num)
{
    for (;num != 0; num--)
    {
        ((uint8_t*)dst)[num-1] = ((uint8_t*)src)[num-1];
    }
    return dst;
}

void* memset(void* ptr, int val, size_t num)
{
    for (;num != 0; num--)
    {
        ((uint8_t*)ptr)[num-1] = (uint8_t)val; //Should we use & here instead?
    }
    return ptr;
}
