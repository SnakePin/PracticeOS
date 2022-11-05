#pragma once
#include <stddef.h>

int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t num);
void *memset(void *ptr, int val, size_t num);

#define NORETURN_ATTR __attribute__((noreturn))
#define CDECL_ATTR __attribute__((cdecl))
#define PACKED_ATTR __attribute__((packed))
#define ALIGN_ADDRESS_CEIL(x, align) ((uintptr_t)(x) + ((align) - (uintptr_t)(x) % (align)))
#define ALIGN_ADDRESS_FLOOR(x, align) ((uintptr_t)(x) - ((uintptr_t)(x) % (align)))
