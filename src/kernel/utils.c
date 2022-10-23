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
        ((uint8_t*)ptr)[num-1] = (uint8_t)val;
    }
    return ptr;
}

size_t strlen(const char *str)
{
    size_t i = 0;
    while(str[i++] != '\x00');
    return --i; //Don't count the null terminator
}
