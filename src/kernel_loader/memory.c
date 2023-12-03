#include <stddef.h>
#include "memory/memory.h"
#include "memory/internal.h"
#include "int_gate16/gate.h"
#include "shared/paging.h"

#define EFLAGS_CF_MASK 0x1

static void update_mem_bitmap();
static void update_memmap_acpi();
static size_t ACPIMemoryMapLength = 0;
static MemoryACPIMapEntry_t ACPIMemoryMap[ACPI_MEMORY_MAP_LENGTH];

/*
 * A temporary bitmap of reserved/allocated pages
 * We'll pass this to the kernel, so that it can allocate memory for the actual bitmap
 */
MemoryBitmapValue_t mem_bitmap[MEM_BITMAP_LENGTH] = {0};

void memory_phy_init()
{
    update_memmap_acpi();
    update_mem_bitmap();
}

physical_ptr_t memory_phy_allocate_aligned(size_t size)
{
    size_t pageCount = LEN_TO_PAGE(size);
    size_t contiguousCounter = 0;
    for (size_t i = 0; i < MEM_BITMAP_LENGTH; i++)
    {
        for (size_t j = 0; j < MEM_BITMAP_VALUE_PAGE_COUNT; j++)
        {
            MemoryBitmapValue_t mask = 1 << j;
            if (!(mem_bitmap[i] & mask))
            {
                contiguousCounter++;
            }
            else
            {
                contiguousCounter = 0;
            }

            if (contiguousCounter == pageCount)
            {
                physical_ptr_t firstPage = (i * MEM_BITMAP_VALUE_PAGE_COUNT) + j - (pageCount - 1);
                memory_bitmap_set_range(mem_bitmap, MEM_BITMAP_LENGTH, firstPage, pageCount, TRUE);
                return firstPage * PAGE_SIZE;
            }
        }
    }

    return PHY_NULL;
}

void memory_phy_free(physical_ptr_t pointer, size_t size)
{
    memory_bitmap_set_range(mem_bitmap, MEM_BITMAP_LENGTH, ADDR_TO_PAGE(pointer), LEN_TO_PAGE(size), FALSE);
}

void memory_phy_reserve(physical_ptr_t pointer, size_t size)
{
    memory_bitmap_set_range(mem_bitmap, MEM_BITMAP_LENGTH, ADDR_TO_PAGE(pointer), LEN_TO_PAGE(size), TRUE);
}

static void update_mem_bitmap()
{
    memset(mem_bitmap, 0, MEM_BITMAP_LENGTH*sizeof(MemoryBitmapValue_t));
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
