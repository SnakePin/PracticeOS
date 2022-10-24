#include <stdint.h>
#include <stddef.h>
#include "memory/memory.h"
#include "memory/internal.h"

static MemoryPhyAllocLink_t *phy_get_last_alloc_in_range(uintptr_t pointer, size_t size);

static inline uint8_t rangeOverlapCheck(uintptr_t r1start, uintptr_t r1end, uintptr_t r2start, uintptr_t r2end)
{
    return IS_IN_RANGE(r1start, r2start, r2end)
        || IS_IN_RANGE(r1end, r2start, r2end)
        || IS_IN_RANGE(r2start, r1start, r1end)
        || IS_IN_RANGE(r2end, r1start, r1end);
}

uintptr_t phy_get_firstfit(size_t size)
{
    for (size_t i = 0; i < memory_MemoryMapEntryCount; i++)
    {
        uintptr_t rangeEnd = memory_MemoryMapACPI[i].BaseAddress + memory_MemoryMapACPI[i].Length;
        uintptr_t allocationTarget = memory_MemoryMapACPI[i].BaseAddress;

        if (memory_MemoryMapACPI[i].Type != (uint32_t)Usable) {
            // Skip reserved ranges
            continue;
        }
        if (allocationTarget <= MEMORY_KERNEL_RESERVED) {
            if (rangeEnd <= MEMORY_KERNEL_RESERVED) {
                // Range is completely reserved
                continue;
            }
            else {
                // Try to allocate from a byte above the reserved range
                allocationTarget = MEMORY_KERNEL_RESERVED+1;
            }
        }

        MemoryPhyAllocLink_t *lastObstruction = NULL;
        do
        {
            uintptr_t allocationTargetEnd = allocationTarget + size;
            if (allocationTargetEnd > rangeEnd)
            {
                // Can't fit the allocation in this range
                goto continue_outer;
            }

            lastObstruction = phy_get_last_alloc_in_range(allocationTarget, size);
            if (lastObstruction != NULL) {
                // Perform the checks again
                allocationTarget = lastObstruction->BaseAddress + lastObstruction->Length;
            }
        } while (lastObstruction != NULL);

        return allocationTarget;

    continue_outer:
        continue;
    }
    return 0;
}

static MemoryPhyAllocLink_t *phy_get_last_alloc_in_range(uintptr_t pointer, size_t size)
{
    MemoryPhyAllocLink_t *lastLinkFound = NULL;
    MemoryPhyAllocLink_t *currentLink = memory_PhyAllocListRoot.Next;
    while (currentLink != NULL)
    {
        uintptr_t pointerLast = pointer + size - 1;
        uintptr_t curLinkRangeLast = currentLink->BaseAddress + currentLink->Length - 1;
        if (rangeOverlapCheck(currentLink->BaseAddress, curLinkRangeLast, pointer, pointerLast))
        {
            if (lastLinkFound == NULL || lastLinkFound->BaseAddress < currentLink->BaseAddress)
            {
                lastLinkFound = currentLink;
            }
        }
        currentLink = currentLink->Next;
    }
    return lastLinkFound;
}
