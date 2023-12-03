#include <stdint.h>
#include "shared/utils.h"

// This must be changed if we want to support PAE in 32-bit mode
typedef uintptr_t physical_ptr_t;
typedef uint8_t MemoryBitmapValue_t;

#define MEM_BITMAP_VALUE_PAGE_COUNT (sizeof(MemoryBitmapValue_t) * 8)
#define PHY_NULL 0

bool_t memory_bitmap_set_range(MemoryBitmapValue_t *memoryBitmap, size_t memoryBitmapLength,
                               physical_ptr_t startPage, size_t pageCount, bool_t isReserved);
