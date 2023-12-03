#include "shared/utils.h"
#include "shared/memory.h"

bool_t memory_bitmap_set_range(MemoryBitmapValue_t *memoryBitmap, size_t memoryBitmapLength,
                               physical_ptr_t startPage, size_t pageCount, bool_t isReserved)
{
    size_t totalBitmapPages = memoryBitmapLength * MEM_BITMAP_VALUE_PAGE_COUNT;
    if (startPage >= totalBitmapPages)
    {
        return FALSE;
    }

    for (physical_ptr_t currentPage = startPage; currentPage < (startPage + pageCount); currentPage++)
    {
        if (currentPage >= totalBitmapPages)
        {
            break;
        }
        size_t bitmapIndex = currentPage / MEM_BITMAP_VALUE_PAGE_COUNT;
        size_t bitIndex = currentPage % MEM_BITMAP_VALUE_PAGE_COUNT;

        if (isReserved)
        {
            memoryBitmap[bitmapIndex] |= (1ULL << bitIndex);
        }
        else
        {
            memoryBitmap[bitmapIndex] &= ~(1ULL << bitIndex);
        }
    }

    return TRUE;
}
