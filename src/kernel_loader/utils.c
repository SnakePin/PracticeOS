#include <stddef.h>
#include <stdint.h>
#include "utils.h"

int memcmp(const void *s1, const void *s2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        uint8_t s1val = *((uint8_t*)s1+i);
        uint8_t s2val = *((uint8_t*)s2+i);
        if(s1val != s2val) {
            return (s1val > s2val) ? 1 : -1;
        }
    }
    return 0;
}

void *memcpy(void *dst, const void *src, size_t num)
{
    for (; num != 0; num--)
    {
        ((uint8_t *)dst)[num - 1] = ((uint8_t *)src)[num - 1];
    }
    return dst;
}

void *memset(void *ptr, int val, size_t num)
{
    for (; num != 0; num--)
    {
        ((uint8_t *)ptr)[num - 1] = (uint8_t)val;
    }
    return ptr;
}

size_t strlen(const char *str)
{
    size_t i = 0;
    while (str[i++] != '\x00')
        ;
    return --i; // Don't count the null terminator
}
