#include <stddef.h>
#include "memory/paging.h"
#include "memory/memory.h"
#include "memory/internal.h"
#include "int_gate16/gate.h"

#define EFLAGS_CF_MASK 0x1

static bool_t set_mem_bitmap_pages(physical_ptr_t firstPage, size_t count, bool_t isReserved);
static void resize_mem_bitmap();
static void update_memmap_acpi();
static size_t ACPIMemoryMapLength = 0;
static MemoryACPIMapEntry_t ACPIMemoryMap[ACPI_MEMORY_MAP_LENGTH];

/*
 * A pointer to the bitmap of allocated/reserved pages, first value is set by entry.s
 */
MemoryBitmapValue_t *memory_phy_bitmap;
size_t memory_phy_bitmap_count = 0;

void memory_phy_init()
{
    update_memmap_acpi();
    resize_mem_bitmap();
}

physical_ptr_t memory_phy_allocate_aligned(size_t size)
{
    size_t pageCount = LEN_TO_PAGE(size);
    size_t contiguosCounter = 0;
    for (size_t i = 0; i < memory_phy_bitmap_count; i++)
    {
        for (size_t j = 0; j < MEM_BITMAP_VALUE_PAGE_COUNT; j++)
        {
            MemoryBitmapValue_t mask = 1 << j;
            if (!(memory_phy_bitmap[i] & mask))
            {
                contiguosCounter++;
            }
            else
            {
                contiguosCounter = 0;
            }

            if (contiguosCounter == pageCount)
            {
                physical_ptr_t firstPage = (i * MEM_BITMAP_VALUE_PAGE_COUNT) + j - (pageCount - 1);
                set_mem_bitmap_pages(firstPage, pageCount, TRUE);
                return firstPage * PAGE_SIZE;
            }
        }
    }

    return PHY_NULL;
}

void memory_phy_free(physical_ptr_t pointer, size_t size)
{
    set_mem_bitmap_pages(ADDR_TO_PAGE(pointer), LEN_TO_PAGE(size), FALSE);
}

void memory_phy_reserve(physical_ptr_t pointer, size_t size)
{
    set_mem_bitmap_pages(ADDR_TO_PAGE(pointer), LEN_TO_PAGE(size), TRUE);
}

static void resize_mem_bitmap()
{
    size_t newMaxPageCount = 0;
    for (size_t i = 0; i < ACPIMemoryMapLength; i++)
    {
        if (ACPIMemoryMap[i].Type != (uint32_t)Usable)
        {
            // Skip non-usable pages
            continue;
        }
        //+1 cause we need the length
        physical_ptr_t pageCount = ADDR_TO_PAGE(ACPIMemoryMap[i].BaseAddress + ACPIMemoryMap[i].Length) + 1;
        newMaxPageCount = MAX(newMaxPageCount, pageCount);
    }

    // Allocate using the old bitmap
    // TODO: virtual allocate here
    size_t oldBitmapSize = memory_phy_bitmap_count * sizeof(MemoryBitmapValue_t);
    void *oldBitmap = memory_phy_bitmap;

    size_t newBitmapSize = (newMaxPageCount / MEM_BITMAP_VALUE_PAGE_COUNT) * sizeof(MemoryBitmapValue_t);
    void *newBitmap = (void *)memory_phy_allocate_aligned(newBitmapSize);

    memset(newBitmap, 0, newBitmapSize);
    memcpy(newBitmap, oldBitmap, MIN(oldBitmapSize, newBitmapSize));

    // Swap the bitmap
    memory_phy_bitmap = newBitmap;
    memory_phy_bitmap_count = newMaxPageCount / MEM_BITMAP_VALUE_PAGE_COUNT;

    // TODO: virtual free here
    memory_phy_free((physical_ptr_t)oldBitmap, oldBitmapSize);

    for (size_t i = 0; i < ACPIMemoryMapLength; i++)
    {
        if (ACPIMemoryMap[i].Type == (uint32_t)Usable)
        {
            // Skip usable pages
            continue;
        }
        memory_phy_reserve(ACPIMemoryMap[i].BaseAddress, ACPIMemoryMap[i].Length);
    }
}

static void update_memmap_acpi()
{
    ACPIMemoryMapLength = 0;
    INTGate16Registers_t outRegs;
    INTGate16Registers_t inRegs = {.eax = ACPI_E820_EAX,
                                   .ebx = 0,
                                   .edi = ACPI_E820_BUFFER,
                                   .ecx = ACPI_E820_BUFFER_SIZE,
                                   .edx = ACPI_E820_SIGNATURE};
    do
    {
        int_gate16_make_call(ACPI_E820_INT, &inRegs, &outRegs);
        if (outRegs.eax != ACPI_E820_SIGNATURE)
        {
            // This isn't supposed to happen but it can on some BIOSes
            break;
        }
        if (outRegs.eflags_ro & EFLAGS_CF_MASK)
        {
            // Apparently this means that we've reached the entry after the last one on some BIOSes
            break;
        }
        memcpy((void *)&ACPIMemoryMap[ACPIMemoryMapLength], (void *)ACPI_E820_BUFFER, sizeof(MemoryACPIMapEntry_t));
        ACPIMemoryMapLength += 1;
        inRegs.ebx = outRegs.ebx; // Continuation value

    } while (outRegs.ebx != 0); // EBX==0 means that we just got the last entry
}

static bool_t set_mem_bitmap_pages(physical_ptr_t firstPage, size_t count, bool_t isReserved)
{
    size_t totalPageCount = memory_phy_bitmap_count * MEM_BITMAP_VALUE_PAGE_COUNT;
    if (firstPage >= totalPageCount)
    {
        return FALSE;
    }
    for (physical_ptr_t currentPage = firstPage; currentPage < (firstPage + count); currentPage++)
    {
        if (currentPage >= totalPageCount)
        {
            break;
        }
        size_t bitmapIndex = currentPage / MEM_BITMAP_VALUE_PAGE_COUNT;
        size_t bitIndex = currentPage % MEM_BITMAP_VALUE_PAGE_COUNT;

        if (isReserved)
        {
            memory_phy_bitmap[bitmapIndex] |= ((MemoryBitmapValue_t)1 << bitIndex);
        }
        else
        {
            memory_phy_bitmap[bitmapIndex] &= ~((MemoryBitmapValue_t)1 << bitIndex);
        }
    }
    return TRUE;
}
