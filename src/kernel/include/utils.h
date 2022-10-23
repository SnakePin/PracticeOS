#pragma once
#include <stddef.h>

void* memcpy(void* dst, const void* src, size_t num);
void* memset(void* ptr, int val, size_t num);
size_t strlen(const char *str);

#define CDECL_ATTR __attribute__((cdecl))
#define PACKED_ATTR __attribute__((packed))
#define SECTION_ATTR(x) __attribute__ ((section (#x)))
#define MAKE_SEGMENT_SELECTOR(index, ti, rpl) (index<<3 | ti<<2 | rpl)
#define IS_IN_RANGE(i, min, max) ((i >= min) && (i <= max))
#define LINKER_VAR(x) (&x)
