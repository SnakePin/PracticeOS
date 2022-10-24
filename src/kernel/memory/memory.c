#include "memory/memory.h"
#include "memory/internal.h"
#include "int_gate16/gate.h"
#include "utils.h"

#define EFLAGS_CF_MASK 0x1

static void update_memmap_acpi();

size_t memory_MemoryMapEntryCount = 0;
MemoryACPIMapEntry_t memory_MemoryMapACPI[ACPI_MEMORY_MAP_LENGTH];
MemoryPhyAllocLink_t memory_PhyAllocListRoot;

// This is very inefficient and it frags the memory quite a lot
uintptr_t memory_phy_allocate(size_t size)
{
    // The allocated memory will contain the allocation link too
    size += sizeof(MemoryPhyAllocLink_t);

    uintptr_t firstfit = phy_get_firstfit(size);
    if (firstfit == 0)
    {
        return 0;
    }

    // Create the allocation link
    MemoryPhyAllocLink_t *allocLink = (MemoryPhyAllocLink_t *)firstfit;
    allocLink->BaseAddress = (uintptr_t)allocLink;
    allocLink->Length = size;
    allocLink->Prev = &memory_PhyAllocListRoot;
    allocLink->Next = memory_PhyAllocListRoot.Next;
    memory_PhyAllocListRoot.Next = allocLink;

    // Link next to us
    if (allocLink->Next != NULL)
    {
        ((MemoryPhyAllocLink_t *)allocLink->Next)->Prev = allocLink;
    }

    // Skip the allocation link when returning the pointer
    return firstfit + sizeof(MemoryPhyAllocLink_t);
}

void memory_phy_free(uintptr_t pointer)
{
    // We don't have to traverse the linked list, the entry should be right under the pointer
    MemoryPhyAllocLink_t *target = ((MemoryPhyAllocLink_t *)pointer) - 1;
    uintptr_t actualDataPointer = target->BaseAddress + sizeof(MemoryPhyAllocLink_t);
    if (actualDataPointer != pointer)
    {
        // Trying to free invalid pointer?
        return;
    }

    if (target->Next != NULL)
    {
        // Connect next to previous
        ((MemoryPhyAllocLink_t *)target->Next)->Prev = target->Prev;
    }

    // Previous link always exists since there's a root
    ((MemoryPhyAllocLink_t *)target->Prev)->Next = target->Next;
}

void memory_init()
{
    update_memmap_acpi();
    memory_PhyAllocListRoot.Prev = NULL;
    memory_PhyAllocListRoot.Next = NULL;
}

static void update_memmap_acpi()
{
    memory_MemoryMapEntryCount = 0;
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
        memcpy((void *)&memory_MemoryMapACPI[memory_MemoryMapEntryCount], (void *)ACPI_E820_BUFFER, sizeof(MemoryACPIMapEntry_t));
        memory_MemoryMapEntryCount += 1;
        inRegs.ebx = outRegs.ebx; // Continuation value

    } while (outRegs.ebx != 0); // EBX==0 means that we just got the last entry
}
